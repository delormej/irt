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
#include "wahoo.h"
#include "ble_ant.h"
#include "ant_bike_power.h"
#include "nrf_delay.h"
#include "ant_ctrl.h"
#include "app_timer.h"
#include "ble_debug_assert_handler.h"
#include "debug.h"
#include "boards.h"
#include "battery.h"

#define ANT_4HZ_INTERVAL				APP_TIMER_TICKS(250, APP_TIMER_PRESCALER)  // Remote control & bike power sent at 4hz.
#define BLE_ADV_BLINK_RATE_MS			500u
#define SCHED_QUEUE_SIZE                16                                          /**< Maximum number of events in the scheduler queue. */
#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE,\
                                            BLE_STACK_HANDLER_SCHED_EVT_SIZE)       /**< Maximum size of scheduler events. */
//
// Default profile and simulation values.
//
#define DEFAULT_WHEEL_SIZE_MM			2096ul										// Default wheel size.
#define DEFAULT_TOTAL_WEIGHT_KG			8180ul 										// Default weight (convert 180.0lbs to KG).
#define DEFAULT_ERG_WATTS				175u										// Default erg target_watts when not otherwise set.
#define DEFAULT_SETTINGS				0ul											// Default 32bit field of settings.
#define SIM_CRR							0.0033f										// Default crr for typical outdoor rolling resistance (not the same as above).
#define SIM_C							0.60f										// Default co-efficient for drag.  See resistance sim methods.

#define REQUEST_RETRY					12ul										// Number of times to retry sending broadcast data requests.

static uint8_t 							m_resistance_level;
static resistance_mode_t				m_resistance_mode;

static app_timer_id_t               	m_ant_4hz_timer_id;                    		// ANT 4hz timer.  TOOD: should rename since it's the core timer for all reporting (not just ANT).

static user_profile_t 					m_user_profile;
static rc_sim_forces_t					m_sim_forces;
static accelerometer_data_t 			m_accelerometer_data;

static uint16_t							m_ant_ctrl_remote_ser_no; 					// Serial number of remote if connected.

static bool								m_crr_adjust_mode;							// Indicator that we're in manual calibration mode.

// Type declarations for profile updating.
static void profile_update_sched_handler(void *p_event_data, uint16_t event_size);
static void profile_update_sched(void);

/**@brief Debug logging for main module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define LOG debug_log
#else
#define LOG(...)
#endif // ENABLE_DEBUG_LOG

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
	if (error_code == NRF_ERROR_NO_MEM)
	{
		LOG("[MAIN]:app_error_handler CRITICAL: NRF_ERROR_NO_MEM in %s:{%lu}. \r\n",
				p_file_name,
				line_num);
	}
	else
	{
		LOG("[MAIN]:app_error_handler {HALTED ON ERRROR: %#.8x}: %s:%lu\r\n",
				error_code, p_file_name, line_num);
	}

    // This call can be used for debug purposes during development of an application.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care.
#if defined(ENABLE_DEBUG_ASSERT)
	set_led_red(3);

	// Note this function does not return - it will hang waiting for a debugger to attach.
	ble_debug_assert_handler(error_code, line_num, p_file_name);
#else
    // On assert, the system can only recover with a reset.
	LOG("[MAIN]:app_error_handler performing SystemReset()");
    NVIC_SystemReset();
#endif // ENABLE_DEBUG_ASSERT
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
	wheel_size = uint16_decode(pBuffer) / 10;

	if (m_user_profile.wheel_size_mm != wheel_size)
	{
		LOG("[MAIN]:set_wheel_params {wheel:%lu}\r\n", wheel_size);

		m_user_profile.wheel_size_mm = wheel_size;
		// Schedule an update to persist the profile to flash.
		profile_update_sched();

		// Call speed module to update the wheel size.
		speed_wheel_size_set(m_user_profile.wheel_size_mm);
	}
}

/**@brief	Parses the SET_SIM message from the KICKR and has user profile info.
 * 			Reinitializes the power module if weight changed.
 */
static void set_sim_params(uint8_t *pBuffer)
{
	// Weight comes through in KG as 8500 85.00kg for example.
	float c, crr;
	uint16_t weight;

	weight = uint16_decode(pBuffer);

	if (m_user_profile.total_weight_kg != weight)
	{
		m_user_profile.total_weight_kg = weight;

		// Schedule an update to persist the profile to flash.
		profile_update_sched();

		// Re-initialize the power module with updated weight.
		power_init(&m_user_profile);
	}

	// Co-efficient for rolling resistance.
	crr = wahoo_decode_crr(&(pBuffer[2]));
	// Co-efficient of drag.
	c = wahoo_decode_c(&(pBuffer[4]));

	// Store and just precision if new values came across.
	if (crr > 0.0f)
		m_sim_forces.crr = crr;
	if (c > 0.0f)
		m_sim_forces.c = c;

	LOG("[MAIN]:set_sim_params {weight:%.2f, crr:%i, c:%i}\r\n",
		(m_user_profile.total_weight_kg / 100.0f),
		(uint16_t)(m_sim_forces.crr * 10000),
		(uint16_t)(m_sim_forces.c * 1000) );
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

		//
		// Check the version of the profile, if it's not the current version
		// set the default parameters.
		//
		if (m_user_profile.version != PROFILE_VERSION)
		{
			LOG("[MAIN]: Older profile version %i. Loading defaults and upgrading\r\n.",
					m_user_profile.version);
			m_user_profile.version			= PROFILE_VERSION;
			m_user_profile.wheel_size_mm 	= DEFAULT_WHEEL_SIZE_MM;
			m_user_profile.total_weight_kg 	= DEFAULT_TOTAL_WEIGHT_KG;
			m_user_profile.settings 		= DEFAULT_SETTINGS;

			// Schedule an update.
			profile_update_sched();
		}
		else
		{
			if (m_user_profile.wheel_size_mm == 0 ||
					m_user_profile.wheel_size_mm == 0xFFFF)
			{
				// Wheel circumference in mm.
				m_user_profile.wheel_size_mm = DEFAULT_WHEEL_SIZE_MM;
			}

			if (m_user_profile.total_weight_kg == 0 ||
					m_user_profile.total_weight_kg == 0xFFFF)
			{
				LOG("[MAIN]:profile_init using default weight.");

				// Total weight of rider + bike + shoes, clothing, etc...
				m_user_profile.total_weight_kg = DEFAULT_TOTAL_WEIGHT_KG;
			}

			if (m_user_profile.ca_slope == 0xFFFF)
			{
				m_user_profile.ca_slope = DEFAULT_CRR;
			}
		}

	 /*	fCrr is the coefficient of rolling resistance (unitless). Default value is 0.004. 
	 *
	 *	fC is equal to A*Cw*Rho where A is effective frontal area (m^2); Cw is drag 
	 *	coefficent (unitless); and Rho is the air density (kg/m^3). The default value 
	 *	for A*Cw*Rho is 0.60. 	
	 */
		m_sim_forces.crr = SIM_CRR;
		m_sim_forces.c = SIM_C;

		LOG("[MAIN]:profile_init:\r\n\t weight: %i \r\n\t wheel: %i \r\n\t settings: %lu \r\n\t ca_slope: %i \r\n\t ca_intercept: %i \r\n",
				m_user_profile.total_weight_kg,
				m_user_profile.wheel_size_mm,
				m_user_profile.settings,
				m_user_profile.ca_slope,
				m_user_profile.ca_intercept);
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

	uint32_t err_code;

	// This method ensures the device is in a proper state in order to update
	// the profile.
	err_code = user_profile_store(&m_user_profile);
	APP_ERROR_CHECK(err_code);

	LOG("[MAIN]:profile_update:\r\n\t weight: %i \r\n\t wheel: %i \r\n\t settings: %lu \r\n\t slope: %i \r\n\t intercept: %i \r\n",
			m_user_profile.total_weight_kg,
			m_user_profile.wheel_size_mm,
			m_user_profile.settings,
			m_user_profile.ca_slope,
			m_user_profile.ca_intercept);
}

/**@brief Schedules an update to the profile.  See notes above in handler.
 *
 */
static void profile_update_sched(void)
{
	app_sched_event_put(NULL, 0, profile_update_sched_handler);
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
	static uint8_t event_count;
	UNUSED_PARAMETER(p_context);
	uint32_t err_code;
	float rr_force;	// Calculated rolling resistance force.
	irt_power_meas_t* p_power_meas_current 		= NULL;
	irt_power_meas_t* p_power_meas_first 		= NULL;
	irt_power_meas_t* p_power_meas_last 		= NULL;

	// Maintain a static event count, we don't do everything each time.
	event_count++;

	// Get pointers to the event structures.
	p_power_meas_current = irt_power_meas_fifo_next();
	p_power_meas_last = irt_power_meas_fifo_last();

	// Set current resistance state.
	p_power_meas_current->resistance_mode = m_resistance_mode;

	switch (m_resistance_mode)
	{
		case RESISTANCE_SET_STANDARD:
			p_power_meas_current->resistance_level = m_resistance_level;
			break;
		case RESISTANCE_SET_ERG: // TOOD: not sure why we're falling through here with SIM?
		case RESISTANCE_SET_SIM:
			p_power_meas_current->resistance_level = (uint16_t)(m_sim_forces.erg_watts);
			break;
		default:
			break;
	}

	p_power_meas_current->servo_position = resistance_position_get();

	// Every 32 seconds.
	if (event_count % 128 == 0)
	{
		// Get current temperature.
		p_power_meas_current->temp = temperature_read();
	}
	else
	{
		p_power_meas_current->temp = p_power_meas_last->temp;
	}

	// Report on accelerometer data.
	if (m_accelerometer_data.source & ACCELEROMETER_SRC_FF_MT)
	{
		p_power_meas_current->accel_y_lsb = m_accelerometer_data.out_y_lsb;
		p_power_meas_current->accel_y_msb = m_accelerometer_data.out_y_msb;

		// Clear the source now that we've read it.
		m_accelerometer_data.source = 0;
	}

	// Record event time.
	p_power_meas_current->event_time_2048 = seconds_2048_get();
	//LOG("[MAIN] event_time: %i\r\n", p_power_meas_current->event_time_2048);

	// Calculate speed.
	err_code = speed_calc(p_power_meas_current, p_power_meas_last);
	APP_ERROR_CHECK(err_code);

	// Calculate power and get back the rolling resistance force for use in adjusting resistance.
	err_code = power_calc(p_power_meas_current, p_power_meas_last, &rr_force);
	APP_ERROR_CHECK(err_code);

	// TODO: this should return an err_code.
	// Transmit the power message.
	cycling_power_send(p_power_meas_current);

	// If in erg or sim mode, adjusts the resistance.
	if (m_resistance_mode == RESISTANCE_SET_ERG || m_resistance_mode == RESISTANCE_SET_SIM)
	{
		// Twice per second adjust resistance.
		if (event_count % 2 == 0)
		{
			// Use the oldest record we have to average with.
			p_power_meas_first = irt_power_meas_fifo_first();
			resistance_adjust(p_power_meas_first, p_power_meas_current, &m_sim_forces,
					m_resistance_mode, rr_force);
		}
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

/*//TODO: Currently not used, should it be used on sleep?
static void application_timers_stop(void)
{
	uint32_t err_code;
		
	err_code = app_timer_stop(m_ant_4hz_timer_id);
    APP_ERROR_CHECK(err_code);
} */

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


/**@brief	Sends data page 2 response.
 */
static void send_data_page2(uint8_t subpage)
{
	uint8_t response[6];

	memset(&response, 0, sizeof(response));

	switch (subpage)
	{
		case IRT_MSG_SUBPAGE_SETTINGS:
			memcpy(&response, &m_user_profile.settings, sizeof(uint32_t));
			break;

		case IRT_MSG_SUBPAGE_CRR:
			memcpy(&response, &m_user_profile.ca_slope, sizeof(uint16_t));
			memcpy(&response[2], &m_user_profile.ca_intercept, sizeof(uint16_t));
			break;

		case IRT_MSG_SUBPAGE_WEIGHT:
			memcpy(&response, &m_user_profile.total_weight_kg, sizeof(uint16_t));
			break;

		case IRT_MSG_SUBPAGE_WHEEL_SIZE:
			memcpy(&response, &m_user_profile.wheel_size_mm, sizeof(uint16_t));
			break;

		default:
			LOG("[MAIN] Unrecognized page request. \r\n");
			return;
	}

	// # of times to send message - byte 5, bits 0:6 - just hard coding for now.
	// Number of times to send.
	for (uint8_t i = 0; i < REQUEST_RETRY; i++)
	{
		ant_bp_page2_tx_send(subpage, response, 0);
	}
}

/*----------------------------------------------------------------------------
 * Event handlers
 * ----------------------------------------------------------------------------*/

// TODO: This event should be registered for a callback when it's time to
// power the system down.
static void on_power_down(bool accelerometer_wake_disable)
{
	LOG("[MAIN]:on_power_down \r\n");

	// Free heap allocation.
	irt_power_meas_fifo_free();

	// Blink red a couple of times to say good-night.
	clear_led(0);

	peripheral_powerdown(accelerometer_wake_disable);

	// TODO: should we be gracefully closing ANT and BLE channels here?

	// Shut the system down.
	sd_power_system_off();
}

static void on_resistance_off(void)
{
	m_resistance_mode = RESISTANCE_SET_STANDARD;
	m_resistance_level = 0;
	resistance_level_set(m_resistance_level);
	ble_ant_resistance_ack(m_resistance_mode, m_resistance_level);
}

static void on_resistance_dec(void)
{
	// decrement
	if (m_resistance_mode == RESISTANCE_SET_STANDARD &&
			m_resistance_level > 0)
	{
		resistance_level_set(--m_resistance_level);
		ble_ant_resistance_ack(m_resistance_mode, m_resistance_level);
	}
	else if (m_resistance_mode == RESISTANCE_SET_ERG &&
			m_sim_forces.erg_watts > 50u)
	{
		// Decrement by 15 watts;
		m_sim_forces.erg_watts -= 15u;
		ble_ant_resistance_ack(m_resistance_mode, m_sim_forces.erg_watts);
	}
}

static void on_resistance_inc(void)
{
	// increment
	if (m_resistance_mode == RESISTANCE_SET_STANDARD &&
			m_resistance_level < (MAX_RESISTANCE_LEVELS-1))
	{
		resistance_level_set(++m_resistance_level);
		ble_ant_resistance_ack(m_resistance_mode, m_resistance_level);
	}
	else if (m_resistance_mode == RESISTANCE_SET_ERG)
	{
		// Increment by 15 watts;
		m_sim_forces.erg_watts += 15u;
		ble_ant_resistance_ack(m_resistance_mode, m_sim_forces.erg_watts);
	}
}

static void on_resistance_max(void)
{
	m_resistance_mode = RESISTANCE_SET_STANDARD;
	m_resistance_level = MAX_RESISTANCE_LEVELS-1;
	resistance_level_set(m_resistance_level);
	ble_ant_resistance_ack(m_resistance_mode, m_resistance_level);
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
		ble_ant_resistance_ack(m_resistance_mode, m_sim_forces.erg_watts);
	}
	else
	{
		m_resistance_mode = RESISTANCE_SET_STANDARD;
		on_resistance_off();
	}
}

// This is the button on the board.
static void on_button_pbsw(void)
{
	// TODO: this button needs to be debounced and a LONG press should power down.
	LOG("[MAIN] Push button switch pressed.\r\n");
	// Shutting device down.
	on_power_down(true);
}

// This event is triggered when there is data to be read from accelerometer.
static void on_accelerometer(void)
{
	uint32_t err_code;

	// Clear the struct.
	memset(&m_accelerometer_data, 0, sizeof(m_accelerometer_data));

	// Read event data from the accelerometer.
	err_code = accelerometer_data_get(&m_accelerometer_data);
	APP_ERROR_CHECK(err_code);

	LOG("[MAIN]:on_accelerometer source:%i y:%i \r\n",
			m_accelerometer_data.source,
			m_accelerometer_data.out_y_lsb |=
					m_accelerometer_data.out_y_msb << 8);

	// Received a sleep interrupt from the accelerometer meaning no motion for a while.
	if (m_accelerometer_data.source == ACCELEROMETER_SRC_WAKE_SLEEP)
	{
		LOG("[MAIN]:about to power down from accelerometer sleep.\r\n");;
		//
		// This is a workaround to deal with GPIOTE toggling the SENSE bits which forces
		// the device to wake up immediately after going to sleep without this.
		//
        NRF_GPIO->PIN_CNF[PIN_SHAKE] &= ~GPIO_PIN_CNF_SENSE_Msk;
        NRF_GPIO->PIN_CNF[PIN_SHAKE] |= GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos;

        if (!(FEATURE_IS_SET(m_user_profile.settings, FEATURE_ACCEL_SLEEP_OFF)))
        {
        	on_power_down(false);
        }
	}
}

// Called when the power adapter is plugged in.
static void on_power_plug(void)
{
	LOG("[MAIN] Power plugged in.\r\n");
}

static void on_ble_connected(void) 
{
	blink_led_green_stop(0);
	set_led_green(0);

	LOG("[MAIN]:on_ble_connected\r\n");
}
	
static void on_ble_disconnected(void) 
{
	// Clear connection LED.
	clear_led(0);
	
	LOG("[MAIN]:on_ble_disconnected\r\n");

	// Restart advertising.
	ble_advertising_start();
}

static void on_ble_timeout(void) 
{
	blink_led_green_stop(0);
	LOG("[MAIN]:on_ble_timeout\r\n");
}

static void on_ble_advertising(void)
{
	blink_led_green_start(0, BLE_ADV_BLINK_RATE_MS);
}

static void on_ble_uart(uint8_t * data, uint16_t length)
{
	LOG("[MAIN]:on_ble_uart data: %*.*s\r\n", length, length, data);
}

/*@brief	Event handler that the cycling power service calls when a set resistance
 *				command is received.
 *
 */
static void on_set_resistance(rc_evt_t rc_evt)
{
	LOG("[MAIN]:on_set_resistance {OP:%#.2x,VAL:%i}\r\n",
			(uint8_t)rc_evt.operation,
			uint16_decode(rc_evt.pBuffer));
	// 
	// Parse the messages and set state or resistance as appropriate.
	//
	switch (rc_evt.operation)
	{
		case RESISTANCE_SET_STANDARD:
			m_resistance_mode = RESISTANCE_SET_STANDARD;
			m_resistance_level = rc_evt.pBuffer[0];
			resistance_level_set(m_resistance_level);
			break;
			
		case RESISTANCE_SET_PERCENT:
			// Reset state since we're not in standard mode any more.
			m_resistance_level = 0;
			m_resistance_mode = RESISTANCE_SET_PERCENT;
			
			// Parse the buffer for percentage.
			float percent = wahoo_resistance_pct_decode(rc_evt.pBuffer);
			resistance_pct_set(percent);
			break;

		case RESISTANCE_SET_ERG:
			// Assign target watt level.
			m_resistance_mode = RESISTANCE_SET_ERG;
			m_sim_forces.erg_watts = uint16_decode(rc_evt.pBuffer);
			break;

		// This message never seems to come via the apps?
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
			LOG("[MAIN]:on_set_resistance: set wind_speed_mps %i\r\n",
					(uint16_t)(m_sim_forces.wind_speed_mps * 1000));
			break;
			
		case RESISTANCE_SET_WHEEL_CR:
			//  Parse wheel size and update accordingly.
			set_wheel_params(rc_evt.pBuffer);
			break;
			
		case RESISTANCE_SET_BIKE_TYPE:
			m_resistance_mode = RESISTANCE_SET_SIM;
			m_sim_forces.crr = wahoo_decode_crr(rc_evt.pBuffer);
			break;

		case RESISTANCE_SET_C:
			m_resistance_mode = RESISTANCE_SET_SIM;
			m_sim_forces.c = wahoo_decode_c(rc_evt.pBuffer);
			break;

		case RESISTANCE_SET_SERVO_POS:
			// Move the servo to a specific position.
			resistance_position_set(uint16_decode(rc_evt.pBuffer));
			break;

		case RESISTANCE_SET_WEIGHT:
			// Set sim params without changing the mode.
			set_sim_params(rc_evt.pBuffer);
			break;

		default:
			break;
	}

	// Send acknowledgment.
	ble_ant_resistance_ack(m_resistance_mode, (int16_t)*rc_evt.pBuffer);
}

// Invoked when a button is pushed on the remote control.
static void on_ant_ctrl_command(ctrl_evt_t evt)
{
	// Remote can transmit no serial number as 0xFFFF, in which case
	// we can't bond specifically, so we'll just process commands.
	if (evt.remote_serial_no != 0xFFFF)
	{
		if (m_ant_ctrl_remote_ser_no == 0)
		{
			// Track the remote we're now connected to.
			m_ant_ctrl_remote_ser_no = evt.remote_serial_no;

			LOG("[MAIN]:on_ant_ctrl_command Remote {serial no:%i} now connected.\r\n",
					evt.remote_serial_no);
		}
		else if (m_ant_ctrl_remote_ser_no != evt.remote_serial_no)
		{
			// If already connected to a remote, don't process another's commands.
			LOG("[MAIN]:on_ant_ctrl_command received command from another serial no: %i\r\n",
					evt.remote_serial_no);
			return;
		}
	}

	LOG("[MAIN]:on_ant_ctrl_command Command: %i\r\n", evt.command);

	switch (evt.command)
	{
		case ANT_CTRL_BUTTON_UP:
			if (m_crr_adjust_mode)
			{
				m_user_profile.ca_slope += 50;
				power_init(&m_user_profile);
				send_data_page2(IRT_MSG_SUBPAGE_CRR);
			}
			else
			{
				// Increment resistance.
				on_resistance_inc();
			}
			break;

		case ANT_CTRL_BUTTON_DOWN:
			if (m_crr_adjust_mode)
			{
				if (m_user_profile.ca_slope > 50)
				{
					m_user_profile.ca_slope -= 50;
					power_init(&m_user_profile);
					send_data_page2(IRT_MSG_SUBPAGE_CRR);
				}
			}
			else
			{
				// Decrement resistance.
				on_resistance_dec();
			}
			break;

		case ANT_CTRL_BUTTON_LONG_UP:
			// Set full resistance
			on_resistance_max();
			break;

		case ANT_CTRL_BUTTON_LONG_DOWN:
			// Turn off resistance
			on_resistance_off();
			break;

		case ANT_CTRL_BUTTON_MIDDLE:
			if (m_crr_adjust_mode)
			{
				// Exit crr mode.
				m_crr_adjust_mode = false;
				clear_led(2);
			}
			else
			{
				on_button_menu();
			}
			break;

		case ANT_CTRL_BUTTON_LONG_MIDDLE:
			// Requires double long push of middle to shut down.
			if (m_crr_adjust_mode)
			{
				// Force a hard power down, accelerometer will not wake.
				on_power_down(true);
			}
			else
			{
				// Change into crr mode.
				// Set the state, start blinking the LED RED
				m_crr_adjust_mode = true;
				set_led_red(2);
			}

			break;

		default:
			break;
	}
}

// Called when instructed to enable device firmware update mode.
static void on_enable_dfu_mode(void)
{
	uint32_t err_code;

	LOG("[MAIN]:Enabling DFU mode.\n\r");

	// TODO: share the mask here in a common include file with bootloader.
	// bootloader needs to share PWM, device name / manuf, etc... so we need
	// to refactor anyways.
	err_code = sd_power_gpregret_set(0x1);
	APP_ERROR_CHECK(err_code);

	// Resets the CPU to start in DFU mode.
	NVIC_SystemReset();
}

/**@brief	Device receives page (0x46) requesting data page.
 */
static void on_request_data(uint8_t* buffer)
{
	uint8_t subpage;
	subpage = (uint8_t)buffer[3];
	LOG("[MAIN] Request to get data page (subpage): %#x\r\n", subpage);
	LOG("[MAIN]:request data message [%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\r\n",
			buffer[0],
			buffer[1],
			buffer[2],
			buffer[3],
			buffer[4],
			buffer[5],
			buffer[6],
			buffer[7]);

	// TODO: just a quick hack for right now for getting battery info.
	if (subpage == ANT_PAGE_BATTERY_STATUS)
	{
		LOG("[MAIN] Requested battery status. \r\n");
		battery_read_start();
		return;
	}

	// Process and send response.
	send_data_page2(subpage);
}

/**@brief	Device receives page (0x02) with values to set.
 */
static void on_set_parameter(uint8_t* buffer)
{
	LOG("[MAIN]:set param message [%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\r\n",
			buffer[0],
			buffer[1],
			buffer[2],
			buffer[3],
			buffer[4],
			buffer[5],
			buffer[6],
			buffer[7]);

	// SubPage index
	switch (buffer[IRT_MSG_PAGE2_SUBPAGE_INDEX])
	{
		case IRT_MSG_SUBPAGE_SETTINGS:
			// The actual settings are a 32 bit int stored in bytes [2:5] IRT_MSG_PAGE2_DATA_INDEX
			// Need to use memcpy, Cortex-M proc doesn't support unaligned casting of 32bit int.
			memcpy(&m_user_profile.settings, &buffer[IRT_MSG_PAGE2_DATA_INDEX],
					sizeof(uint32_t));

			LOG("[MAIN] Request to update settings to: ACCEL:%i, BIG_MAG:%i, EXTRA_INFO:%i, TEST:%i \r\n",
						FEATURE_IS_SET(m_user_profile.settings, FEATURE_ACCEL_SLEEP_OFF),
						FEATURE_IS_SET(m_user_profile.settings, FEATURE_BIG_MAG),
						FEATURE_IS_SET(m_user_profile.settings, FEATURE_ANT_EXTRA_INFO),
						FEATURE_IS_SET(m_user_profile.settings, FEATURE_TEST));
			// Schedule update to the profile.
			profile_update_sched();
			break;

		case IRT_MSG_SUBPAGE_CRR:
			// Update CRR, note this doesn't update the profile in flash.
			memcpy(&m_user_profile.ca_slope, &buffer[IRT_MSG_PAGE2_DATA_INDEX],
					sizeof(uint16_t));
			memcpy(&m_user_profile.ca_intercept, &buffer[IRT_MSG_PAGE2_DATA_INDEX+2],
					sizeof(uint16_t));
			LOG("[MAIN] Updated slope:%i intercept:%i \r\n",
					m_user_profile.ca_slope, m_user_profile.ca_intercept);
			// Reinitialize power.
			power_init(&m_user_profile);
			break;

#ifdef USE_BATTERY
		case IRT_MSG_SUBPAGE_SET_CHARGER:
			// TODO: This shouldn't be sent this way it should really be sent using
			// Common Data Page 72: Command Burst
			battery_charge_set( (BATTERY_CHARGE_OFF) );
			LOG("[MAIN] Toggling battery charger on/off. \r\n");

			break;
#endif
		default:
			LOG("[MAIN] on_set_parameter: Invalid setting, skipping. \r\n");
			return;
	}
}

/**@brief	Called when the result of the battery is determined.
 *
 */
static void on_battery_result(uint16_t battery_level)
{
	LOG("[MAIN] on_battery_result %i \r\n", battery_level);

	// TODO: temporarily sending page 2, need to send page 0x52.
	// Number of times to send.
	for (uint8_t i = 0; i < REQUEST_RETRY; i++)
	{
		ant_bp_page2_tx_send(0x52, (uint8_t*)&battery_level, 0);
	}
}

/**@brief	Configures power supervisor to warn and reset if power drops too low.
 */
static void config_power_supervisor()
{
	// Forces a reset if power drops below 2.7v.
	NRF_POWER->POFCON = POWER_POFCON_POF_Enabled | POWER_POFCON_THRESHOLD_V27;
}

/**@brief	Check if there is a reset reason and log if enabled.
 */
static uint32_t check_reset_reason()
{
	uint32_t reason;

	// Read the reset reason
	reason = NRF_POWER->RESETREAS;
	if (reason > 0)
	{
		// Clear the reason by writing 1 bits.
		NRF_POWER->RESETREAS = reason;
		LOG("[MAIN]:Reset reason: %#08lx\r\n", reason);
	}
	else
	{
		LOG("[MAIN]:Normal power on.\r\n");
	}

	return reason;
}

/**@brief Power manager.
 */
void power_manage(void)
{
    uint32_t err_code;

    err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
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

	// Initialize debug logging if enabled.
	debug_init();

	LOG("**********************************************************************\r\n");
	LOG("[MAIN]:Starting ANT+ id: %i, firmware: %s, serial: %#.8x\r\n",
			ANT_DEVICE_NUMBER, SW_REVISION, SERIAL_NUMBER);
	LOG("**********************************************************************\r\n");

	// Determine what the reason for startup is and log appropriately.
	check_reset_reason();

	// Configure brown out support.
	config_power_supervisor();

	// Initialize timers.
	timers_init();

	// Peripheral interrupt event handlers.
	static peripheral_evt_t on_peripheral_handlers = {
		on_button_pbsw,
		on_accelerometer,
		on_power_plug,
		on_battery_result
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
		on_set_resistance,
		on_ant_ctrl_command,
		on_enable_dfu_mode,
		on_request_data,
		on_set_parameter
	};

	// TODO: Question should we have a separate method to initialize soft device?

	// Initializes the soft device, Bluetooth and ANT stacks.
	ble_ant_init(&ant_ble_handlers);

    // Subscribe for system events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    // Initialize the scheduler.
	scheduler_init();

	// initialize the user profile.
	profile_init();

	// Begin advertising and receiving ANT messages.
	ble_ant_start();

	// Initialize resistance module and initial values.
	resistance_init(PIN_SERVO_SIGNAL, &m_user_profile);
	// TODO: This state should be moved to resistance module.
	m_resistance_level = 0;
	m_resistance_mode = RESISTANCE_SET_STANDARD;

	// Initialize module to read speed from flywheel.
	speed_init(PIN_FLYWHEEL, m_user_profile.wheel_size_mm);

	// Initialize power module with user profile.
	power_init(&m_user_profile);

	// Initialize the FIFO queue for holding events.
	irt_power_meas_fifo_init(IRT_FIFO_SIZE);

	// Start the main loop for reporting ble services.
	application_timers_start();

	LOG("[MAIN]:Initialization done.\r\n");

#ifdef USE_BATTERY
	// TODO: This is just temporary, this will move to a regular event.
	// Can't be done as part of peripheral_init because soft device isn't enabled yet.
	// Get an initial read from the battery.
	battery_read_start();
#endif

    // Enter main loop
    for (;;)
    {
		app_sched_execute();
    	power_manage();
    }
}
