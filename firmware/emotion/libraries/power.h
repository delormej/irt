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
#include "insideride.h"

#define IRT_ERROR_INVALID_RESISTANCE_LEVEL					IRT_ERROR + 0x01

uint8_t calc_power2(float speed_mps, float weight_kg, 
	uint16_t servo_pos, int16_t* p_watts);

/**@brief	Calculates an estimated power in watts based on current speed, rider
 *			profile including total weight (bike+gear+rider) in pounds and the
 *			current mag resistance level.
 *
 * @return	IRT_SUCCESS if successful, otherwise and error code.
 *
 * @params[out]	p_watts	Power in watts..
 *
 * @note	Speed is miles per hour, weight is in pounds, resistance level is 0-9.
 *
 */
uint8_t calc_power(float speed_mph, float total_weight_lb,
	uint8_t resistance_level, int16_t* p_watts);

/**@brief	Calculates torque based on watts for a given period.  The period is
 *			in 1/2048th seconds.  Torque is returned in 1/32 nanometers.
 *
 */
uint8_t calc_torque(int16_t watts, uint16_t period_seconds_2048, uint16_t* p_torque);

#endif
