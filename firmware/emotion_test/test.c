/*
(1) Launch developer command prompt:
%comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat""
(2) build with debug symbols.
cl test.c /ZI 

cl test.c ../emotion/libraries/speed_event_fifo.c  /I . /I ../emotion/libraries /ZI

*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum
{
	OFF = 0,
	SOLID	= 0b11111111,
	SLOW_BLINK = 0b11110000,
	FAST_BLINK = 0b10000000,
	FAST_BLINK_2 = 0b10100000,
	FAST_BLINK_3 = 0b10101000
} blink_pattern_e;

typedef enum
{
	LED_FRONT_GREEN	=		0,
	LED_FRONT_RED =			1,
	LED_BACK_GREEN =		2,
	LED_BACK_RED =			3
} led_e;

typedef struct
{
	const uint8_t pin;
	blink_pattern_e pattern;
} pattern_t;
#define PATTERN_LEN				4								// Working with 4 pins (2 LEDs, 2 pins each)
#define LED_FRONT_GREEN_PIN			14
#define LED_FRONT_RED_PIN			15
#define LED_BACK_GREEN_PIN			16
#define LED_BACK_RED_PIN			17

#define PATTERN_DEFAULT			{			\
									[LED_FRONT_GREEN] = { .pin = LED_FRONT_GREEN_PIN, 	.pattern = 0b00000000 }, 	\
									[LED_FRONT_RED] =	{ .pin = LED_FRONT_RED_PIN, 	.pattern = 0b00000000 },	\
									[LED_BACK_GREEN] =	{ .pin = LED_BACK_GREEN_PIN, 	.pattern = 0b00000000 },	\
									[LED_BACK_RED] =	{ .pin = LED_BACK_RED_PIN, 		.pattern = 0b00000000 }		\
								}

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c\r\n"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

// active pattern per pin
static pattern_t active_pattern[PATTERN_LEN] = PATTERN_DEFAULT;
// storage for patterns that repeat
static blink_pattern_e repeating_pattern[PATTERN_LEN];

static void pattern_set(led_e led, blink_pattern_e pattern, bool repeated)
{
	uint8_t led_pair = (led & 0x1) ? led-1: led+1;		// LEDs are in pairs, get it's pair
	active_pattern[led].pattern = pattern;				// Set the active patter for the specified led
	active_pattern[led_pair].pattern ^= (pattern & active_pattern[led_pair].pattern); 		// XOR of matching bits.

	/*LED_LOG("[LED] setting: %i, pair: %i, pattern: %i, pair: %i\r\n",
			led, led_pair, active_pattern[led].pattern, active_pattern[led_pair].pattern);*/

	if (repeated)
	{
		repeating_pattern[led] = active_pattern[led].pattern;
		repeating_pattern[led_pair] = active_pattern[led_pair].pattern;
	}

	printf("LED(%i)      "BYTE_TO_BINARY_PATTERN, led, BYTE_TO_BINARY(active_pattern[led].pattern));
	printf("LED_PAIR(%i) "BYTE_TO_BINARY_PATTERN, led_pair, BYTE_TO_BINARY(active_pattern[led_pair].pattern));

	/*
	printf("active == %i\r\npair == %i\r\n", 
		active_pattern[led].pattern,
		active_pattern[led_pair].pattern);*/

}

int main(int argc, char *argv [])
{
	pattern_set(LED_FRONT_GREEN, FAST_BLINK_2, false);

	pattern_set(LED_FRONT_GREEN, SOLID, true);
	// 168 == 1010 1000
	// 87  == 0101 0111 

	pattern_set(LED_FRONT_RED, FAST_BLINK_3, false);
}
