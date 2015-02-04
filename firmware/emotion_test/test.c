/*
(1) Launch developer command prompt:
%comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat""
(2) build with debug symbols.
cl test.c /ZI 

cl test.c ../emotion/libraries/math/acosf.c ../emotion/libraries/math/sqrtf.c ../emotion/libraries/math/cosf.c ../emotion/libraries/math/sinf.c ../emotion/libraries/math/fabsf.c ../emotion/libraries/math/rem_pio2f.c /I ../emotion/libraries/math

*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
//#include <math.h>
#include "math_private.h"
#include <float.h>
//#include <windows.h>

#define COEFF_COUNT				4u				// Cubic poynomial has 4 coefficients.
#define POSITION_MAX_RESISTANCE	700u			// Position for maximum resistance.
#define POSITION_MIN_RESISTANCE	1500u			// After this position, no more resistance is applied.
#define POSITION_HOME			2000u			// Home position for the magnet.
#define MIN_SPEED_MPS			7.1f * 0.440704f// Minimum speed for which mag resistance can be calculated.


/**@brief	Calculates the coefficient values for a cubic polynomial
 *			that plots a power curve for the magnet at a given speed.
 *
 *@note		Uses linear interpolation to calculate each coefficient
 *			value based on known curves for 15 mph & 25 mph.
 */
static void curve_coeff(float speed_mps, float *coeff)
{
	static const float SPEED1 =	15.0 * 0.44704;	// Convert to meters per second
	static const float SPEED2 = 25.0 * 0.44704;	// Convert to meters per second

	// 15 mph (6.7056 mps)
	static const float COEFF_1[] =  { 
		0.00000127584, 
		-0.00424935897, 
		4.06025446775, 
		-838.0638694638 }; 
	
	// 25 mph (11.176 mph)
	static const float COEFF_2[] = { 
		0.000002624903, 
		-0.008925233100, 
		8.907428127428, 
		-2139.951981351815 }; 
	
	for (uint8_t ix = 0; ix < COEFF_COUNT; ix++)
	{
		coeff[ix] = COEFF_1[ix] + 
			((speed_mps - SPEED1) / (SPEED2 - SPEED1)) *
			(COEFF_2[ix] - COEFF_1[ix]);
	}
}

/**@brief	Calculates watts added by the magnet for a given speed at magnet
 *			position.
 */
float magnet_watts(float speed_mps, uint16_t position)
{
	float coeff[COEFF_COUNT];
	float watts;

	/* 
	 * After max servo position the curve turns upwards.
	 * Below 7 mps, the method fails, so we don't adjust below this speed for now.
	 */
	if (position > POSITION_MIN_RESISTANCE || 
		speed_mps < MIN_SPEED_MPS)
	{
		return 0.0f;
	}
	
	// Determine the curve for the speed.
	curve_coeff(speed_mps, coeff);

	watts = 
		coeff[0] * pow(position, 3) +
		coeff[1] * pow(position, 2) +
		coeff[2] * position +
		coeff[3];

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

	float f, g, h, r, /*m, m2, n, n2,*/ theta, rc;
	float x2a, x2b, x2c, /*x2d,*/ x3;
	int8_t k;

	// Send the magnet to the home position if no mag watts required.
	if (mag_watts <= 0)
	{
		return POSITION_HOME;
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

int main(int argc, char *argv [])
{
	uint16_t position, new_position;
	float speed_mps, mag_watts, mag_watts_verify;

	mag_watts_verify = 0.0f;

	float speeds [] = { 15, 16, 22, 25 };

	//position = magnet_position(24.5f * 0.44704f, 212 - 235);

	//printf("Positon: %u\r\n", position);
	//return 0;

	// Uncomment to attach debugger.
	//getchar();

	// Print header.
	printf("speed_mph, position, watts, watts_verify\r");

	//printf("\r\nSTARTING\r\n");

	/* Below 7.1mph, it bombs! Need to figure out why.
	for (speed_mps = (7.1f * 0.44704); speed_mps < (46.0 * 0.44704); 
		speed_mps += (2.0f * 0.44704)) */
	for (int i = 0; i < 5; i++)
	{
		speed_mps = speeds[i] * 0.44704f;

		for (position = 2000; position > 700; position -= 5)
		{
			mag_watts = magnet_watts(speed_mps, position);
			new_position = magnet_position(speed_mps, mag_watts);
			mag_watts_verify = magnet_watts(speed_mps, new_position);

		/*
		for (mag_watts = 0.0f; mag_watts < 800.0f; mag_watts += 1.0f)
		{
			position = magnet_position(speed_mps, mag_watts);
			mag_watts_verify = magnet_watts(speed_mps, position);
		*/
			printf("%.1f, %i, %.1f, %.1f\r", 
				speed_mps * 2.23694f, position, mag_watts, mag_watts_verify);
		}
	}
		
	/*for (position = 2000; position > 600; position += 100) */

	return 0;
}
