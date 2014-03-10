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
#include "softdevice_handler.h"
#include "nrf_error.h"
#include "app_scheduler.h"
#include "irt_common.h"
#include "irt_peripheral.h"
#include "resistance.h"
#include "speed.h"
#include "power.h"
#include "user_profile.h"
#include "ble_ant.h"
#include "nrf_delay.h"
#include "ant_ctrl.h"

#define ANT_CTRL_INTERVAL				APP_TIMER_TICKS(2000, APP_TIMER_PRESCALER)  /**< Remote control availability annoncement. */
#define CYCLING_POWER_MEAS_INTERVAL		APP_TIMER_TICKS(250, APP_TIMER_PRESCALER) /**< Bike power measurement interval (ticks). */
#define DEFAULT_WHEEL_SIZE_MM			2070u
#define DEFAULT_TOTAL_WEIGHT_KG			(178.0f * 0.453592)	// Convert lbs to KG

#define BLE_ADV_BLINK_RATE_MS			500u

#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE,\
                                            BLE_STACK_HANDLER_SCHED_EVT_SIZE)       /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE                10                                          /**< Maximum number of events in the scheduler queue. */

static uint8_t 							m_resistance_level = 0;
static resistance_mode_t				m_resistance_mode = RESISTANCE_SET_STANDARD;
static uint16_t							m_servo_pos;
static app_timer_id_t               	m_cycling_power_timer_id;                    /**< Cycling power measurement timer. */
static app_timer_id_t               	m_ant_ctrl_timer_id;

static bool								mb_profile_dirty = false;
static user_profile_t 					m_user_profile;
static rc_sim_forces_t					m_sim_forces;

static bool								mb_ant_ctrl_connected = false;

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
    // TODO: HACK temporarily IGNORE this message.  
	if (error_code == 0x401F) //TRANSFER_IN_PROGRESS
		return;
		
	set_led_red();

    // This call can be used for debug purposes during development of an application.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Define IRT_DEBUG to enable.
#if defined(IRT_DEBUG)
	// Not this function does not return - it will hang waiting for a debugger to attach.
	ble_debug_assert_handler(error_code, line_num, p_file_name);
#endif

    // On assert, the system can only recover with a reset.
    NVIC_SystemReset();
}

/**@brief	Handle Soft Device system events. */
static void sys_evt_dispatch(uint32_t sys_evt)
{
	// TODO: Process storage events.
	//pstorage_sys_event_handler(sys_evt);
}

/*----------------------------------------------------------------------------
 * KICKR resistance commands.
 * ----------------------------------------------------------------------------*/

/**@brief Parses the wheel params message from the KICKR.  It then advises the
 * 		  speed module and updates the user profile.  */
static void set_wheel_params(uint8_t *pBuffer)
{
	uint16_t wheel_size;
	
	// Comes as 20700 for example, we'll round to nearest mm.
	wheel_size = (pBuffer[0] | pBuffer[1] << 8u) / 10;

	if (m_user_profile.wheel_size_mm != wheel_size)
	{
		m_user_profile.wheel_size_mm = wheel_size;
		mb_profile_dirty = true;
	}

	// Call speed module to set the wheel size.
	set_wheel_size(m_user_profile.wheel_size_mm);
}

// Parses the SET_SIM message from the KICKR and has user profile info.
// TODO: move this to the user profile object.
static void set_sim_params(uint8_t *pBuffer)
{
	// Weight comes through in KG as 8500 85.00kg for example.
	float weight = (pBuffer[0] | pBuffer[1] << 8u) / 100.0f;

	if (m_user_profile.total_weight_kg != weight)
	{
		m_user_profile.total_weight_kg = weight;
		mb_profile_dirty = true; 	// flag that we have changes to save.
	}

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
		// TODO: Figure out the right time to store the profile.  I don't think you can write
		// when the radio is active, so you either have to shut the radio down and store or
		// wait until it's time to shut down the device.  However, if the device is plugged in
		// the user could pull the cord before it formally shuts down.

		uint32_t err_code;

		// Initialize hard coded user profile for now.
		memset(&m_user_profile, 0, sizeof(m_user_profile));
		
		err_code = user_profile_load(&m_user_profile);

		if (m_user_profile.wheel_size_mm == 0)
			// Wheel circumference in mm.
			m_user_profile.wheel_size_mm = DEFAULT_WHEEL_SIZE_MM;
		
		if (m_user_profile.total_weight_kg == 0.0f)
			// Total weight of rider + bike + shoes, clothing, etc...
			m_user_profile.total_weight_kg = DEFAULT_TOTAL_WEIGHT_KG;
		
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

/**@brief Persists any updates the user profile. */
static void profile_update(void)
{
	uint32_t err_code;

	if (mb_profile_dirty)
	{
		// This method ensures the device is in a proper state in order to update
		// the profile.
		err_code = user_profile_store(&m_user_profile);

		// If successful, clear the dirty flag.
		if (err_code == NRF_SUCCESS)
		{
			mb_profile_dirty = false;
		}
	}
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

static void resistance_adjust(irt_power_meas_t* p_power_meas_first, irt_power_meas_t* p_power_meas_current)
{
	// Make a local copy we can modify.
	irt_power_meas_t power_meas = *p_power_meas_current;

	if (p_power_meas_first != NULL)
	{
		// Average the speed.  A new average power will get calculated based on this.
		power_meas.instant_speed_mps = get_speed_mps(
				(p_power_meas_current->accum_wheel_revs - p_power_meas_first->accum_wheel_revs),
				(p_power_meas_current->last_wheel_event_time - p_power_meas_first->last_wheel_event_time));
	}

	// Don't attempt to adjust if stopped.
	if (power_meas.instant_speed_mps == 0.0f)
		return;

	// If in erg or sim mode, adjust resistance accordingly.
	switch (m_resistance_mode)
	{
		case RESISTANCE_SET_ERG:
			m_servo_pos = set_resistance_erg(&m_user_profile,
												&m_sim_forces,
												&power_meas);
			break;

		case RESISTANCE_SET_SIM:
			m_servo_pos = set_resistance_sim(&m_user_profile,
												&m_sim_forces,
												&power_meas);
			break;

		default:
			break;
	}
}

/*----------------------------------------------------------------------------
 * Main function for calculating power and transmitting.
 * ----------------------------------------------------------------------------*/

/**@brief	Transmits the ant+ and ble power messages, returns speed & power stats.
 *
 */
static int32_t calculate_power(irt_power_meas_t* p_power_meas)
{
		// Hang on to accumulated torque for a session duration.
		// TODO: make sure this gets cleared at some point, probably
		// the wrong spot to keep this.
		static uint16_t 			m_accum_torque = 0;
		uint32_t err_code;
		int16_t		watts;
		uint16_t 	torque;

		speed_event_t			speed_event;
		memset(&speed_event, 0, sizeof(speed_event));
		
		// Calculate speed.
		// TODO: use error handling here.
		calc_speed(&speed_event);
		/*float speed_mph = get_speed_mph(speed_event.speed_mps);

		// Calculate power.
		/*err_code = calc_power(speed_mph, m_user_profile.total_weight_kg * 2.20462262, m_resistance_level, &watts);
		// TODO: Handle the error for real here, not sure what the overall error 
		// handling strategy will be, but this is not critical, just move on.
		if (err_code != IRT_SUCCESS)
			return;		
		*/
		err_code = calc_power2(speed_event.speed_mps, m_user_profile.total_weight_kg, m_servo_pos, &watts);
		APP_ERROR_CHECK(err_code);
			
		// Calculate torque.
		err_code = calc_torque(watts, speed_event.period_2048, &torque);
		APP_ERROR_CHECK(err_code);

		// Store accumulated torque for the session.
		m_accum_torque += torque;
		
		p_power_meas->instant_power 		= watts;
		p_power_meas->accum_torque 			= m_accum_torque;
		p_power_meas->accum_wheel_revs		= speed_event.accum_wheel_revs;
		p_power_meas->last_wheel_event_time	= speed_event.event_time_2048;
		
		p_power_meas->resistance_mode		= m_resistance_mode;
		p_power_meas->resistance_level		= m_resistance_level;
		p_power_meas->servo_position		= m_servo_pos;

		p_power_meas->instant_speed_mps		= speed_event.speed_mps;

#if defined(BLE_NUS_ENABLED)
/*		static const char format[] = "r,w,l:%i,%i,%i";
		char message[16];
		memset(&message, 0, sizeof(message));
		uint8_t length = sprintf(message, format, 
								speed_event.accum_flywheel_revs, 
								watts, 
								m_resistance_level);
								*/
		static const char format[] = "t,w:%i,%i";
		char message[16];
		memset(&message, 0, sizeof(message));
		uint8_t length = sprintf(message, format,
			speed_event.event_time_2048,
			speed_event.accum_wheel_revs);

		debug_send(message, sizeof(message));
#endif			

		return IRT_SUCCESS;
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
	uint32_t err_code;
	irt_power_meas_t* p_power_meas_current 		= NULL;
	irt_power_meas_t* p_power_meas_first 		= NULL;
		
	// Get pointers to the event structures.
	err_code = irt_power_meas_fifo_op(&p_power_meas_first, &p_power_meas_current);
	APP_ERROR_CHECK(err_code);

	// Calculate the power.
	err_code = calculate_power(p_power_meas_current);
	APP_ERROR_CHECK(err_code);

	// TODO: this should return an err_code.
	// Transmit the power message.
	cycling_power_send(p_power_meas_current);

	// If in erg or sim mode, adjusts the resistance.
	if (m_resistance_mode == RESISTANCE_SET_ERG ||
			m_resistance_mode == RESISTANCE_SET_SIM)
	{
		resistance_adjust(p_power_meas_first, p_power_meas_current);
	}

	// TODO: ideally we run from a battery and we don't have
	// to do this here.  This is just in case the cord some
	// how gets yanked before it's time to shutdown.
	// Only attempt this if dirty is set and every 10 transmits.
	static uint8_t counter = 1;
	if (mb_profile_dirty && (counter++ %10))
		profile_update();
}

static void ant_ctrl_timeout_handler(void * p_context)
{
	UNUSED_PARAMETER(p_context);
	ant_ctrl_device_avail_tx((uint8_t)mb_ant_ctrl_connected);
}

/**@brief Function for starting the application timers.
 */
static void application_timers_start(void)
{
    uint32_t err_code;

    // Start application timers
    err_code = app_timer_start(m_cycling_power_timer_id, CYCLING_POWER_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

	err_code = app_timer_start(m_ant_ctrl_timer_id, ANT_CTRL_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);
}

static void application_timers_stop(void)
{
	uint32_t err_code;
		
	err_code = app_timer_stop(m_cycling_power_timer_id);
    APP_ERROR_CHECK(err_code);

	err_code = app_timer_stop(m_ant_ctrl_timer_id);
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

	err_code = app_timer_create(&m_ant_ctrl_timer_id,
		APP_TIMER_MODE_REPEATED,
		ant_ctrl_timeout_handler);
	
	APP_ERROR_CHECK(err_code);
}

/**@brief Scheduler initialization.
 *
 * @details Initializes the scheduler module which is used under the covers to send
 * the ANT/BLE messages.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}

/*----------------------------------------------------------------------------
 * Event handlers
 * ----------------------------------------------------------------------------*/

static void on_button_i(void)
{
	m_resistance_level = 0;
	m_servo_pos = set_resistance(m_resistance_level);
	ant_bp_resistance_tx_send(m_resistance_mode, &m_resistance_level);
}

static void on_button_ii(void)
{
	/* TODO: wrap this in an ifdef for DEBUG.
	uint8_t data[] = "button_ii_event";
	debug_send(&data[0], sizeof(data));
	*/
	
	// decrement
	if (m_resistance_level > 0)
	{
		m_servo_pos = set_resistance(--m_resistance_level);	
		ant_bp_resistance_tx_send(m_resistance_mode, &m_resistance_level);
	}
}

static void on_button_iii(void)
{
	// increment
	if (m_resistance_level < (MAX_RESISTANCE_LEVELS-1))
	{
		m_servo_pos = set_resistance(++m_resistance_level);
		ant_bp_resistance_tx_send(m_resistance_mode, &m_resistance_level);
	}
}

static void on_button_iv(void)
{
	m_resistance_level = MAX_RESISTANCE_LEVELS-1;
	m_servo_pos = set_resistance(m_resistance_level);
	ant_bp_resistance_tx_send(m_resistance_mode, &m_resistance_level);
}

static void on_accelerometer(uint8_t source)
{
	/* Starving the CPU?
	if (source & 0x01) // Wake
	{
		set_led_green();
	}
	else if (source & 0x80) // Sleep
	{
		// Blink red and go to sleep.
		set_led_red();
		nrf_delay_ms(300);
		clear_led();
	}
	else
	{
		set_led_green();
		nrf_delay_ms(300);
		clear_led();
	}*/
}

static void on_ble_connected(void) 
{
	blink_led_green_stop();
	set_led_green();
}
	
static void on_ble_disconnected(void) 
{
	// Clear connection LED.
	clear_led();
	
	// Restart advertising.
	ble_advertising_start();
}

static void on_ble_timeout(void) 
{
	blink_led_green_stop();
}

static void on_ble_advertising(void)
{
	blink_led_green_start(BLE_ADV_BLINK_RATE_MS);
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
			m_resistance_mode = RESISTANCE_SET_STANDARD;
			m_resistance_level = rc_evt.pBuffer[0];
			m_servo_pos = set_resistance(m_resistance_level);
			break;
			
		case RESISTANCE_SET_PERCENT:
			// Reset state since we're not in standard mode any more.
			m_resistance_level = 0;
			m_resistance_mode = RESISTANCE_SET_PERCENT;
			
			// Parse the buffer for percentage.
			float percent = get_resistance_pct(rc_evt.pBuffer);
			m_servo_pos = set_resistance_pct(percent);
			break;

		case RESISTANCE_SET_ERG:
			// Assign target watt level.
			m_resistance_mode = RESISTANCE_SET_ERG;
			m_sim_forces.erg_watts =
				rc_evt.pBuffer[0] | rc_evt.pBuffer[1] << 8u;
			break;

		case RESISTANCE_SET_SIM:
			m_resistance_mode = RESISTANCE_SET_SIM;
			set_sim_params(rc_evt.pBuffer);
			break;
			
		case RESISTANCE_SET_SLOPE:
			m_resistance_mode = RESISTANCE_SET_SIM;
			// Parse out the slope.
			m_sim_forces.grade = get_sim_grade(rc_evt.pBuffer);
			break;
			
		case RESISTANCE_SET_WIND:
			m_resistance_mode = RESISTANCE_SET_SIM;
			// Parse out the wind speed.
			m_sim_forces.wind_speed_mps = get_sim_wind(rc_evt.pBuffer);
			break;
			
		case RESISTANCE_SET_WHEEL_CR:
			//  Parse wheel size and update accordingly.
			set_wheel_params(rc_evt.pBuffer);
			break;
			
		default:
			break;
	}

	// Send acknowledgement. 
	ant_bp_resistance_tx_send(m_resistance_mode, rc_evt.pBuffer);

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

// Invoked when a button is pushed on the remote control.
static void on_ant_ctrl_command(ctrl_evt_t evt)
{
	// TODO: implementation.
	switch (evt.command)
	{
		case Menu_Up:
			on_button_iii(); 
			break;

		case Menu_Down:
			on_button_ii();
			break;

		default:
			break;
	}
}

// TODO: This event should be registered for a callback when it's time to
// power the system down.
static void on_power_down(void)
{
	uint32_t err_code;

	// Free heap allocation.
	irt_power_meas_fifo_free();

	// Shut the system down.
	err_code = sd_power_system_off();
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

	// Peripheral interrupt event handlers.
	static peripheral_evt_t on_peripheral_handlers = {
		on_button_i,
		on_button_ii,
		on_button_iii,
		on_button_iv,
		on_accelerometer
	};

	// Initialize connected peripherals (temp, accelerometer, buttons, etc..).
	peripheral_init(&on_peripheral_handlers);

	// ANT+, BLE event handlers.
	static ant_ble_evt_handlers_t ant_ble_handlers = {
		on_ble_connected,
		on_ble_disconnected,
		on_ble_timeout,
		on_ble_advertising,
		on_ble_uart,
		on_ant_channel_closed,
		on_ant_power_data,
		on_set_resistance,
		on_ant_ctrl_command
	};

	// TODO: Question should we have a separate method to initialize soft device?

	// Initializes the soft device, Bluetooth and ANT stacks.
	ble_ant_init(&ant_ble_handlers);

	// Initialize the scheduler.
	scheduler_init();

    // Subscribe for system events.
	uint32_t err_code;
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);

	// Begin advertising and receiving ANT messages.
	ble_ant_start();

	// Start off with resistance at 0.
	set_resistance(m_resistance_level);
	m_servo_pos = RESISTANCE_LEVEL[m_resistance_level];

	// Initialize module to read speed from flywheel.
	init_speed(PIN_FLYWHEEL);
	if (m_user_profile.wheel_size_mm > 0)
		set_wheel_size(m_user_profile.wheel_size_mm);

	// Initialize the FIFO queue for holding events.
	irt_power_meas_fifo_init(IRT_FIFO_SIZE);

	// Start the main loop for reporting ble services.
	application_timers_start();

    // Enter main loop
    for (;;)
    {
    	app_sched_execute();
    	power_manage();
    }
}
