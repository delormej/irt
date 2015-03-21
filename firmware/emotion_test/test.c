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

float magnet_watts(float speed_mps, uint16_t position)
{
	//
	// 1. Determine the slope
	//

	// Solve for slope (x: position, y: slope) where slope is the slope of (x: speed(mps), y: watts)
	// y = 9E-07x3 - 0.0029x2 + 2.89x - 689.51
	float slope = 9E-07 * pow(position, 3) - 0.0029 * pow(position, 2) + (2.89 * position) - 689.51;

	return slope;

	//
	// 2. Determine the intercept
	//

	// Watts @ 4.4704 mps (10 mph)
	// y = 3E-07x3 - 0.0011x2 + 1.1201x - 291.52
	float intercept = 3E-07 * pow(position, 3) - 0.0011 * pow(position, 2) + (1.1201 * position) - 291.52;
	
	//
	// 3. Calculate watts for a given speed/position
	//
	// y = mx + b
	float watts = slope * speed_mps + intercept;
	
	return watts;
}

uint16_t magnet_position(float speed_mps, float mag_watts)
{
	return 0;
}


int main(int argc, char *argv [])
{
	printf("watts: %.2f\r\n", magnet_watts(7.15264f, 1200));
	return 0;
}
