/* Copyright (c) 2017 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#ifndef ANT_BG_SCANNER_H__
#define ANT_BG_SCANNER_H__

#include "ble_ant.h"
#include "ant_stack_handler_types.h"

#define POWER_METER_LIST_SIZE       4

typedef {
    uint16_t power_meter_id,
    uint8_t ant_product_page[TX_BUFFER_SIZE],
    uint8_t ant_manufacturer_page[TX_BUFFER_SIZE]    
} power_meter_info_t;

typedef void (*pm_info_handler_t)
    (power_meter_info_t* p_power_meter_list, uint8_t size);

void ant_bg_scanner_start(pm_info_handler_t pm_info_handler);
void ant_bg_scanner_stop();
void ant_bg_scanner_rx_handle(ant_evt_t * p_ant_evt);

#endif // ANT_BG_SCANNER_H__