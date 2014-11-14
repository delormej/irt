/*
 * irt_led.c
 *
 *  Created on: Nov 12, 2014
 *      Author: Jason
 */

#include <stdbool.h>
#include <stdint.h>
#include "irt_led.h"
#include "boards.h"
#include "nrf_gpio.h"
#include "app_timer.h"
#include "app_util.h"
#include "nordic_common.h"
#include "debug.h"

/**@brief Debug logging for main module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define LED_LOG debug_log
#else
#define LED_LOG(...)
#endif // ENABLE_DEBUG_LOG

#define APP_TIMER_PRESCALER		0
#define BLINK_RATE_TICKS		APP_TIMER_TICKS(150, APP_TIMER_PRESCALER)	// 75ms in ticks

// TODO: this module doesn't support HW_VERSION <2
#define LED_FRONT_GREEN_PIN			PIN_LED_D
#define LED_FRONT_RED_PIN			PIN_LED_C
#define LED_BACK_GREEN_PIN			PIN_LED_B
#define LED_BACK_RED_PIN			PIN_LED_A

#define LED_PINS				(1UL << LED_FRONT_GREEN_PIN | 1UL << LED_FRONT_RED_PIN | 1UL << LED_BACK_GREEN_PIN | 1UL << LED_BACK_RED_PIN )
#define PATTERN_LEN				4								// Working with 4 pins (2 LEDs, 2 pins each)
#define PATTERN_END(POS)		(POS == 0x0)
#define PATTERN_DEFAULT			{			\
									[LED_FRONT_GREEN] = { .pin = LED_FRONT_GREEN_PIN, 	.pattern = 0b00000000 }, 	\
									[LED_FRONT_RED] =	{ .pin = LED_FRONT_RED_PIN, 	.pattern = 0b00000000 },	\
									[LED_BACK_GREEN] =	{ .pin = LED_BACK_GREEN_PIN, 	.pattern = 0b00000000 },	\
									[LED_BACK_RED] =	{ .pin = LED_BACK_RED_PIN, 		.pattern = 0b00000000 }		\
								}
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

// active pattern per pin
static pattern_t active_pattern[PATTERN_LEN] = PATTERN_DEFAULT;
// storage for patterns that repeat
static blink_pattern_e repeating_pattern[PATTERN_LEN];

static app_timer_id_t m_blink_timer;
static bool m_running = false;

/**@brief	Helper function to set active pattern.
 *			Pattern is a series of 8 bits representing a sequential pattern for 1 ON, 0 OFF
 *			as the timer advances 1 sequence position of the pattern each tick.
 *			A repeated pattern is one that continues, vs. an temporary pattern only executes one cycle.
 */
static void pattern_set(led_e led, blink_pattern_e pattern, bool repeated)
{
	uint8_t led_pair = (led & 0x1) ? led-1: led+1;		// LEDs are in pairs, get it's pair
	active_pattern[led].pattern = pattern;				// Set the active patter for the specified led
	active_pattern[led_pair].pattern ^= (pattern & active_pattern[led_pair].pattern); 		// XOR of matching bits.

	LED_LOG("[LED] setting: %i, pair: %i, pattern: %i, pair: %i\r\n",
			led, led_pair, active_pattern[led].pattern, active_pattern[led_pair].pattern);

	if (repeated)
	{
		repeating_pattern[led] = active_pattern[led].pattern;
		repeating_pattern[led_pair] = active_pattern[led_pair].pattern;
	}
}

static void led_gpio_init()
{
	// Configure LED-pins as outputs.
	nrf_gpio_cfg_output(LED_FRONT_GREEN_PIN);
	nrf_gpio_cfg_output(LED_FRONT_RED_PIN);
	nrf_gpio_cfg_output(LED_BACK_GREEN_PIN);
	nrf_gpio_cfg_output(LED_BACK_RED_PIN);

	// Clear all LEDs to start
	NRF_GPIO->OUTSET = LED_PINS;
}

static void timer_start()
{
	uint32_t err_code;

	err_code = app_timer_start(m_blink_timer, BLINK_RATE_TICKS, NULL);
	APP_ERROR_CHECK(err_code);

	m_running = true;
}

static void timer_stop()
{
	if (m_running)
	{
		app_timer_stop(m_blink_timer);
		m_running = false;
	}
}

static void blink_handler(void * p_context)
{
	UNUSED_PARAMETER(p_context);

	// running position count
	static uint8_t position = 0;

	// move position in reverse order, 0 == end of pattern
	position = (--position & 0x7);

	// if the end of a non-repeating pattern, restore repeated pattern
	if (PATTERN_END(position))
	{
		for (uint8_t i = 0; i < PATTERN_LEN; i++)
		{
			// is this the repeating pattern for this pin?
			if (active_pattern[i].pattern != repeating_pattern[i])
			{
				active_pattern[i].pattern = repeating_pattern[i];
			}
		}
	}

	// for each pin, determine if the current pattern position sets it ON
	for (uint8_t i = 0; i < PATTERN_LEN; i++)
	{
		// if we're supposed to turn the led on, add it to the clear mask.
		if (active_pattern[i].pattern & (1UL << position))
		{
			// Turn on.
			nrf_gpio_pin_clear(active_pattern[i].pin);
		}
		else
		{
			// Turn off.
			nrf_gpio_pin_set(active_pattern[i].pin);
		}
	}
}

static void blink_timer_init()
{
	uint32_t err_code;
    err_code = app_timer_create(&m_blink_timer,
                                APP_TIMER_MODE_REPEATED,
                                blink_handler);
	APP_ERROR_CHECK(err_code);
}

/**@brief	Turn one or more LEDs on.
 *
 */
void led_set(led_state_e state)
{
	// stop the timer
	timer_stop();

	switch (state)
	{
		case LED_POWER_OFF:
			// Clear them all, don't restart the timer and exit.
			NRF_GPIO->OUTSET = LED_PINS;
			return;

		case LED_POWER_ON:
			pattern_set(LED_FRONT_GREEN, SOLID, true);
			break;

		case LED_BATT_LOW:
			// 1 fast red blink
			pattern_set(LED_FRONT_RED, FAST_BLINK, true);
			break;

		case LED_BATT_WARN:
			// 2 fast red blinks
			pattern_set(LED_FRONT_RED, FAST_BLINK_2, true);
			break;

		case LED_BATT_CRITICAL:
			// Turn off the front green led.
			pattern_set(LED_FRONT_GREEN, OFF, true);
			// 3 fast blinks.
			pattern_set(LED_FRONT_RED, FAST_BLINK_3, true);
			break;

		case LED_CHARGING:
			pattern_set(LED_FRONT_GREEN, SLOW_BLINK, true);
			break;

		case LED_CHARGED:
			pattern_set(LED_FRONT_GREEN, FAST_BLINK_2, true);
			break;

		// Function indicators
		case LED_BLE_ADVERTISTING:
			pattern_set(LED_BACK_GREEN, SLOW_BLINK, true);
			break;

		case LED_BLE_CONNECTED:
			pattern_set(LED_BACK_GREEN, SOLID, true);
			break;

		case LED_BLE_DISCONNECTED:
		case LED_BLE_TIMEOUT:
			pattern_set(LED_BACK_GREEN, OFF, true);
			break;

		case LED_BUTTON_UP:
			pattern_set(LED_BACK_GREEN, FAST_BLINK_3, false);
			break;

		case LED_MODE_STANDARD:
			pattern_set(LED_BACK_GREEN, FAST_BLINK_2, false);
			break;

		case LED_MODE_ERG:
			pattern_set(LED_BACK_GREEN, FAST_BLINK, false);
			break;

		case LED_MIN_MAX:
			pattern_set(LED_BACK_RED, FAST_BLINK_2, false);
			break;

		case LED_CALIBRATION_ENTER:
			pattern_set(LED_BACK_RED, FAST_BLINK_3, true);
			break;

		case LED_CALIBRATION_EXIT:
			pattern_set(LED_BACK_RED, OFF, true);
			break;

		case LED_POWERING_DOWN:
			pattern_set(LED_FRONT_RED, FAST_BLINK, true);
			pattern_set(LED_BACK_RED, FAST_BLINK, true);
			break;

		case LED_ERROR:
			pattern_set(LED_FRONT_RED, SLOW_BLINK, true);
			pattern_set(LED_BACK_RED, SLOW_BLINK, true);
			break;

		default:
			// THROW AN ERROR, not a supported state.
			APP_ERROR_CHECK(NRF_ERROR_INVALID_PARAM);
			break;
	}

	LED_LOG("[LED] Setting LED state: %i\r\n", state);

	// restart the timer
	timer_start();
}

/**@brief	Initializes the LEDs vio GPIO and the timer use to control.
 *
 */
void led_init(void)
{
	led_gpio_init();
	blink_timer_init();
}

