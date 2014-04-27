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

#define MAX_RESISTANCE_LEVELS 10						// Maximum resistance levels available.
#define MIN_RESISTANCE_LEVEL	1500					// Minimum by which there is no longer resistance.
#define	MATH_PI			3.14159265358979f

#define HIGH_BYTE(word)              		(uint8_t)((word >> 8u) & 0x00FFu)           /**< Get high byte of a uint16_t. */
#define LOW_BYTE(word)               		(uint8_t)(word & 0x00FFu)                   /**< Get low byte of a uint16_t. */


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

static float calc_angular_vel(uint8_t wheel_ticks, uint16_t period_2048)
{
	float value;

	value = (2.0f * MATH_PI * wheel_ticks) / (period_2048 / 2048.0f);

	return value;
}

static uint16_t calc_torque(int16_t watts, uint16_t period_seconds_2048)
{
	return (watts * period_seconds_2048) / (128 * MATH_PI);
}


int main(int argc, char *argv [])
{
	uint8_t mode;
	uint16_t level;
	uint8_t output;

	mode = 0x43 - 64u;
	level = 350;

	output = HIGH_BYTE(level);
	output |= mode << 6;

	printf("output: %i\n", output);

	/*
	uint16_t position;
	uint16_t torque;
	float weight_kg;
	float speed_mps;
	float force_needed;
	float partial_wheel_rev;
	float partial_wheel_rev2;
	uint8_t data[2];

	data[0] = 0xFA;
	data[1] = 0xBC;

	position = data[0] | (data[1] << 8);

	printf("%i\n", position);

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
	

	speed_mps = calc_angular_vel(2, 512);
	torque = calc_torque(250, 512);
	

	partial_wheel_rev = fmod(1.8992, 1);
	partial_wheel_rev2 = fmod(0.94996, 1);


	printf("1,2: %f, %f \n", partial_wheel_rev, partial_wheel_rev2);
	
	position = (33539 ^ 0xFFFF) + 8385;
	watts = 0;mak

	printf("val: %i||%i", position, watts);
	*/
}

// New speed module.

static uint32_t accum_flywheel_revs;
static uint16_t last_wheel_event_2048;

// Returns the last flywheel revs.
static uint32_t flywheel_revs_get();

