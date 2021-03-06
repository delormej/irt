/******************************************************************************
 * Copyright (c) 2015 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * @file  	magnet.h
 * @brief	Encapsulates the calculations for magnetic resistance.
 *
 */

#ifndef MAGNET_H_
#define MAGNET_H_

#include <stdint.h>
#include <float.h>
#include "irt_common.h"

/*
 * These MIN & MAX positions represent the limits of the math that has been
 * calculated for resistance based on position.  The values are USER position
 * values as opposed to ACTUAL servo position values, which are different
 * MIN & MAX.
 *
 * ACTUAL servo position is user value + servo offset, i.e.
 * 	800 (user) + 351 (offset) = 1,151 (actual)
 * Technically we can drive the servo to as much as 1,000 in actual value, but
 * power flattens out and starts to decline in this range, so we restrict to
 * about 1,100 since most servo offsets are > 300.
 *
 * Based on early modeling the math has always been based on a system of
 * servo positions roughly 699 (max resistance) - 2,107 (min resistance).
 * For the latest models we're constraining that further to 800 - 2,000.
 *
 * NOTE: in some testing, there may be a small amount of actual resistance between
 * the min level we've modeled and the OFF position.
 */
#define MAGNET_POSITION_OFF						2000		// This is where we park the servo, well out of range.
#define MAGNET_POSITION_MIN_RESISTANCE			1600		// Above this position, we don't calculate any watts.
#define MAGNET_POSITION_MAX_RESISTANCE			800

 // Minimum speed for which mag resistance can be calculated.
#define MPH_TO_MPS				0.440704f
#define MIN_SPEED_MPH			7.1f
#define MIN_SPEED_MPS			MIN_SPEED_MPH * MPH_TO_MPS

/**@brief	Structure to encapsulate magnet calibration factors that define a
 * 			3rd order polynomial from a low speed and high speed.
 */
typedef struct mag_calibration_factors_s
{
	int16_t				gap_offset;			// Variable used to adjust for the magnet spacing gap which changes the force by this percent.  Stored as signed short, divide by 100 to get force value (supports negative)
	uint16_t			low_speed_mps;		// Divide by 100 to get speed.
	uint16_t			high_speed_mps;
	uint16_t			root_position;		// Servo position above which we no longer calculate as the curve turns.
	float				low_factors[4];		// 4 factors for each speed.
	float				high_factors[4];
} mag_calibration_factors_t;

/**@brief	Initializes the magnet module with a set of factors for a low and
 * 			high speed polynomial.
 */
void magnet_init(mag_calibration_factors_t* p_factors);

/**@brief   Returns the default magnet calibration factors.
 * 
 */
mag_calibration_factors_t magnet_get_default_factors();

/**@brief	Calculates watts added by the magnet for a given speed at magnet
 *			position.  
 */
float magnet_watts(float speed_mps, uint16_t position);

/**@brief	Calculates magnet position for a given speed and watt target.
 *
 */
uint16_t magnet_position(float speed_mps, float mag_watts, target_power_e* p_target);

#endif /* MAGNET_H_ */
