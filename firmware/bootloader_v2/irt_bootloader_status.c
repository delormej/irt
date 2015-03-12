#include <stdbool.h>
#include "nrf_gpio.h"
#include "irt_bootloader_status.h"
#include "nrf_delay.h"

// TODO: These defines should be shared with main firmware.
#define LED_FRONT_GREEN_PIN			12
#define LED_FRONT_RED_PIN			13
#define LED_BACK_GREEN_PIN			19
#define LED_BACK_RED_PIN			18


void bootloader_status_init()
{
	// Initialize LEDs.
	nrf_gpio_cfg_output(LED_FRONT_GREEN_PIN);
	nrf_gpio_cfg_output(LED_FRONT_RED_PIN);
	nrf_gpio_cfg_output(LED_BACK_GREEN_PIN);
	nrf_gpio_cfg_output(LED_BACK_RED_PIN);

	// Turn off all LEDs.
	nrf_gpio_pin_set(LED_FRONT_GREEN_PIN);
	nrf_gpio_pin_set(LED_FRONT_RED_PIN);
	nrf_gpio_pin_set(LED_BACK_GREEN_PIN);
	nrf_gpio_pin_set(LED_BACK_RED_PIN);
}

void on_bootloader_advertising(bool advertising)
{
	if (advertising)
	{
		// Back LED is solid red.
		nrf_gpio_pin_clear(LED_BACK_RED_PIN);
	}
	else
	{
		// Turn off back RED LED
		nrf_gpio_pin_set(LED_BACK_RED_PIN);
	}
}

void on_bootloader_connected(bool connected)
{
	if (connected)
	{
		// Back & Front LEDs solid RED.
		nrf_gpio_pin_clear(LED_BACK_RED_PIN);
		nrf_gpio_pin_clear(LED_FRONT_RED_PIN);
	}
	else
	{
		// Turn off Back & Front LEDs
		nrf_gpio_pin_set(LED_BACK_RED_PIN);
		nrf_gpio_pin_set(LED_FRONT_RED_PIN);
	}
}

void on_bootloader_in_progress()
{
	// Nothing
}

void on_bootloader_finished()
{
	// Turn them all off.
	nrf_gpio_pin_set(LED_FRONT_RED_PIN);
	nrf_gpio_pin_set(LED_BACK_RED_PIN);
	nrf_gpio_pin_set(LED_BACK_GREEN_PIN);
	nrf_gpio_pin_set(LED_BACK_GREEN_PIN);
}

