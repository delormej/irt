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

/** @file
* @defgroup gpio_example_pca10001_main main.c
* @{
* @ingroup gpio_example_pca10001
*
* @brief GPIO Example Application main file.
*
* This file contains the source code for a sample application using GPIO to drive LEDs.
*
*/

#include <stdbool.h>
#include <stdint.h>
#include "app_gpiote.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "nrf_pwm.h"
#include "resistance.h"

#define NULL								0		// Not sure why this isn't defined in any other header?
#define PIN_SERVO_SIGNAL		3		// P3 - P0.03
#define PIN_BUTTON_II				2		// P3 - P0.02
#define PIN_BUTTON_III 			1		// P3 - P0.01
#define PIN_DRUM_REV 				0		// P3 - P0.00 
#define GPIOTE_MAX_USERS		1

uint8_t m_resistance_level = 0;

/*****************************************************************************
* Error Handling Functions
*****************************************************************************/


/**@brief Function for error handling, which is called when an error has occurred. 
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of error.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name. 
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    // This call can be used for debug purposes during development of an application.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    // ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover with a reset.
    NVIC_SystemReset();
}

void set_resistance(uint32_t position_us)
{
	// Move the servo.
	for (int i = 0; i<100; i++)
	{
		nrf_gpio_pin_write(PIN_SERVO_SIGNAL, 1);
		nrf_delay_us(position_us);
		nrf_gpio_pin_write(PIN_SERVO_SIGNAL, 0);
		nrf_delay_us(20000-position_us);
	}	
}

// 
// blinks led 0 for 1/2 second.
//
void blink_led(void)
{
		// Blink once for 1/2 second when the button is pushd.
		nrf_gpio_port_write(LED_PORT, 1 << (LED_OFFSET));
		nrf_delay_ms(500);
		nrf_gpio_port_write(LED_PORT, 0 << (LED_OFFSET));	
}

//
// Occurs when a drum revolution occurs
//
void on_drum_rev_event(void)
{
	blink_led();
}

void on_button_ii_event(void)
{
	blink_led();
	// decrement
	if (m_resistance_level > 0)
		set_resistance(RESISTANCE_LEVEL[--m_resistance_level]);	
}

//
// Occurs when button (III) is pressed.
//
void on_button_iii_event(void)
{
	blink_led();
	// increment
	if (m_resistance_level < (MAX_RESISTANCE_LEVELS-1))
		set_resistance(RESISTANCE_LEVEL[++m_resistance_level]);
}

//
// All GPIO events come to this function.
//
void on_gpiote_event(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low)
{
	if (event_pins_low_to_high & (1 << PIN_BUTTON_II))
		on_button_ii_event();
	else if (event_pins_low_to_high & (1 << PIN_BUTTON_III))
		on_button_iii_event();
	else if (event_pins_high_to_low & (1 << PIN_SERVO_SIGNAL))
		on_drum_rev_event();
}

//
// Initializes the GPIO tasks & events library and starts listening for GPIO events.
//
int init_gpiote(void)
{
  // Configure LED-pins as outputs
  nrf_gpio_range_cfg_output(LED_START, LED_STOP);
	nrf_gpio_cfg_output(PIN_SERVO_SIGNAL);
	
	nrf_gpio_cfg_input(PIN_BUTTON_III, NRF_GPIO_PIN_PULLUP);
	nrf_gpio_cfg_input(PIN_BUTTON_II, NRF_GPIO_PIN_PULLUP);	
	nrf_gpio_cfg_input(PIN_DRUM_REV, NRF_GPIO_PIN_NOPULL);
		
	APP_GPIOTE_INIT(GPIOTE_MAX_USERS);	
	
	app_gpiote_user_id_t p_user_id;
	uint32_t pins_low_to_high_mask = (1 << PIN_BUTTON_II) | (1 << PIN_BUTTON_III);	// buttons
	uint32_t pins_high_to_low_mask = 1 << PIN_DRUM_REV;		// drum rotation
	
	uint32_t err_code = app_gpiote_user_register(&p_user_id,
																					pins_low_to_high_mask,
																					pins_high_to_low_mask,
																					(app_gpiote_event_handler_t)on_gpiote_event);
	
	err_code = app_gpiote_user_enable(p_user_id);
	
	if (err_code != NRF_SUCCESS)
		return -1;
	else
		return NRF_SUCCESS;
}


// Blinks the 2 leds to say "hello" we're staring up.
void send_hello(void)
{
  uint8_t output_state = 0;
	nrf_gpio_port_write(LED_PORT, 1 << (output_state + LED_OFFSET));
	output_state = (output_state + 1) & BLINKY_STATE_MASK;
	nrf_delay_ms(500);
	nrf_gpio_port_write(LED_PORT, 1 << (output_state + LED_OFFSET));
	nrf_delay_ms(500);
	nrf_gpio_port_write(LED_PORT, 0 << (output_state + LED_OFFSET));	
}

/**
 * @brief Function for application main entry.
 * @return 0. int return type required by ANSI/ISO standard.
 */
int main(void)
{
	// Configure GPIO Tasks and Events
	init_gpiote();
	
	// Test blinking the light when it turns on.
	send_hello();
	
	// Set servo to 0 position
	m_resistance_level = 0;
	set_resistance(RESISTANCE_LEVEL[m_resistance_level]);

  while(true)
  {
		// no-op endless loop
  }
}

/**
 *@}
 **/
