/*
(1) Launch developer command prompt:
%comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat""
(2) build with debug symbols.
cl test.c /ZI 


cl test.c ..\emotion\libraries\power.c ..\emotion\libraries\resistance.c ..\emotion\libraries\nrf_pwm.c /ZI /I..\emotion\libraries\
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <float.h>

#define GRAVITY 9.8f;

#define debug_log printf

#ifndef DM_DIABLE_LOGS
#define DM_LOG debug_log   /**< Used for logging details. */
#else // DM_DIABLE_LOGS
#define DM_LOG(...)        /**< Diasbles detailed logs. */
#endif // DM_DIABLE_LOGS

#define IRT_ERROR_BASE_NUM      (0x80000)       				///< Error base, hopefully well away from anything else.
#define IRT_ERROR_RC_BASE_NUM   IRT_ERROR_BASE_NUM + (0x100)   	///< Error base for Resistance Control


int16_t calc_watts(float grade, float speed_mps, float wind_mps, float weight_kg, float _c, float _crr)
{
	float f_wind;
	float f_rolling;
	float f_slope;
	float result;

	f_wind = 0.5f * (_c * pow((speed_mps + wind_mps), 2));

	// Weight * GRAVITY * Co-efficient of rolling resistance.
	f_rolling = (weight_kg * 9.8f * _crr);

	f_slope = (weight_kg * 9.8f * grade);

	result = ((f_wind + f_rolling + f_slope) * speed_mps);
	printf("f_wind:%f, f_rolling:%f, f_slope:%f\r\n", 
		f_wind, f_rolling, f_slope);

	return (int16_t) ((f_wind + f_rolling + f_slope) * speed_mps);
}

void check(uint32_t err)
{
	if ((err & IRT_ERROR_RC_BASE_NUM) == IRT_ERROR_RC_BASE_NUM)
	{
		printf("Yes\r\n");
	}
	else
	{
		printf("No\r\n");
	}
}



uint16_t slope_calc(float y, float slope, float intercept)
{
	float x;
	x = y * slope + intercept;

	return (uint16_t) x;
}

//
// Calculates the desired servo position given speed in mps, weight in kg
// and additional needed force in newton meters.
//
uint16_t power_servo_pos_calc(float force_needed)
{
	int16_t servo_pos;

	//
	// Manual multiple linear regression hack.
	//
	if (force_needed < 0.832566f)
	{
		servo_pos = 1500;
	}
	else if (force_needed < 5.028207f)
	{
		// 1,500 - 1,300
		servo_pos = slope_calc(force_needed, -44.8841f, 1517.988f);
	}
	else if (force_needed < 40.51667f)
	{
		// 1,300 - 900
		servo_pos = slope_calc(force_needed, -10.9038f, 1345.708f);
	}
	else if (force_needed < 44.9931f)
	{
		// 900 - 700
		servo_pos = slope_calc(force_needed, -39.4606f, 2505.677f);
	}
	else
	{
		// Max
		servo_pos = 700;
	}

	// Protect min/max.
	if (servo_pos < 700)
		servo_pos = 700;
	else if (servo_pos > 1500)
		servo_pos = 1500;

	return servo_pos;
}

int main(int argc, char *argv [])
{
	int16_t servo_pos;
	float force_needed;
	
	force_needed = 8.3f;
	servo_pos = power_servo_pos_calc(force_needed);

	printf("servo_pos = %i\r\n", servo_pos);
}
