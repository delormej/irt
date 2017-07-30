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
#include "nrf_soc.h"
#include "nrf51_bitfields.h"
#include "ant_stack_handler_types.h"
#include "ble_stack_handler_types.h"
#include "app_error.h"
#include "app_timer.h"
#include "ant_parameters.h"
#include "ant_interface.h"
#include "pstorage.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "irt_peripheral.h"
#include "ant_bike_power.h"
#include "ant_bike_speed.h"
#include "ant_fec.h"
#include "ble_dis.h"
#include "ble_cps.h"
#include "ble_gap.h"
#include "debug.h"

/* Gives the option to compile without BLE, which saves a huge amount of
 * program space (7+kb).
 */
#define BLE_ENABLED

#if defined(BLE_NUS_ENABLED)
#include "ble_nus.h"
#endif

#if defined(S310_V2)
#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                            /**< Whether or not to include the service_changed characteristic. If not enabled, the server's database cannot be changed for the lifetime of the device */
#endif

#define APP_ADV_INTERVAL                40                                           /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      180                                          // BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED /**< The advertising timeout in units of seconds. */

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

#define DEAD_BEEF                       0xDEADBEEF                                   /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */
static ble_cps_t m_cps; /**< Structure used to identify the cycling power service. */
static ant_ble_evt_handlers_t * mp_ant_ble_evt_handlers;

ble_state_e irt_ble_ant_state = DISCONNECTED;

/**@brief Debug logging for module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define BA_LOG printf
#else
#define BA_LOG(...)
#endif // ENABLE_DEBUG_LOG

#if defined(BLE_ENABLED)	// Bluetooth enabled?

#if defined(BLE_NUS_ENABLED)
static ble_nus_t m_nus;				// BLE UART service for debugging purposes.

static void uart_data_handler(ble_nus_t * p_nus, uint8_t * data, uint16_t length)
{
	mp_ant_ble_evt_handlers->on_ble_uart(data, length);
}
#endif



/**@brief	Checks to see if this BLE error could be treated as a warning.
 */
#define BLE_ERROR_AS_WARN(ERR) \
		(ERR ==  NRF_ERROR_INVALID_STATE || \
			ERR == BLE_ERROR_NO_TX_BUFFERS || \
			ERR == NRF_ERROR_BUSY)

/**@brief	Combined with above macro, checks the BLE error to see if we can resolve.
 */
#define BLE_ERROR_CHECK(ERR)							\
do														\
{														\
	if (ERR != NRF_SUCCESS)								\
	{													\
		if (ERR == BLE_ERROR_GATTS_SYS_ATTR_MISSING)	\
		{												\
			sys_attr_missing_set();						\
		}												\
		else if (!BLE_ERROR_AS_WARN(ERR))				\
		{												\
			APP_ERROR_CHECK(err_code);					\
		}												\
	}													\
} while(0)

#endif // BLE_ENABLED
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
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name) {
	app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

#if defined(BLE_ENABLED)
/**@brief	Checks to see if the error is a BLE_ERROR_GATTS_SYS_ATTR_MISSING,
 * 			if so respond to resolve.
 */
static void sys_attr_missing_set()
{
	BA_LOG("[BA]:cycling_power_send sd_ble_gatts_sys_attr_set\r\n");
	APP_ERROR_CHECK(sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0));
}

/**@brief GAP initialization.
 *
 * @details This function shall be used to setup all the necessary GAP (Generic Access Profile)
 *          parameters of the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void) {
	uint32_t err_code;
	ble_gap_conn_params_t gap_conn_params;
	ble_gap_conn_sec_mode_t sec_mode;

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

	err_code = sd_ble_gap_device_name_set(&sec_mode,
			(uint8_t const*) DEVICE_NAME, strlen(DEVICE_NAME));
	APP_ERROR_CHECK(err_code);

	err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_CYCLING_POWER_SENSOR);
	APP_ERROR_CHECK(err_code);

	memset(&gap_conn_params, 0, sizeof(gap_conn_params));

	gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
	gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
	gap_conn_params.slave_latency = SLAVE_LATENCY;
	gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

	err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
	APP_ERROR_CHECK(err_code);
}

/**@brief Advertising functionality initialization.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void) {
	uint32_t err_code;
	ble_advdata_t advdata;
	ble_advdata_t scanrsp;
	uint8_t flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

	ble_uuid_t adv_uuids[] = {
			{ BLE_UUID_CYCLING_POWER_SERVICE, BLE_UUID_TYPE_BLE },
#if defined(BLE_NUS_ENABLED)
			{ BLE_UUID_NUS_SERVICE, m_nus.uuid_type},
#endif
			{ BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE }
	};

	// Build and set advertising data
	memset(&advdata, 0, sizeof(advdata));
	advdata.name_type = BLE_ADVDATA_FULL_NAME;
	advdata.include_appearance = true;
	advdata.flags.size = sizeof(flags);
	advdata.flags.p_data = &flags;
	// Show just the CYCLING_POWER_SERVICE in ServicesMoreAvailable (like the KICKR).
	advdata.uuids_more_available.uuid_cnt = 1;
	advdata.uuids_more_available.p_uuids = adv_uuids;

    memset(&scanrsp, 0, sizeof(scanrsp));
    scanrsp.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    scanrsp.uuids_complete.p_uuids  = adv_uuids;

    err_code = ble_advdata_set(&advdata, &scanrsp);
    APP_ERROR_CHECK(err_code);
}

static void ble_dis_service_init() {
	uint32_t err_code;
	ble_dis_init_t dis_init;
	irt_device_info_t device_info;
	char sw_revision[8];	//xx.xx.xx

	// Initialize Device Information Service
	memset(&dis_init, 0, sizeof(dis_init));

	SET_DEVICE_INFO(&device_info);
	IRT_SW_REV_TO_CHAR(&device_info.sw_revision, sw_revision);

	ble_srv_ascii_to_utf8(&dis_init.manufact_name_str,
			device_info.manufacturer_name);
	ble_srv_ascii_to_utf8(&dis_init.fw_rev_str, sw_revision);

	dis_init.model_num_str.length = sizeof(device_info.model);
	dis_init.model_num_str.p_str = (uint8_t *) &device_info.model;

	dis_init.serial_num_str.length = sizeof(device_info.serial_num);
	dis_init.serial_num_str.p_str = (uint8_t *) &device_info.serial_num;

	dis_init.hw_rev_str.length = sizeof(device_info.hw_revision);
	dis_init.hw_rev_str.p_str = (uint8_t *) &device_info.hw_revision;

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);

	err_code = ble_dis_init(&dis_init);
	APP_ERROR_CHECK(err_code);
}

static void ble_nus_service_init() {
#if defined(BLE_NUS_ENABLED)
	uint32_t err_code;
	ble_nus_init_t nus_init;

	memset(&nus_init, 0, sizeof nus_init);
	nus_init.data_handler = uart_data_handler;

	err_code = ble_nus_init(&m_nus, &nus_init);
	APP_ERROR_CHECK(err_code);

	BA_LOG("[BA]:ble_nus_service_init Initializing debug BLE.\r\n");

	// Enable the logging to BLE.
	// debug_ble_init(&m_nus);
#endif
}

//
// Initialize Cycling Power Service
//
static void ble_cps_service_init() {
	uint32_t err_code;
	ble_cps_init_t cps_init;

	uint8_t sensor_location = BLE_CPS_SENSOR_LOCATION_REAR_WHEEL;

	memset(&cps_init, 0, sizeof(cps_init));

	cps_init.p_sensor_location = &sensor_location;
	//
	// NOTE on TORQUE:
	// We don't use accumulated torque in BLE as the torque # we calc is based on complete
	// wheel rev, for BLE we need to adjust the event time back to the last complete wheel
	// rev which throws off accumulated torque, it would have to be recalculated.
	//
	cps_init.feature = BLE_CPS_FEATURE_WHEEL_REV_BIT; // | BLE_CPS_FEATURE_ACCUMULATED_TORQUE_BIT
	cps_init.use_cycling_power_control_point = true;

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
#endif // BLE_ENABLED

/**@brief Initialize services that will be used by the application.
 *
 * @details Initialize the Heart Rate and Device Information services.
 */
static void services_init() {

#if defined(BLE_ENABLED)
	ble_dis_service_init();		// Discovery Service
	ble_nus_service_init();		// Debug Info Service (BLE_UART)
	ble_cps_service_init();		// Cycling Power Service
#endif

	// Initialize ANT+ FE-C transmit channel.
	ant_fec_tx_init(mp_ant_ble_evt_handlers);

	// Initialize the ANT+ remote control service.
	ant_ctrl_tx_init(ANT_CTRL_CHANNEL,
			mp_ant_ble_evt_handlers->on_ant_ctrl_command);

	// Initialize the ANT+ speed channel.
	ant_sp_tx_init();
}

#if defined(BLE_ENABLED)
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
			BA_LOG("[BA]:on_conn_params_evt BLE_CONN_PARAMS_EVT_FAILED, disconnecting.\r\n");
			err_code = sd_ble_gap_disconnect(m_conn_handle,
					BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
			APP_ERROR_CHECK(err_code);
			break;

		default:
			break;
	}
}

static void ble_sec_params_send() {
	uint32_t err_code;
	ble_gap_sec_params_t sec_params; // Security requirements for this application.

	sec_params.timeout = SEC_PARAM_TIMEOUT;
	sec_params.bond = SEC_PARAM_BOND;
	sec_params.mitm = SEC_PARAM_MITM;
	sec_params.io_caps = SEC_PARAM_IO_CAPABILITIES;
	sec_params.oob = SEC_PARAM_OOB;
	sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;

	sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;

	err_code = sd_ble_gap_sec_params_reply(m_conn_handle,
	BLE_GAP_SEC_STATUS_SUCCESS, &sec_params);

	APP_ERROR_CHECK(err_code);
}

/**@brief Connection Parameters module error handler.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error) {
	APP_ERROR_HANDLER(nrf_error);
}


/**@brief Initialize the Connection Parameters module.
 */
static void conn_params_init(void) {
	uint32_t err_code;
	ble_conn_params_init_t cp_init;

	memset(&cp_init, 0, sizeof(cp_init));

	cp_init.p_conn_params = NULL;
	cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
	cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
	cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
	cp_init.start_on_notify_cccd_handle = m_cps.cprc_handles.cccd_handle; // BLE_GATT_HANDLE_INVALID
	cp_init.disconnect_on_fail = false;
	cp_init.evt_handler = on_conn_params_evt;
	cp_init.error_handler = conn_params_error_handler;

	err_code = ble_conn_params_init(&cp_init);
	APP_ERROR_CHECK(err_code);
}
#endif // BLE_ENABLED

/**@brief Application's TOP LEVEL Stack ANT event handler.
 *
 * @param[in]   p_ant_evt   Event received from the stack.
 */
static void on_ant_evt(ant_evt_t * p_ant_evt) {	
	switch (p_ant_evt->channel) 
	{
		case ANT_FEC_TX_CHANNEL:
			ant_fec_rx_handle(p_ant_evt);                
			break;
		
		case ANT_BP_RX_CHANNEL:
			ant_bp_rx_handle(p_ant_evt);
			break;
	
		case ANT_CTRL_CHANNEL:
			ant_ctrl_rx_handle(p_ant_evt);
			break;
	
		default:
			break;
	}
}

#if defined(BLE_ENABLED)
/**@brief Application's Stack BLE event handler.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
	uint32_t err_code;
    static ble_gap_evt_auth_status_t m_auth_status;
    ble_gap_enc_info_t *             p_enc_info;

	switch (p_ble_evt->header.evt_id)
	{
		case BLE_GAP_EVT_CONNECTED:
			irt_ble_ant_state = CONNECTED;
			mp_ant_ble_evt_handlers->on_ble_connected();
			m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
			break;

		case BLE_GAP_EVT_DISCONNECTED:
			irt_ble_ant_state = DISCONNECTED;
			mp_ant_ble_evt_handlers->on_ble_disconnected();
			m_conn_handle = BLE_CONN_HANDLE_INVALID;

			break;

		case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
			ble_sec_params_send();
			break;

		case BLE_GAP_EVT_TIMEOUT:
			if (p_ble_evt->evt.gap_evt.params.timeout.src
					== BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT)
			{
				irt_ble_ant_state = DISCONNECTED;
				mp_ant_ble_evt_handlers->on_ble_timeout();
			}
			break;

		case BLE_GATTS_EVT_SYS_ATTR_MISSING:
			sys_attr_missing_set();
			break;

		case BLE_GAP_EVT_AUTH_STATUS:
            m_auth_status = p_ble_evt->evt.gap_evt.params.auth_status;
            break;

        case BLE_GAP_EVT_SEC_INFO_REQUEST:
            p_enc_info = &m_auth_status.periph_keys.enc_info;
            if (p_enc_info->div == p_ble_evt->evt.gap_evt.params.sec_info_request.div)
            {
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, p_enc_info, NULL);
                APP_ERROR_CHECK(err_code);
            }
            else
            {
                // No keys found for this device
                err_code = sd_ble_gap_sec_info_reply(m_conn_handle, NULL, NULL);
                APP_ERROR_CHECK(err_code);
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
	ble_conn_params_on_ble_evt(p_ble_evt);
	ble_cps_on_ble_evt(&m_cps, p_ble_evt);
#if defined(BLE_NUS_ENABLED)
	ble_nus_on_ble_evt(&m_nus, p_ble_evt);
#endif		
	on_ble_evt(p_ble_evt);
}
#endif // BLE_ENABLED

/**@brief BLE + ANT stack initialization.
 *
 * @details Initializes the SoftDevice callbacks for BLE and ANT messages..
 */
static void ble_ant_stack_init(void)
{
	uint32_t err_code;

#if defined(S310_V2)
    // Initialize BLE stack
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
    ble_enable_params.gatts_enable_params.service_changed = IS_SRVC_CHANGED_CHARACT_PRESENT;
    err_code = sd_ble_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
#endif


#if defined(BLE_ENABLED)
	// Subscribe for BLE events.
	err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
	APP_ERROR_CHECK(err_code);
#endif //BLE_ENABLED

	// Subscribe for ANT events.
	err_code = softdevice_ant_evt_handler_set(on_ant_evt);
	APP_ERROR_CHECK(err_code);
}

/**@brief	Sends acknolwedgement of resistance control to BLE and ANT recipients.
 */
void ble_ant_resistance_ack(uint8_t op_code, uint16_t value)
{
#if defined(BLE_ENABLED)
	uint32_t err_code;

	if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
	{
		err_code = ble_cps_resistance_indicate(&m_cps, op_code, value);
		BLE_ERROR_CHECK(err_code);
	}
	BA_LOG("[BA]:ble_ant_resistance_ack op:%i value:%i\r\n", op_code, value);
#endif // BLE_ENABLED
}

//
// Sends ble & ant data messages.
//
void cycling_power_send(irt_context_t * p_cps_meas)
{
	uint32_t err_code;
	// TODO: THIS IS A HACK, BUT NEED TO REFACTOR.
	static uint32_t event_count = 0;

#if defined(BLE_ENABLED)
	// Only send BLE power every 4 messages (1hz vs. ANT is 4hz)
	if (++event_count % 4 == 0)
	{
		// Send ble message only if connected.
		if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
		{
			err_code = ble_cps_cycling_power_measurement_send(&m_cps, p_cps_meas);
			BLE_ERROR_CHECK(err_code);
		}
	}
#endif //BLE_ENABLED
}

//
// Sends manufacturer and product pages.
//
void ant_common_page_transmit(uint8_t ant_channel, uint8_t* common_page)
{
	uint32_t err_code = sd_ant_broadcast_message_tx(ant_channel,
										TX_BUFFER_SIZE,
										common_page );
	if (!(ANT_ERROR_AS_WARN(err_code)))
		APP_ERROR_CHECK(err_code);
}

// Encodes the resistance mode into the 2 most significant bits.
uint8_t encode_resistance_level(irt_context_t * p_power_meas)
{
	uint8_t target_msb;
	uint8_t mode;

	// Subtract 64 (0x40) from mode to use only 2 bits.
	// Modes only go from 0x40 - 0x45 or so.
	mode = p_power_meas->resistance_mode - 64u;

	// Grab the most significant bits of the resistance level.
	target_msb = HIGH_BYTE(p_power_meas->resistance_level);

	// Use the 2 most significant bits for the mode and stuff them in the
	// highest 2 bits.  Level should never need to use these 2 bits.
	target_msb |= mode << 6;

	return target_msb;
}

void ble_ant_init(ant_ble_evt_handlers_t * ant_ble_evt_handlers)
{
	// Event pointers.
	mp_ant_ble_evt_handlers = ant_ble_evt_handlers;

	// Initialize the product page.
	ant_product_page[0] = ANT_COMMON_PAGE_81;
	ant_product_page[1] = BP_PAGE_RESERVE_BYTE;
	ant_product_page[2] = SW_REVISION_BLD;
	ant_product_page[3] = (SW_REVISION_MAJ << 4) | SW_REVISION_MIN;
	memcpy((uint8_t*)&(ant_product_page[4]), (uint32_t*)&(SERIAL_NUMBER), sizeof(uint32_t));

	// Initialize the manufacturer's page.
	ant_manufacturer_page[0] = ANT_COMMON_PAGE_80;
	ant_manufacturer_page[1] = BP_PAGE_RESERVE_BYTE;
	ant_manufacturer_page[2] = BP_PAGE_RESERVE_BYTE;
	ant_manufacturer_page[3] = HW_REVISION;
	ant_manufacturer_page[4] = LOW_BYTE(MANUFACTURER_ID);
	ant_manufacturer_page[5] = HIGH_BYTE(MANUFACTURER_ID);
	ant_manufacturer_page[6] = LOW_BYTE(MODEL_NUMBER);
	ant_manufacturer_page[7] = HIGH_BYTE(MODEL_NUMBER);

	// Initialize S310 SoftDevice
	ble_ant_stack_init();

#if defined(BLE_ENABLED)
	// Initialize Bluetooth stack parameters
	gap_params_init();
#endif // BLE_ENABLED

	// Initialize other services.
	services_init();

#if defined(BLE_ENABLED)
	advertising_init();
	conn_params_init();
#endif
}

/**@brief Start advertising.
 */
void ble_advertising_start(void) {
#if defined(BLE_ENABLED)
	uint32_t err_code;
	ble_gap_adv_params_t adv_params;// Parameters to be passed to the stack when starting advertising. */

	// Initialize advertising parameters (used when starting advertising)
	memset(&adv_params, 0, sizeof(ble_gap_adv_params_t));

	adv_params.type = BLE_GAP_ADV_TYPE_ADV_IND;
	adv_params.p_peer_addr = NULL;					// Undirected advertisement
	adv_params.fp = BLE_GAP_ADV_FP_ANY;
	adv_params.interval = APP_ADV_INTERVAL;
	adv_params.timeout = APP_ADV_TIMEOUT_IN_SECONDS;

	err_code = sd_ble_gap_adv_start(&adv_params);
	APP_ERROR_CHECK(err_code);

	irt_ble_ant_state = ADVERTISING;
	mp_ant_ble_evt_handlers->on_ble_advertising();
#endif // BLE_ENABLED
}

void ble_ant_start() {
	uint32_t err_code;
	
	// Start execution
#if defined(BLE_ENABLED)
	ble_advertising_start();
#endif // BLE_ENABLED

	// Assign network address.
    err_code = sd_ant_network_address_set(ANTPLUS_NETWORK_NUMBER, (uint8_t *)m_ant_network_key);
    APP_ERROR_CHECK(err_code);
	
	// Open the ANT channel for transmitting FE-C.
	ant_fec_tx_start();

	// Open the ANT channel for transimitting speed.
	ant_sp_tx_start();

	// Open the ANT channel to start broadcasting availability for remote control.
	ant_ctrl_tx_start();
}
