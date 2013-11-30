/*
*******************************************************************************
*
* By Jason De Lorme <jjdelorme@yahoo.com>
* http://www.roadacious.com
*
********************************************************************************/

#include "power.h"

#define	MATH_PI			3.14159265358979f

float slope[10] = { 0, 2.6, 3.8, 5.0, 6.1, 7.1, 8.2, 9.2, 10.1, 11.0 };
float intercept [10] = { 0, -9.60, -18.75, -25.00, -28.94, -29.99, -29.23, -26.87, -20.90, -13.34 };

// TODO: This only works if you have a predefined LEVEL 0-9.  I need to make it 
// more dynamic given any position the servo might have from min->max.
uint8_t calc_power(float speed_mph, float total_weight_lb, 
	uint8_t resistance_level, int16_t* p_watts)
{
	if (speed_mph == 0)
	{
		*p_watts = 0;
		return IRT_SUCCESS;
	}

	if (resistance_level > 9)
	{
		return IRT_ERROR_INVALID_RESISTANCE_LEVEL;
	}	

	// All calculations start with what level 0 (no resistance) would be.
	float level0 = (speed_mph*14.04 - 33.6) - (((speed_mph*14.04 - 33.06) -
		(speed_mph*8.75 - 16.21)) / 90)*(220 - total_weight_lb);

	if (resistance_level == 0)
	{
		*p_watts = (int16_t)level0;
		return IRT_SUCCESS;
	}

	// TODO: Verify truncation vs. rounding that may occur here? Watts needs to
	// be returned as a signed 16 bit integer.
	*p_watts = (int16_t)(level0 + speed_mph *
		slope[resistance_level] + intercept[resistance_level]);

	return IRT_SUCCESS;
}

uint8_t calc_torque(int16_t watts, uint16_t seconds_2048, uint16_t* p_torque)
{
	if (watts == 0 || seconds_2048 == 0)
	{
		*p_torque = 0;
		return IRT_SUCCESS;
	}
	
	*p_torque = (watts * seconds_2048) / (128 * MATH_PI);
	
	return IRT_SUCCESS;
}
