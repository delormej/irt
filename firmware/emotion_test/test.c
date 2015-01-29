/*
(1) Launch developer command prompt:
%comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat""
(2) build with debug symbols.
cl test.c /ZI 


cl test.c ..\emotion\libraries\power.c ..\emotion\libraries\resistance.c ..\emotion\libraries\nrf_pwm.c /ZI /I..\emotion\libraries\
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <windows.h>

#define COEFF_COUNT		4				// Cubic poynomial has 4 coefficients.

/**@brief	Calculates the coefficient values for a cubic polynomial
 *			that plots a power curve for the magnet at a given speed.
 *
 *@note		Uses linear interpolation to calculate each coefficient
 *			value based on known curves for 15 mph & 25 mph.
 */
static void curve_coeff(float speed_mps, double *coeff)
{
	static const float SPEED1 =	15.0 * 0.44704;	// Convert to meters per second
	static const float SPEED2 = 25.0 * 0.44704;	// Convert to meters per second

	// 15 mph (6.7056 mps)
	static const double COEFF_1 [] = {
		0.00000233333,
		-0.0078375,
		8.044166667,
		-2277 };

	// 25 mph (11.176 mph)
	static const double COEFF_2 [] = {
		0.00000508333,
		-0.017,
		17.60666667,
		-5221 };

	for (uint8_t ix = 0; ix < COEFF_COUNT; ix++)
	{
		coeff[ix] = COEFF_1[ix] + 
			((speed_mps - SPEED1) / (SPEED2 - SPEED1)) *
			(COEFF_2[ix] - COEFF_1[ix]);
	}
}

/**@brief	Calculates magnet position for a given speed and watt target.
 *
 */
uint16_t magnet_position(float speed_mps, float mag_watts)
{
	//double a = 0.00000282833;
	//double b = -0.00948675;
	//double c = 9.765416668;
	//double d = -2806.92;

	double coeff[COEFF_COUNT];

	curve_coeff(speed_mps, coeff);

	double a = coeff[0];
	double b = coeff[1];
	double c = coeff[2];
	double d = coeff[3];

	d = d - mag_watts;

	double f, g, h, r, m, m2, n, n2, theta, rc;
	double x1, x2, x2a, x2b, x2c, x2d, x3;
	int sign, k;
	double dans;

	//<!--EVALUATING THE 'f'TERM-->
	f = (((3 * c) / a) - (((b*b) / (a*a)))) / 3;

	//<!--EVALUATING THE 'g'TERM-->
	g = ((2 * ((b*b*b) / (a*a*a)) - (9 * b*c / (a*a)) + ((27 * (d / a))))) / 27;

	//<!--EVALUATING THE 'h'TERM-->
	h = (((g*g) / 4) + ((f*f*f) / 27));

	if (h > 0)
	{
		m = (-(g / 2) + (sqrt(h)));

		//<!--K is used because math.pow cannot compute negative cube roots-->
		k = 1;
		if (m < 0) k = -1; else k = 1;
		m2 = pow((m*k), (1.0 / 3.0));
		m2 = m2*k;
		k = 1;
		n = (-(g / 2) - (sqrt(h)));
		if (n < 0) k = -1; else k = 1;
		n2 = (pow((n*k), (1.0 / 3.0)));
		n2 = n2*k;
		k = 1;
		x1 = ((m2 + n2) - (b / (3 * a)));

		//<!--((S + U) - (b / (3 * a)))-->
		x2 = (-1 * (m2 + n2) / 2 - (b / (3 * a))); // +" + i* " + ((m2 - n2) / 2)*Math.pow(3, .5));
		//<!-- - (S + U) / 2 - (b / 3a) + i*(S - U)*(3) ^ .5-->
		x3 = (-1 * (m2 + n2) / 2 - (b / (3 * a))); // +" - i* " + ((m2 - n2) / 2)*Math.pow(3, .5));
	}

	//<!-- - (S + U) / 2 - (b / 3a) - i*(S - U)*(3) ^ .5-->

	if (h <= 0)
	{
		r = ((sqrt((g*g / 4) - h)));
		k = 1;
		if (r < 0) k = -1;
		//<!--rc is the cube root of 'r' -->
		rc = pow((r*k), (1.0 / 3.0))*k;
		k = 1;
		theta = acos((-g / (2 * r)));
		x1 = (2 * (rc*cos(theta / 3)) - (b / (3 * a)));
		x2a = rc*-1;
		x2b = cos(theta / 3);
		x2c = sqrt(3)*(sin(theta / 3));
		x2d = (b / 3 * a)*-1;
		x2 = (x2a*(x2b + x2c)) - (b / (3 * a));
		x3 = (x2a*(x2b - x2c)) - (b / (3 * a));

		x1 = x1*1E+14; x1 = round(x1); x1 = (x1 / 1E+14);
		x2 = x2*1E+14; x2 = round(x2); x2 = (x2 / 1E+14);
		//x3 = x3*1E+14; x3 = round(x3); x3 = (x3 / 1E+14);
	}

	if ((f + g + h) == 0)
	{
		if (d<0) { sign = -1; }; if (d >= 0) { sign = 1; }
		if (sign>0){ dans = pow((d / a), (1.0 / 3.0)); dans = dans*-1; }
		if (sign < 0){ d = d*-1; dans = pow((d / a), (1.0 / 3.0)); }
		x1 = dans; x2 = dans; x3 = dans;
	}

	return (uint16_t)x3;
}

int main(int argc, char *argv [])
{
	uint16_t position;
	float speed_mps, mag_watts;

	// Print header.
	printf("speed_mph, position, watts\r");

	for (speed_mps = 5.0f * 0.44704; speed_mps < 45.0 * 0.44704; speed_mps += 5.0f * 0.44704)
	{
		for (mag_watts = 0.0f; mag_watts < 800.0f; mag_watts += 1.0f)
		{
			position = magnet_position(speed_mps, mag_watts);
			printf("%.1f, %i, %.1f\r", speed_mps * 2.23694f, position, mag_watts);
		}
	}
		
	/*for (position = 2000; position > 600; position += 100) */

	return 0;
}
