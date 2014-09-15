/* Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#ifndef ANT_BIKE_POWER_H__
#define ANT_BIKE_POWER_H__

#define ANT_BP_COMMAND_OFFSET			4u
#define ANT_BP_SET_WEIGHT_COMMAND		0x60
#define ANT_BP_MOVE_SERVO_COMMAND		0x61
#define ANT_BP_ENABLE_DFU_COMMAND		0x64

// ANT Bike Power specific pages.
#define ANT_BP_PAGE_1               	0x01   /**< Calibration message main data page. */
#define ANT_BP_PAGE_STANDARD_POWER_ONLY 0x10   /**< Standard Power only main data page. */
#define ANT_BP_PAGE_TORQUE_AT_WHEEL		0x11   /**< Power reported as torque at wheel data page, which includes speed. */
#define ANT_BP_PAGE_EXTRA_INFO			0x24   // TODO: My custom page.  Look for better page no for this.

// Generic ANT pages.
#define ANT_PAGE_GETSET_PARAMETERS		0x02
#define ANT_PAGE_MEASURE_OUTPUT			0x03	// Measurement Output Data Page (0x03)
#define ANT_PAGE_REQUEST_DATA			0x46
#define ANT_PAGE_BATTERY_STATUS			0x52


#include "ble_ant.h"
#include "ant_stack_handler_types.h"

typedef struct ant_speed_sensor_s {
	bool page_change_toggle;
	
} ant_speed_sensor_t;

typedef enum
{
	COUNTDOWN_PROGRESS = 0,
	COUNTDOWN_TIME,
	TORQUE = 8,
	TORQUE_LEFT,
	TORQUE_RIGHT,
	FORCE = 16,
	FORCE_LEFT,
	FORCE_RIGHT,
	ZERO_OFFSET = 24,
	TEMPERATURE,
	VOLTAGE
} ant_output_meas_data_type;

void ant_bp_tx_init(ant_ble_evt_handlers_t * evt_handlers);
void ant_bp_tx_start(void);
void ant_bp_tx_send(irt_power_meas_t * p_power_meas);
uint32_t ant_bp_resistance_tx_send(resistance_mode_t mode, uint16_t value);
void ant_bp_page2_tx_send(uint8_t subpage, uint8_t buffer[6], uint8_t tx_type);	// Sends data page 2.

/**@brief	Sends Measurement Output Data Page (0x03).  Normally sent only during calibration.
 *
 */
void ant_bp_page3_tx_send(uint8_t meas_count,
		ant_output_meas_data_type data_type,
		int8_t scale_factor,
		uint16_t timestamp,
		int16_t value);		// signed

void ant_bp_rx_handle(ant_evt_t * p_ant_evt);

uint32_t ant_bp_battery_tx_send(irt_battery_status_t status);

#endif	// ANT_BIKE_POWER_H__
