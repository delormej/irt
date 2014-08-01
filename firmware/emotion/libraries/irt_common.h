/* 
Declarations common to Inside Ride.

Copyright (c) Inside Ride Technologies, LLC 2013
All rights reserved.
*/

#ifndef IRT_COMMON_H
#define IRT_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include "boards.h"
#include "nrf_error.h"
#include "pstorage_platform.h"

//
// Global defines.
//
#define	GRAVITY						9.8f		// Co-efficent of gravity for Earth.  Change for other planets.
#define	MATH_PI						3.14159f

#define DEVICE_NAME                 "E-Motion"                          /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME           "Inside Ride"            			/**< Manufacturer. Will be passed to Device Information Service. */
//#define HW_REVISION                 0x01  							// This is now defined in boards.h	/**< Hardware revision for manufacturer's identification common page. */
#define MANUFACTURER_ID             0xAAAA             				  	/**< Manufacturer ID for manufacturer's identification common page. */
#define MODEL_NUMBER                0x5248								// Model 'HR' in ASCII /**< Model number for manufacturer's identification common page. */

/* VERSION is currently defined in make file.
#define SW_REVISION_MAJ				0			// Limited to 4 bits
#define SW_REVISION_MIN				0			// Limited to 4 bits
#define SW_REVISION_BLD				0			// Full byte
#define SW_REVISION                 "0.0.0"		// major.minor.build
*/

/* DEVICE specific info */
#define ANT_DEVICE_NUMBER			(uint16_t)NRF_FICR->DEVICEID[1]
#define SERIAL_NUMBER				NRF_FICR->DEVICEID[1]

//
// Available device features.
//
#define FEATURE_RESERVED			1UL
#define FEATURE_SMALL_MAG			2UL				// Small magnet is installed vs. Big magnet
#define FEATURE_74_SERVO			32UL			// 7.4 Volt Servo feature.
#define FEATURE_BATTERY_CHARGER		64UL			// Device has a battery charger IC installed.
#define FEATURE_BATTERY_READ_PIN	128UL			// Device requires the use of enabling flow to a capacitor before reading battery voltage.
#define FEATURE_INVALID				65535UL			// Max feature setting of 16 bit.

//
// IRT Factory configured features. These individual defines are made at build time for the bootloader
// and read in irt_common.c::irt_feature_is_available() from flash for app operation.
//
#define IRT_FEATURES 				0UL				// TODO: Need to write a macro to be able to parse these somehow from build flags
													// OR I should be generating one of multiple HEX files into the bootloader at firmware load time.

#define FEATURE_IS_SET(FEATURE) 	irt_feature_is_available(FEATURE)

/*****************************************************************************
* Inside Ride Defines
*****************************************************************************/
#define IRT_SUCCESS				NRF_SUCCESS
#define IRT_ERROR_BASE_NUM      (0x80000)       				///< Error base, hopefully well away from anything else.
#define IRT_ERROR_RC_BASE_NUM   IRT_ERROR_BASE_NUM + (0x100)   	///< Error base for Resistance Control
#define IRT_ERROR_AC_BASE_NUM   IRT_ERROR_BASE_NUM + (0x200)   	///< Error base for Accelerometer

//
// Parameter identifiers for ANT get/set parameters.
//
#define IRT_MSG_PAGE2_SUBPAGE_INDEX			1u					// Index of the subpage in the message buffer.
#define IRT_MSG_PAGE2_DATA_INDEX			2u					// Index of the data in the message buffer.
#define IRT_MSG_SUBPAGE_CRR					16u
#define IRT_MSG_SUBPAGE_SETTINGS			17u
#define IRT_MSG_SUBPAGE_WEIGHT				18u
#define IRT_MSG_SUBPAGE_WHEEL_SIZE			19u
#define IRT_MSG_SUBPAGE_BUTTON_STOPS		20u
#define IRT_MSG_SUBPAGE_SET_CHARGER			21u					// Setting to toggle the charger.
#define IRT_MSG_SUBPAGE_GET_ERROR			22u					// Gets the last error code.

#define IRT_FIFO_SIZE		4	// Must be a power of 2: 4,16,64,256, 1024, see NRF FIFO docs.

/**@brief Cycling Power Service measurement type. */
typedef struct irt_power_meas_s
{
	uint16_t	event_time_2048;											// Event time in 1/2048s.
	int16_t		instant_power;         										// Note this is a SIGNED int16
	float		instant_speed_mps;
	uint16_t	accum_torque;												// Unit is in newton meters with a resolution of 1/32
	uint32_t	accum_wheel_revs;											// BLE uses 32bit value, ANT uses 8 bit.
	uint16_t	accum_wheel_period;											// Increments of 1/2048s rolls over at 32 seconds.
	uint32_t	accum_flywheel_ticks;										// Currently 2 ticks per flywheel rev.
	// TODO: ble cps spec uses accum_energy but we haven't implmented it yet.
	// uint16_t	accum_energy;												// Unit is in kilojoules with a resolution of 1 (used by ble cps).
	uint16_t	last_wheel_event_2048;										// Last time the virtual wheel completed a rotation, basd on wheel size.

	// Resistance state.  TODO: Should this be refactored somewhere else? A pointer would save 8 bytes per entry.
	uint8_t 	resistance_mode;
	uint16_t	resistance_level;
	uint16_t	servo_position;

	// Additional values reported.
	float		temp;														// Measured temperature in c.
	uint8_t		accel_y_lsb;												// Accelerometer reading. TODO: determine if we're going to use.
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
	uint16_t		ant_device_id;
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
		(P_DEVICE)->ant_device_id = ANT_DEVICE_NUMBER;		\
		(P_DEVICE)->hw_revision = HW_REVISION;				\
		(P_DEVICE)->sw_revision.major = SW_REVISION_MAJ;	\
		(P_DEVICE)->sw_revision.minor = SW_REVISION_MIN;	\
		(P_DEVICE)->sw_revision.build = SW_REVISION_BLD;	\
		(P_DEVICE)->serial_num = SERIAL_NUMBER;				\
	} while(0) 												\

// Abstracts a FIFO queue of events.
uint32_t 			irt_power_meas_fifo_init(uint8_t size);
void 	 			irt_power_meas_fifo_free();
irt_power_meas_t* 	irt_power_meas_fifo_next();
irt_power_meas_t* 	irt_power_meas_fifo_first();
irt_power_meas_t* 	irt_power_meas_fifo_last();
__inline__ bool		irt_feature_is_available(uint16_t feature_mask);

#endif // IRT_COMMON_H
