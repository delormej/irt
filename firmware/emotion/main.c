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
 *		ANT FE-C profile
 *		ANT Controls profile
 *		ANT Cycling Power RX profile
 *		ANT Speed Sensor TX profile
 *
 * @file 		main.c 
 * @brief 		The purpose of main is to control program flow and manage any
 *				application specific state.  All other protocol details are handled
 *				in referenced modules.
 *
 *				This module is responsible for the timing of all activities, for
 *				example it determines when (not how) to adjust resistance, send
 *				ANT & BLE messages, etc...
 *
 *
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "nrf_gpio.h"
#include "nordic_common.h"
#include "nrf51.h"
#include "softdevice_handler.h"
#include "nrf_error.h"
#include "app_scheduler.h"
#include "app_util.h"
#include "app_util_platform.h"
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
#include "ant_bike_speed.h"
#include "ant_fec.h"
#include "nrf_delay.h"
#include "ant_ctrl.h"
#include "app_timer.h"
#include "debug.h"
#include "boards.h"
#include "battery.h"
#include "magnet.h"
//#include "irt_error_log.h"
#include "irt_led.h"
#include "bp_response_queue.h"
#include "calibration.h"

#define ANT_4HZ_INTERVAL				APP_TIMER_TICKS(250, APP_TIMER_PRESCALER)  	// Remote control & bike power sent at 4hz.
#define SENSOR_READ_INTERVAL			APP_TIMER_TICKS(128768, APP_TIMER_PRESCALER) // ~2 minutes sensor read interval, which should be out of sequence with 4hz.

#define WATCHDOG_TICK_COUNT				APP_TIMER_CLOCK_FREQ * 60 * 5 				// (NRF_WDT->CRV + 1)/32768 seconds * 60 seconds * 'n' minutes
#define WDT_RELOAD()					NRF_WDT->RR[0] = WDT_RR_RR_Reload			// Keep the device awake.

#ifdef ENABLE_DEBUG_LOG
#define SCHED_QUEUE_SIZE                32
#else // ENABLE_DEBUG_LOG
#define SCHED_QUEUE_SIZE                8                                          /**< Maximum number of events in the scheduler queue. */
#endif // ENABLE_DEBUG_LOG
#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE,\
                                            BLE_STACK_HANDLER_SCHED_EVT_SIZE)       /**< Maximum size of scheduler events. */
//
// Default simulation values.
//
#define CRR_ADJUST_VALUE				1											// Amount to adjust (up/down) CRR on button command.
#define SHUTDOWN_VOLTS					6											// Shut the device down when battery drops below 6.0v

//
// General purpose retention register states used.
//
#define IRT_GPREG_DFU	 				0x1
#define IRT_GPREG_ERROR 				0x2

#define DATA_PAGE_RESPONSE_TYPE			0x80										// 0X80 For acknowledged response or number of times to send broadcast data requests.

static irt_context_t 					m_current_state =	{						// Current state structure, initalized:
    .fe_state = FE_READY,
    .elapsed_time = 0, 
    .distance_m = 0, 
    .lap_toggle = 0,
    .instant_speed_mps = 0.0f,
    .target_power_limits = TARGET_UNDETERMINED,
    .bike_power_calibration_required = 0,
	.resistance_calibration_required = 0,
	.user_configuration_required = 0    
    };
    
static app_timer_id_t               	m_ant_4hz_timer_id;                    		// ANT 4hz timer.  TOOD: should rename since it's the core timer for all reporting (not just ANT).
static app_timer_id_t					m_sensor_read_timer_id;						// Timer used to read sensors, out of phase from the 4hz messages.

static user_profile_t*                  mp_user_profile;                             // Pointer to user profile object.
static accelerometer_data_t 			m_accelerometer_data;
static const irt_resistance_state_t* 	mp_resistance_state;

static uint16_t							m_ant_ctrl_remote_ser_no; 					// Serial number of remote if connected.

static uint32_t 						m_battery_start_ticks __attribute__ ((section (".NoInit")));			// Time (in ticks) when we started running on battery.

static void on_get_parameter(ant_request_data_page_t* p_request);
static void send_temperature();
static void on_enable_dfu_mode();
static void on_resistance_off();
static void on_request_calibration();
static void on_bp_power_data(ant_bp_message_type_e state, uint16_t data);

// // Hard coding ANT power meter device id for testing.
// #warning "HARD CODED m_ant_bp_device_id = 52652"
// static uint16_t m_ant_bp_device_id = 52652;

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
	led_set(LED_ERROR);

	// Fetch the stack and save the error.
	//irt_error_save(error_code, line_num, p_file_name);

    // Indicate error state in General Purpose Register.
	sd_power_gpregret_set(IRT_GPREG_ERROR);

	// Kill the softdevice and any pending interrupt requests.
	sd_softdevice_disable();
	__disable_irq();

#if defined(ENABLE_DEBUG_ASSERT)
	//irt_error_log_data_t* p_error = irt_error_last();

	LOG("[MAIN]:app_error_handler {HALTED ON ERROR: %#.8x}: %s:%lu\r\n", //COUNT = %i\r\n",
			error_code, p_file_name, line_num /*, p_error->failure*/);

	#if defined(PIN_EN_SERVO_PWR)
	// Kill power to the servo in case it's in flight.
	nrf_gpio_pin_clear(PIN_EN_SERVO_PWR);
	#endif // PIN_EN_SERVO_PWR

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
	if ((sys_evt == NRF_EVT_FLASH_OPERATION_SUCCESS) ||
            (sys_evt == NRF_EVT_FLASH_OPERATION_ERROR))
	{
    		// Process storage events.
		pstorage_sys_event_handler(sys_evt);
    }
}

/**@brief	Dispatches ANT messages in response to requests such as Request Data Page and
 * 			resistance control acknowledgments.
 *
 *@return	Returns true if it had messages to dequeue and sent, false if there were none.
 *@note		Will send no more than ANT_RESPONSE_LIMIT in sequential calls.
 */
static bool dequeue_ant_response(void)
{
	ant_request_data_page_t request;

	if (!bp_dequeue_ant_response(&request))
	{
		// Nothing to dequeue.
		return false;
	}

	// Prioritize data response messages first.
	if (request.data_page == ANT_PAGE_REQUEST_DATA)
	{
		switch (request.tx_page)
		{
			case ANT_PAGE_MEASURE_OUTPUT:
				// TODO: This is the only measurement being sent right now, but we'll have more
				// to cycle through here.
				send_temperature();
				break;

			case ANT_PAGE_BATTERY_STATUS:
				//ant_bp_battery_tx_send(m_current_state.battery_status);
				break;
				/*
			case ANT_COMMON_PAGE_80:
				ant_common_page_transmit(ANT_BP_TX_CHANNEL, ant_manufacturer_page);
				break;

			case ANT_COMMON_PAGE_81:
				ant_common_page_transmit(ANT_BP_TX_CHANNEL, ant_product_page);
				break;
				*/
			default:
				on_get_parameter(&request);
				break;
		}

	}
	else if (request.data_page == WF_ANT_RESPONSE_PAGE_ID)
	{
		ble_ant_resistance_ack(request.descriptor[0],
				(request.reserved[0] | request.reserved[1] << 8)  );
	}
	else
	{
		LOG("[MAIN] dequeue_ant_response -- undefined page request %i.\r\n", request.data_page);
		return false;
	}

	return true;
}

/*----------------------------------------------------------------------------
 * KICKR resistance commands.
 * ----------------------------------------------------------------------------*/

/**@brief Parses the wheel params message from the KICKR.  It then advises the
 * 		  speed module and updates the user profile.  */
static void set_wheel_params(uint16_t value)
{
	uint16_t wheel_size;
	
	// Comes as 20700 for example, we'll round to nearest mm.
	wheel_size = value / 10;

	if (mp_user_profile->wheel_size_mm != wheel_size && wheel_size > 1800)
	{
		LOG("[MAIN]:set_wheel_params {wheel:%lu}\r\n", wheel_size);

		mp_user_profile->wheel_size_mm = wheel_size;
		// Schedule an update to persist the profile to flash.
		user_profile_store();
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

	if (weight  > 30.0f) // minimum weight.
	{
		if (mp_user_profile->total_weight_kg != weight)
		{
			mp_user_profile->total_weight_kg = weight;

			// Schedule an update to persist the profile to flash.
			user_profile_store();
		}
	}

	// Co-efficient for rolling resistance.
	crr = wahoo_decode_crr(&(pBuffer[2]));
	// Co-efficient of drag.
	c = wahoo_decode_c(&(pBuffer[4]));

	// Store and just precision if new values came across.
	if (crr > 0.0f)
		resistance_crr_set(crr);
	if (c > 0.0f)
		resistance_c_set(c);

	/*
	LOG("[MAIN]:set_sim_params {weight:%.2f, crr:%i, c:%i}\r\n",
		(mp_user_profile->total_weight_kg / 100.0f),
		(uint16_t)(m_sim_forces.crr * 10000),
		(uint16_t)(m_sim_forces.c * 1000) ); */
}

/**@brief	Helper method that updates context object with current resistance state.
 * 			Copies the resistance state for point in time reporting.
 *          NOTE: this must be called @4hz in main handler.
 *
 */
static void update_resistance_state()
{
	m_current_state.servo_position = mp_resistance_state->servo_position;
	m_current_state.resistance_mode = mp_resistance_state->mode;

	switch (m_current_state.resistance_mode)
	{
		case RESISTANCE_SET_ERG:
			m_current_state.resistance_level = (int16_t)mp_resistance_state->erg_watts;
            m_current_state.target_power_limits = mp_resistance_state->power_limit;
			break;

        case RESISTANCE_SET_SIM:
            m_current_state.target_power_limits = mp_resistance_state->power_limit;
            break;

		case RESISTANCE_SET_STANDARD:
			m_current_state.resistance_level = mp_resistance_state->level;
            m_current_state.target_power_limits = TARGET_UNDETERMINED;
			break;

		default:
			m_current_state.resistance_level = 0;
            m_current_state.target_power_limits = TARGET_UNDETERMINED;
			break;
	}
    
    /* 
     * State transition: READY -> IN_USE <-> FINISHED/PAUSED
     * If we're moving and in READY, move to IN USE
     * If we're IN USE and stopped, move to FINISHED/PAUSED
     */
    switch (m_current_state.fe_state)
    {
        case FE_READY:
        case FE_FINISHED_PAUSED:
            if (m_current_state.instant_speed_mps > 1.0f)
            {
                // Transition to in use.
                m_current_state.fe_state = FE_IN_USE;                
            }
            break;
        
        case FE_IN_USE:
            if (m_current_state.instant_speed_mps < 1.0f)
            {
                // Transition to finished / paused.
                m_current_state.fe_state = FE_FINISHED_PAUSED;   
                m_current_state.target_power_limits = TARGET_UNDETERMINED;             
            }
            else
            {
                // Increment time & distance.
                m_current_state.distance_m = speed_distance_get();
                m_current_state.elapsed_time++;
            }
            break;
            
        default:
            break;
    }
}

/**@brief	Toggles between standard and erg mode.
 *
 */
static void toggle_resistance_mode()
{
	if (mp_resistance_state->mode == RESISTANCE_SET_STANDARD)
	{
		// Current mode is standard, switch to Erg.

		// Use last or default erg setting.
		resistance_erg_set(0);

		// Queue acknowledgment.
		bp_queue_resistance_ack(mp_resistance_state->mode, mp_resistance_state->erg_watts);

		// Quick blink for feedback.
		led_set(LED_MODE_ERG);
	}
	else
	{
		// Current mode is Erg or another, turn off resistance.
		on_resistance_off();
	}
}

/**@brief	Queues a resistance acknowledgment based on current resistance state.
 *
 * @note	Only handles VALUE for Erg & Standard modes.
 *
 */
static void acknowledge_resistance()
{
	switch (mp_resistance_state->mode)
	{
		case RESISTANCE_SET_ERG:
			bp_queue_resistance_ack(mp_resistance_state->mode, mp_resistance_state->erg_watts);
			break;

		case RESISTANCE_SET_STANDARD:
			bp_queue_resistance_ack(mp_resistance_state->mode, mp_resistance_state->level);
			break;

		default:
			// For all others (percent, sim, etc...), send 0 for value.
			bp_queue_resistance_ack(mp_resistance_state->mode, 0);
			break;
	}
}

/**@brief	Sends a heart beat message for the ANT+ remote control.
  */
static void ant_ctrl_available(void)
{
	// Send remote control availability.
	ant_ctrl_device_avail_tx((uint8_t) (m_ant_ctrl_remote_ser_no != 0));
}

/*----------------------------------------------------------------------------
 * Timer functions
 * ----------------------------------------------------------------------------*/

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
	UNUSED_PARAMETER(p_context);
	static uint16_t event_count;
	uint32_t err_code;

	// All ANT messages on the Bike Power channel are sent in this function:

	// Standard ANT+ Device broadcast (cycle of 5 messages)
	// Request Data Page responses
	// Extra Info debug messages (to be removed before production and replaced with request data page)
	// Acknowledgments to resistance control

	// Maintain a static event count, we don't do everything each time.
	event_count++;

	// Make a copy of the resistance state
	update_resistance_state();

	// Report on accelerometer data.
	if (m_accelerometer_data.source & ACCELEROMETER_SRC_FF_MT)
	{
		m_current_state.accel_y_lsb = m_accelerometer_data.out_y_lsb;
		m_current_state.accel_y_msb = m_accelerometer_data.out_y_msb;

		// Clear the source now that we've read it.
		m_accelerometer_data.source = 0;
	}

	//
	// Only calculate speed/power every 1/2 second, unless in calibration.
	//
	if (event_count % 2 == 0 || calibration_in_progress)
	{
		m_current_state.event_count++;	// Increment to indicate we're recording data.

		// Calculate speed.
		err_code = speed_calc(&m_current_state);
		APP_ERROR_CHECK(err_code);

		// If we're moving.
		if (m_current_state.instant_speed_mps > 0.0f)
		{
			// Reload the WDT since there was motion, preventing the device from going to sleep.
			WDT_RELOAD();

			// Calculate power.
			err_code = power_calc(&m_current_state);
			APP_ERROR_CHECK(err_code);
		}
		else
		{
			//
			// Stopped, no speed = no power.
			//

			// From the ANT spec:
			// To indicate zero rotational velocity, do not increment the accumulated wheel period and do not increment the wheel ticks.
			// The update event count continues incrementing to indicate that updates are occurring, but since the wheel is not rotating
			// the wheel ticks do not increase.
			m_current_state.instant_power = 0;
		}
	}

    if (calibration_in_progress)
    {
        calibration_status_t* p_state = calibration_progress(&m_current_state);
        ant_fec_calibration_send(&m_current_state, p_state);
    }
    else 
    {
        // If there was another ANT+ message response in queue it was sent as a response
        // so skip until next time slot for cycling power.
        if (!dequeue_ant_response())
        {
            // Transmit the power messages.
            cycling_power_send(&m_current_state);
        }

        // Transmit FE-C messages.
        ant_fec_tx_send(&m_current_state);
    }
    
	// Send speed data.
	ant_sp_tx_send(m_current_state.last_wheel_event_2048 / 2, 	// Requires 1/1024 time
			(int16_t)m_current_state.accum_wheel_revs); 		// Requires truncating to 16 bits.

	// Send remote control a heart beat.
	ant_ctrl_available();
}

/**@brief	Timer handler for reading sensors.
 *
 */
static void sensor_read_timeout_handler(void * p_context)
{
	// Increment battery ticks.  Each "tick" represents 16 seconds.
	m_battery_start_ticks += (uint32_t)(SENSOR_READ_INTERVAL / 32768 / 16);

	// Initiate async battery read.
	battery_read_start();

	// Read temperature sensor.
	m_current_state.temp = temperature_read();
	LOG("[MAIN] Temperature read: %2.1f. \r\n", m_current_state.temp);
}

/**@brief Function for starting the application timers.
 */
static void application_timers_start(void)
{
    uint32_t err_code;

    // Start regular application timers.
    err_code = app_timer_start(m_ant_4hz_timer_id, ANT_4HZ_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_sensor_read_timer_id, SENSOR_READ_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

static void application_timers_stop(void)
{
	uint32_t err_code;
		
	err_code = app_timer_stop_all();
    APP_ERROR_CHECK(err_code);
}

/**@brief	Initializes the watchdog timer which is configured to
 * 			force a device reset after a period of inactivity, either
 * 			a CPU hang or no real user activity.  The reaset reason is
 * 			inspected on load to see if the WDT caused the reset.
 */
static void wdt_init(void)
{
    // Configure to keep running while CPU is in sleep mode.
    NRF_WDT->CONFIG = (WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);
    NRF_WDT->CRV = WATCHDOG_TICK_COUNT;
    // This is the reload register, write to this to signal activity.
    NRF_WDT->RREN = WDT_RREN_RR0_Enabled << WDT_RREN_RR0_Pos;
    NRF_WDT->TASKS_START = 1;
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

    // Standard ANT 4hz interval to calculate and send messages.
    err_code = app_timer_create(&m_ant_4hz_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                ant_4hz_timeout_handler);
	APP_ERROR_CHECK(err_code);

    // Reading battery / temp sensors less frequently.
	err_code = app_timer_create(&m_sensor_read_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                sensor_read_timeout_handler);
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

/**@brief	Sets relevant servo positions in response.
 * 			TODO: all message parsing belongs somewhere else (NOT the resistance
 * 			nor profile modules in this case), but a separate object that abstracts
 * 			the protocol.
 */
static void servo_pos_to_response(ant_request_data_page_t* p_request, uint8_t* p_response)
{
	uint8_t roffset = 0;	// Start data position offset in the response buffer.
	uint8_t start_idx;

	// If not specified, start at index 0.
	start_idx = p_request->descriptor[1] == 0xFF ? 0 : p_request->descriptor[1];

	p_response[roffset++] = RESISTANCE_LEVELS;	// # of levels
	p_response[roffset++] = start_idx; // start index.

	LOG("[MAIN] sending servo_positions count:%i, start %i\r\n", p_response[0], p_response[1]);

	// send 2 positions, 2 bytes each
	for (uint8_t i = start_idx; i < (start_idx + 2); i++)
	{
		LOG("[MAIN] servo_position[%i] %i\r\n", i,
				mp_user_profile->servo_positions.positions[i]);

		if (i >= RESISTANCE_LEVELS)
		{
			p_response[roffset++] = 0xFF;
			p_response[roffset++] = 0xFF;
		}
		else
		{
			p_response[roffset++] = LOW_BYTE(mp_user_profile->servo_positions.positions[i]);
			p_response[roffset++] = HIGH_BYTE(mp_user_profile->servo_positions.positions[i]);
		}
	}
}

/**@brief	Copies the last error to a response structure.
 *
static void error_to_response(uint8_t* p_response)
{
	irt_error_log_data_t* p_err;

	p_err = irt_error_last();
	memcpy(p_response, &(p_err->failure), sizeof(uint16_t));
	memcpy(&(p_response[2]), &(p_err->line_number), sizeof(uint16_t));
	memcpy(&(p_response[4]), &(p_err->err_code), sizeof(uint8_t));
}*/

/**@brief	Sends data page 2 response.
 */
static void on_get_parameter(ant_request_data_page_t* p_request)
{
	// uint8_t subpage;
	// uint8_t response[6];

	// subpage = p_request->descriptor[0];
	// memset(&response, 0, sizeof(response));

	// switch (subpage)
	// {
	// 	case IRT_MSG_SUBPAGE_SETTINGS:
	// 		memcpy(&response, &mp_user_profile->settings, sizeof(uint16_t));
	// 		break;

	// 	case IRT_MSG_SUBPAGE_CRR:
	// 		memcpy(&response, &mp_user_profile->ca_slope, sizeof(uint16_t));
	// 		memcpy(&response[2], &mp_user_profile->ca_intercept, sizeof(uint16_t));
	// 		break;

	// 	case IRT_MSG_SUBPAGE_WEIGHT:
	// 		memcpy(&response, &mp_user_profile->total_weight_kg, sizeof(uint16_t));
	// 		break;

	// 	case IRT_MSG_SUBPAGE_WHEEL_SIZE:
	// 		memcpy(&response, &mp_user_profile->wheel_size_mm, sizeof(uint16_t));
	// 		break;

	// 	/*case IRT_MSG_SUBPAGE_GET_ERROR:
	// 		error_to_response(response);
	// 		break; */

	// 	case IRT_MSG_SUBPAGE_SERVO_OFFSET:
	// 		memcpy(&response, &mp_user_profile->servo_offset, sizeof(int16_t));
	// 		break;

	// 	case IRT_MSG_SUBPAGE_CHARGER:
	// 		response[0] = battery_charge_status();
	// 		break;

	// 	case IRT_MSG_SUBPAGE_FEATURES:
	// 		memcpy(&response, FEATURES, sizeof(uint16_t));
	// 		break;

	// 	case IRT_MSG_SUBPAGE_SERVO_POS:
	// 		// Start index is stored in descriptor[1]
	// 		servo_pos_to_response(p_request, response);
	// 		break;

	// 	case IRT_MSG_SUBPAGE_DRAG:
	// 		memcpy(&response, &mp_user_profile->ca_drag, sizeof(uint32_t));
	// 		break;

	// 	case IRT_MSG_SUBPAGE_RR:
	// 		memcpy(&response, &mp_user_profile->ca_rr, sizeof(uint32_t));
	// 		break;

	// 	case IRT_MSG_SUBPAGE_GAP_OFFSET:
	// 		memcpy(&response, &mp_user_profile->ca_mag_factors.gap_offset, sizeof(uint32_t));
	// 		break;

	// 	default:
	// 		LOG("[MAIN] Unrecognized subpage: %i. \r\n", subpage);
	// 		return;
	// }

	// ant_bp_page2_tx_send(subpage, response, p_request->tx_response);
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

	// ant_bp_page3_tx_send(1, TEMPERATURE, 10, 0, value);
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

	mp_user_profile->settings = SETTING_VALUE(settings);

	/*LOG("[MAIN] Request to update settings to: ACCEL:%i \r\n",
				SETTING_IS_SET(mp_user_profile->settings, SETTING_ACL_SLEEP_ON) );*/

	LOG("[MAIN]:settings_update raw: %i, translated: %i\r\n",
			settings, mp_user_profile->settings);

	if (SETTING_PERSIST(settings))
	{
		LOG("[MAIN] Scheduled persistence of updated settings: %i\r\n",
				mp_user_profile->settings);
		// Schedule update to the profile.
		user_profile_store();
	}
}

/**@brief	Adjusts the crr intercept by the value.
 *			TODO: I don't think this is used, validate.
 */
static void crr_adjust(int8_t value)
{
	ant_request_data_page_t request = {
			.data_page = 0x46, // RequestDataPage -- TODO: this is defined somewhere
			.descriptor[0] = IRT_MSG_SUBPAGE_CRR,
			.tx_page = 0x02, // Get / set parameter page.
			.tx_response = DATA_PAGE_RESPONSE_TYPE
	};

	if (mp_user_profile->ca_intercept != 0xFFFF)
	{
		// Wire value of intercept is sent in 1/1000, i.e. 57236 == 57.236
		mp_user_profile->ca_intercept += (value * 1000);
		bp_queue_data_response(request);
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

	CRITICAL_REGION_ENTER()
	//sd_softdevice_disable();							// Disable the radios, so no new commands come in.
	application_timers_stop();							// Stop running timers.
	//irt_power_meas_fifo_free();							// Free heap allocation.
	peripheral_powerdown(accelerometer_wake_disable);	// Shutdown peripherals.
	CRITICAL_REGION_EXIT()

	sd_power_system_off();								// Enter lower power mode.
}

static void on_resistance_off(void)
{
	// Execute the command.
	resistance_level_set(0);

	// Queue acknowledgment for the command.
	bp_queue_resistance_ack(RESISTANCE_SET_STANDARD, 0);

	// Quick blink for feedback.
	led_set(LED_MODE_STANDARD);
}

// Steps resistance up or down.
static void on_resistance_step(bool increment, bool minor)
{
	uint32_t err_code;

	err_code = resistance_step(increment, minor);

	// Decrement resistance.
	if (err_code == NRF_SUCCESS)
	{
		// Queue acknowledgment.
		acknowledge_resistance();

		// LED indication.
		led_set(increment ? LED_BUTTON_UP : LED_BUTTON_DOWN);
	}
	else
	{
		LOG("[MAIN] on_resistance_step hit min/max.\r\n");
		led_set(LED_MIN_MAX);
	}
}

static void on_button_up(void)
{
	if (calibration_in_progress)
	{
		crr_adjust(CRR_ADJUST_VALUE);
		led_set(LED_BUTTON_UP);
	}
	else
	{
		// Increment resistance.
		on_resistance_step(true, true);
	}
}

static void on_button_down(void)
{
	if (calibration_in_progress)
	{
		crr_adjust(CRR_ADJUST_VALUE*-1);
		led_set(LED_BUTTON_DOWN);
	}
	else
	{
		// Decrement resistance.
		on_resistance_step(false, true);
	}
}

static void on_button_long_down(void)
{
	resistance_step(false, false);
}

static void on_button_long_up(void)
{
	resistance_step(true, false);
}

static void on_button_menu(void)
{
	if (!calibration_in_progress)
	{
		// Start / stop TR whenever the middle button is pushed.
		if ( SETTING_IS_SET(mp_user_profile->settings, SETTING_ANT_TR_PAUSE_ENABLED) )
		{
			bp_queue_resistance_ack(RESISTANCE_START_STOP_TR, 0);
		}

		// Toggle between erg mode.
		toggle_resistance_mode();
	}
}

static void on_button_long_menu(void)
{
	if (!calibration_in_progress)
	{
		// Change into crr mode.
		on_request_calibration();
	}
}

// This is the button on the board.
static void on_button_pbsw(bool long_press)
{
	if (long_press)
	{
		// TODO: this button needs to be debounced and a LONG press should power down.
		LOG("[MAIN] Push button switch pressed (long).\r\n");
		on_enable_dfu_mode();
	}
	else
	{
		LOG("[MAIN] Push button switch pressed (short).\r\n");
		// Shutting device down.
		on_power_down(true);
	}
}

// This event is triggered when there is data to be read from accelerometer.
static void on_accelerometer(void)
{
	uint32_t err_code;

	// Reload the WDT.
	WDT_RELOAD();

	// Clear the struct.
	memset(&m_accelerometer_data, 0, sizeof(m_accelerometer_data));

	// Read event data from the accelerometer.
	err_code = accelerometer_data_get(&m_accelerometer_data);
	APP_ERROR_CHECK(err_code);

	// Received a sleep interrupt from the accelerometer meaning no motion for a while.
	if (m_accelerometer_data.source == ACCELEROMETER_SRC_WAKE_SLEEP)
	{
		//
		// This is a workaround to deal with GPIOTE toggling the SENSE bits which forces
		// the device to wake up immediately after going to sleep without this.
		//
        NRF_GPIO->PIN_CNF[PIN_SHAKE] &= ~GPIO_PIN_CNF_SENSE_Msk;
        NRF_GPIO->PIN_CNF[PIN_SHAKE] |= GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos;

        LOG("[MAIN]:on_accelerometer received sleep signal.\r\n");;
	}
	else
	{
		LOG("[MAIN]:on_accelerometer source:%i y:%i \r\n",
				m_accelerometer_data.source,
				m_accelerometer_data.out_y_lsb |=
						m_accelerometer_data.out_y_msb << 8);
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
	led_set(LED_BLE_CONNECTED);
	LOG("[MAIN]:on_ble_connected\r\n");
}
	
static void on_ble_disconnected(void) 
{
	// Clear connection LED.
	led_set(LED_BLE_DISCONNECTED);
	LOG("[MAIN]:on_ble_disconnected\r\n");

	// Restart advertising.
	ble_advertising_start();
}

static void on_ble_timeout(void) 
{
	LOG("[MAIN]:on_ble_timeout\r\n");
	led_set(LED_BLE_TIMEOUT);
}

static void on_ble_advertising(void)
{
	led_set(LED_BLE_ADVERTISTING);
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
	uint16_t value = 0;

    // Only applies to the older WAHOO messages and not FE-C
    if (rc_evt.operation >= RESISTANCE_SET_PERCENT &&
        rc_evt.operation <= RESISTANCE_START_STOP_TR )
    {
	   value =  uint16_decode(rc_evt.pBuffer);
    }

	LOG("[MAIN]:on_set_resistance {OP:%#.2x,VAL:%i}\r\n",
			(uint8_t)rc_evt.operation, value);
	// 
	// Parse the messages and set state or resistance as appropriate.
	//
	switch (rc_evt.operation)
	{
		case RESISTANCE_SET_STANDARD:
			value = (uint8_t)value;
			resistance_level_set(value);
			break;
			
		case RESISTANCE_SET_PERCENT:
			// Parse the buffer for percentage.
			resistance_pct_set(wahoo_resistance_pct_decode(value));
			break;

		case RESISTANCE_SET_ERG:
			// Assign target watt level.
			resistance_erg_set(value);
			break;

		// This message never seems to come via the apps?
		case RESISTANCE_SET_SIM:
			set_sim_params(rc_evt.pBuffer);
			break;
			
		case RESISTANCE_SET_SLOPE:
			// Parse out the slope.
			resistance_grade_set(wahoo_sim_grade_decode(value));
			break;
			
		case RESISTANCE_SET_WIND:
			// Parse out the wind speed.
			resistance_windspeed_set(wahoo_sim_wind_decode(value));
			break;
			
		case RESISTANCE_SET_WHEEL_CR:
			//  Parse wheel size and update accordingly.
			set_wheel_params(value);
			break;
			
		case RESISTANCE_SET_CRR:
			resistance_crr_set(wahoo_decode_crr(rc_evt.pBuffer));
			break;

		case RESISTANCE_SET_C:
			resistance_c_set(wahoo_decode_c(rc_evt.pBuffer));
			break;

		case RESISTANCE_SET_SERVO_POS:
			// Move the servo to a specific position.
			resistance_position_set(value, false);
			break;

		case RESISTANCE_SET_WEIGHT:
			set_sim_params(rc_evt.pBuffer);
            break;

		case RESISTANCE_SET_BASIC:
            resistance_pct_set(rc_evt.total_resistance);
            break;
            
        case RESISTANCE_SET_TARGET_POWER:
             resistance_erg_set(rc_evt.target_power);
            break;
                    
        case RESISTANCE_SET_FE_WIND:
			// set c, wind speed, drafting factor.
            resistance_c_set(rc_evt.wind.wind_coefficient);
            // Convert to meters per second.
            resistance_windspeed_set(rc_evt.wind.wind_speed_km * 0.277778f);
            // Set drafting factor.
            resistance_drafting_set(rc_evt.wind.drafting_factor);
			break;
			
        case RESISTANCE_SET_TRACK:
        	resistance_grade_set(rc_evt.track.grade);
			resistance_crr_set(rc_evt.track.crr);
			break;

		default:
			break;
	}

	// Queue to send acknowledgment (only used for BLE now).
	bp_queue_resistance_ack(rc_evt.operation, value);
}

// Invoked when a button is pushed on the remote control.
static void on_ant_ctrl_command(ctrl_evt_t evt)
{
	LOG("[MAIN]:on_ant_ctrl_command Command: %i\r\n", evt.command);
	
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
	
	switch (evt.command)
	{
		case ANT_CTRL_BUTTON_UP:
			on_button_up();
			break;

		case ANT_CTRL_BUTTON_DOWN:
			on_button_down();
			break;

		case ANT_CTRL_BUTTON_LONG_UP:
			on_button_long_up();
			break;

		case ANT_CTRL_BUTTON_LONG_DOWN:
			on_button_long_down();
			break;

		case ANT_CTRL_BUTTON_MIDDLE:
			on_button_menu();
			break;

		case ANT_CTRL_BUTTON_LONG_MIDDLE:
			on_button_long_menu();
			break;

		default:
			break;
	}
}

// Called when a relevant bike power message is received. 
static void on_bp_power_data(ant_bp_message_type_e state, uint16_t data)
{
	switch (state)
	{
		case BP_MSG_POWER_DATA:
			m_current_state.instant_power = data;
			// LOG("Watts: %i\r\n", data);
			break;

		case BP_MSG_DEVICE_CONNECTED:
			if (mp_user_profile->power_meter_ant_id != data)
			{
				// TODO: should we schedule profile update??
				LOG("[MAIN] Power Meter Connected: %i\r\n", data);
				mp_user_profile->power_meter_ant_id = data;
			}
			
			m_current_state.power_meter_paired = true;
			break;

		case BP_MSG_DEVICE_SEARCH_TIME_OUT:
		case BP_MSG_DEVICE_CLOSED:
			LOG("[MAIN] Power meter disconnected or search timed out.\r\n");	
			m_current_state.power_meter_paired = false;
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

	/* Hard-coded case for testing error handler.
	if (request.descriptor[0] == 0xFF)
	{
		APP_ERROR_HANDLER(NRF_ERROR_INVALID_PARAM);
	}*/

	switch (request.tx_page)
	{
		// Request is for get/set parameters or measurement output.
		case ANT_PAGE_GETSET_PARAMETERS:
		case ANT_PAGE_MEASURE_OUTPUT:
		case ANT_COMMON_PAGE_80:
		case ANT_COMMON_PAGE_81:
			bp_queue_data_response(request);
			LOG("[MAIN] Request to get data page (subpage): %#x, type:%i\r\n",
					request.descriptor[0],
					request.tx_response);
			break;

		case ANT_PAGE_BATTERY_STATUS:
			LOG("[MAIN] Request to get battery reading\r\n");
			battery_read_start();
			bp_queue_data_response(request);
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
			memcpy(&mp_user_profile->ca_slope, &buffer[IRT_MSG_PAGE2_DATA_INDEX],
					sizeof(uint16_t));
			memcpy(&mp_user_profile->ca_intercept, &buffer[IRT_MSG_PAGE2_DATA_INDEX+2],
					sizeof(uint16_t));
			LOG("[MAIN] Updated slope:%i intercept:%i \r\n",
					mp_user_profile->ca_slope, mp_user_profile->ca_intercept);

			// Remove drag & rr values as they will conflict.
			mp_user_profile->ca_drag = NAN;
			mp_user_profile->ca_rr = NAN;

			// Schedule update to the user profile.
			user_profile_store();
			break;

		case IRT_MSG_SUBPAGE_DRAG:
			// Update co-efficient of drag from calibration.
			memcpy(&mp_user_profile->ca_drag, &buffer[IRT_MSG_PAGE2_DATA_INDEX], sizeof(uint32_t));
			LOG("[MAIN] on_set_parameter ca_drag:%.7f\r\n", mp_user_profile->ca_drag);
			break;

		case IRT_MSG_SUBPAGE_RR:
			// Update co-efficient of rolling resistance from calibration.
			memcpy(&mp_user_profile->ca_rr, &buffer[IRT_MSG_PAGE2_DATA_INDEX], sizeof(uint32_t));
			LOG("[MAIN] on_set_parameter ca_rr:%.7f\r\n", mp_user_profile->ca_rr);

			if (!isnan(mp_user_profile->ca_drag))
			{
				// If we've received both params, schedule a profile update.
				user_profile_store();
			}

			break;

		case IRT_MSG_SUBPAGE_SERVO_OFFSET:
			mp_user_profile->servo_offset = (int16_t)(buffer[IRT_MSG_PAGE2_DATA_INDEX] |
				buffer[IRT_MSG_PAGE2_DATA_INDEX+1] << 8);
			LOG("[MAIN] Updated servo_offset:%i \r\n", mp_user_profile->servo_offset);

			// Schedule update to the user profile.
			user_profile_store();
			break;

		case IRT_MSG_SUBPAGE_POWER_METER_ID:
			mp_user_profile->power_meter_ant_id = (int16_t)(buffer[IRT_MSG_PAGE2_DATA_INDEX] |
				buffer[IRT_MSG_PAGE2_DATA_INDEX+1] << 8);
			LOG("[MAIN] Updated power_meter_ant_id:%i \r\n", mp_user_profile->power_meter_ant_id);			
			// Schedule update to the user profile.
			user_profile_store();
			break;

		case IRT_MSG_SUBPAGE_SLEEP:
			LOG("[MAIN] on_set_parameter: Request device sleep.\r\n");
			on_power_down(true);
			break;

		case IRT_MSG_SUBPAGE_CHARGER:
			if (FEATURE_AVAILABLE(FEATURE_BATTERY_CHARGER))
			{
				// Turns charger on if currently off, else turns off.
				battery_charge_set( (BATTERY_CHARGER_IS_OFF) );

				LOG("[MAIN] on_set_parameter: Toggled battery charger.\r\n");
			}
			else
			{
				LOG("[MAIN] on_set_parameter: No battery charger present.\r\n");
			}
			break;

		case IRT_MSG_SUBPAGE_GAP_OFFSET:
			memcpy(&mp_user_profile->ca_mag_factors.gap_offset, &buffer[IRT_MSG_PAGE2_DATA_INDEX], sizeof(uint16_t));
			LOG("[MAIN] on_set_parameter ca_gap_offset:%i\r\n", mp_user_profile->ca_mag_factors.gap_offset);

			// Schedule update to the user profile.
			user_profile_store();
			break;

			/*
		case IRT_MSG_SUBPAGE_FEATURES:

			err_code = features_store(&buffer[IRT_MSG_PAGE2_DATA_INDEX]);
			APP_ERROR_CHECK(err_code);

			LOG("[MAIN] on_set_parameter: FEATURES STORE NO LONGER SUPPORTED.\r\n");
			break; */

#ifdef SIM_SPEED
		case IRT_MSG_SUBPAGE_DEBUG_SPEED:
			// # of ticks to simulate in debug mode.
			LOG("[MAIN] setting debug speed ticks to: %i\r\n",
					buffer[IRT_MSG_PAGE2_DATA_INDEX]);
			speed_debug_ticks = buffer[IRT_MSG_PAGE2_DATA_INDEX];
			break;
#endif

		default:
			LOG("[MAIN] on_set_parameter: Invalid setting, skipping. \r\n");
			return;
	}
}

/**@brief	Called when a command is received to set servo positions.
 *
 */
static void on_set_servo_positions(servo_positions_t* positions)
{
	if (!resistance_positions_validate(positions))
	{
		LOG("[MAIN] on_set_servo_positions ERROR: invalid servo positions.\r\n");
		return;
	}

	// Save the value.
	mp_user_profile->servo_positions = *positions;

#ifdef ENABLE_DEBUG_LOG
	LOG("[MAIN] on_set_servo_positions count:%i\r\n",
			mp_user_profile->servo_positions.count);

	for (uint8_t i = 0; i <= mp_user_profile->servo_positions.count-1; i++)
	{
		LOG("[MAIN] handle_burst_set_position[%i]: %i\r\n", i,
				mp_user_profile->servo_positions.positions[i]);
	}
#endif

	user_profile_store();
}

/**@brief	Called when a charge status has been indicated.
 *
 */
static void on_charge_status(irt_charger_status_t status)
{
	LOG("[MAIN] on_charge_status: %i\r\n", status);

	switch  (status)
	{
		case BATTERY_CHARGING:
			led_set(LED_CHARGING);
			break;

		case BATTERY_CHARGE_COMPLETE:
			led_set(LED_CHARGED);
			break;

		default:
			led_set(LED_NOT_CHARGING);
			break;
	}
}

/**@brief	Called when the result of the battery is determined.
 *
 */
static void on_battery_result(uint16_t battery_level)
{
	LOG("[MAIN] on_battery_result %i, ticks: %i \r\n",
			battery_level, m_battery_start_ticks);

	m_current_state.battery_status = battery_status(battery_level, m_battery_start_ticks);

	// If we're not plugged in and we have a LOW or CRITICAL status, display warnings.
	if (!peripheral_plugged_in())
	{
		switch (m_current_state.battery_status.status)
		{
			case BAT_LOW:
				// Blink a warning.
				led_set(LED_BATT_LOW);

				break;

			case BAT_CRITICAL:

				// Set the servo to HOME position.
				on_resistance_off();

				// If we're below 6 volts, shut it all the way down.
				if (m_current_state.battery_status.coarse_volt < SHUTDOWN_VOLTS)
				{
					// Indicate and then shut down.
					led_set(LED_BATT_CRITICAL);

					// Critical battery level.
					LOG("[MAIN] on_battery_result critical low battery coarse volts: %i, powering down.\r\n",
							m_current_state.battery_status.coarse_volt);
					nrf_delay_ms(3000);
					on_power_down(false);
				}
				else
				{
					// Blink a warning.
					led_set(LED_BATT_WARN);

					// Critical battery level.
					LOG("[MAIN] on_battery_result critical low battery coarse volts: %i, displaying warning.\r\n",
							m_current_state.battery_status.coarse_volt);

					// Turn all extra power off.
					peripheral_low_power_set();
				}
				break;

			default:
				break;
		}
	}
}

/**@brief	When the display requests calibration to begin.
 *
 */
static void on_request_calibration()
{
    // Force standard level 0 resistance.
	// TODO: This should send a standard ack indicating changed state (if it has).
	resistance_level_set(0);
	calibration_start();
}

/**@brief	Called when the magnet calibration is received.
 *
 */
static void on_set_mag_calibration(mag_calibration_factors_t* p_factors)
{
	LOG("[MAIN] on_set_mag_calibration: received magnet factors: %i, %i\r\n%.5f,%.5f,%.5f,%.5f",
			p_factors->low_speed_mps, p_factors->high_speed_mps,
			p_factors->low_factors[0],
			p_factors->low_factors[1],
			p_factors->low_factors[2],
			p_factors->low_factors[3]);

	// Update profile.
	memcpy(&mp_user_profile->ca_mag_factors, p_factors, 
		sizeof(mag_calibration_factors_t));
	user_profile_store(); 
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
    //APP_ERROR_CHECK(err_code);
}


/**@brief	Check if there is a reset reason and log if enabled.
 */
static uint32_t check_reset_reason()
{
	uint32_t reason, gpreg;
	//irt_error_log_data_t* p_error;

	// Read the reset reason
	reason = NRF_POWER->RESETREAS;

	// Reset the battery clock ONLY if the reset reason was NOT from an
	// interrupt or watch dog timer.
	if ( ((POWER_RESETREAS_OFF_Msk | POWER_RESETREAS_DOG_Msk) & reason) == 0)
	{
		// Reset the battery clock.
		m_battery_start_ticks = 0;
	}

	LOG("[MAIN]:Battery Ticks: %i\r\n", m_battery_start_ticks);

	if (reason > 0)
	{
		// Clear the reason by writing 1 bit.
		NRF_POWER->RESETREAS = reason;
		LOG("[MAIN]:Reset reason: %#08lx\r\n", reason);

		// If reset from the WDT, we timed out from no activity or system hang.
		if (reason & POWER_RESETREAS_DOG_Msk)
		{
			LOG("[MAIN] WDT timeout caused reset, enabling interrupts and shutting down.\r\n");

			// Initialize the status LEDs, which ensures they are off.
			led_init();

			// Enable interrupts that will wake the system since retained registers are reset on WDT reset.
			peripheral_wakeup_set();

			// Shut the system down.
			NRF_POWER->SYSTEMOFF = 1;
		}

		if (reason & POWER_RESETREAS_OFF_Msk)
		{
			// Reset was because of a SENSE input, print out the current state.
			LOG("[MAIN] Pins: %u\r\n", NRF_GPIO->IN);
		}
	}
	else
	{
		//  A "cold" start, first time after the battery was unplugged.
		LOG("[MAIN]:Normal power on.\r\n");
	}

	gpreg = NRF_POWER->GPREGRET;
	// Check and see if the device last reset due to error.
	if (gpreg == IRT_GPREG_ERROR)
	{
		// Log the occurrence.
		//p_error = irt_error_last();

		LOG("[MAIN]:check_reset_reason Resetting from previous error.\r\n");
		/*LOG("\t{COUNT: %i, ERROR: %#.8x}: %s:%lu\r\n",
				p_error->failure,
				p_error->err_code,
				p_error->message,
				p_error->line_number); */

		// Now clear the register.
		NRF_POWER->GPREGRET = 0;
	}
	else
	{
		// Initialize the error structure.
		//irt_error_init();
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
	SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_4000MS_CALIBRATION, true);

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
	LOG("[MAIN]:Features: %i, addr:%#.8x\r\n", *FEATURES, FEATURES);
	LOG("**********************************************************************\r\n");

	// Determine what the reason for startup is and log appropriately.
	check_reset_reason();

	// Initialize the watchdog timer.
	wdt_init();

    // Initialize the scheduler, make sure to init before timers.
	scheduler_init();

	// Initialize timers.
	timers_init();

	// Peripheral interrupt event handlers.
	static peripheral_evt_t on_peripheral_handlers = {
		on_button_pbsw,
		on_accelerometer,
		on_power_plug,
		on_battery_result,
		on_charge_status
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
		on_set_parameter,
		on_set_servo_positions,
		on_request_calibration,
		on_set_mag_calibration,
		on_bp_power_data
	};

	// Initialize and enable the softdevice.
	s310_init();

	// Configure the chip's power options (after sd enabled).
	config_dcpower();

	// Initializes the Bluetooth and ANT stacks.
	ble_ant_init(&ant_ble_handlers, mp_user_profile->power_meter_ant_id);

	// Initialize the user profile.
	mp_user_profile = user_profile_get();

	// Initialize the magnet module.
	magnet_init(&mp_user_profile->ca_mag_factors);

	// Initialize resistance module and initial values.
	mp_resistance_state = resistance_init(PIN_SERVO_SIGNAL, mp_user_profile, 
		&m_current_state);

	// Initialize module to read speed from flywheel.
	speed_init(PIN_FLYWHEEL, mp_user_profile);

	// Initialize power module with user profile.
	power_init(mp_user_profile);

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
