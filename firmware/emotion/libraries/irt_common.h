/* 
Declarations common to Inside Ride.

Copyright (c) Inside Ride Technologies, LLC 2013
All rights reserved.
*/

#ifndef IRT_COMMON_H
#define IRT_COMMON_H

#include <stdint.h>

/*****************************************************************************
* INSIDERIDE Defines
*****************************************************************************/
#define IRT_SUCCESS			0x0
#define IRT_ERROR			0x1
#define IRT_FIFO_SIZE		4	// Must be a power of 2: 4,16,64,256, 1024, see NRF FIFO docs.


// TODO: This should be genericized to work for BLE & ANT
/**@brief Cycling Power Service measurement type. */
typedef struct irt_power_meas_s
{
	uint16_t  	flags;																		// 16 bits defined here: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.cycling_power_measurement.xml
	int16_t		instant_power;         										// Note this is a SIGNED int16
	uint8_t		pedal_power_balance;
	uint16_t	accum_torque;															// Unit is in newton metres with a resolution of 1/32
	uint32_t	accum_wheel_revs;
	uint16_t	last_wheel_event_time;										// Unit is in seconds with a resolution of 1/2048. 
	uint16_t	accum_crank_rev;
	uint16_t	last_crank_event_time;										// Unit is in seconds with a resolution of 1/1024. 
	int16_t		max_force_magnitude;											// Unit is in newtons with a resolution of 1.
	int16_t		min_force_magnitude;											// Unit is in newtons with a resolution of 1.
	int16_t		max_torque_magnitude;											// Unit is in newton metres with a resolution of 1/32
	int16_t		min_torque_magnitude;											// Unit is in newton metres with a resolution of 1/32
	uint16_t	max_angle:12;																// Unit is in degrees with a resolution of 1. 
	uint16_t	min_angle:12;																// Unit is in degrees with a resolution of 1. 
	uint16_t	top_dead_spot_angle;											// Unit is in degrees with a resolution of 1. 
	uint16_t	bottom_dead_spot_angle;										// Unit is in degrees with a resolution of 1. 
	uint16_t	accum_energy;															// Unit is in kilojoules with a resolution of 1.
	float		instant_speed_mps;
	// TODO: these should be removed & seperated from power measurement.  
	// This is only temporary until we use the scheduler.
	uint8_t 	resistance_mode;
	uint16_t	resistance_level;
	uint16_t	servo_position;
} irt_power_meas_t;

// Abstracts a FIFO queue of events.
uint32_t irt_power_meas_fifo_init(uint8_t size);
void 	 irt_power_meas_fifo_free();
uint32_t irt_power_meas_fifo_op(irt_power_meas_t** first, irt_power_meas_t** next);

#endif // IRT_COMMON_H
