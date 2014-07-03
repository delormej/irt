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

#ifndef __POWER_H__
#define __POWER_H__

#include <stdint.h>
#include "irt_common.h"
#include "user_profile.h"

#define IRT_ERROR_INVALID_RESISTANCE_LEVEL					IRT_ERROR_RC_BASE_NUM + 0x01

/**@brief	Calculates an estimated power in watts based on current speed, rider
 *			profile including total weight (bike+gear+rider) in kg and the
 *			current magnet servo position.
 */
uint16_t power_watts_calc(float speed_mps, float weight_kg, uint16_t servo_pos);

uint16_t power_torque_calc(int16_t watts, uint16_t period_seconds_2048);

/**@brief	Determines speed and calculates an estimated power in watts based on
 * 			current speed, rider profile including total weight (bike+gear+rider)
 * 			in kg and the current magnet servo position.
 *
 * @return	0 if successful, otherwise and error code.
 */
int32_t power_measure(float total_weight_kg, irt_power_meas_t* p_power_meas);


/**@brief	Initializes power module with a profile pointer that contains things like
 * 			the total rider weight, calibration details, etc...
 *
 */
void power_init(user_profile_t* p_profile);

/**@brief	Calculates and records current power measurement relative to last measurement.
 *
 */
uint32_t power_calc(irt_power_meas_t * current, irt_power_meas_t * last);

uint16_t power_servo_pos_calc(float weight_kg, float speed_mps, float force_needed);

#endif // __POWER_H__
