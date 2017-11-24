/*
*******************************************************************************
*
* By Jason De Lorme <jason@insideride.com>
* Inside Ride Technologies, LLC
*
* This module is responsible for calculating power based on rider profile 
* e.g. weight, current speed and resistance.
*
********************************************************************************/

#ifndef __POWER_H__
#define __POWER_H__

#include <stdint.h>
#include "irt_common.h"
#include "user_profile.h"

/**@brief	Determines the right power calculation method (linear vs. bicycling science)
 * 			and returns estimated power based on speed (no magnet).
 */
float power_magoff(float speed_mps);

/**@brief	Calculates and records current power measurement relative to last measurement.
 *
 */
uint32_t power_calc(irt_context_t* p_meas);

/**@brief	Initializes power module with a profile pointer that contains things like
 * 			the total rider weight, calibration details, etc...
 *
 */
void power_init(user_profile_t* p_profile);

#endif // __POWER_H__
