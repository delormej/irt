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
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "app_gpiote.h"
#include "simple_uart.h"

#define LED_0					12
#define LED_1					13

#define LED_2					18
#define LED_3					19

#define PIN_FLYWHEEL 										0		// This is the output of the optical sensor.
#define PIN_UART_RTS			23
#define PIN_UART_CTS			25
#define PIN_UART_TXD			21
#define PIN_UART_RXD			24
#define UART_HWFC				false

void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
}

static void interrupt_handler(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low)
{
	simple_uart_put('|');
}

static void gpio_init()
{
	uint32_t err_code;
	uint32_t pins_low_to_high_mask, pins_high_to_low_mask;
	app_gpiote_user_id_t p_user_id;

	APP_GPIOTE_INIT(1);

	pins_low_to_high_mask = 1;
	pins_high_to_low_mask = 0;

	err_code = app_gpiote_user_register(&p_user_id,
		pins_low_to_high_mask,
		pins_high_to_low_mask,
		interrupt_handler);
	//APP_ERROR_CHECK(err_code);

	err_code = app_gpiote_user_enable(p_user_id);
	//APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
  // Configure LED-pins as outputs.
  nrf_gpio_cfg_output(LED_0);
  nrf_gpio_cfg_output(LED_1);
  nrf_gpio_cfg_input(PIN_FLYWHEEL, NRF_GPIO_PIN_NOPULL);
 
  simple_uart_config(PIN_UART_RTS, PIN_UART_TXD, PIN_UART_CTS, PIN_UART_RXD, UART_HWFC);
  simple_uart_putstring((const uint8_t *)" \n\rBlinky Starting\n\r: ");

  gpio_init();

  // LED 0 and LED 1 blink alternately.
  while(true)
  {
    nrf_gpio_pin_clear(LED_0);
    nrf_gpio_pin_set(LED_1);
    
    nrf_delay_ms(500);
    
    nrf_gpio_pin_clear(LED_1);
    nrf_gpio_pin_set(LED_0);
    
    nrf_delay_ms(500);
  }
}

