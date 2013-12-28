/****************************************************************************** 
 * Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * @file  	ble_ant.c
 * @brief		This file contains all implementation details specific to 
 *					initializing the BLE and ANT stacks.
 */

#include <stdint.h>
#include <string.h>
#include "ble_ant.h"
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_assert.h"
#include "app_util.h"
#include "app_error.h"
#include "nrf51_bitfields.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_bas.h"
//#include "ble_hrs.h"
#include "ble_dis.h"
#include "ble_conn_params.h"
#include "ble_sensorsim.h"
#include "ble_bondmngr.h"
#include "ble_radio_notification.h"
#include "ble_flash.h"
#include "ant_parameters_ds.h"
#include "ant_interface_ds.h"
#include "irt_peripheral.h"

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

static ble_hrs_t                        m_hrs;                                       /**< Structure used to identify the heart rate service. */
static ble_gap_sec_params_t             m_sec_params;                                /**< Security requirements for this application. */
static ble_gap_adv_params_t             m_adv_params;                                /**< Parameters to be passed to the stack when starting advertising. */

static ble_ant_stack_ble_evt_handler_t	 m_ble_evt_callback;		// Function pointer to main.c ble_evt 
static ble_ant_stack_ble_evt_handler_t   m_ble_evt_handler;     /**< Application event handler for handling BLE stack events. */
static ble_ant_stack_ant_evt_handler_t   m_ant_evt_handler;     /**< Application event handler for handling ANT stack events. */
static ble_ant_stack_evt_schedule_func_t m_evt_schedule_func;   /**< Pointer to function for propagating button events to the scheduler. */
static uint8_t *                         m_evt_buffer;          /**< Buffer for receiving events. */
static uint16_t                          m_evt_buffer_size;     /**< Size of event buffer. */



/**@brief Connection Parameters module error handler.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
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

/**@brief Bond Manager module error handler.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void bond_manager_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief SoftDevice assert callback function.
 *
 * @details A pointer to this function will be passed to the SoftDevice. This function will be
 *          called if an ASSERT statement in the SoftDevice fails.
 *
 * @param[in]   pc         The value of the program counter when the ASSERT call failed.
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
static void softdevice_assertion_handler(uint32_t pc, uint16_t line_num, const uint8_t * file_name)
{
    UNUSED_PARAMETER(pc);
    assert_nrf_callback(line_num, file_name);
}

/*----------------------------------------------------------------------------
 * BLE Functions
 * ----------------------------------------------------------------------------*/

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
    ble_hrs_on_ble_evt(&m_hrs, p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    m_ble_evt_callback(p_ble_evt);
}

/**@brief Bond Manager initialization.
 */
static void bond_manager_init(void)
{
    uint32_t            err_code;
    ble_bondmngr_init_t bond_init_data;

    // Initialize the Bond Manager
    bond_init_data.flash_page_num_bond     = FLASH_PAGE_BOND;
    bond_init_data.flash_page_num_sys_attr = FLASH_PAGE_SYS_ATTR;
    bond_init_data.evt_handler             = NULL;
    bond_init_data.error_handler           = bond_manager_error_handler;
    bond_init_data.bonds_delete            = false;

    err_code = ble_bondmngr_init(&bond_init_data);
    APP_ERROR_CHECK(err_code);
}


/**@brief Initialize Radio Notification event handler.
 */
static void radio_notification_init(void)
{
    uint32_t err_code;

    err_code = ble_radio_notification_init(NRF_APP_PRIORITY_HIGH,
                                           NRF_RADIO_NOTIFICATION_DISTANCE_800US,
                                           ble_flash_on_radio_active_evt);
    APP_ERROR_CHECK(err_code);
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
    cp_init.start_on_notify_cccd_handle    = m_hrs.hrm_handles.cccd_handle;
    cp_init.disconnect_on_fail             = true;
    cp_init.evt_handler                    = NULL;
    cp_init.error_handler                  = conn_params_error_handler;
    
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
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
    
    err_code = sd_ble_gap_device_name_set(&sec_mode, DEVICE_NAME, strlen(DEVICE_NAME));
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
    uint8_t       flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    
    ble_uuid_t adv_uuids[] = 
    {
        {BLE_UUID_HEART_RATE_SERVICE,         BLE_UUID_TYPE_BLE}, 
        {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}
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

    // Initialise advertising parameters (used when starting advertising)
    memset(&m_adv_params, 0, sizeof(m_adv_params));
    
    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    m_adv_params.p_peer_addr = NULL;
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval    = APP_ADV_INTERVAL;
    m_adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;
}

/**@brief Initialize services that will be used by the application.
 *
 * @details Initialize the Heart Rate and Device Information services.
 */
static void services_init(void)
{
    uint32_t       err_code;
    ble_hrs_init_t hrs_init;
    ble_dis_init_t dis_init;
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

    // Initialize Device Information Service
    memset(&dis_init, 0, sizeof(dis_init));
    
    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, MANUFACTURER_NAME);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
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


/*----------------------------------------------------------------------------
 * ANT Functions
 * ----------------------------------------------------------------------------*/

/**@brief Initialize the ANT HRM reception.
 */
static void ant_hrm_rx_init(void)
{
    uint32_t err_code;
    
    err_code = sd_ant_network_address_set(ANTPLUS_NETWORK_NUMBER, m_ant_network_key);
    APP_ERROR_CHECK(err_code);
    
    err_code = sd_ant_channel_assign(ANT_HRMRX_ANT_CHANNEL,
                                     ANT_HRMRX_CHANNEL_TYPE,
                                     ANTPLUS_NETWORK_NUMBER,
                                     ANT_HRMRX_EXT_ASSIGN);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ant_channel_id_set(ANT_HRMRX_ANT_CHANNEL,
                                     ANT_HRMRX_DEVICE_NUMBER,
                                     ANT_HRMRX_DEVICE_TYPE,
                                     ANT_HRMRX_TRANS_TYPE);
    APP_ERROR_CHECK(err_code);
    
    err_code = sd_ant_channel_radio_freq_set(ANT_HRMRX_ANT_CHANNEL, ANTPLUS_RF_FREQ);
    APP_ERROR_CHECK(err_code);
    
    err_code = sd_ant_channel_period_set(ANT_HRMRX_ANT_CHANNEL, ANT_HRMRX_MSG_PERIOD);
    APP_ERROR_CHECK(err_code);
}

/*----------------------------------------------------------------------------
 * Public Functions
 * ----------------------------------------------------------------------------*/

void ble_gap_sec_params_reply(uint16_t m_conn_handle)
{
	uint32_t err_code = NRF_SUCCESS;
	err_code = sd_ble_gap_sec_params_reply(m_conn_handle, 
																				 BLE_GAP_SEC_STATUS_SUCCESS, 
																				 &m_sec_params);
	APP_ERROR_CHECK(err_code);	
}

/**@brief Start advertising.
 */
void ble_advertising_start(void)
{
    uint32_t err_code;
    
    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);
}

uint32_t ble_ant_stack_handler_init(nrf_clock_lfclksrc_t              clock_source,
                                    void *                            p_evt_buffer,
                                    uint16_t                          evt_buffer_size,
                                    ble_ant_stack_ble_evt_handler_t   ble_evt_handler,
                                    ble_ant_stack_ant_evt_handler_t   ant_evt_handler,
                                    ble_ant_stack_evt_schedule_func_t evt_schedule_func)
{
    uint32_t err_code;

    // Check parameters
    if (!is_word_aligned(p_evt_buffer))
    {
        return NRF_ERROR_INVALID_PARAM;
    }
    if (p_evt_buffer == NULL)
    {
        return NRF_ERROR_INVALID_PARAM;
    }    
    
    // Save configuration
    m_evt_buffer        = (uint8_t *)p_evt_buffer;
    m_evt_buffer_size   = evt_buffer_size;
    m_ble_evt_handler   = ble_evt_handler;
    m_ant_evt_handler   = ant_evt_handler;
    m_evt_schedule_func = evt_schedule_func;

    // Initialise SoftDevice
    err_code = sd_softdevice_enable(clock_source, softdevice_assertion_handler);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Enable stack event interrupt (interrupt priority has already been set by the stack)
    return sd_nvic_EnableIRQ(SD_EVENT_IRQn);
}


void intern_ble_ant_stack_events_execute(void)
{
    // Handle BLE events
    for (;;)
    {
        uint32_t err_code;
        uint16_t evt_len = m_evt_buffer_size;

        // Pull event from stack
        err_code = sd_ble_evt_get(m_evt_buffer, &evt_len);
        if (err_code == NRF_ERROR_NOT_FOUND)
        {
            break;
        }
        else if (err_code != NRF_SUCCESS)
        {
            APP_ERROR_HANDLER(err_code);
        }
        
        // Call application's BLE stack event handler
        m_ble_evt_handler((ble_evt_t *)m_evt_buffer);
    }
    
    // Handle ANT events
    for (;;)
    {
        uint32_t err_code;
        
        err_code = sd_ant_event_get(&((ant_evt_t *)m_evt_buffer)->channel,
                                    &((ant_evt_t *)m_evt_buffer)->event,
                                    ((ant_evt_t *)m_evt_buffer)->evt_buffer);

        if (err_code == NRF_ERROR_NOT_FOUND)
        {
            break;
        }
        else if (err_code != NRF_SUCCESS)
        {
            APP_ERROR_HANDLER(err_code);
        }
        
        // Call application's ANT stack event handler
        m_ant_evt_handler((ant_evt_t *)m_evt_buffer);
    }
}

void ble_ant_init(ble_ant_stack_ble_evt_handler_t   ble_evt_handler,
                  ble_ant_stack_ant_evt_handler_t   ant_evt_handler,
									ble_hrs_t                  				*ble_hrs)
{
		// Assign ble_evt callback.
		m_ble_evt_callback = ble_evt_handler;
	
		// Initializes the S310 SoftDevice and the stack event interrupt.
    BLE_ANT_STACK_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM,
                               BLE_L2CAP_MTU_DEF,
                               ble_evt_dispatch,
                               ant_evt_handler,
                               false);
    
    // Initialize Bluetooth helper modules
    bond_manager_init();
    radio_notification_init();
    
    // Initialize Bluetooth stack parameters
    gap_params_init();
    advertising_init();
    services_init();
		ble_hrs = &m_hrs;	// Assign HRS 
		
    conn_params_init();
    sec_params_init();

    // Initialize ANT HRM recieve channel
    ant_hrm_rx_init();	
}

/**@brief Stack event interrupt handler.
 *
 * @details This function is called whenever an event is ready to be pulled.
 */
void SD_EVENT_IRQHandler(void)
{
    if (m_evt_schedule_func != NULL)
    {
        uint32_t err_code = m_evt_schedule_func();
        APP_ERROR_CHECK(err_code);
    }
    else
    {
        intern_ble_ant_stack_events_execute();
    }
}
