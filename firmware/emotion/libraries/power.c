/*
*******************************************************************************
*
* By Jason De Lorme <jjdelorme@yahoo.com>
* http://www.roadacious.com
*
********************************************************************************/

#include "power.h"

#define	MATH_PI			3.14159265358979f

static const float slope[10] = { 0, 2.6, 3.8, 5.0, 6.1, 7.1, 8.2, 9.2, 10.1, 11.0 };
static const float intercept [10] = { 0, -9.60, -18.75, -25.00, -28.94, -29.99, -29.23, -26.87, -20.90, -13.34 };

int16_t __DEBUG_POWER[2];

//
// Returns force in newton meters given total weight in kg and speed in mps
// with no additional magnet force.
//
static float get_mag0_force(float weight_kg, float speed_mps)
{
#define SPEED_FORCE_A_SLOPE 			0.194426f
#define SPEED_FORCE_A_INTERCEPT 	23.45062f
#define SPEED_FORCE_A_WEIGHT			 	94.34714f
#define SPEED_FORCE_B_SLOPE 			-0.03644f
#define SPEED_FORCE_B_INTERCEPT 	22.31482f
#define SPEED_FORCE_B_WEIGHT			 	79.83219f

	float force = (speed_mps*SPEED_FORCE_A_SLOPE + SPEED_FORCE_A_INTERCEPT) -
		(((speed_mps*SPEED_FORCE_A_SLOPE + SPEED_FORCE_A_INTERCEPT) - 
		(speed_mps*SPEED_FORCE_B_SLOPE + SPEED_FORCE_B_INTERCEPT)) / (SPEED_FORCE_A_WEIGHT - SPEED_FORCE_B_WEIGHT))*
		(SPEED_FORCE_A_WEIGHT - weight_kg);

	return force;
}

// TODO: This only works if you have a predefined LEVEL 0-9.  I need to make it 
// more dynamic given any position the servo might have from min->max.
// We should also be able to calc power far more acurately based on flywheel revs
// instead of speed.
uint8_t calc_power(float speed_mph, float total_weight_lb, 
	uint8_t resistance_level, int16_t* p_watts)
{
	int16_t power = 0;
	
	if (resistance_level > 9)
	{
		return IRT_ERROR_INVALID_RESISTANCE_LEVEL;
	}	

	if (speed_mph == 0)
	{
		power = 0;
	}
	else
	{
		// All calculations start with what level 0 (no resistance) would be.
		float level0 = (speed_mph*14.04 - 33.6) - (((speed_mph*14.04 - 33.06) -
			(speed_mph*8.75 - 16.21)) / 90)*(220 - total_weight_lb);

		if (resistance_level == 0)
		{
			power = (int16_t)level0;
		}
		else 
		{
			// TODO: Verify truncation vs. rounding that may occur here? Watts needs to
			// be returned as a signed 16 bit integer.
			power = (int16_t)(level0 + speed_mph *
				slope[resistance_level] + intercept[resistance_level]);
		}
	}
__DEBUG_POWER[0] = power;
	*p_watts = power;
__DEBUG_POWER[1] = *p_watts;
	return IRT_SUCCESS;
}

uint8_t calc_torque(int16_t watts, uint16_t period_seconds_2048, uint16_t* p_torque)
{
	if (watts == 0 || period_seconds_2048 == 0)
	{
		*p_torque = 0;
		return IRT_SUCCESS;
	}
	
	*p_torque = (watts * period_seconds_2048) / (128 * MATH_PI);
	
	return IRT_SUCCESS;
}
