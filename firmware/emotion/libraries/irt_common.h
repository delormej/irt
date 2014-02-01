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
#define IRT_SUCCESS		0x0
#define IRT_ERROR			0x1


// TODO: This should be genericized to work for BLE & ANT
/**@brief Cycling Power Service measurement type. */
typedef struct irt_power_meas_s
{
	uint16_t  flags;																		// 16 bits defined here: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.cycling_power_measurement.xml
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
	uint8_t 	resistance_mode;
	uint16_t	resistance_level;
	float			instant_speed_mps;
} irt_power_meas_t;


#endif // IRT_COMMON_H
