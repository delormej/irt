/* Copyright (c) 2017 Inside Ride Technologies, LLC. All Rights Reserved.
*/
#include <string.h>
#include <stdbool.h>
#include "ant_parameters.h"
#include "ant_bg_scanner.h"
#include "ant_stack_handler_types.h"

#define ANT_MANUFACTURER_PAGE           0x50u   /**< Manufacturer's identification common data page. */
#define ANT_PRODUCT_PAGE          		0x51u   /**< Product information common data page. */

static power_meter_info_t pm_info[POWER_METER_LIST_SIZE];
static uint8_t pm_info_size;
static pm_info_handler_t m_pm_info_handler;

static void open_bg_scanning_channel()
{

}

static void close_bg_scanning_channel()
{

}


static uint16_t device_id_get(ANT_MESSAGE* p_mesg)
{
    //67.750 {  79873671} Rx - [A4][14][4E][00][10][19][00][00][00][00][FF][24][E0][89][E0][11][05][10][01][6A][00][0C][F4][32]
    uint16_t device_id = p_mesg->ANT_MESSAGE_aucExtData[1] << 8 | 
        p_mesg->ANT_MESSAGE_aucExtData[0];
    return device_id;
}

static uint8_t rssi_get(ANT_MESSAGE* p_mesg)
{
    /* This only works for FLag Byte 0xE0, need to support other flag variations.
    if (p_mesg->ANT_MESSAGE_stExtMesgBF.bANTRssi)
        return p_mesg->ANT_MESSAGE_aucExtData[5];
    else
        return 0;
    */
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

void ant_bg_scanner_start(pm_info_handler_t pm_info_handler)
{
    m_pm_info_handler = pm_info_handler;
    pm_info_init();
    open_bg_scanning_channel();
}

void ant_bg_scanner_stop()
{
    close_bg_scanning_channel();
}

void ant_bg_scanner_rx_handle(ant_evt_t * p_ant_evt)
{
    bool pm_info_updated = false;
	if (p_ant_evt->event == EVENT_RX)
        pm_info_updated = read_page(p_ant_evt);
    if (pm_info_updated)
        m_pm_info_handler(&pm_info[0], pm_info_size);
}
