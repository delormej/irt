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
#include <string.h>
#include <math.h>
#include <float.h>
#include <windows.h>

#define LED_FRONT_GREEN_PIN		18
#define LED_FRONT_RED_PIN		19
#define LED_BACK_GREEN_PIN		20
#define LED_BACK_RED_PIN		21

#define LED_FRONT_GREEN			0
#define LED_FRONT_RED			1
#define LED_BACK_GREEN			2
#define LED_BACK_RED			3


int main(int argc, char *argv [])
{
	uint8_t count = 0;
	uint8_t tick_buffer[5];

	while (-1)
	{
		if (++count % (sizeof(tick_buffer) / sizeof(uint8_t)) == 0)
		{
			printf("[IN]\t%i\r\n", count);
		}
		else
		{
			printf("[OUT]\t%i\r\n", count);
		}
		
		Sleep(250);
	}
	

	return 0;
}
