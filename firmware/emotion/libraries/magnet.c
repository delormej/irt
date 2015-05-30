/******************************************************************************
 * Copyright (c) 2015 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * @file  	magnet.c
 * @brief	Encapsulates the calculations for magnetic resistance.
 *
 */

#include "magnet.h"

#include <stdbool.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include "irt_common.h"

#include "debug.h"
#include "math/math_private.h"

/**@brief Debug logging for module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define MAG_LOG debug_log
#else
#define MAG_LOG(...)
#endif // ENABLE_DEBUG_LOG

#define COEFF_COUNT							4u				// Cubic poynomial has 4 coefficients.
#define MAGNET_POSITION_MODEL_MIN			1500u			// Represents the minimum position the 3r order polynomial supports.
															// The position between MAGNET_POSITION_MIN_RESISTANCE and this is linear.
// Macro to convert gap offset storage format to percent.
#define GAP_OFFSET_TO_PCT(x) 				(gap_offset / 1000.0f)

#define SPEED1 		mp_mag_calibration_factors->low_speed_mps
#define SPEED2 		mp_mag_calibration_factors->high_speed_mps
#define COEFF_1		mp_mag_calibration_factors->low_factors
#define COEFF_2		mp_mag_calibration_factors->high_factors

static mag_calibration_factors_t* mp_mag_calibration_factors;

/**@brief	Calculates the coefficient values for a cubic polynomial
 *			that plots a power curve for the magnet at a given speed.
 *
 *@note		Uses linear interpolation to calculate each coefficient
 *			value based on known curves for 15 mph & 25 mph.
 */
static void curve_coeff(float speed_mps, float *coeff)
{
	for (uint8_t ix = 0; ix < COEFF_COUNT; ix++)
	{
		coeff[ix] = COEFF_1[ix] +
			((speed_mps - SPEED1) / (SPEED2 - SPEED1)) *
			(COEFF_2[ix] - COEFF_1[ix]);
	}
}

/**@brief	Calculates the bottom linear portion of watts returned between
 * 			the lowest spot the model calcuates and where we have known 0
 * 			watt values returned for a given speed.
 */
static float watts_linear(const float watts, uint16_t position)
{
	/* Adjust to a simple linear formula between the minimum watts returned by the model
			 * position and the position where we know it is 0.  */
	float linear_watts;

	linear_watts = (
				(float)(MAGNET_POSITION_MIN_RESISTANCE - position) /
				(float)(MAGNET_POSITION_MIN_RESISTANCE - MAGNET_POSITION_MODEL_MIN)
			) * watts;

	MAG_LOG("pos: %i, watts_linear: %.2f from min: %.2f\r\n", position,
			linear_watts, watts);

	return linear_watts;
}

/*@brief 	The model only goes so low.  For higher speeds at the model min
 * 			there may be less watts obtainable in between the model min and
 * 			where we know 0 occurs.
 *
 * 			Returns 0 if the value is in range.
 */
static uint16_t position_linear(float speed_mps, float mag_watts, uint16_t gap_offset)
{
	uint16_t position = 0;
	float min_watts = magnet_watts(speed_mps, MAGNET_POSITION_MODEL_MIN, gap_offset);

	// Check if we're asking for less than the model can support.
	if (mag_watts < min_watts)
	{
		position = ( ((min_watts - mag_watts) / min_watts) *
				(MAGNET_POSITION_MIN_RESISTANCE - MAGNET_POSITION_MODEL_MIN) ) +
				MAGNET_POSITION_MODEL_MIN;
	}

	return position;
}

/*@brief	Calculates the *MAG ONLY* portion of power given an offset.
 *
 */
static float watts_offset(float speed_mps, float watts, uint16_t gap_offset, bool invert)
{
	float offset = GAP_OFFSET_TO_PCT(gap_offset);

	// If we're doing the inverted calculation (matching power to position).
	if (invert)
	{
		offset = 1.0f / offset;
	}

	float force = watts / speed_mps;
	float adjusted_force = force * offset;
	float adjusted_watts = adjusted_force * speed_mps;

	return adjusted_watts;
}

/**@brief	Initializes the magnet module with a set of factors for a low and
 * 			high speed polynomial.
 */
void magnet_init(mag_calibration_factors_t* p_factors)
{
	mp_mag_calibration_factors = p_factors;
}

/**@brief	Calculates watts added by the magnet for a given speed at magnet
 *			position.  Accepts an offset for the gap as a multiple of 1,000.
 *			Divides gap_offset by 1,000 to get a % offset to Force.
 */
float magnet_watts(float speed_mps, uint16_t position, uint16_t gap_offset)
{
	float coeff[COEFF_COUNT];
	float watts;
	uint16_t linear_position = 0;

	/*
	 * After max servo position the curve turns upwards.
	 * Below 7 mps, the method fails, so we don't adjust below this speed for now.
	 */
	if (position >= MAGNET_POSITION_MIN_RESISTANCE ||
		speed_mps < MIN_SPEED_MPS)
	{
		return 0.0f;
	}
	else if (position < MAGNET_POSITION_MAX_RESISTANCE)
	{
		position = MAGNET_POSITION_MAX_RESISTANCE;
	}
	else if (position > MAGNET_POSITION_MODEL_MIN &&
			position < MAGNET_POSITION_MIN_RESISTANCE)
	{
		/* Between where the model stops and 0 watts is very little at low speed
		 * but could be ~10-15 watts at higher speeds.  Use a linear model to
		 * determine watts in this zone.
		 */
		linear_position = position;
		position = MAGNET_POSITION_MODEL_MIN;
	}

	// Determine the curve for the speed.
	curve_coeff(speed_mps, coeff);

	watts =
		coeff[0] * pow(position, 3) +
		coeff[1] * pow(position, 2) +
		coeff[2] * position +
		coeff[3];

	if (linear_position > 0)
	{
		// We're at the bottom range, so use linear equation.
		watts = watts_linear(watts, linear_position);
	}

	// If we have a gap offset, use it here.
	if (gap_offset > 0)
	{
		watts = watts_offset(speed_mps, watts, gap_offset, false);
	}

	return watts;
}

/**@brief	Calculates magnet position for a given speed and watt target.
 *			Accepts an offset for the gap as a multiple of 1,000.
 *			Divides gap_offset by 1,000 to get a % offset to Force.
 */
uint16_t magnet_position(float speed_mps, float mag_watts, uint16_t gap_offset)
{
	float coeff[COEFF_COUNT];

	// A set of math-intensive formula friendly names.
	#define a	coeff[0]
	#define b	coeff[1]
	#define c	coeff[2]
	#define d	coeff[3]

	float f, g, h, r, theta, rc;
	float x2a, x2b, x2c, x3;
	int8_t k;

	// Calculate the offset.
	if (gap_offset > 0)
	{
		mag_watts = watts_offset(speed_mps, mag_watts, gap_offset, true);
	}

	// Send the magnet to the home position if no mag watts required.
	if (mag_watts <= 0)
	{
		return MAGNET_POSITION_MIN_RESISTANCE;
	}
	else if (mag_watts < WATTS_MODEL_MIN)
	{
		// Check and see if we need to solve linearly.
		x3 = position_linear(speed_mps, mag_watts, gap_offset);

		if (x3 != 0)
		{
			return x3;
		}
	}

	// Interpolate to calculate the coefficients of the position:pwoercurve.
	curve_coeff(speed_mps, coeff);

	// To solve for a specific watt target, subtract from coefficient d.
	d = d - mag_watts;

	//<!--EVALUATING THE 'f'TERM-->
	f = (((3 * c) / a) - (((b*b) / (a*a)))) / 3;

	//<!--EVALUATING THE 'g'TERM-->
	g = ((2 * ((b*b*b) / (a*a*a)) - (9 * b*c / (a*a)) + ((27 * (d / a))))) / 27;

	//<!--EVALUATING THE 'h'TERM-->
	h = (((g*g) / 4) + ((f*f*f) / 27));

	/* Original code adopted from javascript website, need to refactor, but it
	* works.  Code could solve for 3 solutions (x1, x2, x3) given a cubic
	* polynomial, however we only need to solve for the last form (x3).
	*/
	if (h <= 0)
	{
		//<!-- - (S + U) / 2 - (b / 3a) - i*(S - U)*(3) ^ .5-->
		r = ((j_sqrtf((g*g / 4) - h)));
		k = 1;
		if (r < 0) k = -1;
		//<!--rc is the cube root of 'r' -->
		rc = pow((r*k), (1.0 / 3.0))*k;
		k = 1;
		theta = j_acosf((-g / (2 * r)));
		x2a = rc*-1;
		x2b = j_cosf(theta / 3);
		x2c = j_sqrtf(3)*(j_sinf(theta / 3));
		x3 = (x2a*(x2b - x2c)) - (b / (3 * a));
	}
	else
	{
		return 0u;
	}

	return (uint16_t) x3;
}
