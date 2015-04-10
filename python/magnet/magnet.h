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
#define WATTS_MODEL_MIN							13.0f		// Below this threshold, we might need to solve linearly.

#define MIN_SPEED_MPS			7.1f * 0.440704f// Minimum speed for which mag resistance can be calculated.

/**@brief	Structure used to store polynomial coefficients.
 *
 */
typedef struct poly_coeff_s {
	float speed_mps;
	float coeff[];
} poly_coeff_t;

/**@brief	Sets the cofficients for the 3rd order polynomial.
 *
 */
void magnet_set_coeff(poly_coeff_t low_speed, poly_coeff_t high_speed);

/**@brief	Calculates watts added by the magnet for a given speed at magnet
 *			position.
 */
float magnet_watts(float speed_mps, uint16_t position);

/**@brief	Calculates magnet position for a given speed and watt target.
 *
 */
uint16_t magnet_position(float speed_mps, float mag_watts);

#endif /* MAGNET_H_ */
