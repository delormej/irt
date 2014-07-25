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
#include "nrf_delay.h"
#include "nrf_gpio.h"
//#include "app_gpiote.h"
#include "boards.h"
#include "simple_uart.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"

#define PIN_VIN					1
#define PIN_POT_PWR				3
#define LED_0					PIN_LED_A
#define LED_1					PIN_LED_B

int _write(int fd, char * str, int len)
{
	for (int i = 0; i < len; i++)
	{
		simple_uart_put(str[i]);
	}
	return len;
}

void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
}

static void interrupt_handler(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low)
{
	simple_uart_put('|');
}
/*
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
}*/

static void config_adc()
{
	// Configure ADC
	NRF_ADC->INTENSET = ADC_INTENSET_END_Msk;
	NRF_ADC->CONFIG = (ADC_CONFIG_RES_10bit << ADC_CONFIG_RES_Pos) |
		(ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos) |
		(ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos) |
		(ADC_CONFIG_PSEL_AnalogInput2 << ADC_CONFIG_PSEL_Pos) |
		(ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos);
}

static void start_adc()
{
	uint32_t err_code;

	// Stop any running conversions.
	NRF_ADC->EVENTS_END = 0;

	// Enable the ADC and start it.
	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;
	NRF_ADC->TASKS_START = 1;
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
  // Configure LED-pins as outputs.
  nrf_gpio_cfg_output(LED_0);
  nrf_gpio_cfg_output(LED_1);
  //nrf_gpio_cfg_input(PIN_FLYWHEEL, NRF_GPIO_PIN_NOPULL);
 
  // Send power to the POT.
  nrf_gpio_cfg_output(PIN_POT_PWR);
  nrf_gpio_pin_set(PIN_POT_PWR);

  simple_uart_config(PIN_UART_RTS, PIN_UART_TXD, PIN_UART_CTS, PIN_UART_RXD, UART_HWFC);

  simple_uart_putstring((const uint8_t *)" \n\rBlinky Starting\n\r: ");
  /*
  config_adc();
  start_adc();

  uint32_t result, last_result;
  
  while (true)
  {
	  if (!NRF_ADC->BUSY)
	  {
		  result = NRF_ADC->RESULT;

		  if (result != last_result)
		  {
			  printf("Read: %i\r\n", result);
			  last_result = result;
		  }

		  // Start it again.
		  start_adc();
		  nrf_delay_ms(100);
	  }
  } */

#ifdef PIN_PBSW
  nrf_gpio_cfg_input(PIN_PBSW, NRF_GPIO_PIN_NOPULL);
  while( (nrf_gpio_pin_read(PIN_PBSW) == 0) )
#else
  // LED 0 and LED 1 blink alternately.
  while(true)
#endif
  {
    nrf_gpio_pin_clear(LED_0);
    nrf_gpio_pin_set(LED_1);
    
    nrf_delay_ms(500);
	
	simple_uart_put('.');
    
    nrf_gpio_pin_clear(LED_1);
    nrf_gpio_pin_set(LED_0);
    
    nrf_delay_ms(500);
  }

  // Reset the system.
  NVIC_SystemReset();
}

