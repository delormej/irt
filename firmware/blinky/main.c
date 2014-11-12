/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/**
* @example Board/pca10003/blinky_example/main.c
*
* @brief Blinky Example Application main file.
*
* This file contains the source code for a sample application using GPIO to drive LEDs.
*
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf_error.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "simple_uart.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"
#include "app_timer.h"

#define PIN_VIN					1
#define PIN_POT_PWR				3
//#define LED_0					PIN_LED_A
//#define LED_1					PIN_LED_B

#define LED_FRONT_GREEN			PIN_LED_D
#define LED_FRONT_RED			PIN_LED_C
#define LED_BACK_GREEN			PIN_LED_B
#define LED_BACK_RED			PIN_LED_A

#define APP_TIMER_PRESCALER		0
#define APP_TIMER_MAX_TIMERS    2
#define APP_TIMER_OP_QUEUE_SIZE APP_TIMER_MAX_TIMERS


typedef struct blink_s
{
	uint32_t pin_mask; 			// Mask of pins to toggle
	uint8_t pattern; 			// each bit represents what to in 1 of 8 slots
	uint8_t count : 3; 			// running count of where in the sequence
	uint8_t interval_ms;		// Interval time to repeat in ms.
} blink_t;

static blink_t m_repeated_blink = { .interval_ms = 0 };
static app_timer_id_t m_blink_timer;

void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
	nrf_gpio_pin_clear(LED_FRONT_RED);
}

static void blink_gpio_init()
{
	// Configure LED-pins as outputs.
	nrf_gpio_cfg_output(LED_FRONT_GREEN);
	nrf_gpio_cfg_output(LED_FRONT_RED);
	nrf_gpio_cfg_output(LED_BACK_GREEN);
	nrf_gpio_cfg_output(LED_BACK_RED);

	// Clear all LEDs to start
	NRF_GPIO->OUTSET = (1UL << LED_BACK_RED | 1UL << LED_FRONT_RED | 1UL << LED_BACK_GREEN | 1UL << LED_FRONT_GREEN);
}

static inline void blink_timer_start(blink_t *p_blink)
{
	uint32_t err_code;

	// Setup for 20ms
	uint32_t ticks = APP_TIMER_TICKS(p_blink->interval_ms, APP_TIMER_PRESCALER);

	err_code = app_timer_start(m_blink_timer, ticks, p_blink);
	APP_ERROR_CHECK(err_code);
}

static inline void blink_timer_restart()
{
	app_timer_stop(m_blink_timer);

	// If there is a repeated timer, restart it.
	if (m_repeated_blink.interval_ms != 0)
	{
		blink_timer_start(&m_repeated_blink);
	}
}

static void blink_handler(void * p_context)
{
	blink_t* p_blink = (blink_t*)p_context;

	if ( --(p_blink->count) == 0 && p_blink != &m_repeated_blink)
	{
		// Not in a repeated timer, we're through with a pattern, so restart repeated.
		blink_timer_restart();
	}
	else
	{
		// Pattern is a bitmask of positions indicating when to turn on/off.
		// Walk through the pattern in reverse, eg. 10100000 starts with most significant bit 1.
		if ( p_blink->pattern & (1 << p_blink->count) )
		{
			// Turn on
			NRF_GPIO->OUTCLR = p_blink->pin_mask;
		}
		else
		{
			// Turn off
			NRF_GPIO->OUTSET = p_blink->pin_mask;
		}
	}
}

static void blink_timer_init()
{
	uint32_t err_code;

	// Initialize the APP timer.
	APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);

	NRF_CLOCK->TASKS_LFCLKSTART = 1;

	while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
	NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;

	NRF_CLOCK->TASKS_LFCLKSTART = 1;

	while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);

    err_code = app_timer_create(&m_blink_timer,
                                APP_TIMER_MODE_REPEATED,
                                blink_handler);
	APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
	blink_gpio_init();
	blink_timer_init();

	blink_t blink = {
			.pin_mask = (1UL << LED_BACK_GREEN),	// Just blink 1 LED.
			.pattern =  0b11001100,	// 0b10101000,
			.count = 0,
			.interval_ms = 200 };

	m_repeated_blink = blink;

	blink_timer_start(&m_repeated_blink);

	nrf_delay_ms(2000);

	app_timer_stop(m_blink_timer);

	blink_t blink2 = {
			.pin_mask = (1UL << LED_FRONT_GREEN),	// Just blink 1 LED.
			.pattern =  0b11100000,	// 0b10101000,
			.count = 0,
			.interval_ms = 200 };

	blink_timer_start(&blink2);

	/*
	nrf_gpio_pin_set(LED_BACK_RED);
	nrf_gpio_pin_set(LED_FRONT_RED);
	nrf_gpio_pin_set(LED_BACK_GREEN);
	nrf_gpio_pin_set(LED_FRONT_GREEN);
	 */

	for(;;)
	{

	}

	// Reset the system.
	NVIC_SystemReset();
}

