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

static void float_to_buffer(float value, uint8_t* p_buffer)
{
	uint8_t i = 0;
	bool sign;
	float fractional;
	float intpart;
	uint32_t binvalue;
	uint32_t exponent;

	//sign = *((uint32_t*)&value) >> 31;
	sign = value < 0.0f;

	// Strip the sign if it's negative.
	if (sign)
	{
		value = value *-1;
	}

	// Determine the exponent size required.
	for (i = 0; i < 24; i++) // max bits to use are 23
	{
		exponent = pow(2, i);
		fractional = modff(value * exponent, &intpart);
		
		// Keep going until fraction is 0 or we used all the bits.
		if (fractional == 0)
			break;
	}

	binvalue = (sign << 31) | (i << 23) | (int32_t) intpart;

	memcpy(&p_buffer[2], &binvalue, sizeof(uint32_t));

	printf("f2b::sign is: %i, exponent is: %i, binvalue: %i, intpart:%i\r\n", 
		sign, i, binvalue, (int32_t)intpart);

	/*
		modf stores the integer part in *integer-part, and returns the 
		fractional part. For example, modf (2.5, &intpart) returns 0.5 and 
		stores 2.0 into intpart.*/
}

static float float_from_buffer(uint8_t* p_buffer)
{
	// Signed int contains the fraction, starting at byte 2.
	// IEEE754 binary32 format for float.
	// Sign 1 bit, exponent (8 bits), fraction (23 bits)
	// 0 00000000 00000000000000000000000

	uint32_t* p_buf = (uint32_t*) &p_buffer[2];

	printf("got: %i\r\n", *p_buf);

	bool sign = (*p_buf) >> 31;

	// mask out 23 least significant bits for the fraction value.
	uint32_t value = ((*p_buf) & 0x7FFFFF); // | (sign << 31);
	// mask out bits for the exponent and shift into an 8 bit int
	uint8_t exponent = *p_buf >> 23;

	float fraction = value / pow(2, exponent);

	if (sign)
	{
		fraction *= -1;
	}

	printf("val:%i, exp:%i, sign:%i, fraction:%.7f\r\n",
		value, exponent, sign, fraction);

	return fraction;
}

int main(int argc, char *argv [])
{
	uint8_t buffer[8];
	uint8_t buffer2[8]; // for reverse calc.

	buffer[0] = 0xF1; // Message ID
	buffer[1] = 0xFF; // Placeholder

	/*int32_t x = -2105540500;
	uint32_t x = 0x8280006C;
	uint8_t y = x >> 23;
	printf("y==%u\r\n", y); */

	// Manufacture an IEEE754 value representation.
	// Create a signed value (negative), with exponent 23 and raw value.
	//uint32_t value = ((1 << 31) | 5767294 | 385875968); /* 23 << 31 bits*/
	uint32_t value = (1 << 31) | 108 | (5 << 23);	// -3.375
	printf("Original = %i\r\n", value);

	memcpy(&buffer[2], &value, sizeof(uint32_t));

	float result = float_from_buffer(buffer);

	printf("result:%.7f\r\n", result);

	float_to_buffer(result, buffer2);

	result = float_from_buffer(buffer2);

	printf("result:%.7f\r\n", result);

	return 0;
}
