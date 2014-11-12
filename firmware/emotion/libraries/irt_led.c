/*
 * irt_led.c
 *
 *  Created on: Nov 12, 2014
 *      Author: Jason
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "irt_led.h"
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

static bool m_running = false;
static uint32_t m_last_pin_mask; 	// holds state of the last set of lights to be set on.
static blink_t m_repeating_blink = { .interval_ms = 0 };			// holds onto the last running blink pattern
static blink_t m_active_blink;		// actively running blink pattern
static app_timer_id_t m_blink_timer;


static inline void blink_timer_start()
{
	uint32_t err_code;

	uint32_t ticks = APP_TIMER_TICKS(m_active_blink.interval_ms, APP_TIMER_PRESCALER);

	err_code = app_timer_start(m_blink_timer, ticks, NULL);
	APP_ERROR_CHECK(err_code);

	m_running = true;
}

static void led_gpio_init()
{
	// Configure LED-pins as outputs.
	nrf_gpio_cfg_output(LED_FRONT_GREEN);
	nrf_gpio_cfg_output(LED_FRONT_RED);
	nrf_gpio_cfg_output(LED_BACK_GREEN);
	nrf_gpio_cfg_output(LED_BACK_RED);

	// Clear all LEDs to start
	NRF_GPIO->OUTSET = (
			1UL << LED_BACK_RED | 1UL << LED_FRONT_RED |
			1UL << LED_BACK_GREEN | 1UL << LED_FRONT_GREEN);
}

static void blink_handler(void * p_context)
{
	UNUSED_PARAMETER(p_context);

	// Walk through the pattern in reverse, eg. 10100000 starts with most significant bit 1.
	// If this isn't a repeating blink pattern, stop it.
	if (--m_active_blink.count == 0 && !m_active_blink.repeated)
	{
		led_blink_stop();

		// Restore last pin mask.
		NRF_GPIO->OUTCLR = m_last_pin_mask;

		// If we have a repeating pattern, restart it.
		if (m_repeating_blink.repeated)
		{
			m_active_blink = m_repeating_blink;
			blink_timer_start();
		}

		return;
	}

	// Pattern is a bitmask of positions indicating when to turn on/off.
	if ( m_active_blink.pattern & (1UL << m_active_blink.count) )
	{
		// Turn on
		NRF_GPIO->OUTCLR = m_active_blink.pin_mask;
	}
	else
	{
		// Turn off
		NRF_GPIO->OUTSET = m_active_blink.pin_mask;
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
void led_on(uint32_t pin_mask)
{
	m_last_pin_mask = pin_mask;
	// TODO: this is dangerous because we don't validate that it's an LED pin.
	NRF_GPIO->OUTCLR = pin_mask;
}

/**@brief	Turn one or more LEDs off.
 *
 */
void led_off(uint32_t pin_mask)
{
	// TODO: this is dangerous because we don't validate that it's an LED pin.
	NRF_GPIO->OUTSET = pin_mask;
}

/**@brief	Stop blinking, shut off all LEDs.
 *
 */
void led_blink_stop()
{
	// Shut down the running blink
	if (m_running)
	{
		app_timer_stop(m_blink_timer);
		m_running = false;

		// Turn any of these LEDs off.
		led_off(m_active_blink.pin_mask);

		if (m_active_blink.repeated)
		{
			// Copy for later use and clear the active one.
			m_repeating_blink = m_active_blink;
			memset(&m_active_blink, 0, sizeof(blink_t));
		}
	}
}

/**@brief	Blink LED according to the pattern defined.
 *
 */
void led_blink(blink_t blink)
{
	// Stop a current blink if there is one running.
	led_blink_stop();

	// if this is not a repeating blink, clear any existing LEDs temporarily.
	if (!blink.repeated)
	{
		// Clear any existing LEDs.
		led_off(LED_MASK_ALL);
	}

	m_active_blink = blink;
	blink_timer_start();
}

/**@brief	Initializes the LEDs vio GPIO and the timer use to control.
 *
 */
void led_init(void)
{
	led_gpio_init();
	blink_timer_init();

	// Turn on the power light.
	led_on(LED_MASK_POWER_ON);
}

