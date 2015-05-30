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
#include <float.h>
#include <math.h>

#define COEFF_COUNT 4


static void decode_magnet_factors(const uint8_t* p_buffer, float* p_factors)
{
	memcpy(&p_factors[0], &p_buffer[0], sizeof(float));
	memcpy(&p_factors[1], &p_buffer[4], sizeof(float));
}

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

	// Low speed (15 mph)
	static const float COEFF_1[] =  {
		1.27516039631e-06,
		-0.00401345920329,
		3.58655403892,
		-645.523540742 };

	// High speed (25 mph)
	static const float COEFF_2[] = {
		2.19872670294e-06,
		-0.00686992504214,
		6.03431060782,
		-998.115074474 };

	for (uint8_t ix = 0; ix < COEFF_COUNT; ix++)
	{
		coeff[ix] = COEFF_1[ix] +
			((speed_mps - SPEED1) / (SPEED2 - SPEED1)) *
			(COEFF_2[ix] - COEFF_1[ix]);
	}
}

float magnet_watts(float speed_mps, uint16_t position)
{
	float coeff[COEFF_COUNT];
	float watts;	
	
	// Determine the curve for the speed.
	curve_coeff(speed_mps, coeff);

	watts =
		coeff[0] * pow(position, 3) +
		coeff[1] * pow(position, 2) +
		coeff[2] * position +
		coeff[3];
}

uint16_t magnet_position(float speed_mps, float mag_watts)
{
	return 0;
}


int main(int argc, char *argv [])
{
	float afloat1 = 3.14f;
	float afloat2 = -645.99934f;
	float afloat3 = 0.0;
	
	float my_floats[4];
	uint8_t buffer[16];
	
	// Clear them out.
	memset(&buffer, 0, sizeof(buffer));
	memset(&my_floats, 0, sizeof(my_floats));

	// Copy floats into the buffer.	
	memcpy(&buffer[4], &afloat1, sizeof(float));
	memcpy(&buffer[8], &afloat2, sizeof(float));

	decode_magnet_factors(&buffer[4], &my_floats[2]);	
		
	printf("watts: %.6f\r\n", my_floats[2]);
	return 0;
}
