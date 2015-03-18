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

/**@brief	Calculates and records current power measurement relative to last measurement.
 *
 */
uint32_t power_calc(user_profile_t* p_profile, irt_context_t* p_meas);

/**@brief	Initializes power module with a profile pointer that contains things like
 * 			the total rider weight, calibration details, etc...
 * 			Takes a default co-efficient for rolling resistance (i.e. 0.03).
 * 			This is only used if profile doesn't contain a slope/intercept override.
 *
 */
void power_init(user_profile_t* p_profile, uint16_t default_crr);

#endif // __POWER_H__
