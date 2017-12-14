/* Copyright (c) 2017 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#ifndef ANT_BG_SCANNER_H__
#define ANT_BG_SCANNER_H__

#include <stdint.h>
#include "ant_stack_handler_types.h"

#define POWER_METER_LIST_SIZE       4u
#define TX_BUFFER_SIZE              8u

typedef struct 
{
    uint16_t power_meter_id;
    uint8_t rssi;
    uint8_t ant_product_page[TX_BUFFER_SIZE];
    uint8_t ant_manufacturer_page[TX_BUFFER_SIZE];
} power_meter_info_t;

typedef void (*pm_info_handler_t)
    (power_meter_info_t* p_power_meter_list, uint8_t size);

uint32_t ant_bg_scanner_start(pm_info_handler_t pm_info_handler);
void ant_bg_scanner_stop();
void ant_bg_scanner_rx_handle(ant_evt_t * p_ant_evt);

#endif // ANT_BG_SCANNER_H__