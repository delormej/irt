/* Copyright (c) 2017 Inside Ride Technologies, LLC. All Rights Reserved.
*/
#include <string.h>
#include <stdbool.h>
#include "nrf_error.h"
#include "app_error.h"
#include "ant_interface.h"
#include "ant_parameters.h"
#include "ant_bg_scanner.h"
#include "ant_stack_handler_types.h"
#include "ble_ant.h"
#include "debug.h"

#ifdef ENABLE_DEBUG_LOG
#define BG_SCAN_LOG debug_log
#else
#define BG_SCAN_LOG(...)
#endif // ENABLE_DEBUG_LOG

#define ANT_MANUFACTURER_PAGE           0x50u   /**< Manufacturer's identification common data page. */
#define ANT_PRODUCT_PAGE          		0x51u   /**< Product information common data page. */
#define ANT_BG_SCAN_CHANNEL_TYPE        0x40
#define ANT_BP_DEVICE_TYPE              0x0B                   /**< Channel ID device type. */
#define ANT_BP_TRANS_TYPE               0x00 					/**< Transmission Type. */
#define WILDCARD_DEVICE_ID              0x00
#define SEARCH_TIMEOUT_INFINITE         0xFF
#define SEARCH_TIMEOUT_DISABLED         0x00
#define EXTENDED_BYTES_CONFIG           (ANT_LIB_CONFIG_MESG_OUT_INC_TIME_STAMP | ANT_LIB_CONFIG_MESG_OUT_INC_RSSI | ANT_LIB_CONFIG_MESG_OUT_INC_DEVICE_ID)

static power_meter_info_t pm_info[POWER_METER_LIST_SIZE];
static uint8_t pm_info_size;
static pm_info_handler_t m_pm_info_handler;
static bool is_started = false;

static uint32_t open_bg_scanning_channel()
{
    uint32_t err_code;

    BG_SCAN_LOG("[BG_SCAN] Openning bg scanning channel.\r\n");

    err_code = sd_ant_channel_assign(ANT_BG_SCAN_CHANNEL,
                                     ANT_BG_SCAN_CHANNEL_TYPE,
                                     ANTPLUS_NETWORK_NUMBER,
                                     EXT_PARAM_ALWAYS_SEARCH);
	if (err_code != NRF_SUCCESS)
		return err_code;

    err_code = sd_ant_channel_id_set(ANT_BG_SCAN_CHANNEL,
                                     WILDCARD_DEVICE_ID,
                                     ANT_BP_DEVICE_TYPE,
                                     ANT_BP_TRANS_TYPE);
	if (err_code != NRF_SUCCESS)
		return err_code;
    
    err_code = sd_ant_lib_config_set(EXTENDED_BYTES_CONFIG);
	if (err_code != NRF_SUCCESS)
		return err_code;

    err_code = sd_ant_channel_low_priority_rx_search_timeout_set(ANT_BG_SCAN_CHANNEL, 
                                    SEARCH_TIMEOUT_INFINITE);
	if (err_code != NRF_SUCCESS)
		return err_code;
    
    err_code = sd_ant_channel_rx_search_timeout_set(ANT_BG_SCAN_CHANNEL, SEARCH_TIMEOUT_DISABLED);
	if (err_code != NRF_SUCCESS)
        return err_code;
        
    err_code = sd_ant_channel_radio_freq_set(ANT_BG_SCAN_CHANNEL, ANTPLUS_RF_FREQ);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ant_channel_open(ANT_BG_SCAN_CHANNEL);
    APP_ERROR_CHECK(err_code);
    is_started = true;

    BG_SCAN_LOG("[BG_SCAN] Opened bg scanning channel.\r\n");

	return NRF_SUCCESS;
}

static void close_bg_scanning_channel()
{
	uint8_t status;
	uint32_t err;
	
	err = sd_ant_channel_status_get(ANT_BG_SCAN_CHANNEL, &status);
	APP_ERROR_CHECK(err);

    BG_SCAN_LOG("[BG_SCAN] Closing bg scanning channel, status: %i\r\n", status);

	switch (status)
	{
		case STATUS_SEARCHING_CHANNEL:
		case STATUS_TRACKING_CHANNEL:
            err = sd_ant_channel_close(ANT_BG_SCAN_CHANNEL);
            APP_ERROR_CHECK(err);
            err = sd_ant_lib_config_clear(EXTENDED_BYTES_CONFIG);
            APP_ERROR_CHECK(err);
            break;

        default:
            return;
    }
        
    is_started = false;
}

static uint16_t device_id_get(ANT_MESSAGE* p_mesg)
{
    //67.750 {  79873671} Rx - [A4][14][4E][00][10][19][00][00][00][00][FF][24][E0][89][E0][11][05][10][01][6A][00][0C][F4][32]
    uint16_t device_id = p_mesg->ANT_MESSAGE_aucExtData[1] << 8 | 
        p_mesg->ANT_MESSAGE_aucExtData[0];
    
    BG_SCAN_LOG("[BG_SCAN] device_id_get: %i\r\n", device_id);
    return device_id;
}

static uint8_t rssi_get(ANT_MESSAGE* p_mesg)
{
    if (p_mesg->ANT_MESSAGE_ucExtMesgBF == EXTENDED_BYTES_CONFIG)
        return p_mesg->ANT_MESSAGE_aucExtData[5];
    else
        return 0;
}

static void pm_info_init()
{
    memset(&pm_info, 0, sizeof(power_meter_info_t) * POWER_METER_LIST_SIZE);
    pm_info_size = 0;
}

static power_meter_info_t* pm_info_get(uint16_t power_meter_id)
{
    for (int i = 0; i < pm_info_size; i++)
        if (pm_info[i].power_meter_id == power_meter_id)
            return &pm_info[i];
    pm_info[pm_info_size].power_meter_id = power_meter_id;
    return &pm_info[pm_info_size++];
}

static bool pm_info_set_message(power_meter_info_t* p_pm_info, ANT_MESSAGE* p_mesg)
{
    p_pm_info->rssi = rssi_get(p_mesg);
    uint8_t* p_buffer;
    uint8_t page_number = p_mesg->ANT_MESSAGE_aucPayload[0];
    switch (page_number)
    {
        case ANT_MANUFACTURER_PAGE:
            p_buffer = (uint8_t*) &p_pm_info->ant_manufacturer_page;
            break;
            
        case ANT_PRODUCT_PAGE:
            p_buffer = (uint8_t*) &p_pm_info->ant_product_page;
            break;

        default:
            return false;
    }
    memcpy(p_buffer, &p_mesg->ANT_MESSAGE_aucPayload, TX_BUFFER_SIZE);
    return true;
}

static bool read_page(ant_evt_t * p_ant_evt)
{
    BG_SCAN_LOG("[BG_SCAN] read_page\r\n");
    ANT_MESSAGE* p_mesg = (ANT_MESSAGE*)p_ant_evt->evt_buffer;
   
    bool new_pm_info = false;
    uint8_t original_size = pm_info_size;

    uint16_t device_id = device_id_get(p_mesg);
    if (device_id > 0)
    {
        power_meter_info_t* p_pm_info = pm_info_get(device_id);
        new_pm_info = pm_info_set_message(p_pm_info, p_mesg);
        new_pm_info |= pm_info_size > original_size;
    }
    return new_pm_info;
}

uint32_t ant_bg_scanner_start(pm_info_handler_t pm_info_handler)
{
    m_pm_info_handler = pm_info_handler;
	close_bg_scanning_channel();
    pm_info_init();
    return open_bg_scanning_channel();
}

void ant_bg_scanner_stop()
{
    close_bg_scanning_channel();
}

void ant_bg_scanner_rx_handle(ant_evt_t * p_ant_evt)
{
    BG_SCAN_LOG("[BG_SCAN] ant_bg_scanner_rx_handle\r\n");
    bool pm_info_updated = false;
	if (p_ant_evt->event == EVENT_RX)
        pm_info_updated = read_page(p_ant_evt);
    if (pm_info_updated)
        m_pm_info_handler(&pm_info[0], pm_info_size);
}

bool ant_bg_scanner_is_started()
{
    return is_started;
}
