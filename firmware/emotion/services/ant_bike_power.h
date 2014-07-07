/* Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#ifndef ANT_BIKE_POWER_H__
#define ANT_BIKE_POWER_H__

#define ANT_BP_COMMAND_OFFSET			4u
#define ANT_BP_SET_WEIGHT_COMMAND		0x60
#define ANT_BP_MOVE_SERVO_COMMAND		0x61
#define ANT_BP_ENABLE_DFU_COMMAND		0x64

#include "ble_ant.h"
#include "ant_stack_handler_types.h"

typedef struct ant_speed_sensor_s {
	bool page_change_toggle;
	
} ant_speed_sensor_t;

void ant_bp_tx_init(ant_ble_evt_handlers_t * evt_handlers);
void ant_bp_tx_start(void);
void ant_bp_tx_send(irt_power_meas_t * p_power_meas);
uint32_t ant_bp_resistance_tx_send(resistance_mode_t mode, uint16_t value);
void ant_bp_page2_tx_send(uint8_t subpage, uint8_t buffer[6], uint8_t tx_type, uint8_t tx_count);	// Sends data page 2.
void ant_bp_rx_handle(ant_evt_t * p_ant_evt);

#endif	// ANT_BIKE_POWER_H__
