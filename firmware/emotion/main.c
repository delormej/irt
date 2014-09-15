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
#include "nrf51.h"
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
#include "debug.h"
#include "boards.h"
#include "battery.h"
#include "irt_error_log.h"

#define ANT_4HZ_INTERVAL				APP_TIMER_TICKS(250, APP_TIMER_PRESCALER)  // Remote control & bike power sent at 4hz.
#define CALIBRATION_INTERVAL			APP_TIMER_TICKS(250, APP_TIMER_PRESCALER)  // Calibration message interval.
#define BLE_ADV_BLINK_RATE_MS			500u
#define SCHED_QUEUE_SIZE                8                                          /**< Maximum number of events in the scheduler queue. */
#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE,\
                                            BLE_STACK_HANDLER_SCHED_EVT_SIZE)       /**< Maximum size of scheduler events. */
#define BATTERY_4HZ_INTERVAL			512											// Every 2 minutes read the battery voltage (512 * 4hz) read the battery.

//
// Default profile and simulation values.
//
#define DEFAULT_WHEEL_SIZE_MM			2096ul										// Default wheel size.
#define DEFAULT_TOTAL_WEIGHT_KG			8180ul 										// Default weight (convert 180.0lbs to KG).
#define DEFAULT_ERG_WATTS				175u										// Default erg target_watts when not otherwise set.
#define DEFAULT_SETTINGS				SETTING_INVALID								// Default 32bit field of settings.
#define DEFAULT_CRR						30ul										// Default Co-efficient for rolling resistance used when no slope/intercept defined.  Divide by 1000 to get 0.03f.
#define SIM_CRR							0.0033f										// Default crr for typical outdoor rolling resistance (not the same as above).
#define SIM_C							0.60f										// Default co-efficient for drag.  See resistance sim methods.

//
// General purpose retention register states used.
//
#define IRT_GPREG_DFU	 				0x1
#define IRT_GPREG_ERROR 				0x2

#define DATA_PAGE_RESPONSE_TYPE			0x80										// 0X80 For acknowledged response or number of times to send broadcast data requests.

static uint8_t 							m_resistance_level;
static resistance_mode_t				m_resistance_mode;

static app_timer_id_t               	m_ant_4hz_timer_id;                    		// ANT 4hz timer.  TOOD: should rename since it's the core timer for all reporting (not just ANT).
static app_timer_id_t					m_ca_timer_id;								// Timer used during calibration.

static user_profile_t  					m_user_profile  __attribute__ ((aligned (4))); // Force required 4-byte alignment of struct loaded from flash.
static rc_sim_forces_t					m_sim_forces;
static accelerometer_data_t 			m_accelerometer_data;

static uint16_t							m_ant_ctrl_remote_ser_no; 					// Serial number of remote if connected.

static irt_battery_status_t				m_battery_status;
static uint32_t 						m_battery_start_ticks __attribute__ ((section(".noinit")));			// Time (in ticks) when we started running on battery.

static bool								m_crr_adjust_mode;							// Indicator that we're in manual calibration mode.

// Type declarations for profile updating.
static void profile_update_sched_handler(void *p_event_data, uint16_t event_size);
static void profile_update_sched(void);

static void send_data_page2(uint8_t subpage, uint8_t response_type);
static void send_temperature();

/* TODO:	Total hack for request data page & resistance control ack, we will fix.
 *		 	Simple logic right now.  If there is a pending request data page, send
 *		 	up to 2 of these messages as priority (based on requested tx count).
 *		 	If there is a pending resistance acknowledgment, send that as second
 *		 	priority.  Otherwise, resume normal power message.
 *		 	Ensure that at least once per second we are sending an actual power
 *		 	message.
 */
static uint8_t							m_resistance_ack_pending[3] = {0};		// byte 0: operation, byte 1:2: value
static ant_request_data_page_t			m_request_data_pending;
#define ANT_RESPONSE_LIMIT				3										// Only send max 3 sequential response messages before interleaving real power message.

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
	// Set LED indicator.
	set_led_red(LED_BOTH);

	// Fetch the stack and save the error.
	irt_error_save(error_code, line_num, p_file_name);

    // Indicate error state in General Purpose Register.
	sd_power_gpregret_set(IRT_GPREG_ERROR);

	// Kill the softdevice and any pending interrupt requests.
	sd_softdevice_disable();
	__disable_irq();

#if defined(ENABLE_DEBUG_ASSERT)
	irt_error_log_data_t* p_error = irt_error_last();

	LOG("[MAIN]:app_error_handler {HALTED ON ERROR: %#.8x}: %s:%lu\r\nCOUNT = %i\r\n",
			error_code, p_file_name, line_num, p_error->failure);

	// TODO: figure out how to display the stack (p_error->stack_info).

#if defined(PIN_PBSW)
	// Note this function does not return - it will hang waiting for a debugger to attach.
	LOG("[MAIN]:app_error_handler -- PRESS BUTTON TO RESET\r\n.");
	while (nrf_gpio_pin_read(PIN_PBSW) == 1) {}			// Do nothing, attach debugger until the button is pressed.
#else // PIN_PBSW
	for (;;) {}
#endif // PIN_PBSW

#endif // ENABLE_DEBUG_ASSERT
	// Wait 1 second, to give user a chance to see LED indicators and then reset the system.
	nrf_delay_ms(1000);
	NVIC_SystemReset();
}

/**@brief	Handle Soft Device system events. */
static void sys_evt_dispatch(uint32_t sys_evt)
{
	// Process storage events.
	pstorage_sys_event_handler(sys_evt);
}

/**@brief	Queues up the last resistance control acknowledgment.
 *
 */
static void queue_resistance_ack(uint8_t op_code, uint16_t value)
{
	m_resistance_ack_pending[0] = op_code;
	m_resistance_ack_pending[1] = LSB(value);
	m_resistance_ack_pending[2] = MSB(value);

	LOG("[MAIN] Queued resistance ack for [%.2x][%.2x]:\r\n",
			m_resistance_ack_pending[1],
			m_resistance_ack_pending[2]);
}

static void queue_data_response(ant_request_data_page_t request)
{
	m_request_data_pending = request;
}

static void queue_data_response_clear()
{
	memset(&m_request_data_pending, 0, sizeof(ant_request_data_page_t));
}

/**@brief	Dispatches ANT messages in response to requests such as Request Data Page and
 * 			resistance control acknowledgments.
 *
 *@return	Returns true if it had messages to dequeue and sent, false if there were none.
 *@note		Will send no more than ANT_RESPONSE_LIMIT in sequential calls.
 */
static bool dequeue_ant_response(void)
{
	// Static response count so that no more than ANT_RESPONSE_LIMIT are sent sequentially
	// as not to starve the normal power messages.
	static uint8_t response_count = 0;

	if (response_count >= ANT_RESPONSE_LIMIT)
	{
		// Reset sequential response count and return.
		response_count = 0;
		return false;
	}

	// Prioritize data response messages first.
	if (m_request_data_pending.data_page == ANT_PAGE_REQUEST_DATA)
	{

		if (m_request_data_pending.tx_page == ANT_PAGE_MEASURE_OUTPUT)
		{
			// TODO: This is the only measurement being sent right now, but we'll have more
			// to cycle through here.
			send_temperature();
		}
		else
		{
			// Deal with the # of times that it has to be sent.
			// for (times = (tx_type & 0x7F); times > 0; times--)
			//m_request_data_pending[5]
			//subpage = buffer[3];
			//response_type = buffer[5];
			send_data_page2(m_request_data_pending.descriptor[0], m_request_data_pending.tx_response);
		}

		// byte 1 of the buffer contains the flag for either acknowledged (0x80) or a value
		// indicating how many times to send the message.
		if (m_request_data_pending.tx_response == 0x80 || (m_request_data_pending.tx_response & 0x7F) <= 1)
		{
			// Clear the buffer.
			queue_data_response_clear();
		}
		else
		{
			// Decrement the count, we'll need to send again.
			m_request_data_pending.tx_response--;
		}

	}
	else if (m_resistance_ack_pending[0] != 0)
	{
		uint16_t* value;
		value = (uint16_t*)&m_resistance_ack_pending[1];

		ble_ant_resistance_ack(m_resistance_ack_pending[0], *value);
		// Clear the buffer.
		memset(&m_resistance_ack_pending, 0, sizeof(m_resistance_ack_pending));
	}
	else
	{
		return false;
	}

	response_count++;
	return true;
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

	if (m_user_profile.wheel_size_mm != wheel_size && wheel_size > 1800)
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

	if (weight > 30.0f) // minimum weight.
	{
		if (m_user_profile.total_weight_kg != weight)
		{
			m_user_profile.total_weight_kg = weight;

			// Schedule an update to persist the profile to flash.
			profile_update_sched();

			// Re-initialize the power module with updated weight.
			power_init(&m_user_profile, DEFAULT_CRR);
		}
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

	/*
	LOG("[MAIN]:set_sim_params {weight:%.2f, crr:%i, c:%i}\r\n",
		(m_user_profile.total_weight_kg / 100.0f),
		(uint16_t)(m_sim_forces.crr * 10000),
		(uint16_t)(m_sim_forces.c * 1000) ); */
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
			LOG("[MAIN]: Older profile version %i. Loading defaults and upgrading. \r\n",
					m_user_profile.version);
			m_user_profile.version			= PROFILE_VERSION;
			m_user_profile.wheel_size_mm 	= DEFAULT_WHEEL_SIZE_MM;
			m_user_profile.total_weight_kg 	= DEFAULT_TOTAL_WEIGHT_KG;
			m_user_profile.settings 		= DEFAULT_SETTINGS;
			m_user_profile.servo_offset		= 0;

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

/**@brief	Sends a heart beat message for the ANT+ remote control.
  */
static void ant_ctrl_available(void)
{
	// Send remote control availability to keep it alive during calibration.
	ant_ctrl_device_avail_tx((uint8_t) (m_ant_ctrl_remote_ser_no != 0));
}

/*----------------------------------------------------------------------------
 * Timer functions
 * ----------------------------------------------------------------------------*/

/**@brief Called during calibration to send frequent calibration messages.
 *
 */
static void calibration_timeout_handler(void * p_context)
{
	static uint8_t count;
	uint8_t response[6];
	uint32_t flywheel;
	uint16_t seconds2048;

	seconds2048 = seconds_2048_get();
	flywheel = flywheel_ticks_get();

	LOG("[MAIN]flywheel: %u\r\n", flywheel);

	// 6 data bytes available.
	memcpy(&response, &seconds2048, sizeof(uint16_t));
	memcpy(&response[2], &flywheel, sizeof(uint32_t));

	ant_bp_page2_tx_send(IRT_MSG_SUBPAGE_CA_SPEED, response, DATA_PAGE_RESPONSE_TYPE);

	/* Send remote control heart beat less frequently.
	if (count++ % 8 == 0)
	{
		ant_ctrl_available();
	}*/
}

/**@brief Function for handling the ANT 4hz measurement timer timeout.
 *
 * @details This function will be called each timer expiration.  The default period
 * 			for the ANT Bike Power service is 4hz.
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

	//LOG("[MAIN] Enter 4hz: %.2f \r\n", SECONDS_CURRENT);

	// All ANT messages on the Bike Power channel are sent in this function:

	// Standard ANT+ Device broadcast (cycle of 5 messages)
	// Request Data Page responses
	// Extra Info debug messages (to be removed before production and replaced with request data page)
	// Acknowledgments to resistance control

	// If there was another ANT+ message dequeued, move on.
	if (dequeue_ant_response())
	{
		return;
	}

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
	p_power_meas_current->battery_status = m_battery_status;

	// Every 32 seconds.
	if (event_count % 128 == 0)
	{
		// Get current temperature.
		p_power_meas_current->temp = temperature_read();
		LOG("[MAIN] Temperature read: %2.1f. \r\n", p_power_meas_current->temp);
	}
	else
	{
		p_power_meas_current->temp = p_power_meas_last->temp;
	}

	if (event_count % BATTERY_4HZ_INTERVAL == 0)
	{
		// Every 2 minutes initiate battery read.
		battery_read_start();
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

	// Send remote control a heartbeat.
	ant_ctrl_available();

	//LOG("[MAIN] Exit 4hz: %.2f \r\n", SECONDS_CURRENT);
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
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, true);

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

/**@brief	Copies the last error to a response structure.
 */
static void error_to_response(uint8_t* p_response)
{
	irt_error_log_data_t* p_err;

	p_err = irt_error_last();
	memcpy(p_response, &(p_err->failure), sizeof(uint16_t));
	memcpy(&(p_response[2]), &(p_err->line_number), sizeof(uint16_t));
	memcpy(&(p_response[4]), &(p_err->err_code), sizeof(uint8_t));
}

/**@brief	Sends data page 2 response.
 */
static void send_data_page2(uint8_t subpage, uint8_t response_type)
{
	uint8_t response[6];

	memset(&response, 0, sizeof(response));

	switch (subpage)
	{
		case IRT_MSG_SUBPAGE_SETTINGS:
			memcpy(&response, &m_user_profile.settings, sizeof(uint16_t));
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

		case IRT_MSG_SUBPAGE_GET_ERROR:
			error_to_response(response);
			break;

		case IRT_MSG_SUBPAGE_SERVO_OFFSET:
			memcpy(&response, &m_user_profile.servo_offset, sizeof(int16_t));
			break;

		case IRT_MSG_SUBPAGE_CHARGER:
			response[0] = battery_charge_status();
			break;

		default:
			LOG("[MAIN] Unrecognized page request. \r\n");
			return;
	}

	ant_bp_page2_tx_send(subpage, response, response_type);
}

/**@brief	Sends temperature value as measurement output page (0x03)
 *
 */
static void send_temperature()
{
	float temp;
	int16_t value;

	temp = temperature_read();
	value = (int16_t)(1024.0 * temp);  // TODO: look closer at how the scale factor works. (2^10)

	LOG("[MAIN] Sending temperature as: %i \r\n", value);

	ant_bp_page3_tx_send(1, TEMPERATURE, 10, 0, value);
}

/**@brief 	Suspends normal messages and starts sending calibration messages.
 */
static void calibration_start(void)
{
	uint32_t err_code;

	// Stop existing ANT timer, start the new one.
	application_timers_stop();

    if (m_ca_timer_id == NULL)
    {
		err_code = app_timer_create(&m_ca_timer_id,
									APP_TIMER_MODE_REPEATED,
									calibration_timeout_handler);
		APP_ERROR_CHECK(err_code);
    }

    err_code = app_timer_start(m_ca_timer_id, CALIBRATION_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

    set_led_red(LED_1);
    m_crr_adjust_mode = true;
}

/**@brief 	Stops calibration and resumes normal activity.
 */
static void calibration_stop(void)
{
	uint32_t err_code;

	// Stop the calibration timer.
	err_code = app_timer_stop(m_ca_timer_id);
    APP_ERROR_CHECK(err_code);

	clear_led(LED_1);
	m_crr_adjust_mode = false;

	// Restart the normal timer.
	application_timers_start();
}

/**@brief	Updates settings either temporarily or with persistence.
 * @warn	NOTE that if any other profile persistence occurs during the session
 * 			these settings will be updated to flash.
 * 			TODO: fix this as it is a bug -- use a settings dirty flag for instance.
 */
static void settings_update(uint8_t* buffer)
{
	uint16_t settings;

	// The actual settings are a 32 bit int stored in bytes [2:5] IRT_MSG_PAGE2_DATA_INDEX
	// Need to use memcpy, Cortex-M proc doesn't support unaligned casting of 32bit int.
	// MSB is flagged to indicate if we should persist or if this is just temporary.
	memcpy(&settings, &buffer[IRT_MSG_PAGE2_DATA_INDEX], sizeof(uint16_t));

	m_user_profile.settings = SETTING_VALUE(settings);

	/*LOG("[MAIN] Request to update settings to: ACCEL:%i \r\n",
				SETTING_IS_SET(m_user_profile.settings, SETTING_ACL_SLEEP_ON) );*/

	LOG("[MAIN]:settings_update raw: %i, translated: %i\r\n",
			settings, m_user_profile.settings);

	if (SETTING_PERSIST(settings))
	{
		LOG("[MAIN] Scheduled persistence of updated settings: %i\r\n",
				m_user_profile.settings);
		// Schedule update to the profile.
		profile_update_sched();
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

	// TODO: should we be gracefully closing ANT and BLE channels here?
	application_timers_stop();							// Stop running timers.
	irt_power_meas_fifo_free();							// Free heap allocation.

	peripheral_powerdown(accelerometer_wake_disable);	// Shutdown peripherals.

	sd_power_system_off();								// Enter lower power mode.
}

static void on_resistance_off(void)
{
	m_resistance_mode = RESISTANCE_SET_STANDARD;
	m_resistance_level = 0;
	resistance_level_set(m_resistance_level);
	queue_resistance_ack(m_resistance_mode, m_resistance_level);
}

static void on_resistance_dec(void)
{
	// decrement
	if (m_resistance_mode == RESISTANCE_SET_STANDARD &&
			m_resistance_level > 0)
	{
		resistance_level_set(--m_resistance_level);
		queue_resistance_ack(m_resistance_mode, m_resistance_level);
	}
	else if (m_resistance_mode == RESISTANCE_SET_ERG &&
			m_sim_forces.erg_watts > 50u)
	{
		// Decrement by 15 watts;
		m_sim_forces.erg_watts -= 15u;
		queue_resistance_ack(m_resistance_mode, m_sim_forces.erg_watts);
	}
}

static void on_resistance_inc(void)
{
	// increment
	if (m_resistance_mode == RESISTANCE_SET_STANDARD &&
			m_resistance_level < (RESISTANCE_LEVELS-1))
	{
		resistance_level_set(++m_resistance_level);
		queue_resistance_ack(m_resistance_mode, m_resistance_level);
	}
	else if (m_resistance_mode == RESISTANCE_SET_ERG)
	{
		// Increment by 15 watts;
		m_sim_forces.erg_watts += 15u;
		queue_resistance_ack(m_resistance_mode, m_sim_forces.erg_watts);
	}
}

static void on_resistance_max(void)
{
	m_resistance_mode = RESISTANCE_SET_STANDARD;
	m_resistance_level = RESISTANCE_LEVELS-1;
	resistance_level_set(m_resistance_level);
	queue_resistance_ack(m_resistance_mode, m_resistance_level);
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
		queue_resistance_ack(m_resistance_mode, m_sim_forces.erg_watts);
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
		//
		// This is a workaround to deal with GPIOTE toggling the SENSE bits which forces
		// the device to wake up immediately after going to sleep without this.
		//
        NRF_GPIO->PIN_CNF[PIN_SHAKE] &= ~GPIO_PIN_CNF_SENSE_Msk;
        NRF_GPIO->PIN_CNF[PIN_SHAKE] |= GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos;

        if ((SETTING_IS_SET(m_user_profile.settings, SETTING_ACL_SLEEP_ON)))
        {
    		LOG("[MAIN]:about to power down from accelerometer sleep.\r\n");;
        	on_power_down(false);
        }
        else
        {
    		LOG("[MAIN]:skipping sleep signal from accelerometer.\r\n");;
        }
	}
}

// Called when the power adapter is plugged in.
static void on_power_plug(bool plugged_in)
{
	if (plugged_in)
	{
		LOG("[MAIN] Power plugged in.\r\n");
	}
	else
	{
		LOG("[MAIN] Power unplugged.\r\n");
	}

	// Either way, reset the battery ticks.
	m_battery_start_ticks = 0;
}

static void on_ble_connected(void) 
{
	blink_led_green_stop(LED_1);
	set_led_green(LED_1);

	LOG("[MAIN]:on_ble_connected\r\n");
}
	
static void on_ble_disconnected(void) 
{
	// Clear connection LED.
	clear_led(LED_1);
	
	LOG("[MAIN]:on_ble_disconnected\r\n");

	// Restart advertising.
	ble_advertising_start();
}

static void on_ble_timeout(void) 
{
	blink_led_green_stop(LED_1);
	LOG("[MAIN]:on_ble_timeout\r\n");
}

static void on_ble_advertising(void)
{
	blink_led_green_start(LED_1, BLE_ADV_BLINK_RATE_MS);
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
	queue_resistance_ack(rc_evt.operation, (int16_t)*rc_evt.pBuffer);
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
			{/*
				m_user_profile.ca_slope += 50;
				power_init(&m_user_profile, DEFAULT_CRR);
				send_data_page2(IRT_MSG_SUBPAGE_CRR, DATA_PAGE_RESPONSE_TYPE);*/
			}
			else
			{
				// Increment resistance.
				on_resistance_inc();
			}
			break;

		case ANT_CTRL_BUTTON_DOWN:
			if (m_crr_adjust_mode)
			{/*
				if (m_user_profile.ca_slope > 50)
				{
					m_user_profile.ca_slope -= 50;
					power_init(&m_user_profile, DEFAULT_CRR);
					send_data_page2(IRT_MSG_SUBPAGE_CRR, DATA_PAGE_RESPONSE_TYPE);
				}*/
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
				calibration_stop();
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
				calibration_start();
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
	err_code = sd_power_gpregret_set(IRT_GPREG_DFU);
	APP_ERROR_CHECK(err_code);

	// Resets the CPU to start in DFU mode.
	NVIC_SystemReset();
}

/**@brief	Device receives page (0x46) requesting data page.
 */
static void on_request_data(uint8_t* buffer)
{
	ant_request_data_page_t request;
	memcpy(&request, buffer, sizeof(ant_request_data_page_t));

	switch (request.tx_page)
	{
		// Kick off battery read.
		case ANT_PAGE_BATTERY_STATUS:
			LOG("[MAIN] Requested battery status. \r\n");
			battery_read_start();
			return;

		// Request is for get/set parameters or measurement output.
		case ANT_PAGE_GETSET_PARAMETERS:
		case ANT_PAGE_MEASURE_OUTPUT:
			queue_data_response(request);
			LOG("[MAIN] Request to get data page (subpage): %#x, type:%i\r\n",
					request.descriptor[0],
					request.tx_response);
			break;

		default:
			LOG("[MAIN] Unrecognized request page:%i, descriptor:%i. \r\n",
					request.data_page, request.descriptor[0]);
	}
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
			settings_update(buffer);
			break;

		case IRT_MSG_SUBPAGE_CRR:
			// Update CRR.
			memcpy(&m_user_profile.ca_slope, &buffer[IRT_MSG_PAGE2_DATA_INDEX],
					sizeof(uint16_t));
			memcpy(&m_user_profile.ca_intercept, &buffer[IRT_MSG_PAGE2_DATA_INDEX+2],
					sizeof(uint16_t));
			LOG("[MAIN] Updated slope:%i intercept:%i \r\n",
					m_user_profile.ca_slope, m_user_profile.ca_intercept);
			// Reinitialize power.
			power_init(&m_user_profile, DEFAULT_CRR);

			// Schedule update to the user profile.
			profile_update_sched();
			break;

		case IRT_MSG_SUBPAGE_SERVO_OFFSET:
			m_user_profile.servo_offset = (int16_t)(buffer[IRT_MSG_PAGE2_DATA_INDEX] |
				buffer[IRT_MSG_PAGE2_DATA_INDEX+1] << 8);
			LOG("[MAIN] Updated servo_offset:%i \r\n", m_user_profile.servo_offset);

			// Schedule update to the user profile.
			profile_update_sched();
			break;

		case IRT_MSG_SUBPAGE_SLEEP:
			LOG("[MAIN] on_set_parameter: Request device sleep.\r\n");
			on_power_down(true);
			break;

		case IRT_MSG_SUBPAGE_CHARGER:
			if (FEATURE_AVAILABLE(FEATURE_BATTERY_CHARGER))
			{
				// Turns charger on if currently off, else turns off.
				battery_charge_set( (BATTERY_CHARGE_OFF) );

				LOG("[MAIN] on_set_parameter: Toggled battery charger.\r\n");
			}
			else
			{
				LOG("[MAIN] on_set_parameter: No battery charger present.\r\n");
			}
			break;

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
	// Increment battery ticks.  Each "tick" represents 16 seconds.
	// BATTERY_4HZ_INTERVAL is the number of events in between reads, each event is @4hz.
	// i.e. { 512 == ~2 mins (128 seconds) } / {16 second reporting * 4 per second}
	//
	m_battery_start_ticks += (BATTERY_4HZ_INTERVAL / (16*4));

	// TODO: Hard coded for the moment, we will send battery page.
	LOG("[MAIN] on_battery_result %i, ticks: %i \r\n", battery_level, m_battery_start_ticks);
	//ant_bp_page2_tx_send(0x52, (uint8_t*)&battery_level, DATA_PAGE_RESPONSE_TYPE);

	m_battery_status = battery_status(battery_level, m_battery_start_ticks);
}

/**@brief	Configures chip power options.
 *
 * @note	Note this must happen after softdevice is enabled.
 */
static void config_dcpower()
{
	uint32_t err_code;

    // Enabling constant latency as indicated by PAN 11 "HFCLK: Base current with HFCLK
    // running is too high" found at Product Anomaly document found at
    // https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822/#Downloads
    //
    // @note This setting will ensure correct behavior when routing TIMER events through
    //       PPI and low power mode simultaneously.
    // NRF_POWER->TASKS_CONSTLAT = 1; // Use sd call as the sd is enabled.
    err_code = sd_power_mode_set(NRF_POWER_MODE_CONSTLAT);
    APP_ERROR_CHECK(err_code);

	// Forces a reset if power drops below 2.7v.
	//NRF_POWER->POFCON = POWER_POFCON_POF_Enabled | POWER_POFCON_THRESHOLD_V27;
    err_code = sd_power_pof_threshold_set(NRF_POWER_THRESHOLD_V27);
    APP_ERROR_CHECK(err_code);

    err_code = sd_power_pof_enable(SD_POWER_POF_ENABLE);
    APP_ERROR_CHECK(err_code);

    // Configure the DCDC converter to save battery.
    // This shows now appreciable difference, see https://devzone.nordicsemi.com/question/5980/dcdc-and-softdevice/
    //err_code = sd_power_dcdc_mode_set(NRF_POWER_DCDC_MODE_AUTOMATIC);
	//err_code = sd_power_dcdc_mode_set(NRF_POWER_DCDC_MODE_OFF);
    APP_ERROR_CHECK(err_code);
}


/**@brief	Check if there is a reset reason and log if enabled.
 */
static uint32_t check_reset_reason()
{
	uint32_t reason, gpreg;
	irt_error_log_data_t* p_error;

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

	gpreg = NRF_POWER->GPREGRET;
	// Check and see if the device last reset due to error.
	if (gpreg == IRT_GPREG_ERROR)
	{
		// Log the occurrence.
		p_error = irt_error_last();

		LOG("[MAIN]:check_reset_reason Resetting from previous error.\r\n");
		LOG("\t{COUNT: %i, ERROR: %#.8x}: %s:%lu\r\n",
				p_error->failure,
				p_error->err_code,
				p_error->message,
				p_error->line_number);

		// Now clear the register.
		NRF_POWER->GPREGRET = 0;
	}
	else
	{
		// Initialize the error structure.
		irt_error_init();
	}

	return reason;
}

/**@brief	Initializes the Nordic Softdevice.  This must be done before
 * 			initializing ANT/BLE or calling any softdevice functions.
 */
static void s310_init()
{
	uint32_t err_code;

	// Initialize SoftDevice
	SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, true);

    // Subscribe for system events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
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
	// Initialize default remote serial number.
	m_ant_ctrl_remote_ser_no = 0;

	// Initialize debug logging if enabled.
	debug_init();

	LOG("**********************************************************************\r\n");
	LOG("[MAIN]:Starting ANT+ id: %i, firmware: %s, serial: %#.8x \r\n",
			ANT_DEVICE_NUMBER, SW_REVISION, SERIAL_NUMBER);
	LOG("[MAIN]:Features: %i\r\n", *FEATURES);
	LOG("**********************************************************************\r\n");

	// Determine what the reason for startup is and log appropriately.
	check_reset_reason();

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

	// Initialize and enable the softdevice.
	s310_init();

	// Configure the chip's power options (after sd enabled).
	config_dcpower();

	// Initializes the Bluetooth and ANT stacks.
	ble_ant_init(&ant_ble_handlers);

    // Initialize the scheduler.
	scheduler_init();

	// initialize the user profile.
	profile_init();

	// Initialize resistance module and initial values.
	resistance_init(PIN_SERVO_SIGNAL, &m_user_profile);
	// TODO: This state should be moved to resistance module.
	m_resistance_level = 0;
	m_resistance_mode = RESISTANCE_SET_STANDARD;

	// Initialize module to read speed from flywheel.
	speed_init(PIN_FLYWHEEL, m_user_profile.wheel_size_mm);

	// Initialize power module with user profile.
	power_init(&m_user_profile, DEFAULT_CRR);

	// Initialize the FIFO queue for holding events.
	irt_power_meas_fifo_init(IRT_FIFO_SIZE);

	// Begin advertising and receiving ANT messages.
	ble_ant_start();

	// Start the main loop for reporting ble services.
	application_timers_start();

	// Initiate first battery read.
	battery_read_start();

	LOG("[MAIN]:Initialization done.\r\n");

    // Enter main loop
    for (;;)
    {
		app_sched_execute();
    	power_manage();
    }
}
