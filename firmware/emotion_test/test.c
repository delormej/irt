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


double calculate(float watt_target)
{
	double a = 0.00000282833;
	double b = -0.00948675;
	double c = 9.765416668;
	double d = -2806.92;

	d = d - watt_target;

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
		printf("here\r\n");

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

	printf("x1 = %.12f", x1);
	printf("x2 = %.12f", x2);
	printf("x3 = %.12f", x3);
}

int main(int argc, char *argv [])
{
	double x;
	float input;

	printf("Enter watt target: ");
	scanf("%f", &input);
	
	calculate(input);
	//printf("%.12f\r\n", x);
	
	return 0;
}
