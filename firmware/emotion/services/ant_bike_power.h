/* Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#ifndef ANT_BIKE_POWER_H__
#define ANT_BIKE_POWER_H__

// ANT Bike Power specific pages.
#define ANT_BP_PAGE_CALIBRATION			0x01	// Calibration page
#define ANT_BP_PAGE_STANDARD_POWER_ONLY 0x10   /**< Standard Power only main data page. */
#define ANT_BP_PAGE_TORQUE_AT_WHEEL		0x11   /**< Power reported as torque at wheel data page, which includes speed. */
#define ANT_BP_PAGE_CTF_MAIN            0x20    // Crank Torque Frequency Main Data Page

#define ANT_BP_CAL_PARAM_RESPONSE		0xBB
#define ANT_BP_CAL_SUCCESS_RESPONSE		0xAC
#define ANT_BP_CAL_FAIL_RESPONSE		0xAF

#include <stdbool.h>
#include <stdint.h>
#include "ble_ant.h"
#include "ant_stack_handler_types.h"

uint32_t ant_bp_rx_init(bp_evt_handler_t on_bp_power_data, uint16_t device_id);
uint32_t ant_bp_rx_close(); // stop & close the channel.
void ant_bp_rx_handle(ant_evt_t * p_ant_evt);
void ant_bp_rx_start(void);
float ant_bp_avg_power(uint8_t seconds); // returns average power.
void ant_bp_avg_power_reset(); // resets averaging.
uint16_t ant_bp_power_meter_id_get(); // returns connected power meter Id or 0 if not connected.

#endif	// ANT_BIKE_POWER_H__

