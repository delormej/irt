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
															// The position between MAGNET_POSITION_MIN_RESISTANCE and this is linear.

#define SPEED1 		(float)(mp_mag_calibration_factors->low_speed_mps) / 1000.0f
#define SPEED2 		(float)(mp_mag_calibration_factors->high_speed_mps) / 1000.0f
#define COEFF_1		mp_mag_calibration_factors->low_factors
#define COEFF_2		mp_mag_calibration_factors->high_factors
#define GAP_OFFSET	(float)(mp_mag_calibration_factors->gap_offset) / 100.0f	// Macro to convert gap offset storage format to percent.

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
				(float)(MAGNET_POSITION_MIN_RESISTANCE - mp_mag_calibration_factors->root_position)
			) * watts;

	/*
    MAG_LOG("pos: %i, watts_linear: %.2f from min: %.2f\r\n", position,
			linear_watts, watts);
            */
	return linear_watts;
}

/*@brief    Solves for the roots of the 3rd order polynomial which represents
 *          power to servo position.       
 *
 */
static uint16_t position_root(float speed_mps, float mag_watts)
{
	float coeff[COEFF_COUNT];

	// A set of math-intensive formula friendly names.
	#define a	coeff[0]
	#define b	coeff[1]
	#define c	coeff[2]
	#define d	coeff[3]

	float f, g, h, r, theta, rc;
	float x2a, x2b, x2c, x3 = 0.0f;
	int8_t k;

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

    return (uint16_t)x3;    
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
	float min_watts = magnet_watts(speed_mps, mp_mag_calibration_factors->root_position);

	// Check if we're asking for less than the model can support.
	if (mag_watts < min_watts)
	{
		position = ( ((min_watts - mag_watts) / min_watts) *
				(MAGNET_POSITION_MIN_RESISTANCE - mp_mag_calibration_factors->root_position) ) +
				mp_mag_calibration_factors->root_position;
	}

	return position;
}

/*@brief	Calculates the *MAG ONLY* portion of power given an offset.
 *
 */
static float watts_offset(float speed_mps, float watts)
{
	return watts;
	
	float offset = GAP_OFFSET;

	float force = watts / speed_mps;
	float adjusted_force = force + offset;
	float adjusted_watts = adjusted_force * speed_mps;

    // MAG_LOG("gap_offset: %.2f adjusted_watts: %.2f, speed_mps: %2.f\r\n", offset,
	// 		adjusted_watts, speed_mps);            

	return adjusted_watts;
}

static inline uint32_t ipow(uint32_t base, uint32_t exp)
{
    int32_t result = 1;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}

/**@brief	Initializes the magnet module with a set of factors for a low and
 * 			high speed polynomial.
 */
void magnet_init(mag_calibration_factors_t* p_factors)
{
	mp_mag_calibration_factors = p_factors;
}

mag_calibration_factors_t magnet_get_default_factors()
{
	mag_calibration_factors_t ca_mag_factors;

	// Default to no gap offset.
	ca_mag_factors.gap_offset = 0;				

	// 15 mph in meters per second * 1,000.
	ca_mag_factors.low_speed_mps = 6705;

	// 25 mph in meters per second * 1,000.
	ca_mag_factors.high_speed_mps = 11176;

	// Position at which we no longer use power curve, revert to linear.
	ca_mag_factors.root_position = 1454;

	ca_mag_factors.low_factors[0] = 1.27516039631e-06f;
	ca_mag_factors.low_factors[1] = -0.00401345920329f;
	ca_mag_factors.low_factors[2] = 3.58655403892f;
	ca_mag_factors.low_factors[3] = -645.523540742f;

	ca_mag_factors.high_factors[0] = 2.19872670294e-06f;
	ca_mag_factors.high_factors[1] = -0.00686992504214f;
	ca_mag_factors.high_factors[2] = 6.03431060782f;
	ca_mag_factors.high_factors[3] = -998.115074474f;	

	return ca_mag_factors;
}

/**@brief	Calculates watts added by the magnet for a given speed at magnet
 *			position.  
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
	else if (position > mp_mag_calibration_factors->root_position &&
			position < MAGNET_POSITION_MIN_RESISTANCE)
	{
		/* Between where the model stops and 0 watts is very little at low speed
		 * but could be ~10-15 watts at higher speeds.  Use a linear model to
		 * determine watts in this zone.
		 */
		linear_position = position;
		position = mp_mag_calibration_factors->root_position;
	}

	// Determine the curve for the speed.
	curve_coeff(speed_mps, coeff);

	watts =
		coeff[0] * ipow(position, 3) +
		coeff[1] * ipow(position, 2) +
		coeff[2] * position +
		coeff[3];

	if (linear_position > 0)
	{
		// We're at the bottom range, so use linear equation.
		watts = watts_linear(watts, linear_position);
	}

	// If we have a gap offset, use it here.
	if (GAP_OFFSET != 0 && GAP_OFFSET != 0xFFFF)
	{
		watts = watts_offset(speed_mps, watts);
	}

	return watts;
}

/**@brief	Calculates magnet position for a given speed and watt target.
 *
 */
uint16_t magnet_position(float speed_mps, float mag_watts, target_power_e* p_target)
{
    uint16_t position = 0;

	// Calculate the offset.
	if (GAP_OFFSET != 0 && GAP_OFFSET != 0xFFFF)
	{
		mag_watts = watts_offset(speed_mps, mag_watts);
	}

	// Send the magnet to the home position if no mag watts required.
	if (mag_watts <= 0.0f)
	{
		position = MAGNET_POSITION_MIN_RESISTANCE;
	}
    else
    {
        // Attempt to solve for the position linearly. 
		position = position_linear(speed_mps, mag_watts);        
        
        // Otherwise, solve for the root.
        if (position == 0)
        {
            position = position_root(speed_mps, mag_watts);
        }
    }
    
    //
    // Assign power target state.
    //
    if (mag_watts < 0.0f)
    {
        // If the watt target is negative, rider is going too fast to hit target.
        *p_target = TARGET_SPEED_TOO_HIGH;         
    }    
    else if (position > MAGNET_POSITION_MIN_RESISTANCE || 
        position < MAGNET_POSITION_MAX_RESISTANCE)
    {
        // If position returned is 0 or out of the acceptable range, we can't
        // hit that target.    
        *p_target = TARGET_SPEED_TOO_LOW;
    }
    else
    {
        *p_target = TARGET_AT_POWER;
    } 

	return position;    
}
