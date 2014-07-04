/*
*******************************************************************************
*
* By Jason De Lorme <jjdelorme@yahoo.com>
* http://www.roadacious.com
*
* This module is responsible for calculating power based on rider profile 
* e.g. weight, current speed and resistance.
*
********************************************************************************/

#ifndef __USER_PROFILE_H__
#define __USER_PROFILE_H__

#include <stdint.h>

//
// Available device features. Default setting should result in 0.
//
#define FEATURE_DEFAULT					0xFFFFFFFF
#define FEATURE_RESERVED				1UL
#define FEATURE_ACCEL_SLEEP_OFF			2UL
// FUTURE features:
// BTLE_OFF
// ANT_CTRL_OFF
// ANT_BP_OFF
// ANT_SEND_EXTRA_INFO

#define FEATURE_IS_SET(SETTINGS, FEATURE) \
	((SETTINGS & FEATURE) == FEATURE)

typedef struct user_profile_s {
	float		total_weight_kg;
	uint16_t	wheel_size_mm;
	uint32_t	settings;					// Bitmask of settings.
	uint16_t	calibration_crr;			// Calibration offset (1/10,000 value).
} user_profile_t;


/**@brief Initializes access to storage. */
uint32_t user_profile_init(void);

/**@brief Loads the user's profile from device persistent storage. */
uint32_t user_profile_load(user_profile_t *p_user_profile);

/**@brief Stores user profile in persistent storage on the device. */
uint32_t user_profile_store(user_profile_t *p_user_profile);

#endif // __USER_PROFILE_H__
