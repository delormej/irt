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

void print_buf(uint8_t* p_buf)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		printf("[%i]\t%i\r\n", i, p_buf[i]);
	}

}

int main(int argc, char *argv [])
{
	uint8_t count = 0;
	uint8_t buffer[8] = { 255, 254, 253, 252, 251, 250, 249, 248 };

	//(uint8_t*) &buffer
	print_buf((uint8_t*)&buffer);

	while (true)
	{
		printf("%i\r\n", count);
		count = count++ | 0x05;
	}
	

	return 0;
}
