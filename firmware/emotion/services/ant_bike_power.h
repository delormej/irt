/* Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#ifndef ANT_BIKE_POWER_H__
#define ANT_BIKE_POWER_H__

#define ANT_BP_COMMAND_OFFSET			4u
#define ANT_BP_SET_WEIGHT_COMMAND		0x60
#define ANT_BP_ENABLE_DFU_COMMAND		0x64

#include "ble_ant.h"
#include "ant_stack_handler_types.h"

typedef struct ant_speed_sensor_s {
	bool page_change_toggle;
	
} ant_speed_sensor_t;

void ant_bp_tx_init(rc_evt_handler_t on_set_resistance, ant_bp_evt_dfu_enable on_enable_dfu_mode);
void ant_bp_tx_start(void);
void ant_bp_tx_send(irt_power_meas_t * p_power_meas);
void ant_bp_resistance_tx_send(resistance_mode_t mode, uint8_t* level);
void ant_bp_rx_handle(ant_evt_t * p_ant_evt);

#endif	// ANT_BIKE_POWER_H__
