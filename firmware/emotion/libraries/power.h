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

#define DEFAULT_CRR						0.03f										// Default value of rolling resistance if not overridden with slope/intercept.

/**@brief	Calculates and records current power measurement relative to last measurement.
 *
 */
uint32_t power_calc(irt_power_meas_t* current, irt_power_meas_t* last, float* p_rr_force);


/**@brief	Calculates the required servo position given force needed.
 *
 */
uint16_t power_servo_pos_calc(float force_needed);

/**@brief	Initializes power module with the total rider weight & calibrated rolling resistance.
 *
 */
void power_init(user_profile_t* p_profile);

#endif // __POWER_H__
