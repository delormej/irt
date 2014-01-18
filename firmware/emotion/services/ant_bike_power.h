/* Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#ifndef ANT_BIKE_POWER_H__
#define ANT_BIKE_POWER_H__

#include "ble_ant.h"

typedef struct ant_speed_sensor_s {
	bool page_change_toggle;
	
} ant_speed_sensor_t;

void ant_bp_tx_init(void) { }
void ant_bp_tx_start(void) { }
void ant_bp_tx_send(ble_cps_meas_t * p_cps_meas) { }

#endif	// ANT_BIKE_POWER_H__
