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

#define PROFILE_VERSION					4u	// Current version of the profile.

#define SETTING_ACL_SLEEP_ON			1UL				// Put device to sleep when accelerometer signals no motion.
#define SETTING_BTLE_ENABLED			2UL				// BTLE Enabled
#define SETTING_ANT_CTRL_ENABLED		4UL				// ANT+ Remote Control enabled.
#define SETTING_ANT_BP_ENABLED			8UL				// ANT+ Bike Power profile enabled.
#define SETTING_ANT_FEC_ENABLED			16UL			// ANT+ Fitness Equipment Control profile enabled.
#define SETTING_ANT_EXTRA_INFO_SEND		32UL			// Send custom IRT EXTRA_INFO message (mostly for debugging).
#define SETTING_INVALID					65535UL			// Max for 16 bit settings.

/**@brief	Helper macro for determining if a setting is flagged.
 */
#define SETTING_IS_SET(SETTINGS, SETTING) \
	(SETTINGS != SETTING_INVALID) && \
	((SETTINGS & SETTING) == SETTING)

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
	uint16_t	future[2];					// For block size alignment -- 16 bytes
} user_profile_t;

/**@brief Initializes access to storage. */
uint32_t user_profile_init(void);

/**@brief Loads the user's profile from device persistent storage. */
uint32_t user_profile_load(user_profile_t *p_user_profile);

/**@brief Stores user profile in persistent storage on the device. */
uint32_t user_profile_store(user_profile_t *p_user_profile);

#endif // __USER_PROFILE_H__
