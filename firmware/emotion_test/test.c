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

#define RESISTANCE_LEVELS 	7 	// Number of resistance levels available.

/**@brief		Array representing the servo position in micrseconds (us) by
*					resistance level 0-9.
*
*/
static const uint16_t RESISTANCE_LEVEL[RESISTANCE_LEVELS] = {
	2000, // 0 - no resistance
	/*	1300,
	1225,
	1150,
	1075,
	1000,
	925,
	850,
	775,
	700}; // Max resistance
	*/ // TESTING 7 positions.
	1300,
	1200,
	1100,
	1000,
	900,
	800 };

#define MIN_RESISTANCE_LEVEL	1700					// Minimum by which there is no longer resistance.
#define MAX_RESISTANCE_LEVEL	RESISTANCE_LEVEL[RESISTANCE_LEVELS-1]


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

float servo_pos_force_calc(uint16_t servo_pos)
{
	float force;

	force = (
		-0.0000000000012401 * pow(servo_pos, 5)
		+ 0.0000000067486647 * pow(servo_pos, 4)
		- 0.0000141629606351 * pow(servo_pos, 3)
		+ 0.0142639827784839 * pow(servo_pos, 2)
		- 6.92836459712442 * servo_pos
		+ 1351.463567618);

	return force;
}

//
// Calculates the desired servo position given speed in mps, weight in kg
// and additional needed force in newton meters.
//
uint16_t power_servo_pos_calc(float force)
{
	int16_t servo_pos;

	servo_pos = (
		0.001461686  * pow(force, 5)
		- 0.076119976 * pow(force, 4)
		+ 1.210189005 * pow(force, 3)
		- 5.221468861 * pow(force, 2)
		- 37.59134617 * force
		+ 1526.614724);

	return servo_pos;
}

int main(int argc, char *argv [])
{
	//				  [       uint32_t     ], [       uint32_t     ]
	//uint8_t arr[] = { 0x00, 0x00, 0x03, 0x00, 0x00, 0x05, 0x00, 0x00 };

	int16_t servo_pos;
	float force;

	uint8_t sequence;
	sequence = 0x21;

	printf("Sequence is: %i\r\n", (sequence >> 5));;
	printf("Last byte: %i\r\n", ((sequence >> 5) & 0x04) == 0x04);
}
