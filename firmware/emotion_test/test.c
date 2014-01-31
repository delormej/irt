/*
(1) Launch developer command prompt:
%comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat""
(2) build with debug symbols.
cl test.c /Z1 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>

float get_resistance_pct(uint8_t *buffer)
{
	/*	Not exactly sure why it is this way, but it seems that 2 bytes hold a
	value that is a percentage of the MAX which seems arbitrarily to be 16383.
	Use that value to calculate the percentage, for example:

	10.7% example:
	(16383-14630) / 16383 = .10700 = 10.7%
	*/
	static const float PCT_100 = 16383.0f;
	uint16_t value = buffer[0] | buffer[1] << 8u;

	float percent = (PCT_100 - value) / PCT_100;
	
	return percent;
}

uint16_t get_position(float percent)
{
	uint16_t position = 0;

	position = (uint16_t) (1500 - (
		(1500 - 2107) *
		percent));
}


int main(int argc, char *argv [])
{
	int i = scanf("Any key to start...");

	uint8_t buffer[4] = { 0x00, 0x00, 0xFF, 0x1F };
	
	float value = get_resistance_pct(&buffer[2]);
	uint16_t position = get_position(value);

	printf("val: ||%f, %i||", value, position);
}