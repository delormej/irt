/* 
Declarations common to Inside Ride.

Copyright (c) Inside Ride Technologies, LLC 2013
All rights reserved.
*/

#ifndef IRT_COMMON_H
#define IRT_COMMON_H

#include <stdint.h>
#include "nrf_error.h"

/*****************************************************************************
* Inside Ride Defines
*****************************************************************************/
#define IRT_SUCCESS				NRF_SUCCESS
#define IRT_ERROR_BASE_NUM      (0x80000)       				///< Error base, hopefully well away from anything else.
#define IRT_ERROR_RC_BASE_NUM   IRT_ERROR_BASE_NUM + (0x100)   	///< Error base for Resistance Control
#define IRT_ERROR_AC_BASE_NUM   IRT_ERROR_BASE_NUM + (0x200)   	///< Error base for Accelerometer

#define IRT_FIFO_SIZE		4	// Must be a power of 2: 4,16,64,256, 1024, see NRF FIFO docs.

/**@brief Cycling Power Service measurement type. */
typedef struct irt_power_meas_s
{
	uint16_t  	flags;																		// 16 bits defined here: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.cycling_power_measurement.xml
	int16_t		instant_power;         										// Note this is a SIGNED int16
//	uint8_t		pedal_power_balance;
	uint16_t	accum_torque;															// Unit is in newton metres with a resolution of 1/32
	uint32_t	accum_wheel_revs;
	uint16_t	last_wheel_event_time;										// Unit is in seconds with a resolution of 1/2048. 
/*	uint16_t	accum_crank_rev;
	uint16_t	last_crank_event_time;										// Unit is in seconds with a resolution of 1/1024. 
	int16_t		max_force_magnitude;											// Unit is in newtons with a resolution of 1.
	int16_t		min_force_magnitude;											// Unit is in newtons with a resolution of 1.
	int16_t		max_torque_magnitude;											// Unit is in newton metres with a resolution of 1/32
	int16_t		min_torque_magnitude;											// Unit is in newton metres with a resolution of 1/32
	uint16_t	max_angle:12;																// Unit is in degrees with a resolution of 1. 
	uint16_t	min_angle:12;																// Unit is in degrees with a resolution of 1. 
	uint16_t	top_dead_spot_angle;											// Unit is in degrees with a resolution of 1. 
	uint16_t	bottom_dead_spot_angle;*/										// Unit is in degrees with a resolution of 1. 
	uint16_t	accum_energy;															// Unit is in kilojoules with a resolution of 1.
	uint16_t	wheel_period_2048;
	float		instant_speed_mps;
	// TODO: these should be removed & seperated from power measurement.  
	// This is only temporary until we use the scheduler.
	uint8_t 	resistance_mode;
	uint16_t	resistance_level;
	uint16_t	servo_position;
	uint32_t	accum_flywheel_revs;
	float		temp;
	uint8_t		accel_y_lsb;
	uint8_t		accel_y_msb;
} irt_power_meas_t;

//
// 2 byte version supports 4 bits for major, 4 bits for minor, 8 bits for build #.
//
typedef struct irt_sw_revision_s
{
	uint8_t		major:4;
	uint8_t		minor:4;
	uint8_t		build;
} sw_revision_t;

//
// Device information structure used by ANT & BLE stacks.
//
typedef struct irt_device_info_s
{
	char* 			device_name;
	char* 			manufacturer_name;
	uint16_t		manufacturer_id;
	uint16_t		model;
	uint8_t			ant_device_num;
	uint8_t			hw_revision;
	sw_revision_t	sw_revision;
	uint32_t		serial_num;
} irt_device_info_t;

#define IRT_SW_REV_TO_CHAR(P_REV, P_CHAR)			\
	do												\
	{												\
		sprintf(P_CHAR, "%i.%i.%i",					\
			(P_REV)->major,							\
			(P_REV)->minor,							\
			(P_REV)->build);							\
	} while(0)										\

// Macro to populate device info.
#define SET_DEVICE_INFO(P_DEVICE)							\
	do 														\
	{ 														\
		memset((P_DEVICE), 0, sizeof(irt_device_info_t));	\
		(P_DEVICE)->device_name = DEVICE_NAME; 				\
		(P_DEVICE)->manufacturer_name = MANUFACTURER_NAME;	\
		(P_DEVICE)->manufacturer_id = MANUFACTURER_ID;		\
		(P_DEVICE)->model = MODEL_NUMBER;					\
		(P_DEVICE)->ant_device_num = ANT_DEVICE_NUMBER;		\
		(P_DEVICE)->hw_revision = HW_REVISION;				\
		(P_DEVICE)->sw_revision.major = SW_REVISION_MAJ;	\
		(P_DEVICE)->sw_revision.minor = SW_REVISION_MIN;	\
		(P_DEVICE)->sw_revision.build = SW_REVISION_BLD;	\
		(P_DEVICE)->serial_num = SERIAL_NUMBER;				\
	} while(0) 												\

// Abstracts a FIFO queue of events.
uint32_t irt_power_meas_fifo_init(uint8_t size);
void 	 irt_power_meas_fifo_free();
uint32_t irt_power_meas_fifo_op(irt_power_meas_t** first, irt_power_meas_t** next);

#endif // IRT_COMMON_H
