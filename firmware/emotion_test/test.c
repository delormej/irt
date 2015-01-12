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

static float float_from_buffer(uint8_t* p_buffer)
{
	// Signed int contains the fraction, starting at byte 2.
	// IEEE754 binary32 format for float.
	// Sign 1 bit, exponent (8 bits), fraction (23 bits)
	// 0 00000000 00000000000000000000000

	uint32_t* p_buf = (uint32_t*) &p_buffer[2];

	bool sign = (*p_buf) >> 31;

	// mask out 23 least significant bits for the fraction value.
	int32_t value = ((*p_buf) & 0xFFFFFF); // | (sign << 31);
	// mask out most significant bits 2 - 8
	uint8_t exponent = ((*p_buf) & 0x7F000000) >> 24;

	float fraction = value / pow(2, exponent);

	if (sign)
	{
		fraction *= -1;
	}

	printf("val:%i, exp:%i, sign:%i, fraction:%.7f\r\n", 
		value, exponent, sign, fraction);

	return 0.0f;
}

int main(int argc, char *argv [])
{
	uint8_t buffer[8];
	buffer[0] = 0xF1; // Message ID
	buffer[1] = 0xFF; // Placeholder

	//int32_t value = -5767294;
	uint32_t value = ((1 << 31) | 5767294 | 385875968); /* 23 << 31 bits*/
	printf("Original = %i\r\n", value);
	//float_from_buffer((uint8_t*)&value);

	memcpy(&buffer[2], &value, sizeof(uint32_t));

	float_from_buffer(buffer);

	return 0;
}
