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

#define BASE_SPEED_MPS		6.7056f
#define MAX_POSITION		1464	// temporary
#define MIN_POSITION		800
#define MAX_RECURSION		MAX_POSITION - MIN_POSITION
#define START_SKIP			100
#define MAX_WATTS			2000	// arbitrary maximum watts for bounds checking

/*
**@brief Returns the slope of speed (mps):power for a given servo position.
*
*/
float speed_slope_from_position(uint16_t position)
{
	float slope;

	if (position >= MAX_POSITION)
	{
		return 0.0f;
	}

	slope = (
		-2.032933219f * powf(10, -12) * powf(position, 5)
		+ 1.066543186 * powf(10, -8) * powf(position, 4)
		- 2.14085667 * powf(10, -5) * powf(position, 3)
		+ 2.031246676 * powf(10, -2) * powf(position, 2)
		- 9.039501598 * position + 1555.297882f);

	return slope;
}

/*
**@brief Returns the watts for a given position at base speed (6.7056 mps).
*
*/
uint16_t watts_from_position_base(uint16_t position)
{
	uint16_t base_watts;

	// Step 1. Solve for watts @ base speed (6.7056 mps).
	if (position >= MAX_POSITION)
	{
		return 0;
	}

	base_watts = (
		1.381313131f * powf(10, -6) * powf(position, 3)
		- 4.520887446f * powf(10, -3) * powf(position, 2)
		+ 4.257886003f * position - 870.2186148f);

	return base_watts;
}

/*
**@brief Returns the watts for a given position & speed.
*
*/
float watts_from_position(uint16_t position, float speed_mps)
{
	float result;
	uint16_t base_watts;
	float slope;

	base_watts = watts_from_position_base(position);
	slope = speed_slope_from_position(position);

	result = (speed_mps - BASE_SPEED_MPS) * slope + base_watts;

	if (result < 0.0f)
	{
		result = 0.0f;
	}
	else if (result > MAX_WATTS)
	{
		result = MAX_WATTS;
	}

	// Cast to get only the integer portion.
	return result;
}


static uint16_t position_from_watts_recursive(uint16_t target, float speed_mps, uint16_t start, uint16_t skip)
{
	static uint16_t count = 0;
	uint16_t position = 0;
	float watts;

	// Keep track of runaway recursion.
	if (skip == START_SKIP)
	{
		count = 0;
	}
	position = start;

	do
	{
		if (count++ > MAX_RECURSION)
		{
			return MAX_POSITION;
		}
		watts = watts_from_position(position, speed_mps);

	} while (watts <= target &&
		(position -= skip) > MIN_POSITION); // match within a watt

	if (watts > target + 1.0f && skip > 1)
	{
		// Recursively try to get closer.
		return position_from_watts_recursive(target, speed_mps, position + skip, (uint16_t) skip / 10);
	}

	if (position > MAX_POSITION)
	{
		position = MAX_POSITION;
	}
	else if (position < MIN_POSITION)
	{
		position = MIN_POSITION;
	}

	printf("position_from_watts iterations: %i\r\n", count);

	return position;
}

/*
 **@brief Solves for the servo position given desired target mag only watts.
 *
 */
uint16_t position_from_watts(uint16_t target, float speed_mps)
{
	return position_from_watts_recursive(target, speed_mps, MAX_POSITION, START_SKIP);
}

int main(int argc, char *argv [])
{
	uint16_t input;
	uint16_t position;
	uint16_t watts;
	float speed;

	for (int i = 1800; i > 800; i--)
	{
		printf("position: %i, watts: %.1f\r\n", 
			i, watts_from_position(i, 5.0f));
	}

	return;

	printf("Enter watt target: ");
	scanf("%i", &input);

	printf("Enter speed (mph): ");
	scanf("%f", &speed);

	speed = speed * 0.44704f;

	position = position_from_watts(input, speed);
	watts = (uint16_t) watts_from_position(position, speed);

	printf("Position: %i, Watts: %i for speed: %.4f mps\r\n", position, watts, speed);
	
	return 0;
}
