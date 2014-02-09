/****************************************************************************** 
 * Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * E-Motion Roller firmware using BLE and ANT.
 *
 * The application uses the follwowing BLE services:
 *		Cycling Power Service
 *		Device Information Service, 
 * 		Battery Service 
 *		Nordic UART Service (for debugging purposes)
 *
 * The application uses the following ANT services:
 *		ANT Cycling Power RX profile
 *		ANT Cycling Power TX profile
 *		ANT Speed Sensor TX profile
 *
 * @file 		main.c 
 * @brief 	The purpose of main is to control program flow and manage any
 *					application specific state.  All other protocol details are handled 
 *					in referenced modules.
 *
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf_error.h"
#include "irt_common.h"
#include "irt_peripheral.h"
#include "resistance.h"
#include "speed.h"
#include "power.h"
#include "user_profile.h"
#include "ble_ant.h"

#define CYCLING_POWER_MEAS_INTERVAL       APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER)/**< Bike power measurement interval (ticks). */

static uint8_t 														m_resistance_level = 0;
static resistance_mode_t									m_resistance_mode = RESISTANCE_SET_STANDARD;
static uint16_t														m_servo_pos;
static app_timer_id_t               			m_cycling_power_timer_id;                    /**< Cycling power measurement timer. */
static user_profile_t 										m_user_profile;
static rc_sim_forces_t										m_sim_forces;

/*----------------------------------------------------------------------------
 * Error Handlers
 * ----------------------------------------------------------------------------*/

/**@brief Error handler function, which is called when an error has occurred. 
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of error.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name. 
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    // TODO: temporarily IGNORE this message.  
		if (error_code == 0x401F) //TRANSFER_IN_PROGRESS
			return;
		
		nrf_gpio_pin_set(ASSERT_LED_PIN_NO);
		
		/* TODO: doesn't work, but we need to do something like this.
		uint8_t data[64];
		sprintf(data, "ERR: %i, LINE: %i, FILE: %s", 
		error_code, line_num, *p_file_name);
		//debug_send(data, sizeof(data)); */

    // This call can be used for debug purposes during development of an application.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    // ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover with a reset.
    NVIC_SystemReset();
}


/*----------------------------------------------------------------------------
 * KICKR resistance commands.
 * ----------------------------------------------------------------------------*/

// Parses the SET_SIM message from the KICKR and has user profile info.
// TODO: move this to the user profile object.
static void set_sim_params(uint8_t *pBuffer)
{
	// Weight comes through in KG as 8500 85.00kg for example.
	m_user_profile.total_weight_kg = (pBuffer[0] | pBuffer[1] << 8u) / 100.0f;
	// Co-efficient for rolling resistance.
	m_sim_forces.crr = (pBuffer[2] | pBuffer[3] << 8u) / 10000.0f;
	// Co-efficient of drag.
	m_sim_forces.c = (pBuffer[4] | pBuffer[5] << 8u) / 1000.0f;

#if defined(BLE_NUS_ENABLED)
		static const char format[] = "k,r,c:%i,%i,%i";
		char message[16];
		memset(&message, 0, sizeof(message));
		uint8_t length = sprintf(message, format, 
															(uint16_t)m_user_profile.total_weight_kg,
															((uint16_t)m_sim_forces.crr)*10000, 
															((uint16_t)m_sim_forces.c)*1000);
		debug_send(message, sizeof(message));
#endif		
}

/**@brief	Initializes user profile and loads from flash.  Sets defaults for
 * 				simulation parameters if not set.
 */
static void profile_init(void)
{
		// Initialize hard coded user profile for now.
		memset(&m_user_profile, 0, sizeof(m_user_profile));
		
		// Wheel circumference in mm.
		m_user_profile.wheel_size_mm = 2070;
		
		// Total weight of rider + bike + shoes, clothing, etc...
		m_user_profile.total_weight_kg = 178.0f * 0.453592;	// Convert lbs to KG
		
		// Initialize simulation forces structure.
		memset(&m_sim_forces, 0, sizeof(m_sim_forces));	
		
	 /*	fCrr is the coefficient of rolling resistance (unitless). Default value is 0.004. 
	 *
	 *	fC is equal to A*Cw*Rho where A is effective frontal area (m^2); Cw is drag 
	 *	coefficent (unitless); and Rho is the air density (kg/m^3). The default value 
	 *	for A*Cw*Rho is 0.60. 	
	 */
		m_sim_forces.crr 								= 0.004f;
		m_sim_forces.c 									= 0.60f;
}

/**@brief Parses the resistance percentage out of the KICKR command.
 *
 */
static float get_resistance_pct(uint8_t *buffer)
{
	/*	Not exactly sure why it is this way, but it seems that 2 bytes hold a
	value that is a percentage of the MAX which seems arbitrarily to be 16383.
	Use that value to calculate the percentage, for example:

	10.7% example:
	(16383-14630) / 16383 = .10700 = 10.7%
	*/
	static const float PCT_100 = 16383.0f;
	uint16_t value = buffer[0] | buffer[1] << 8u;

	float percent = (PCT_100 - value) / PCT_100;

	return percent;
}

/**@brief Parses the simulated wind speed out of the KICKR command.
 * @note	This is the headwind in meters per second. A negative headwind 
 *				represents a tailwind. The range for mspWindSpeed is -30.0:30.0.
 */
static float get_sim_wind(uint8_t *buffer)
{
	// Note this is a signed int.
	int16_t value = buffer[0] | buffer[1] << 8u;
	
	// First bit is the sign.
	bool negative = value >> 15u;
	
	if (negative)
	{
		// Remove the negative sign.
		value = value & 0x7FFF;
	}
	else
	{
		value = (32768 - value) *-1;
	}
	
	// Set the right scale.
	return value / 1000.0f;
}


/**@brief Parses the simulated slope out of the KICKR command.
 *
 */
static float get_sim_grade(uint8_t *buffer)
{
	uint16_t value = buffer[0] | buffer[1] << 8u;
	
	// First bit is the sign.
	bool negative = value >> 15u;
	
	float percent = 0.0f;
	
	if (negative)
	{
		// Strip the negative sign bit.
		value = value & 0x7FFF;
		// results in a positive (uphill) grade.
		percent = 1 - ((32768.0f - value) / 32768.0f);
	}
	else 
	{
		// results in a negative (downhill) grade.
		percent = ((value - 32768.0f) / 32768.0f);
	}

	// Initial value sent on the wire from KICKR ranges from -1.0 to 1.0 and 
	// represents a percentage of 45 degrees up/downhill.  
	// e.g. 1.0% = 45 degree uphill, 
	// -1.0% = 45 degree downhill, 
	// 0% = flat, 
	// 0.10% = 4.5% uphill grade.
	return percent * 0.45;
}


/*----------------------------------------------------------------------------
 * Main function for calculating power and transmiting.
 * ----------------------------------------------------------------------------*/

/**@brief	Transmits the ant+ and ble power messages, returns speed & power stats.
 *
 */
static irt_power_meas_t transmit_power(void)
{
		// Hang on to accumulated torque for a session duration.
		// TODO: make sure this gets cleared at some point, probably
		// the wrong spot to keep this.
		static uint16_t 			m_accum_torque = 0;

    uint32_t err_code;

		// Initialize structures.
		irt_power_meas_t power_meas;
		memset(&power_meas, 0, sizeof(power_meas));
		
		speed_event_t			speed_event;
		memset(&speed_event, 0, sizeof(speed_event));

		// Calculate speed.
		// TODO: use error handling here.
		calc_speed(&speed_event);
		float speed_mph = get_speed_mph(speed_event.speed_mps);
		
		// Calculate power.
		int16_t		watts					= 0;
		/*err_code = calc_power(speed_mph, m_user_profile.total_weight_kg * 2.20462262, m_resistance_level, &watts);
		// TODO: Handle the error for real here, not sure what the overall error 
		// handling strategy will be, but this is not critical, just move on.
		if (err_code != IRT_SUCCESS)
			return;		
		*/
		err_code = calc_power2(speed_event.speed_mps, m_user_profile.total_weight_kg, m_servo_pos, &watts);
		// TODO: Handle the error for real here, not sure what the overall error 
		// handling strategy will be, but this is not critical, just move on.
		if (err_code != IRT_SUCCESS)
			return power_meas;		
			
		// Calculate torque.
		uint16_t 	torque				= 0;
		err_code = calc_torque(watts, speed_event.period_2048, &torque);
		if (err_code != IRT_SUCCESS)
			// TODO: handle error here, or at least bubble up.
			return power_meas;
		
		// Store accumulated torque for the session.
		m_accum_torque += torque;
		
		power_meas.instant_power 				= watts;
		power_meas.accum_torque 				= m_accum_torque;
		power_meas.accum_wheel_revs 		= speed_event.accum_wheel_revs;
		power_meas.last_wheel_event_time= speed_event.event_time_2048;
		
		power_meas.resistance_mode			= m_resistance_mode;
		power_meas.resistance_level			= m_resistance_level;

		power_meas.instant_speed_mps		= speed_event.speed_mps;

		cycling_power_send(&power_meas);

#if defined(BLE_NUS_ENABLED)
		static const char format[] = "r,w,l:%i,%i,%i";
		char message[16];
		memset(&message, 0, sizeof(message));
		uint8_t length = sprintf(message, format, 
															speed_event.accum_flywheel_revs, 
															watts, 
															m_resistance_level);
		debug_send(message, sizeof(message));
#endif			

		// Return power measurement.
		return power_meas;
}

/*----------------------------------------------------------------------------
 * Timer functions
 * ----------------------------------------------------------------------------*/

/**@brief Function for handling the cycling power measurement timer timeout.
 *
 * @details This function will be called each timer expiration.
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void cycling_power_meas_timeout_handler(void * p_context)
{
	  UNUSED_PARAMETER(p_context);
	
		// Calculate and transmit power messages.
		irt_power_meas_t power_meas = transmit_power();
		
		// If in erg or sim mode, adjust resistance accordingly. 
		switch (m_resistance_mode)
		{
			case RESISTANCE_SET_ERG:
				set_resistance_erg(&m_user_profile, 
													&m_sim_forces,
													&power_meas);
				break;
				
			case RESISTANCE_SET_SIM:
				set_resistance_sim(&m_user_profile,
													&m_sim_forces,
													&power_meas);
				break;
				
			default:
				break;
		}
}

/**@brief Function for starting the application timers.
 */
static void application_timers_start(void)
{
    uint32_t err_code;

    // Start application timers
    err_code = app_timer_start(m_cycling_power_timer_id, CYCLING_POWER_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

static void application_timers_stop(void)
{
		uint32_t err_code;
		
		err_code = app_timer_stop(m_cycling_power_timer_id);
    APP_ERROR_CHECK(err_code);
}

/**@brief Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    uint32_t err_code;

		// Initialize timer module
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);

    err_code = app_timer_create(&m_cycling_power_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                cycling_power_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);

}

/*----------------------------------------------------------------------------
 * Event handlers
 * ----------------------------------------------------------------------------*/

static void on_button_i_event(void)
{
	m_resistance_level = 0;
	m_servo_pos = set_resistance(m_resistance_level);
}

static void on_button_ii_event(void)
{
	/* TODO: wrap this in an ifdef for DEBUG.
	uint8_t data[] = "button_ii_event";
	debug_send(&data[0], sizeof(data));
	*/
	
	// decrement
	if (m_resistance_level > 0)
	{
		m_servo_pos = set_resistance(--m_resistance_level);	
	}
}

static void on_button_iii_event(void)
{
	// increment
	if (m_resistance_level < (MAX_RESISTANCE_LEVELS-1))
	{
		m_servo_pos = set_resistance(++m_resistance_level);
	}
}

static void on_button_iv_event(void)
{
	m_resistance_level = MAX_RESISTANCE_LEVELS-1;
	m_servo_pos = set_resistance(m_resistance_level);
}

static void on_ble_connected(void) 
{
		nrf_gpio_pin_set(CONNECTED_LED_PIN_NO);
		nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);	
}
	
static void on_ble_disconnected(void) 
{
		nrf_gpio_pin_clear(CONNECTED_LED_PIN_NO);
}

static void on_ble_timeout(void) 
{
		nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);
}

static void on_ble_advertising(void)
{
		nrf_gpio_pin_set(ADVERTISING_LED_PIN_NO);	
}

static void on_ble_uart(uint8_t * data, uint16_t length)
{
#ifdef LOOPBACK
    debug_send(data, length);
#endif
}

static void on_ant_channel_closed(void) {}
static void on_ant_power_data(void) {}

/*@brief	Event handler that the cycling power service calls when a set resistance
 *				command is received.
 *
 */
static void on_set_resistance(rc_evt_t rc_evt)
{
	// TODO: Write a macro for extracting the 2 byte value with endianness.			
	
	// 
	// Parse the messages and set state or resistance as appropriate.
	//
	switch (rc_evt.operation)
	{
		case RESISTANCE_SET_STANDARD:
			m_resistance_mode = rc_evt.operation;
			m_resistance_level = rc_evt.pBuffer[0];
			m_servo_pos = set_resistance(m_resistance_level);
			break;
			
		case RESISTANCE_SET_PERCENT:
			// Reset state since we're not in standard mode any more.
			m_resistance_level = 0;
			m_resistance_mode = rc_evt.operation;
			
			// Parse the buffer for percentage.
			float percent = get_resistance_pct(rc_evt.pBuffer);
			m_servo_pos = set_resistance_pct(percent);
			break;
		
		case RESISTANCE_SET_SIM:
			m_resistance_mode = rc_evt.operation;
			set_sim_params(rc_evt.pBuffer);
			break;

		case RESISTANCE_SET_ERG:
			// Assign target watt level.
			m_sim_forces.erg_watts = 
				rc_evt.pBuffer[0] | rc_evt.pBuffer[1] << 8u;
			break;
			
		case RESISTANCE_SET_SLOPE:
			// Parse out the slope.
			m_sim_forces.grade = get_sim_grade(rc_evt.pBuffer);
			m_resistance_mode = RESISTANCE_SET_SIM;
			break;
			
		case RESISTANCE_SET_WIND:
			// Parse out the wind speed.
			m_sim_forces.wind_speed_mps = get_sim_wind(rc_evt.pBuffer);
			m_resistance_mode = RESISTANCE_SET_SIM;
			break;
			
		case RESISTANCE_SET_WHEEL_CR:
			m_user_profile.wheel_size_mm = 
				rc_evt.pBuffer[0] | rc_evt.pBuffer[1] << 8u;
			// TODO: this is something that should be persisted in
			// flash storage.
			set_wheel_size(m_user_profile.wheel_size_mm);
			break;
			
		default:
			break;
	}
#if defined(BLE_NUS_ENABLED)
		static const char format[] = "OP:%i,VAL:%i";
		char message[16];
		memset(&message, 0, sizeof(message));
		uint8_t length = sprintf(message, format, 
															(uint8_t)rc_evt.operation,
															(int16_t)(rc_evt.pBuffer[0] | rc_evt.pBuffer[1] << 8u));
		debug_send(message, sizeof(message));
#endif		

}

		
static void on_button_evt(uint8_t pin_no)
{
    switch (pin_no)
    {
		case PIN_BUTTON_I:
				on_button_i_event();
				break;
		case PIN_BUTTON_II:
				on_button_ii_event();
				break;
		case PIN_BUTTON_III:
				on_button_iii_event();
				break;
		case PIN_BUTTON_IV:
				on_button_iv_event();
				break;
		case PIN_SHAKE:
				nrf_gpio_pin_clear(PIN_LED_A);
				nrf_gpio_pin_set(PIN_LED_B);
				nrf_delay_ms(300);
				nrf_gpio_pin_clear(PIN_LED_B);
				break;
        default:
            APP_ERROR_HANDLER(pin_no);
    }	
}
		
/*----------------------------------------------------------------------------
 * Main program functions
 * ----------------------------------------------------------------------------*/

/**@brief Application main function.
 */
int main(void)
{
		// initialize the user profile.
		profile_init();

		// Initialize timers.
		timers_init();
		
    // Initialize peripherals
		peripheral_init(on_button_evt);

		// Event handlers.
		static ant_ble_evt_handlers_t handlers = { 
			on_ble_connected,
			on_ble_disconnected,
			on_ble_timeout,
			on_ble_advertising,
			on_ble_uart,
			on_ant_channel_closed,
			on_ant_power_data,
			on_set_resistance
		};

		// Initialize Bluetooth and ANT stacks.
		ble_ant_init(&handlers);

		// Begin advertising and receiving ANT messages.
		ble_ant_start();

		// Start off with resistance at 0.
		set_resistance(m_resistance_level);	
		m_servo_pos = RESISTANCE_LEVEL[m_resistance_level];
		
		// Initialize module to read speed from flywheel.
		init_speed(PIN_FLYWHEEL);
		if (m_user_profile.wheel_size_mm > 0)
			set_wheel_size(m_user_profile.wheel_size_mm);
		
		// TOOD: we need to ensure we're being woken up here by a HW
		// interrupt or something before we actually start reporting.
		// Start the main loop for reporting ble services.
		application_timers_start();
		
		// TODO: when do we stop timers? We need to track a timeout since last
		// HW interupt, i.e. 5 mins or so? 
		// Stop reporting ble services.
		// application_timers_stop();

    // Enter main loop
    for (;;)
    {
        power_manage();
    }
}
