/******************************************************************************
 * Copyright (c) 2015 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * @file  	magnet.c
 * @brief	Encapsulates the calculations for magnetic resistance.
 *
 */

#include <stdint.h>
#include <float.h>
#include "magnet.h"
#include "math.h"
#include "math_private.h"
//#include "debug.h"

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

// Coefficients used for polynomial interpolation.
static poly_coeff_t low_speed = { 
	.speed_mps = 10.0 * 0.44704,
	.coeff = { 0.0f, 0.0f, 0.0f, 0.0f }
};

static poly_coeff_t high_speed = {
	.speed_mps = 19.0 * 0.44704,
	.coeff = { 0.0f, 0.0f, 0.0f, 0.0f }
};
															
/**@brief	Sets the cofficients for the 3rd order polynomial.
 *
 */
 void magnet_set_coeff(poly_coeff_t low, poly_coeff_t high)
 {
	 low_speed = low;
	 high_speed = high;
 }
															
/**@brief	Calculates the coefficient values for a cubic polynomial
 *			that plots a power curve for the magnet at a given speed.
 *
 *@note		Uses linear interpolation to calculate each coefficient
 *			value based on known curves for 15 mph & 25 mph.
 */
static void curve_coeff(float speed_mps, float *coeff)
{
	#define SPEED1 low_speed.speed_mps
	#define SPEED2 high_speed.speed_mps

	for (uint8_t ix = 0; ix < COEFF_COUNT; ix++)
	{
		coeff[ix] = low_speed.coeff[ix] +
			((speed_mps - SPEED1) / (SPEED2 - SPEED1)) *
			(high_speed.coeff[ix] - low_speed.coeff[ix]);
	}
}

/**@brief	Calculates the bottom linear portion of watts returned between
 * 			the lowest spot the model calcuates and where we have known 0
 * 			watt values returned for a given speed.
 */
static float watts_linear(float watts, uint16_t position)
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
static uint16_t position_linear(float speed_mps, float mag_watts)
{
	uint16_t position = 0;
	float min_watts = magnet_watts(speed_mps, MAGNET_POSITION_MODEL_MIN);

	// Check if we're asking for less than the model can support.
	if (mag_watts < min_watts)
	{
		position = ( ((min_watts - mag_watts) / min_watts) *
				(MAGNET_POSITION_MIN_RESISTANCE - MAGNET_POSITION_MODEL_MIN) ) +
				MAGNET_POSITION_MODEL_MIN;
	}

	return position;
}

/**@brief	Calculates watts added by the magnet for a given speed at magnet
 *			position.  This is the user mag position, not actual position.
 */
float magnet_watts(float speed_mps, uint16_t position)
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
		return watts_linear(watts, linear_position);
	}

	return watts;
}

/**@brief	Calculates magnet position for a given speed and watt target.
 *
 */
uint16_t magnet_position(float speed_mps, float mag_watts)
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

	// Send the magnet to the home position if no mag watts required.
	if (mag_watts <= 0)
	{
		return MAGNET_POSITION_MIN_RESISTANCE;
	}
	else if (mag_watts < WATTS_MODEL_MIN)
	{
		// Check and see if we need to solve linearly.
		x3 = position_linear(speed_mps, mag_watts);

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
