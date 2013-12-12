/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup ble_sdk_app_hrs_eval_main main.c
 * @{
 * @ingroup ble_sdk_app_hrs_eval
 * @brief Main file for Heart Rate Service Sample Application for nRF51822 evaluation board
 *
 * This file contains the source code for a sample application using the Heart Rate service
 * (and also Battery and Device Information services) for the nRF51822 evaluation board (PCA10001).
 * This application uses the @ref ble_sdk_lib_conn_params module.
 */

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_bas.h"
#include "ble_hrs.h"
#include "ble_dis.h"
#include "ble_cps.h"
#include "ble_conn_params.h"
#include "ble_eval_board_pins.h"
#include "ble_stack_handler.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "led.h"
#include "battery.h"
#include "ble_bondmngr.h"
#include "app_gpiote.h"
#include "app_button.h"
#include "ble_radio_notification.h"
#include "ble_flash.h"
#include "ble_debug_assert_handler.h"
#include "nrf_delay.h"
#include "boards.h"

#include "insideride.h"
#include "resistance.h"
#include "speed.h"
#include "power.h"

#define HR_INC_BUTTON_PIN_NO                 EVAL_BOARD_BUTTON_0                       /**< Button used to increment heart rate. */
#define HR_DEC_BUTTON_PIN_NO                 EVAL_BOARD_BUTTON_1                       /**< Button used to decrement heart rate. */
#define BONDMNGR_DELETE_BUTTON_PIN_NO        HR_DEC_BUTTON_PIN_NO                      /**< Button used for deleting all bonded masters during startup. */

#define DEVICE_NAME                          "Nordic_HRM"                              /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME                    "NordicSemiconductor"                     /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                     40                                        /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS           180                                       /**< The advertising timeout in units of seconds. */

#define APP_TIMER_PRESCALER                  0                                         /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS                 4                                         /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE              5                                         /**< Size of timer operation queues. */

#define BATTERY_LEVEL_MEAS_INTERVAL          APP_TIMER_TICKS(2000, APP_TIMER_PRESCALER)/**< Battery level measurement interval (ticks). */

#define HEART_RATE_MEAS_INTERVAL             APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER)/**< Heart rate measurement interval (ticks). */
#define MIN_HEART_RATE                       60                                        /**< Minimum heart rate as returned by the simulated measurement function. */
#define MAX_HEART_RATE                       300                                       /**< Maximum heart rate as returned by the simulated measurement function. */
#define HEART_RATE_CHANGE                    2                                         /**< Value by which the heart rate is incremented/decremented during button press. */

#define APP_GPIOTE_MAX_USERS                 2                                         /**< Maximum number of users of the GPIOTE handler. */

#define BUTTON_DETECTION_DELAY               APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)  /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define MIN_CONN_INTERVAL                    MSEC_TO_UNITS(500, UNIT_1_25_MS)          /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL                    MSEC_TO_UNITS(1000, UNIT_1_25_MS)         /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                        0                                         /**< Slave latency. */
#define CONN_SUP_TIMEOUT                     MSEC_TO_UNITS(4000, UNIT_10_MS)           /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY       APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)/**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY        APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)/**< Time between each call to sd_ble_gap_conn_param_update after the first (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT         3                                         /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_TIMEOUT                    30                                        /**< Timeout for Pairing Request or Security Request (in seconds). */
#define SEC_PARAM_BOND                       1                                         /**< Perform bonding. */
#define SEC_PARAM_MITM                       0                                         /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES            BLE_GAP_IO_CAPS_NONE                      /**< No I/O capabilities. */
#define SEC_PARAM_OOB                        0                                         /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE               7                                         /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE               16                                        /**< Maximum encryption key size. */

#define FLASH_PAGE_SYS_ATTR                  (BLE_FLASH_PAGE_END - 3)                  /**< Flash page used for bond manager system attribute information. */
#define FLASH_PAGE_BOND                      (BLE_FLASH_PAGE_END - 1)                  /**< Flash page used for bond manager bonding information. */

#define DEAD_BEEF                            0xDEADBEEF                                /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

/*****************************************************************************
* InsideRide defines.
*****************************************************************************/
#define PIN_SERVO_SIGNAL		3		// P3 - P0.03
#define PIN_BUTTON_II				2		// P3 - P0.02
#define PIN_BUTTON_III 			1		// P3 - P0.01
#define PIN_DRUM_REV 				0		// P3 - P0.00 

#define CYCLING_POWER_MEAS_INTERVAL             APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER)/**< Heart rate measurement interval (ticks). */
/*****************************************************************************/

/*****************************************************************************
* InsideRide managed state.
*****************************************************************************/
static volatile uint8_t m_resistance_level 		= 0;			// Current mag resistance.
static volatile uint16_t m_accum_torque 			= 0;					// Tracks accumlated torque.
static volatile uint16_t m_last_seconds_2048 	= 0;		// Last time of reported event in 1/2048th seconds.
static volatile uint32_t m_accum_wheel_revs 	= 0;			// Last count of wheel revolutions.

static ble_gap_sec_params_t                  m_sec_params;                             /**< Security requirements for this application. */
static ble_gap_adv_params_t                  m_adv_params;                             /**< Parameters to be passed to the stack when starting advertising. */
ble_bas_t                                    bas;                                      /**< Structure used to identify the battery service. */
static ble_hrs_t                             m_hrs;                                    /**< Structure used to identify the heart rate service. */
static volatile uint16_t                     m_cur_heart_rate;                         /**< Current heart rate value. */

static app_timer_id_t                        m_battery_timer_id;                       /**< Battery timer. */
static app_timer_id_t                        m_heart_rate_timer_id;                    /**< Heart rate measurement timer. */

static ble_cps_t                             m_cps;                                    /**< Structure used to identify the cycling power service. */

static void ble_evt_dispatch(ble_evt_t * p_ble_evt);

/*****************************************************************************
* InsideRide functions.
*****************************************************************************/

static volatile uint32_t last_report_ticks = 0;

/*@brief 	Retuns the count of 1/2048th seconds (2048 per second) since the 
 *				the counter started.  
 *
 *@note		This value rolls over at 32 seconds.
 */
static uint16_t get_seconds_2048()
{
	// TODO: Optimize this out - only need to figure this out once.
	
	// RTC1 is based on a 32.768khz crystal, or in other words it oscillates
	// 32768 times per second.  The PRESCALER determins how often a tick gets
	// counted.  With a prescaler of 0, there are 32,768 ticks in 1 second
	// 1/2048th of a second would be 16 ticks (32768/2048)
	// # of 2048th's would then be ticks / 16.
	float freq = 32768/(NRF_RTC1->PRESCALER+1);

	// Get current tick count.
	uint32_t ticks = NRF_RTC1->COUNTER;

	// Based on frequence of ticks, calculate 1/2048 seconds.
	// freq (hz) = times per second.
	uint16_t seconds_2048 = ROUNDED_DIV(ticks, (freq / 2048));

	return seconds_2048;
}

/*****************************************************************************
* Error Handling Functions
*****************************************************************************/


/**@brief Function for error handling, which is called when an error has occurred. 
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


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for handling a Bond Manager error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void bond_manager_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/*****************************************************************************
* Static Timeout Handling Functions
*****************************************************************************/

/**@brief Function for handling the Battery measurement timer timeout.
 *
 * @details This function will be called each time the battery level measurement timer expires.
 *          This function will start the ADC.
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void battery_level_meas_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);
    battery_start();
}



/**@brief Function for handling the Heart rate measurement timer timeout.
 *
 * @details This function will be called each time the heart rate measurement timer expires.
 *          It will exclude RR Interval data from every third measurement.
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void heart_rate_meas_timeout_handler(void * p_context)
{
    uint32_t err_code;

    UNUSED_PARAMETER(p_context);

/*
    err_code = ble_hrs_heart_rate_measurement_send(&m_hrs, m_cur_heart_rate);

    if (
        (err_code != NRF_SUCCESS)
        &&
        (err_code != NRF_ERROR_INVALID_STATE)
        &&
        (err_code != BLE_ERROR_NO_TX_BUFFERS)
        &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
    )
    {
        APP_ERROR_HANDLER(err_code);
    }
	*/	
		//
		// Also send the instant power data.
		//
		ble_cps_meas_t cps_meas;
		memset(&cps_meas, 0, sizeof(cps_meas));
		
		//
		// Time period.
		//
		uint16_t 	seconds_2048 	=	get_seconds_2048();
		// Calculate the amount of time in 1/2048th seconds since last reporting event.
		uint16_t period_seconds_2048 = seconds_2048 - m_last_seconds_2048;
		// Update the last event time.
		m_last_seconds_2048 = seconds_2048;
		
		// Get the last wheel revolution count which is cumulative since the start.
		uint32_t 	accum_revs		= get_wheel_revolutions();
		uint32_t 	period_revs		= 0;
		
		if (m_accum_wheel_revs == 0)
		{
			// Handle first count scenario.
			m_accum_wheel_revs 	= accum_revs;
			period_revs 				= accum_revs;
		} 
		else
		{
			// Calculate revs since last reported.
			period_revs = accum_revs - m_accum_wheel_revs;
			// Set last recorded #.
			m_accum_wheel_revs = accum_revs;
		}
		
		float 		speed 				= get_speed_mph(period_revs, period_seconds_2048);				
		int16_t		watts					= 0;
		uint16_t 	torque				= 0;
		
		const float	total_weight = 175.0f;	// Hard coded value for now.
		
		err_code = calc_power(speed, total_weight, m_resistance_level, &watts);
		// TODO: Handle the error for real here, not sure what the overall error 
		// handling strategy will be, but this is not critical, just move on.
		if (err_code != IRT_SUCCESS)
			return;		
		
		err_code = calc_torque(watts, period_seconds_2048, &torque);
		if (err_code != IRT_SUCCESS)
			return;
		
		// Store accumulated torque for the session.
		m_accum_torque += torque;
		
		cps_meas.instant_power = watts;
		cps_meas.accum_torque = 0; // m_accum_torque; 
		cps_meas.accum_wheel_revs = accum_revs;
		cps_meas.last_wheel_event_time = m_last_seconds_2048;

		err_code = ble_cps_cycling_power_measurement_send(&m_cps, &cps_meas);

    if (
        (err_code != NRF_SUCCESS)
        &&
        (err_code != NRF_ERROR_INVALID_STATE)
        &&
        (err_code != BLE_ERROR_NO_TX_BUFFERS)
        &&
        (err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
    )
    {
        APP_ERROR_HANDLER(err_code);
    }
}

void on_button_ii_event(void)
{
	blink_led();
	// decrement
	if (m_resistance_level > 0)
		set_resistance(--m_resistance_level);	
}

//
// Occurs when button (III) is pressed.
//
void on_button_iii_event(void)
{
	// increment
	if (m_resistance_level < (MAX_RESISTANCE_LEVELS-1))
		set_resistance(++m_resistance_level);
}

/**@brief Function for handling button events.
 *
 * @param[in]   pin_no   The pin number of the button pressed.
 */
static void button_event_handler(uint8_t pin_no)
{
    switch (pin_no)
    {
        case HR_INC_BUTTON_PIN_NO:
            // Increase Heart Rate measurement
            m_cur_heart_rate += HEART_RATE_CHANGE;
            if (m_cur_heart_rate > MAX_HEART_RATE)
            {
                m_cur_heart_rate = MIN_HEART_RATE; // Loop back
            }
            break;
            
        case HR_DEC_BUTTON_PIN_NO:
            // Decrease Heart Rate measurement
            m_cur_heart_rate -= HEART_RATE_CHANGE;
            if (m_cur_heart_rate < MIN_HEART_RATE)
            {
                m_cur_heart_rate = MAX_HEART_RATE; // Loop back
            }
            break;
						
				case PIN_BUTTON_II:
						on_button_ii_event();
						break;

				case PIN_BUTTON_III:
						on_button_iii_event();
						break;

        default:
            APP_ERROR_HANDLER(pin_no);
    }
}

/*@brief	Event handler that the cycling power service calls when a set resistance
 *				command is received.
 *
 */
static void cps_set_resistance_handler(ble_cps_t * p_cps, ble_cps_rc_evt_t * p_evt)
{
	switch (p_evt->resistance_mode)
	{
		case BLE_CPS_RESISTANCE_SET_STANDARD:
			m_resistance_level = (uint8_t)p_evt->p_value[0];
			set_resistance(m_resistance_level);
			break;
			/*
		case BLE_CPS_RESISTANCE_SET_PERCENT:
			set_resistance_pct(x);
			break;
			
		case BLE_CPS_RESISTANCE_SET_ERG:
			set_resistance_erg(x);
			break;
			
		case BLE_CPS_RESISTANCE_SET_SLOPE:
			set_resistance_slope(x);
			break;
			
		case BLE_CPS_RESISTANCE_SET_WIND:
			set_resistance_wind(x);
			break;
			*/
		default:
			break;
	}
}

/*****************************************************************************
* Static Initialization Functions
*****************************************************************************/

/**@brief Function for the Timer initialization.
 *
* @details Initializes the timer module. This creates and starts application timers.
*/
static void timers_init(void)
{
    uint32_t err_code;

    // Initialize timer module
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);

    // Create timers
    err_code = app_timer_create(&m_battery_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                battery_level_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_heart_rate_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                heart_rate_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the GAP initialization.
 *
 * @details This function shall be used to setup all the necessary GAP (Generic Access Profile)
 *          parameters of the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
    uint8_t       flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    ble_uuid_t adv_uuids[] =
    {
        {BLE_UUID_HEART_RATE_SERVICE,         BLE_UUID_TYPE_BLE},
        {BLE_UUID_BATTERY_SERVICE,            BLE_UUID_TYPE_BLE},
        {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE},
				{BLE_UUID_CYCLING_POWER_SERVICE, 			BLE_UUID_TYPE_BLE}
    };

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags.size              = sizeof(flags);
    advdata.flags.p_data            = &flags;
    advdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = adv_uuids;

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);

    // Initialize advertising parameters (used when starting advertising)
    memset(&m_adv_params, 0, sizeof(m_adv_params));
    
    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    m_adv_params.p_peer_addr = NULL;                           // Undirected advertisement
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = APP_ADV_INTERVAL;
    m_adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;
}


/**@brief Function for initializing the services that will be used by the application.
 *
 * @details Initialize the Heart Rate, Battery and Device Information services.
 */
static void services_init(void)
{
    uint32_t       err_code;
    ble_hrs_init_t hrs_init;
    ble_bas_init_t bas_init;
    ble_dis_init_t dis_init;
		ble_cps_init_t cps_init;
    uint8_t        body_sensor_location;

    // Initialize Heart Rate Service
    body_sensor_location = BLE_HRS_BODY_SENSOR_LOCATION_FINGER;

    memset(&hrs_init, 0, sizeof(hrs_init));

    hrs_init.is_sensor_contact_supported = false;
    hrs_init.p_body_sensor_location      = &body_sensor_location;

    // Here the sec level for the Heart Rate Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&hrs_init.hrs_hrm_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hrs_init.hrs_hrm_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hrs_init.hrs_hrm_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&hrs_init.hrs_bsl_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hrs_init.hrs_bsl_attr_md.write_perm);

    err_code = ble_hrs_init(&m_hrs, &hrs_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Battery Service
    memset(&bas_init, 0, sizeof(bas_init));

    // Here the sec level for the Battery Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bas_init.battery_level_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_report_read_perm);

    bas_init.evt_handler          = NULL;
    bas_init.support_notification = true;
    bas_init.p_report_ref         = NULL;
    bas_init.initial_batt_level   = 100;

    err_code = ble_bas_init(&bas, &bas_init);
    APP_ERROR_CHECK(err_code);

    // Initialize Device Information Service
    memset(&dis_init, 0, sizeof(dis_init));

    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, MANUFACTURER_NAME);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
		
		//
		// Initialize Cycling Power Service
		//
		uint8_t sensor_location = BLE_CPS_SENSOR_LOCATION_REAR_WHEEL;
		
		memset(&cps_init, 0, sizeof(cps_init));
		
		cps_init.p_sensor_location = &sensor_location;
		/*cps_init.feature = BLE_CPS_FEATURE_ACCUMULATED_TORQUE_BIT 
														| BLE_CPS_FEATURE_WHEEL_REV_BIT;*/
		cps_init.feature = BLE_CPS_FEATURE_WHEEL_REV_BIT;
		cps_init.evt_handler = cps_set_resistance_handler;

    // Here the sec level for the Cycling Power Service can be changed/increased.
		// TODO: all of this could be put into the ble_cps_init function, no need because
		// we're never going to change the permission level, it's as defined in the 
		// bluetooth specification for the service.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cps_init.cps_sl_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&cps_init.cps_sl_attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cps_init.cps_cpf_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&cps_init.cps_cpf_attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cps_init.cps_cpm_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&cps_init.cps_cpm_attr_md.write_perm);

		err_code = ble_cps_init(&m_cps, &cps_init);
		APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the security parameters.
 */
static void sec_params_init(void)
{
    m_sec_params.timeout      = SEC_PARAM_TIMEOUT;
    m_sec_params.bond         = SEC_PARAM_BOND;
    m_sec_params.mitm         = SEC_PARAM_MITM;
    m_sec_params.io_caps      = SEC_PARAM_IO_CAPABILITIES;
    m_sec_params.oob          = SEC_PARAM_OOB;  
    m_sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    m_sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = m_hrs.hrm_handles.cccd_handle;
    cp_init.disconnect_on_fail             = true;
    cp_init.evt_handler                    = NULL;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the Bond Manager initialization.
 */
static void bond_manager_init(void)
{
    uint32_t            err_code;
    ble_bondmngr_init_t bond_init_data;
    bool                bonds_delete;

    // Clear all bonded masters if the Bonds Delete button is pushed
    err_code = app_button_is_pushed(BONDMNGR_DELETE_BUTTON_PIN_NO, &bonds_delete);
    APP_ERROR_CHECK(err_code);

    // Initialize the Bond Manager
    bond_init_data.flash_page_num_bond     = FLASH_PAGE_BOND;
    bond_init_data.flash_page_num_sys_attr = FLASH_PAGE_SYS_ATTR;
    bond_init_data.evt_handler             = NULL;
    bond_init_data.error_handler           = bond_manager_error_handler;
    bond_init_data.bonds_delete            = bonds_delete;

    err_code = ble_bondmngr_init(&bond_init_data);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Radio Notification event.
 */
static void radio_notification_init(void)
{
    uint32_t err_code;

    err_code = ble_radio_notification_init(NRF_APP_PRIORITY_HIGH,
                                           NRF_RADIO_NOTIFICATION_DISTANCE_4560US,
                                           ble_flash_on_radio_active_evt);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    BLE_STACK_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM,
                           BLE_L2CAP_MTU_DEF,
                           ble_evt_dispatch,
                           false);
}


/**@brief Function for initializing the GPIOTE module.
 */
static void gpiote_init(void)
{
    APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);
}


/**@brief Function for initializing the button module.
 */
static void buttons_init(void)
{
    // Configure HR_INC_BUTTON_PIN_NO and HR_DEC_BUTTON_PIN_NO as wake up buttons and also configure
    // for 'pull up' because the eval board does not have external pull up resistors connected to
    // the buttons.
    static app_button_cfg_t buttons[] =
    {
        {HR_INC_BUTTON_PIN_NO, false, NRF_GPIO_PIN_PULLUP, button_event_handler},
        {HR_DEC_BUTTON_PIN_NO, false, NRF_GPIO_PIN_PULLUP, button_event_handler},  // Note: This pin is also BONDMNGR_DELETE_BUTTON_PIN_NO
				{PIN_BUTTON_II, false, NRF_GPIO_PIN_PULLUP, button_event_handler},
				{PIN_BUTTON_III, false, NRF_GPIO_PIN_PULLUP, button_event_handler}
    };
    
    APP_BUTTON_INIT(buttons, sizeof(buttons) / sizeof(buttons[0]), BUTTON_DETECTION_DELAY, false);
}


/**@brief Function for checking if this is the first start, or if it was a restart due to a pushed button.
 */
static bool is_first_start(void)
{
    uint32_t err_code;
    bool     inc_button_pushed;
    bool     dec_button_pushed;
    
    err_code = app_button_is_pushed(HR_INC_BUTTON_PIN_NO, &inc_button_pushed);
    APP_ERROR_CHECK(err_code);
    
    err_code = app_button_is_pushed(HR_DEC_BUTTON_PIN_NO, &dec_button_pushed);
    APP_ERROR_CHECK(err_code);
    
    return (!inc_button_pushed && !dec_button_pushed);
}


/*****************************************************************************
* Static Start Functions
*****************************************************************************/

/**@brief Function for starting the application timers.
 */
static void application_timers_start(void)
{
    uint32_t err_code;

    // Start application timers
    err_code = app_timer_start(m_battery_timer_id, BATTERY_LEVEL_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_heart_rate_timer_id, CYCLING_POWER_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

static void application_timers_stop(void)
{
		uint32_t err_code;
		
		err_code = app_timer_stop(m_battery_timer_id);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_stop(m_heart_rate_timer_id);
    APP_ERROR_CHECK(err_code);	
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    uint32_t err_code;

    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);

    //led_start();
}


/**@brief Function for putting the chip in System OFF Mode
 */
static void system_off_mode_enter(void)
{
	/*
    uint32_t err_code;
    
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
	*/
}


/*****************************************************************************
* Static Event Handling Functions
*****************************************************************************/

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t        err_code      = NRF_SUCCESS;
    static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            nrf_gpio_port_write(LED_PORT, 1 << (LED_OFFSET));	
            
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            
            // Initialize the current heart rate to the average of max and min values. So that
            // everytime a new connection is made, the heart rate starts from the same value.
            m_cur_heart_rate = (MAX_HEART_RATE + MIN_HEART_RATE) / 2;

            // Start timers used to generate battery and HR measurements.
            application_timers_start();

            // Start handling button presses
            err_code = app_button_enable();
            break;

        case BLE_GAP_EVT_DISCONNECTED:
						nrf_gpio_port_write(LED_PORT, 0 << (LED_OFFSET));	
						
						// Stop broadcasting data.
						application_timers_stop();
						
						
            // Since we are not in a connection and have not started advertising, store bonds
            //err_code = ble_bondmngr_bonded_masters_store();
            //APP_ERROR_CHECK(err_code);

            // Go to system-off mode, should not return from this function, wakeup will trigger
            // a reset.
						
						// start advertising again?
						advertising_start();
						
            system_off_mode_enter();
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, 
                                                BLE_GAP_SEC_STATUS_SUCCESS, 
                                                &m_sec_params);
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT)
            {
							/*
                led_stop();
                GPIO_WAKEUP_BUTTON_CONFIG(HR_INC_BUTTON_PIN_NO);
                GPIO_WAKEUP_BUTTON_CONFIG(HR_DEC_BUTTON_PIN_NO);
                system_off_mode_enter();
								*/
            }
            break;

        default:
            break;
    }

    APP_ERROR_CHECK(err_code);
}


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the BLE Stack event interrupt handler after a BLE stack
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    ble_bondmngr_on_ble_evt(p_ble_evt);
    ble_hrs_on_ble_evt(&m_hrs, p_ble_evt);
		ble_cps_on_ble_evt(&m_cps, p_ble_evt);
    ble_bas_on_ble_evt(&bas, p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    on_ble_evt(p_ble_evt);
}

/*****************************************************************************
* Main Function
*****************************************************************************/

/**@brief Function for the application main entry.
 */
int main(void)
{
    uint32_t err_code;

		m_resistance_level = 0;

		init_led();
		blink_led();
		blink_led2();
		
    timers_init();
    gpiote_init();
    buttons_init();
		
		/*
    if (is_first_start())
    {
        // The startup was not because of button presses. This is the first start.
        // Go into System-Off mode.
        // NOTE: This register cannot be set directly after ble_stack_init() because the SoftDevice
        //       will be enabled.
        GPIO_WAKEUP_BUTTON_CONFIG(HR_INC_BUTTON_PIN_NO);
        GPIO_WAKEUP_BUTTON_CONFIG(HR_DEC_BUTTON_PIN_NO);
        NRF_POWER->SYSTEMOFF = 1;
    }
		*/

    bond_manager_init();
    ble_stack_init();
    radio_notification_init();

    // Initialize Bluetooth Stack parameters
    gap_params_init();
    advertising_init();
    services_init();
    conn_params_init();
    sec_params_init();

    // Actually start advertising
    advertising_start();

		set_resistance(m_resistance_level);	
		init_speed(PIN_DRUM_REV, 2070);
		app_button_enable();

    // Enter main loop
    for (;;)
    {
			/*
        // Switch to a low power state until an event is available for the application
        err_code = sd_app_event_wait();
        APP_ERROR_CHECK(err_code);
			*/
    }
}

/**
 * @}
 */
