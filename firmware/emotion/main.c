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
#include <math.h>
#include "float.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "nrf_error.h"
#include "app_scheduler.h"
#include "pstorage.h"
#include "irt_common.h"
#include "irt_peripheral.h"
#include "temperature.h"
#include "resistance.h"
#include "accelerometer.h"
#include "speed.h"
#include "power.h"
#include "user_profile.h"
#include "ble_ant.h"
#include "nrf_delay.h"
#include "ant_ctrl.h"


#define ANT_4HZ_INTERVAL				APP_TIMER_TICKS(250, APP_TIMER_PRESCALER)  // Remote control & bike power sent at 4hz.
#define DEFAULT_WHEEL_SIZE_MM			2069u
#define DEFAULT_TOTAL_WEIGHT_KG			(178.0f * 0.453592)	// Convert lbs to KG
#define DEFAULT_ERG_WATTS				175u
#define SIM_CRR							0.004f
#define SIM_C							0.60f
#define BLE_ADV_BLINK_RATE_MS			500u
#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE,\
                                            BLE_STACK_HANDLER_SCHED_EVT_SIZE)       /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE                10                                          /**< Maximum number of events in the scheduler queue. */

static uint8_t 							m_resistance_level = 0;
static resistance_mode_t				m_resistance_mode = RESISTANCE_SET_STANDARD;
static uint16_t							m_servo_pos;
static app_timer_id_t               	m_ant_4hz_timer_id;                    		// ANT 4hz timer.

static user_profile_t 					m_user_profile;
static rc_sim_forces_t					m_sim_forces;

static uint16_t							m_ant_ctrl_remote_ser_no; 					// Serial number of remote if connected.

static void profile_update_sched_handler(void *p_event_data, uint16_t event_size);

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
	// Process storage events.
	pstorage_sys_event_handler(sys_evt);
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
		// Schedule an update to persist the profile to flash.
		app_sched_event_put(NULL, 0, profile_update_sched_handler);
	}

	// Call speed module to set the wheel size.
	set_wheel_size(m_user_profile.wheel_size_mm);
}

// Parses the SET_SIM message from the KICKR and has user profile info.
static void set_sim_params(uint8_t *pBuffer)
{
	// Weight comes through in KG as 8500 85.00kg for example.
	float weight = (pBuffer[0] | pBuffer[1] << 8u) / 100.0f;

	if (m_user_profile.total_weight_kg != weight)
	{
		m_user_profile.total_weight_kg = weight;

		// Schedule an update to persist the profile to flash.
		app_sched_event_put(NULL, 0, profile_update_sched_handler);
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
		uint32_t err_code;

		err_code = user_profile_init();
		APP_ERROR_CHECK(err_code);

		// Initialize hard coded user profile for now.
		memset(&m_user_profile, 0, sizeof(m_user_profile));
		
		// Initialize simulation forces structure.
		memset(&m_sim_forces, 0, sizeof(m_sim_forces));

		// Attempt to load stored profile from flash.
		err_code = user_profile_load(&m_user_profile);
		APP_ERROR_CHECK(err_code);

		if (m_user_profile.wheel_size_mm == 0 ||
				m_user_profile.wheel_size_mm == 0xFFFF)
		{
			// Wheel circumference in mm.
			m_user_profile.wheel_size_mm = DEFAULT_WHEEL_SIZE_MM;
		}
		
		if (isnan(m_user_profile.total_weight_kg))
		{
			// Total weight of rider + bike + shoes, clothing, etc...
			m_user_profile.total_weight_kg = DEFAULT_TOTAL_WEIGHT_KG;
		}
		
	 /*	fCrr is the coefficient of rolling resistance (unitless). Default value is 0.004. 
	 *
	 *	fC is equal to A*Cw*Rho where A is effective frontal area (m^2); Cw is drag 
	 *	coefficent (unitless); and Rho is the air density (kg/m^3). The default value 
	 *	for A*Cw*Rho is 0.60. 	
	 */
		m_sim_forces.crr = SIM_CRR;
		m_sim_forces.c = SIM_C;
}

/**@brief Persists any updates the user profile. */
static void profile_update(void)
{
	uint32_t err_code;

	// This method ensures the device is in a proper state in order to update
	// the profile.
	err_code = user_profile_store(&m_user_profile);
	APP_ERROR_CHECK(err_code);
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
			m_servo_pos = resistance_erg_set(
					power_meas.instant_speed_mps,
					m_user_profile.total_weight_kg,
					m_servo_pos,
					&m_sim_forces);
			break;

		case RESISTANCE_SET_SIM:
			m_servo_pos = resistance_sim_set(
					power_meas.instant_speed_mps,
					m_user_profile.total_weight_kg,
					m_servo_pos,
					&m_sim_forces);
			break;

		default:
			break;
	}
}

/**@brief Function for handling profile update.
 *
 * @details This function will be called by the scheduler to update the profile.
 * 			Writing to flash causes the radio to stop, so we don't want to do this
 * 			too often, so we let the scheduler decide when.
 */
static void profile_update_sched_handler(void *p_event_data, uint16_t event_size)
{
	UNUSED_PARAMETER(p_event_data);
	UNUSED_PARAMETER(event_size);
	profile_update();
}

/*----------------------------------------------------------------------------
 * Timer functions
 * ----------------------------------------------------------------------------*/

/**@brief Function for handling the ANT 4hz measurement timer timeout.
 *
 * @details This function will be called each timer expiration.
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void ant_4hz_timeout_handler(void * p_context)
{
	UNUSED_PARAMETER(p_context);
	uint32_t err_code;
	irt_power_meas_t* p_power_meas_current 		= NULL;
	irt_power_meas_t* p_power_meas_first 		= NULL;
		
	// Get pointers to the event structures.
	err_code = irt_power_meas_fifo_op(&p_power_meas_first, &p_power_meas_current);
	APP_ERROR_CHECK(err_code);

	// Set current resistance state.
	p_power_meas_current->resistance_mode = m_resistance_mode;
	p_power_meas_current->resistance_level = m_resistance_level;
	p_power_meas_current->servo_position = m_servo_pos;

	// Get current temperature.
	p_power_meas_current->temp = temperature_read();

	// Calculate the power.
	err_code = power_measure(m_user_profile.total_weight_kg, p_power_meas_current);
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

	// Send remote control availability.  Notification flag is 1 if a serial number
	// is connected, 0 if none are connected.
	ant_ctrl_device_avail_tx((uint8_t) (m_ant_ctrl_remote_ser_no != 0));
}

/**@brief Function for starting the application timers.
 */
static void application_timers_start(void)
{
    uint32_t err_code;

    // Start application timers
    err_code = app_timer_start(m_ant_4hz_timer_id, ANT_4HZ_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

static void application_timers_stop(void)
{
	uint32_t err_code;
		
	err_code = app_timer_stop(m_ant_4hz_timer_id);
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

    err_code = app_timer_create(&m_ant_4hz_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                ant_4hz_timeout_handler);
    
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

// TODO: This event should be registered for a callback when it's time to
// power the system down.
static void on_power_down(void)
{
	// Free heap allocation.
	irt_power_meas_fifo_free();

	// Blink red a couple of times to say good-night.
	clear_led();

	// Shut the system down.
	sd_power_system_off();
}

static void on_button_i(void)
{
	m_resistance_mode = RESISTANCE_SET_STANDARD;
	m_resistance_level = 0;
	m_servo_pos = resistance_level_set(m_resistance_level);
	ant_bp_resistance_tx_send(m_resistance_mode, &m_resistance_level);
}

static void on_button_ii(void)
{
	// decrement
	if (m_resistance_mode == RESISTANCE_SET_STANDARD &&
			m_resistance_level > 0)
	{
		m_servo_pos = resistance_level_set(--m_resistance_level);
		ant_bp_resistance_tx_send(m_resistance_mode, &m_resistance_level);
	}
	else if (m_resistance_mode == RESISTANCE_SET_ERG &&
			m_sim_forces.erg_watts > 100u)
	{
		// Decrement by 15 watts;
		m_sim_forces.erg_watts -= 15u;
		ant_bp_resistance_tx_send(m_resistance_mode, &m_sim_forces.erg_watts);
	}
}

static void on_button_iii(void)
{
	// increment
	if (m_resistance_mode == RESISTANCE_SET_STANDARD &&
			m_resistance_level < (MAX_RESISTANCE_LEVELS-1))
	{
		m_servo_pos = resistance_level_set(++m_resistance_level);
		ant_bp_resistance_tx_send(m_resistance_mode, &m_resistance_level);
	}
	else if (m_resistance_mode == RESISTANCE_SET_ERG)
	{
		// Increment by 15 watts;
		m_sim_forces.erg_watts += 15u;
		ant_bp_resistance_tx_send(m_resistance_mode, &m_sim_forces.erg_watts);
	}
}

static void on_button_iv(void)
{
	m_resistance_mode = RESISTANCE_SET_STANDARD;
	m_resistance_level = MAX_RESISTANCE_LEVELS-1;
	m_servo_pos = resistance_level_set(m_resistance_level);
	ant_bp_resistance_tx_send(m_resistance_mode, &m_resistance_level);
}

static void on_button_menu(void)
{
	// Toggle between erg mode.
	if (m_resistance_mode == RESISTANCE_SET_STANDARD)
	{
		m_resistance_mode = RESISTANCE_SET_ERG;
		if (m_sim_forces.erg_watts == 0)
		{
			m_sim_forces.erg_watts = DEFAULT_ERG_WATTS;
		}
		ant_bp_resistance_tx_send(m_resistance_mode, &m_sim_forces.erg_watts);
	}
	else
	{
		m_resistance_mode = RESISTANCE_SET_STANDARD;
		on_button_i();
	}
}

// This event is triggered when there is data to be read from accelerometer.
static void on_accelerometer(void)
{
#if defined(BLE_NUS_ENABLED)
	uint32_t err_code;
	accelerometer_data_t data;

	// Read event data from the accelerometer.
	err_code = accelerometer_data(&data);
	APP_ERROR_CHECK(err_code);

	// TODO: Remove the hard coding here.
	if (data.source & 0x04)
	{
		// Event should contain data.
		static const char format[] = "ACL:%i,%i";
		char message[16];
		memset(&message, 0, sizeof(message));
		uint8_t length = sprintf(message, format,
				data.out_y_lsb,
				data.out_y_msb);
		debug_send(message, sizeof(message));
	}
#endif
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

// TODO: implement this behavior, reopen the channel, etc...?
static void on_ant_channel_closed(void) {}
// TODO: This is to handle when we pair to a power meter.
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
			m_servo_pos = resistance_level_set(m_resistance_level);
			break;
			
		case RESISTANCE_SET_PERCENT:
			// Reset state since we're not in standard mode any more.
			m_resistance_level = 0;
			m_resistance_mode = RESISTANCE_SET_PERCENT;
			
			// Parse the buffer for percentage.
			float percent = wahoo_resistance_pct_decode(rc_evt.pBuffer);
			m_servo_pos = resistance_pct_set(percent);
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
			m_sim_forces.grade = wahoo_sim_grade_decode(rc_evt.pBuffer);
			break;
			
		case RESISTANCE_SET_WIND:
			m_resistance_mode = RESISTANCE_SET_SIM;
			// Parse out the wind speed.
			m_sim_forces.wind_speed_mps = wahoo_sim_wind_decode(rc_evt.pBuffer);
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
	// Remote can transmit no serial number as 0xFFFF, in which case
	// we can't bond specifically, so we'll just process commands.
	if (m_ant_ctrl_remote_ser_no == 0 && evt.remote_serial_no != 0xFFFF)
	{
		// Track the remote we're now connected to.
		m_ant_ctrl_remote_ser_no = evt.remote_serial_no;
	}
	else if (m_ant_ctrl_remote_ser_no != evt.remote_serial_no)
	{
		// If already connected to a remote, don't process another's commands.
		return;
	}

	switch (evt.command)
	{
		case ANT_CTRL_BUTTON_UP:
			// Increment resistance.
			on_button_iii(); 
			break;

		case ANT_CTRL_BUTTON_DOWN:
			// Decrement resistance.
			on_button_ii();
			break;

		case ANT_CTRL_BUTTON_LONG_UP:
			// Set full resistance
			on_button_iv();
			break;

		case ANT_CTRL_BUTTON_LONG_DOWN:
			// Turn off resistance
			on_button_i();
			break;

		case ANT_CTRL_BUTTON_MIDDLE:
			on_button_menu();
			break;

		case ANT_CTRL_BUTTON_LONG_MIDDLE:
			on_power_down();
			break;

		default:
			break;
	}
}


/*----------------------------------------------------------------------------
 * Main program functions
 * ----------------------------------------------------------------------------*/

/**@brief Application main function.
 */
int main(void)
{
	uint32_t err_code;

	// Initialize default remote serial number.
	m_ant_ctrl_remote_ser_no = 0;

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

    // Subscribe for system events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);

	// Initialize the scheduler.
	scheduler_init();

	// Begin advertising and receiving ANT messages.
	ble_ant_start();

	// Start off with resistance at 0.
	resistance_level_set(m_resistance_level);
	m_servo_pos = RESISTANCE_LEVEL[m_resistance_level];

	// Initialize module to read speed from flywheel.
	init_speed(PIN_FLYWHEEL);

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
