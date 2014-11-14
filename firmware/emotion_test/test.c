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

#define LED_PINS				(1UL << LED_FRONT_GREEN | 1UL << LED_FRONT_RED | 1UL << LED_BACK_GREEN | 1UL << LED_BACK_RED )
#define PATTERN_LEN				4								// Working with 4 pins (2 LEDs, 2 pins each)
#define PATTERN_END(POS)		(POS == 0x0)
#define PATTERN_DEFAULT			{			\
									[LED_FRONT_GREEN] = { .pin = LED_FRONT_GREEN_PIN }, \
									[LED_FRONT_RED] =	{ .pin = LED_FRONT_RED_PIN },	\
									[LED_BACK_GREEN] =	{ .pin = LED_BACK_GREEN_PIN },	\
									[LED_BACK_RED] =	{ .pin = LED_BACK_RED_PIN }		\
								}

#define POSITION_NEXT(POS)		(POS = (--POS & 0x7))

// this goes in the header:
typedef enum
{
	LED_POWER_ON,
	LED_BLE_ADV,
	LED_BUTTON_UP,
	LED_BATT_WARN,
	LED_BATT_LOW,

	LED_CALIBRATION_ENTER,
	LED_CALIBRATION_EXIT		// Need this state to clear.


} led_pattern_e;

typedef union 
{
	const uint8_t pin;
	uint8_t repeat  : 1;
	uint8_t pattern : 7;
} pattern_t;

// active pattern per pin
static pattern_t active_pattern[PATTERN_LEN] = PATTERN_DEFAULT;
// storage for patterns that repeat
static uint8_t repeating_pattern[PATTERN_LEN];

static void blink_handler(void * p_context)
{
	// running position count
	static uint8_t position = 0;
	uint32_t pin_mask_clear = 0, pin_mask_set = 0;
	
	// move position in reverse order, 0 == end of pattern
	POSITION_NEXT(position);

	// if the end of a non-repeating pattern, restore repeated pattern
	if (PATTERN_END(position))
	{
		for (uint8_t i = 0; i < PATTERN_LEN; i++)
		{
			if (active_pattern[i].repeat == 0)
			{
				active_pattern[i].pattern = repeating_pattern[i];
			}
		}

		//POSITION_NEXT(position);
	}
	
	// for each pin, determine if the current pattern position sets it ON 
	for (uint8_t i; i < PATTERN_LEN; i++)
	{
		// if we're supposed to turn the led on, add it to the clear mask.
		if (active_pattern[i].pattern & (1UL << position))
		{
			pin_mask_clear |= (1UL << active_pattern[i].pin);
		}
		else
		{
			pin_mask_set |= (1UL << active_pattern[i].pin);
		}
	}

	printf("position: %u\tpin state: %#.8x, %#.8x\r\n", position, pin_mask_clear, pin_mask_set);
}

// we could eliminate repeat bit and just say if (active_pattern[i].pattern == repeat_pattern[i])

/*
	Method has the logic required to figure out what lights to turn on / off based on state type passed in.
*/

static void led_set(led_pattern_e type)
{
	// stop the timer

	switch (type)
	{
		case LED_POWER_ON:
			active_pattern[LED_FRONT_GREEN].pattern = 5;
			active_pattern[LED_FRONT_GREEN].repeat = 1;
			break;

		case LED_BLE_ADV:
			active_pattern[LED_BACK_GREEN].pattern = 10; // 0b1111111;
			active_pattern[LED_BACK_GREEN].repeat = 1;
			break;

		case LED_BATT_LOW:
			// Invert the green pattern.
			active_pattern[LED_FRONT_GREEN].pattern = 10; // 0b1111111;
			active_pattern[LED_FRONT_GREEN].repeat = 1;
			active_pattern[LED_FRONT_RED].pattern = 10; // 0b1111111;
			active_pattern[LED_FRONT_RED].repeat = 1;
			break;

		default:
			// THROW AN ERROR, not a supported state.
			break;
	}

	// restart the timer
}

int main(int argc, char *argv [])
{
	uint8_t green = 0, red = 0, new_red = 0;

	green = 160;
	red = 161;
	red ^= (green & red);


	printf("Starting...%u,%u\r\n", green, red);
	
	return;

	led_set(LED_POWER_ON);

	memset(active_pattern, 0, PATTERN_LEN * sizeof(pattern_t));
	memset(repeating_pattern, 0, PATTERN_LEN * sizeof(pattern_t));

	while (1)
	{
		blink_handler((void*) 0);
		Sleep(750);
	}

	return 0;
}
