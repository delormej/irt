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

#define FLYWHEEL_ROAD_DISTANCE		0.173f										// Virtual road distance traveled in meters per complete flywheel rev.
#define FLYWHEEL_TICK_PER_METER		((1.0f / FLYWHEEL_ROAD_DISTANCE) * 24.0f)	// 24 ticks per drum rev.

int main(int argc, char *argv [])
{
	printf("Flywheel ticks per meter: %.4f\r\n", FLYWHEEL_TICK_PER_METER);
	return 0;
}
