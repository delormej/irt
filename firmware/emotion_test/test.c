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

float float_from_buffer(uint32_t* p_encoded)
{
	// Signed int contains the fraction, starting at byte 2.
	// IEEE754 binary32 format for float.
	// Sign 1 bit, exponent (8 bits), fraction (23 bits)
	// 0 00000000 00000000000000000000000
	bool sign = *p_encoded >> 31;

	// mask out 23 bits for the fractional value.
	uint32_t fraction = *p_encoded & 0x7FFFFF;

	// mask out the exponent and shift into an 8 bit int
	// exponent is transmitted with binary offset of 127
	uint8_t exponent = (*p_encoded >> 23) - 127;

	// calculate the float value.
	float value = fraction / pow(2, exponent);

	if (sign)
	{
		value *= -1;
	}

	printf("[CN] float_from_buffer encoded:%u, exp:%i, sign:%i, fraction:%i, value:%.7f\r\n",
		*p_encoded, exponent, sign, fraction, value);

	return value;
}

/**@brief	Encodes a float as binary32 into a uint8_t* buffer for sending. */
void float_to_buffer(float value, uint8_t* p_buffer)
{
	uint8_t exp = 0;
	bool sign;
	float fractional;
	float intpart;
	uint32_t binvalue;
	uint32_t exponent;

	sign = value < 0.0f;

	// Strip the sign if it's negative.
	if (sign)
	{
		value = value *-1;
	}

	// Determine the exponent size required.
	for (exp = 0; exp < 24; exp++) // max bits to use are 23
	{
		exponent = pow(2, exp);

		// Just get the fractional portion of a number
		fractional = modff(value * exponent, &intpart);

		// Keep going until fraction is 0 or we used all the bits.
		if (fractional == 0.0f)
			break;
	}

	// Exponent is binary offset by 127.
	exp += 127;

	binvalue = (sign << 31) | (exp << 15) | (int32_t) intpart;

	memcpy(p_buffer, &binvalue, sizeof(uint32_t));
}


int main(int argc, char *argv [])
{
	/*
	uint8_t buffer[8];
	uint8_t buffer2[8]; // for reverse calc.

	buffer[0] = 0xF1; // Message ID
	buffer[1] = 0xFF; // Placeholder


	printf("x==%.7f\r\n", fmod((1.0 * 1.0), 1.0));

	
	uint32_t value = ((1 << 31) | 5767294 | (23+127 << 23)); 
	printf("Original = %i\r\n", value);

	memcpy(&buffer[2], &value, sizeof(uint32_t));

	float result = float_from_buffer(buffer);

	printf("result:%.7f\r\n", result);

	float_to_buffer(result, buffer2);

	result = float_from_buffer(buffer2);
	
	printf("result:%.7f\r\n", result);
	*/
	float f,f2;
	uint32_t value;
	//printf("Enter float: ");
	//scanf("%f", &f);
	//f = 8.2319200f;
	//printf("thanks, will process: %4.7f\r\n", f);
	
	//uint32_t value = 1335400207u;

	//float_to_buffer(f, &f_buf);
	//float fr = float_from_buffer(&value);
	f = 17.53709547f;
	//f = 15.19211f;

	float_to_buffer(f, &value);
	f2 = float_from_buffer(&value);
	
	//printf("done, original: %.7f, result is: %.7f, %u\r\n", f, fr, f_buf);
	printf("result is: %.7f, %u, inverse:%.7f\r\n", f, value, f2);

	uint8_t exp = 23 + 127;

	// 150 == 0000 0000 x000 0000 0000 0000 1001 0110
	// 10010110
	printf("test: %u, %u\r\n", exp, exp << 15);


	return 0;
}
