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

#define PROFILE_VERSION					2u	// Current version of the profile.

//
// Available device features. Default setting should result in 0.
//
#define FEATURE_DEFAULT					0xFFFFFFFF
#define FEATURE_RESERVED				1UL
#define FEATURE_ACCEL_SLEEP_OFF			2UL
#define FEATURE_BIG_MAG					4UL
#define FEATURE_SMALL_MAG				8UL
#define FEATURE_OTHER_MAG				16UL
// FUTURE features:
// BTLE_OFF
// ANT_CTRL_OFF
// ANT_BP_OFF
// ANT_FEC_OFF
#define FEATURE_ANT_EXTRA_INFO			65536UL			// Set a mid bit
#define FEATURE_TEST					0x80000000		// Set the highest bit
// Number of wheel revs to flywheel?

#define FEATURE_IS_SET(SETTINGS, FEATURE) \
	((SETTINGS & FEATURE) == FEATURE)

/**@brief	Structure used to for storing/reading user profile.
 * 			NOTE: This must be divisable by 4 (sizeof(uin32_t).
 */
typedef struct user_profile_s {
	uint8_t		version;					// Version of the profile for future compatibility purposes.
	uint16_t	total_weight_kg;
	uint16_t	wheel_size_mm;
	uint32_t	settings;					// Bitmask of settings.
	uint16_t	calibrated_crr;				// Calibrated co-efficient of rolling resistance (1/10,000 value). e.g. value = 0.02823f
	uint8_t		reserved[5];
} user_profile_t;

/**@brief Initializes access to storage. */
uint32_t user_profile_init(void);

/**@brief Loads the user's profile from device persistent storage. */
uint32_t user_profile_load(user_profile_t *p_user_profile);

/**@brief Stores user profile in persistent storage on the device. */
uint32_t user_profile_store(user_profile_t *p_user_profile);

#endif // __USER_PROFILE_H__
