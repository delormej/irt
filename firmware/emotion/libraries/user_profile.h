/*
*******************************************************************************
*
* By Jason De Lorme <jason@insideride.com>
* http://www.insideride.com
*
* This module is responsible for calculating power based on rider profile 
* e.g. weight, current speed and resistance.
*
********************************************************************************/

#ifndef __USER_PROFILE_H__
#define __USER_PROFILE_H__

#include <stdint.h>
#include <float.h>
#include "pstorage.h"
#include "irt_common.h"

#define PROFILE_VERSION					9u	// Current version of the profile.

#define SETTING_ACL_SLEEP_ON			1UL				// Put device to sleep when accelerometer signals no motion.
#define SETTING_BTLE_ENABLED			2UL				// BTLE Enabled
#define SETTING_ANT_CTRL_ENABLED		4UL				// ANT+ Remote Control enabled.
#define SETTING_ANT_BP_ENABLED			8UL				// ANT+ Bike Power profile enabled.
#define SETTING_ANT_FEC_ENABLED			16UL			// ANT+ Fitness Equipment Control profile enabled.
#define SETTING_ANT_EXTRA_INFO_ENABLED	32UL			// Send custom IRT EXTRA_INFO message (mostly for debugging).
#define SETTING_ANT_BIKE_SPEED_ENABLED	64UL			// ANT+ Bike Speed enabled.
#define SETTING_ANT_TR_PAUSE_ENABLED	128UL			// ANT+ Command via button to pause Trainer Road.
#define SETTING_INVALID					65535UL			// Max for 16 bit settings.

/**@brief	Helper macro for determining if a setting is flagged.
 */
#define SETTING_IS_SET(SETTINGS, SETTING) \
	((SETTINGS & SETTING) == SETTING_VALUE(SETTING))

/**@brief	Determine if the setting should be persisted.
 */
#define SETTING_PERSIST(SETTING) \
	(SETTING & 0x8000)					// Most significant bit indicates if we should persist to flash or not.

/**@brief	Helper to get the value independent of whether we persist.
 */
#define SETTING_VALUE(SETTING) \
	(SETTING & 0x7FFF)

/**@brief	Structure used to for storing/reading user profile.
 * 			Must be at least PSTORAGE_MIN_BLOCK_SIZE (i.e. 16 bytes) in size and should be word aligned (16 bits).
 */
typedef struct user_profile_s {
	uint8_t		version;					// Version of the profile for future compatibility purposes.
	uint8_t		reserved;					// Padding (word alignment size is 16 bits).
	uint16_t	settings;					// Bitmask of feature/settings to turn on/off.
	uint16_t	total_weight_kg;			// Stored in int format 1/100, e.g. 8181 = 81.81kg
	uint16_t	wheel_size_mm;
	uint16_t	ca_slope;					// Calibration slope.  Stored in 1/1,000 e.g. 20741 = 20.741
	uint16_t	ca_intercept;				// Calibration intercept. This value is inverted on the wire -1/1,000 e.g. 40144 = -40.144
	uint16_t	ca_temp;					// need Temperature recorded when calibration was set.  See: Bicycling Science (1% drop in Crr proportional to 1 degree change in temp).
	uint16_t	ca_reserved;				// Placeholder for another calibration value if necessary.
	int16_t		servo_offset;				// Calibration offset for servo position.
	servo_positions_t servo_positions;		// Servo positions (size should be 21 bytes)
	float		ca_drag;					// Calibration co-efficient of drag which produces the "curve" from a coastdown.
	float		ca_rr;						// Co-efficient of rolling resistance.
	uint16_t	ca_gap_offset;				// Variable used to adjust for the magnet spacing gap which changes the force by this percent.  Stored as unsigned short, divide by 1,000 to get % value.
	mag_calibration_factors_t ca_mag_factors; // Magnet calibration factors.
	//uint8_t		reserved_2[7]; // (sizeof(servo_positions_t)+2) % 16];					// For block size alignment -- 16 bit alignment
} user_profile_t;

/**@brief Initializes access to storage. */
uint32_t user_profile_init(pstorage_ntf_cb_t cb);

/**@brief Loads the user's profile from device persistent storage. */
uint32_t user_profile_load(user_profile_t *p_user_profile);

/**@brief Stores user profile in persistent storage on the device. */
uint32_t user_profile_store(user_profile_t *p_user_profile);

#endif // __USER_PROFILE_H__
