/****************************************************************************** 
 * Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * @file  	ble_ant.c
 * @brief		This file contains all implementation details specific to 
 *					initializing the BLE and ANT stacks.
 */

/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
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

#include <stdint.h>
#include <string.h>
#include "ble_ant.h"
#include "nordic_common.h"
#include "nrf.h"
#include "softdevice_handler.h"
#include "ant_stack_handler_types.h"
#include "app_error.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_bas.h"
#include "ble_hrs.h"
#include "ble_dis.h"
#include "ble_conn_params.h"
#include "ble_sensorsim.h"
#include "ble_bondmngr.h"
#include "app_timer.h"
#include "ant_parameters.h"
#include "ant_interface.h"
#include "nrf_soc.h"
#include "pstorage.h"
#include "irt_peripheral.h"
#include "irt_emotion.h"
#include "ant_bike_power.h"
#include "ble_nus.h"
#include "ble_cps.h"
#include "ble_gap.h"

#define APP_ADV_INTERVAL                40                                           /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      180                                          /**< The advertising timeout in units of seconds. */

#define SECOND_1_25_MS_UNITS            800                                          /**< Definition of 1 second, when 1 unit is 1.25 ms. */
#define SECOND_10_MS_UNITS              100                                          /**< Definition of 1 second, when 1 unit is 10 ms. */
#define MIN_CONN_INTERVAL               (SECOND_1_25_MS_UNITS / 2)                   /**< Minimum acceptable connection interval (0.5 seconds), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               (SECOND_1_25_MS_UNITS)                       /**< Maximum acceptable connection interval (1 second), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                   0                                            /**< Slave latency. */
#define CONN_SUP_TIMEOUT                (4 * SECOND_10_MS_UNITS)                     /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)   /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)   /**< Time between each call to sd_ble_gap_conn_param_update after the first (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                            /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_TIMEOUT               30                                           /**< Timeout for Pairing Request or Security Request (in seconds). */
#define SEC_PARAM_BOND                  1                                            /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                            /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                         /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                            /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                            /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                           /**< Maximum encryption key size. */

#define FLASH_PAGE_SYS_ATTR             (NRF_FICR->CODESIZE - 3)                     /**< Flash page used for bond manager system attribute information. */
#define FLASH_PAGE_BOND                 (NRF_FICR->CODESIZE - 1)                     /**< Flash page used for bond manager bonding information. */

#define DEAD_BEEF                       0xDEADBEEF                                   /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;     /**< Handle of the current connection. */
static bool                             m_is_advertising = false;                    /**< True when in advertising state, False otherwise. */
static ble_gap_sec_params_t             m_sec_params;                                /**< Security requirements for this application. */
static ble_gap_adv_params_t             m_adv_params;                                /**< Parameters to be passed to the stack when starting advertising. */
static ble_hrs_t                        m_hrs;                                       /**< Structure used to identify the heart rate service. */
#if defined(BLE_NUS_ENABLED)
static ble_nus_t                       	m_nus;																			 // BLE UART service for debugging purposes.
#endif
static ble_cps_t                        m_cps;                                    	 /**< Structure used to identify the cycling power service. */

static ant_ble_evt_handlers_t * 				mp_ant_ble_evt_handlers;

static void uart_data_handler(ble_nus_t * p_nus, uint8_t * data, uint16_t length)
{
		mp_ant_ble_evt_handlers->on_ble_uart(data, length);
}

/**@brief Assert macro callback function.
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


/**@brief GAP initialization.
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
    
    err_code = sd_ble_gap_device_name_set(&sec_mode, (uint8_t const*)DEVICE_NAME, strlen(DEVICE_NAME));
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


/**@brief Advertising functionality initialization.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;
		ble_advdata_t scanrsp;
    uint8_t       flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    
    ble_uuid_t adv_uuids[] = 
    {
        {BLE_UUID_HEART_RATE_SERVICE,         BLE_UUID_TYPE_BLE}, 
        {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE},
				{BLE_UUID_CYCLING_POWER_SERVICE, 			BLE_UUID_TYPE_BLE}
#if defined(BLE_NUS_ENABLED)
				,{BLE_UUID_NUS_SERVICE, 								m_nus.uuid_type}
#endif
    };

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));
    
    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags.size              = sizeof(flags);
    advdata.flags.p_data            = &flags;

    memset(&scanrsp, 0, sizeof(scanrsp));
    scanrsp.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    scanrsp.uuids_complete.p_uuids  = adv_uuids;
    
    err_code = ble_advdata_set(&advdata, &scanrsp);
    APP_ERROR_CHECK(err_code);
		
    // Initialize advertising parameters (used when starting advertising)
    memset(&m_adv_params, 0, sizeof(m_adv_params));
    
    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    m_adv_params.p_peer_addr = NULL;                           // Undirected advertisement
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = APP_ADV_INTERVAL;
    m_adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;		
}

static void ble_dis_service_init()
{
		uint32_t       err_code;
		ble_dis_init_t dis_init;
    
		// Initialize Device Information Service
    memset(&dis_init, 0, sizeof(dis_init));
    
    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, MANUFACTURER_NAME);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);	
}

static void ble_hrs_service_init()
{
    uint32_t       err_code;
    ble_hrs_init_t hrs_init;
    uint8_t        body_sensor_location;
    
    // Initialize Heart Rate Service
    body_sensor_location = BLE_HRS_BODY_SENSOR_LOCATION_FINGER;
    
    memset(&hrs_init, 0, sizeof(hrs_init));
    
    hrs_init.evt_handler                 = NULL;
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
}

static void ble_nus_service_init()
{
#if defined(BLE_NUS_ENABLED)
		uint32_t       err_code;
		ble_nus_init_t nus_init;
    
    memset(&nus_init, 0, sizeof nus_init);
    nus_init.data_handler = uart_data_handler;
    
    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);	
#endif
}

static void ble_cps_service_init()
{
		uint32_t       err_code;
		ble_cps_init_t cps_init;
		
		//
		// Initialize Cycling Power Service
		//
		uint8_t sensor_location = BLE_CPS_SENSOR_LOCATION_REAR_WHEEL;
		
		memset(&cps_init, 0, sizeof(cps_init));
		
		cps_init.p_sensor_location = &sensor_location;
		cps_init.feature = BLE_CPS_FEATURE_WHEEL_REV_BIT; /*BLE_CPS_FEATURE_ACCUMULATED_TORQUE_BIT | */ 
						   
		cps_init.rc_evt_handler = mp_ant_ble_evt_handlers->on_set_resistance;

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


/**@brief Initialize services that will be used by the application.
 *
 * @details Initialize the Heart Rate and Device Information services.
 */
static void services_init(void)
{
		ble_dis_service_init();
		ble_hrs_service_init();
		ble_nus_service_init();
		ble_cps_service_init();
}


/**@brief Initialize security parameters.
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


/**@brief Start advertising.
 */
static void advertising_start(void)
{
    uint32_t err_code;
    
    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);
    
    m_is_advertising = true;
		
		mp_ant_ble_evt_handlers->on_ble_advertising();
}


/**@brief Connection Parameters Module handler.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in]   p_evt   Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;
    
    switch (p_evt->evt_type)
    {
        case BLE_CONN_PARAMS_EVT_FAILED:
            err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
            APP_ERROR_CHECK(err_code);
            break;
            
        default:
            break;
    }
}


/**@brief Connection Parameters module error handler.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Initialize the Connection Parameters module.
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
    cp_init.start_on_notify_cccd_handle    = m_hrs.hrm_handles.cccd_handle; // TODO: need to change this and remove hrs service.
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;
    
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

static __INLINE bool is_message_to_process(uint8_t message_id)
{
    switch (message_id)
    {
        case MESG_BROADCAST_DATA_ID:
        case MESG_BURST_DATA_ID:
				case MESG_ACKNOWLEDGED_DATA_ID:
            return true;
        default:
						return false;
    }
}

static void ant_data_hrm_messages_handle(ant_evt_t * p_ant_evt)
{
		// Determie if this is an interesting event or not.
		if ( !is_message_to_process( p_ant_evt->evt_buffer[ANT_BUFFER_INDEX_MESG_ID] ))
		{
			return;
		}
	
		uint8_t * p_evt_buffer = p_ant_evt->evt_buffer;
	
    static uint32_t s_previous_beat_count = 0;    // Heart beat count from previously received page
    uint32_t        err_code;
    uint32_t        current_page;
    uint8_t         beat_count;
    uint8_t         computed_heart_rate;
    uint16_t        beat_time;

    // Decode the default page data present in all pages
    beat_time           = uint16_decode(&p_evt_buffer[ANT_BUFFER_INDEX_MESG_DATA + 4]);
    beat_count          = (uint8_t)p_evt_buffer[ANT_BUFFER_INDEX_MESG_DATA + 6];
    computed_heart_rate = (uint8_t)p_evt_buffer[ANT_BUFFER_INDEX_MESG_DATA + 7];

    // Decode page specific data
    current_page = p_evt_buffer[ANT_BUFFER_INDEX_MESG_DATA];
    switch (current_page & ~ANT_HRM_TOGGLE_MASK)
    {
        case ANT_HRM_PAGE_4:
            // Ensure that there is only one beat between time intervals.
            if ((beat_count - s_previous_beat_count) == 1)
            {
                uint16_t prev_beat = uint16_decode(&p_evt_buffer[ANT_BUFFER_INDEX_MESG_DATA + 2]);
                
                // Subtracting the event time gives the R-R interval
                ble_hrs_rr_interval_add(&m_hrs, beat_time - prev_beat);
            }

            s_previous_beat_count = beat_count;
            break;
          
        case ANT_HRM_PAGE_0:
        case ANT_HRM_PAGE_1:
        case ANT_HRM_PAGE_2:
        case ANT_HRM_PAGE_3:
        default:
            break;
    }
    
    // Notify the received heart rate measurement
    err_code = ble_hrs_heart_rate_measurement_send(&m_hrs, computed_heart_rate);
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

/**@brief ANT CHANNEL_CLOSED event handler.
 */
static void on_ant_evt_channel_closed(void)
{
    //uint32_t err_code;

    if (!m_is_advertising && (m_conn_handle == BLE_CONN_HANDLE_INVALID))
    {
				// TOOD: ensure this is safe to do, i.e. that the timers are running, etc...
        // We do not have any activity on the radio at this time, so it is safe to store bonds
        //err_code = ble_bondmngr_bonded_masters_store();
        //APP_ERROR_CHECK(err_code);
        
        // ANT channel was closed due to a BLE disconnection, restart advertising
        advertising_start();
    }

    // Reopen ANT channel
    ant_bp_tx_start();
}


/**@brief Application's TOP LEVEL Stack ANT event handler.
 *
 * @param[in]   p_ant_evt   Event received from the stack.
 */
static void on_ant_evt(ant_evt_t * p_ant_evt)
{
		switch (p_ant_evt->channel)
		{
			case ANT_HRMRX_ANT_CHANNEL:
				ant_data_hrm_messages_handle(p_ant_evt);
				break;
			case ANT_BP_TX_CHANNEL:
				ant_bp_rx_handle(p_ant_evt);
				break;
			default:
				break;
		}
}

/**@brief Application's Stack BLE event handler.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            m_is_advertising = false;
						mp_ant_ble_evt_handlers->on_ble_connected();
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
						mp_ant_ble_evt_handlers->on_ble_disconnected();
            m_conn_handle = BLE_CONN_HANDLE_INVALID;

            // Need to close the ANT channel to make it safe to write bonding information to flash
            err_code = sd_ant_channel_close(ANT_HRMRX_ANT_CHANNEL);
            APP_ERROR_CHECK(err_code);
            
            // Note: Bonding information will be stored, advertising will be restarted and the
            //       ANT channel will be reopened when ANT event CHANNEL_CLOSED is received.
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, 
                                                   BLE_GAP_SEC_STATUS_SUCCESS, 
                                                   &m_sec_params);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT)
            { 
                m_is_advertising = false;
                mp_ant_ble_evt_handlers->on_ble_timeout();
                
                /* Go to system-off mode (this function will not return; wakeup will cause a reset)
                // Don't power off here - we'll do it when there is no motion/activity not here.
                err_code = sd_power_system_off();
                APP_ERROR_CHECK(err_code); */
            }
            break;
            
        default:
            break;
    }
}


/**@brief Dispatches a stack event to all modules with a stack BLE event handler.
 *
 * @details This function is called from the Stack event interrupt handler after a stack BLE
 *          event has been received.
 *
 * @param[in]   p_ble_evt   Stack Bluetooth event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    ble_bondmngr_on_ble_evt(p_ble_evt);
    ble_cps_on_ble_evt(&m_cps, p_ble_evt);
		ble_hrs_on_ble_evt(&m_hrs, p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
#if defined(BLE_NUS_ENABLED)
		ble_nus_on_ble_evt(&m_nus, p_ble_evt);
#endif		
    on_ble_evt(p_ble_evt);
}


/**@brief BLE + ANT stack initialization.
 *
 * @details Initializes the SoftDevice and the stack event interrupt.
 */
static void ble_ant_stack_init(void)
{
    // Initialize SoftDevice
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, false);
    
    // Subscribe for BLE events.
    uint32_t err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);
        
    // Subscribe for ANT events.
    err_code = softdevice_ant_evt_handler_set(on_ant_evt);
    APP_ERROR_CHECK(err_code);
}

/**@brief Bond Manager module error handler.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void bond_manager_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Bond Manager initialization.
 */
static void bond_manager_init(void)
{
    uint32_t            err_code;
    ble_bondmngr_init_t bond_init_data;
    bool                bonds_delete;

    // Clear all bonded masters if the Bonds Delete button is pushed
    //bonds_delete = (nrf_gpio_pin_read(BONDMNGR_DELETE_BUTTON_PIN) == 0);

    // Initialize the Bond Manager
    bond_init_data.flash_page_num_bond     = FLASH_PAGE_BOND;
    bond_init_data.flash_page_num_sys_attr = FLASH_PAGE_SYS_ATTR;
    bond_init_data.evt_handler             = NULL;
    bond_init_data.error_handler           = bond_manager_error_handler;
    bond_init_data.bonds_delete            = bonds_delete;

    err_code = ble_bondmngr_init(&bond_init_data);
    APP_ERROR_CHECK(err_code);
}

void debug_send(uint8_t * data, uint16_t length)
{
#if defined(BLE_NUS_ENABLED)
		data[length] = '\0';
		ble_nus_send_string(&m_nus, data, length);
#endif		
}

//
// Sends ble & ant data messages.
//
void cycling_power_send(irt_power_meas_t * p_cps_meas)
{
		uint32_t err_code;
		
		// Send ble message only if connected.
		if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
		{
			err_code = ble_cps_cycling_power_measurement_send(&m_cps, p_cps_meas);

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
		// Send ANT+ message.
		ant_bp_tx_send(p_cps_meas);
}

/**@brief Power manager.
 */
void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

void ble_ant_init(ant_ble_evt_handlers_t * ant_ble_evt_handlers)
{
		// Event pointers.
		mp_ant_ble_evt_handlers = ant_ble_evt_handlers;
	
		// Initialize S310 SoftDevice
    ble_ant_stack_init();
		
    // Initialize persistent storage module.
    uint32_t err_code;
		err_code = pstorage_init();
    APP_ERROR_CHECK(err_code);		
    
    // Initialize Bluetooth helper modules
    bond_manager_init();
    
    // Initialize Bluetooth stack parameters
    gap_params_init();
    services_init();		
    advertising_init();
    conn_params_init();
    sec_params_init();

    // Initialize ANT channels
    ant_bp_tx_init(mp_ant_ble_evt_handlers->on_set_resistance);
}

void ble_ant_start()
{
    // Start execution
    advertising_start();
		
		// Open the ANT channel for transmitting power.
		ant_bp_tx_start();
}
