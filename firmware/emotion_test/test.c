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
#include <float.h>


#define MAX_RESISTANCE_LEVELS 10						// Maximum resistance levels available.
#define MIN_RESISTANCE_LEVEL	1500					// Minimum by which there is no longer resistance.


static const uint16_t RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS] = {
	2107, // 0 - no resistance
	1300,
	1225,
	1150,
	1075,
	1000,
	925,
	850,
	775,
	700}; // Max resistance


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

float get_temp(void)
{
	uint16_t value;
	uint8_t buffer [] = { 19, 112 };

	// Combine two bytes and align to 12 bits.
	value = (buffer[0] << 4) | (buffer[1] >> 4);

	if (buffer[0] & 128u)
	{
		// This is a negative temp.
		value &= 0x7FFF;
		return (value - 4096.0f) / 16.0f;
	}
	else
	{
		return value / 16.0f;
	}
}

typedef struct
{
	int x;
	int y;
	float f;
} my_t;


int get_addr(my_t** pp_my_t)
{

	my_t* p_my;
	p_my = (my_t*)calloc(1, sizeof(my_t));
	p_my->f = 3.14f;
	*pp_my_t = p_my;
}

static uint16_t calc_servo_pos(float weight_kg, float speed_mps, float force_needed)
{
#define SERVO_FORCE_A_SLOPE 			-24.66803762f
#define SERVO_FORCE_A_INTERCEPT 	1489.635063f
#define SERVO_FORCE_A_SPEED			 	8.94f
#define SERVO_FORCE_B_SLOPE 			-26.68991676f
#define SERVO_FORCE_B_INTERCEPT 	1468.153562f
#define SERVO_FORCE_B_SPEED			 	4.47f

	float value = (force_needed * SERVO_FORCE_A_SLOPE + SERVO_FORCE_A_INTERCEPT) -
		(((force_needed * SERVO_FORCE_A_SLOPE + SERVO_FORCE_A_INTERCEPT) -
		(force_needed * SERVO_FORCE_B_SLOPE + SERVO_FORCE_B_INTERCEPT)) /
		SERVO_FORCE_A_SPEED - SERVO_FORCE_B_SPEED)*(SERVO_FORCE_A_SPEED - speed_mps);

	// Round the position.
	uint16_t servo_pos = (uint16_t) value; //  ceil(value);

	// Enforce min/max position.
	if (servo_pos > RESISTANCE_LEVEL[0])
		servo_pos = RESISTANCE_LEVEL[0];
	else if (servo_pos < RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS - 1])
		servo_pos = RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS - 1];

	return servo_pos;
}


int main(int argc, char *argv [])
{
	uint16_t position;
	uint16_t watts;
	float weight_kg;
	float speed_mps;
	float force_needed;


	printf("Press any key.");
	scanf("Starting...");
	/*
	my_t* p_my;
	my_t** pp_my;
	get_addr(pp_my);

	p_my = *pp_my;


	my_t* p_my	= NULL;
	get_addr(&p_my);

	weight_kg = 79.83219f;
	speed_mps = 6.71f;
	force_needed = 13.95185f;

	position = calc_servo_pos(weight_kg, speed_mps, force_needed);
	calc_power2(speed_mps, weight_kg, position, &watts);
	*/

	position = 5;

	while (--position)
		printf("count: %i\n", position);
	/*
	position = (33539 ^ 0xFFFF) + 8385;
	watts = 0;mak

	printf("val: %i||%i", position, watts);
	*/
}
