/* 
Declarations common to Inside Ride.

Copyright (c) Inside Ride Technologies, LLC 2013
All rights reserved.
*/

#ifndef IRT_COMMON_H
#define IRT_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include "boards.h"
#include "nrf_error.h"
#include "nrf51.h"
#include "pstorage_platform.h"

//
// Global defines.
//
#define FACTORY_SETTINGS_BASE		(((uint8_t*)(NRF_UICR)) + 0x080) 	// Should be NRF_UICR->CUSTOMER[0] or 0x10001080 as per nRF51 Series Manual v3.0
#define FEATURES 					((uint16_t *) FACTORY_SETTINGS_BASE)

#define DEVICE_NAME                 "E-Motion"                          /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME           "Inside Ride"            			/**< Manufacturer. Will be passed to Device Information Service. */
//#define HW_REVISION                 0x01  							// This is now defined in boards.h	/**< Hardware revision for manufacturer's identification common page. */
#define MANUFACTURER_ID             0x005D             				  	// Manufacturer ID 93 (0x005D) for Inside Ride
#define MODEL_NUMBER                0x5248								// Model 'HR' in ASCII /**< Model number for manufacturer's identification common page. */

/* VERSION is currently defined in make file.
#define SW_REVISION_MAJ				0			// Limited to 4 bits
#define SW_REVISION_MIN				0			// Limited to 4 bits
#define SW_REVISION_BLD				0			// Full byte
#define SW_REVISION                 "0.0.0"		// major.minor.build
*/

/* DEVICE specific info */
#define ANT_DEVICE_NUMBER			(uint16_t)NRF_FICR->DEVICEID[1]
#define SERIAL_NUMBER				NRF_FICR->DEVICEID[1]				// TODO: We should have something unique here.

//
// Available device features.
//
#define FEATURE_RESERVED			1UL
//#define FEATURE_BIG_MAG			2UL				// Small magnet is installed vs. Big magnet
//#define FEATURE_74_SERVO			32UL			// 7.4 Volt Servo feature.
#define FEATURE_BATTERY_CHARGER		64UL			// Device has a battery charger IC installed.
//#define FEATURE_BATTERY_READ_PIN	128UL			// Device requires the use of enabling flow to a capacitor before reading battery voltage.
#define FEATURE_INVALID				65535UL			// Max feature setting of 16 bit.

/*
 * Returns whether a specific feature is available on this board as configured at manufacturing time by IRT.
static bool __inline__ irt_feature_is_available(uint16_t feature)
{
	uint32_t features;
	bool available;

	features = *FEATURES;

	available = ( features != FEATURE_INVALID ) &&
			(  ( features & feature ) == feature  );

	return available;
}

//#define FEATURE_AVAILABLE(FEATURE) 	irt_feature_is_available(FEATURE)
 */
#define FEATURE_AVAILABLE(FEATURE) \
	((*FEATURES & FEATURE) == FEATURE)

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
#define IRT_MSG_PAGE2_SUBPAGE_INDEX			2u					// Index of the subpage in the message buffer.
#define IRT_MSG_PAGE2_DATA_INDEX			3u					// Index of the data in the message buffer.

#define IRT_MSG_SUBPAGE_CRR					16u
#define IRT_MSG_SUBPAGE_SETTINGS			17u
#define IRT_MSG_SUBPAGE_WEIGHT				18u
#define IRT_MSG_SUBPAGE_WHEEL_SIZE			19u
#define IRT_MSG_SUBPAGE_SERVO_POS			20u					// Gets servo positions.
#define IRT_MSG_SUBPAGE_CHARGER				21u					// Get/set charger status.
#define IRT_MSG_SUBPAGE_GET_ERROR			22u					// Gets the last error code.
#define IRT_MSG_SUBPAGE_SERVO_OFFSET		23u					// Get/set servo offset.
#define IRT_MSG_SUBPAGE_CA_SPEED			24u					// Gets instant speed + time for calibration.
#define IRT_MSG_SUBPAGE_AUXPWR				25u					// Gets/set whether power goes to J7-4.
#define IRT_MSG_SUBPAGE_TEMP				26u					// TODO: this is not really a get/set, move this. Gets current temperature.
#define IRT_MSG_SUBPAGE_SLEEP				27u					// When set puts the device in lower power mode.
#define IRT_MSG_SUBPAGE_DEBUG_SPEED			28u					// # of ticks to emulate at 4hz for debugging simulated speed.
#define IRT_MSG_SUBPAGE_FEATURES			29u					// Configured features.
#define IRT_MSG_SUBPAGE_DRAG				30u					// Co-efficient of drag factor for calibration.
#define IRT_MSG_SUBPAGE_RR					31u					// Co-efficient of rolling resistance for calibration.
#define IRT_MSG_SUBPAGE_GAP_OFFSET			32u					// Co-efficient for magnet gap offset.
#define IRT_MSG_SUBPAGE_POWER_METER_ID		33u					// Get/set power meter ant device id to pair with.

#define IRT_FIFO_SIZE		4	// Must be a power of 2: 4,16,64,256, 1024, see NRF FIFO docs.

/**@brief	Battery status structure.
 */
typedef struct irt_battery_status_s
{
	uint8_t	  	fractional_volt;
	uint8_t		coarse_volt : 4;
	uint8_t		status : 3;
	uint8_t		resolution : 1;
	uint32_t	operating_time;
} irt_battery_status_t;

/**@brief FE-C Specific device state.
 */
typedef enum  {
	FE_RESERVED = 0,
	FE_ASLEEP_OFF,
	FE_READY,
	FE_IN_USE,
	FE_FINISHED_PAUSED
} fe_state_e;	// Used in FE State bit.

/**@brief Possible states for power target.
 */
typedef enum {
	TARGET_AT_POWER = 0,													// Able to hit target at current speed.
	TARGET_SPEED_TOO_LOW,													// Rider must speed up in order to hit power target.
	TARGET_SPEED_TOO_HIGH,													// Rider must slow down in order to lower power to target.
	TARGET_UNDETERMINED														// Undetermined.
} target_power_e;

/**@brief   Structure represents calibration status.
 *
 */
typedef struct {
    uint8_t     status : 2;      // 0 = not requested, 1 = requested, 2 = in progress, 3 = finished  
    uint8_t     zero_offset : 1; // Flagged if requested
    uint8_t     spin_down : 1;   // Flagged if requested
    uint8_t     temperature : 2; // Status of whether temp is too low or OK
    uint8_t     speed : 2;       // Status if speed is too low or OK.
} calibration_status_t;

/**@brief Cycling Power Service measurement type. */
typedef struct
{
	uint8_t		event_count;												// Incremented each time speed/power are calculated.
	uint32_t	last_wheel_event_2048;										// Last time the virtual wheel completed a rotation, based on wheel size.
	uint16_t	wheel_period;												// Time between the previous and current virtual wheel rotation in 1/2048s.

	int16_t		instant_power;         										// Note this is a SIGNED int16
	float		instant_speed_mps;

	uint16_t	accum_torque;												// Unit is in newton meters with a resolution of 1/32
	uint32_t	accum_wheel_revs;											// BLE uses 32bit value, ANT uses 8 bit.
	uint16_t	accum_wheel_period;											// Increments of 1/2048s rolls over at 32 seconds.
	uint16_t	accum_flywheel_ticks;										// Currently 2 ticks per flywheel rev.

	// TODO: ble cps spec uses accum_energy but we haven't implmented it yet.
	// uint16_t	accum_energy;												// Unit is in kilojoules with a resolution of 1 (used by ble cps).

	uint8_t 	resistance_mode;
	uint16_t	resistance_level;
	uint16_t	servo_position;

	// Additional values reported.
	float		temp;														// Measured temperature in c.
	uint8_t		accel_y_lsb;												// Accelerometer reading. TODO: determine if we're going to use.
	uint8_t		accel_y_msb;

	irt_battery_status_t battery_status;
	
	//
	// FE-C State maintained.
	//
	uint8_t 		elapsed_time; 											// 1/4 seconds since the start of the workout.
	uint8_t			distance_m;												// Meters travelled since the start of the workout. 
	fe_state_e		fe_state : 3;
	uint8_t			lap_toggle : 1;
	uint8_t			virtual_speed_flag : 1;
	target_power_e	target_power_limits : 2;
	uint8_t			bike_power_calibration_required : 1;
	uint8_t			resistance_calibration_required : 1;
	uint8_t			user_configuration_required : 1;

	// Flag if power meter is connected.
	bool			power_meter_paired;
} irt_context_t;

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

#define MAX_RESISTANCE_LEVEL_COUNT 		10u 			// Max number of resistance levels possible to set.

/**@brief	Servo positions available.
 */
typedef struct servo_positions_s
{
	uint8_t				count;
	uint16_t			positions[MAX_RESISTANCE_LEVEL_COUNT];
} servo_positions_t;

#endif // IRT_COMMON_H
