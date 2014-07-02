//
// This module wraps all peripheral interaction by the device.
//

#include "irt_peripheral.h"
#include "nordic_common.h"
#include "app_util.h"
#include "app_gpiote.h"
#include "app_timer.h"
#include "app_button.h"
#include "accelerometer.h"
#include "temperature.h"
#include "boards.h"

// RTC1 is based on a 32.768khz crystal, or in other words it oscillates
// 32768 times per second.  The PRESCALER determins how often a tick gets
// counted.  With a prescaler of 0, there are 32,768 ticks in 1 second
// 1/2048th of a second would be 16 ticks (32768/2048)
// # of 2048th's would then be ticks / 16.
#define	TICK_FREQUENCY	(32768 / (NRF_RTC1->PRESCALER + 1))

static peripheral_evt_t *mp_on_peripheral_evt;
static app_timer_id_t m_led_blink_timer_id;

/**@brief Function for handling interrupt events.
 *
 * @param[in]   pin_no   The pin number of the button pressed.
 */
static void interrupt_handler(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low)
{
	if (event_pins_low_to_high & (1 << PIN_PBSW))
		mp_on_peripheral_evt->on_button_i();
	else if (event_pins_high_to_low & (1 << PIN_SHAKE))
		mp_on_peripheral_evt->on_accelerometer_evt();
}

static void blink_timeout_handler(void * p_context)
{
	UNUSED_PARAMETER(p_context);

	// Toggle the green LED on/off.
	nrf_gpio_pin_toggle(PIN_LED_A);
}

/**@brief Initialize all peripherial pins.
 */
static void irt_gpio_init()
{
	uint32_t err_code;
	uint32_t pins_low_to_high_mask, pins_high_to_low_mask;
	app_gpiote_user_id_t p_user_id;

	// Initialize the LED pins.
	nrf_gpio_cfg_output(PIN_LED_A);		// Green
	nrf_gpio_cfg_output(PIN_LED_B);		// Red

#ifdef IRT_REV_2A_H
	nrf_gpio_cfg_output(PIN_EN_SERVO_PWR);
	nrf_gpio_cfg_output(PIN_SLEEP_N);
	// Enable servo / LED.
	nrf_gpio_pin_set(PIN_EN_SERVO_PWR);
	// Enable the power regulator.
	nrf_gpio_pin_set(PIN_SLEEP_N);
#endif

	// Initialize the pin to wake the device on movement from the accelerometer.
	nrf_gpio_cfg_input(PIN_SHAKE, NRF_GPIO_PIN_NOPULL);
	//nrf_gpio_cfg_sense_input(PIN_SHAKE, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);

	// These pins are used for UART on boards where there is no RXD.
#ifndef SIMPLE_UART
	// Initialize the button inputs from the RXD4140.
	nrf_gpio_cfg_input(PIN_BUTTON_I, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(PIN_BUTTON_II, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(PIN_BUTTON_III, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(PIN_BUTTON_IV, NRF_GPIO_PIN_NOPULL);
	
	pins_low_to_high_mask = (1 << PIN_BUTTON_I |
		1 << PIN_BUTTON_II |
		1 << PIN_BUTTON_III |
		1 << PIN_BUTTON_IV);
#else // SIMPLE_UART
	pins_low_to_high_mask = 0;
#endif // SIMPLE_UART

	pins_high_to_low_mask = 1 << PIN_SHAKE;

	APP_GPIOTE_INIT(1);

	err_code = app_gpiote_user_register(&p_user_id,
		pins_low_to_high_mask,
		pins_high_to_low_mask,
		interrupt_handler);
	APP_ERROR_CHECK(err_code);

	err_code = app_gpiote_user_enable(p_user_id);
	APP_ERROR_CHECK(err_code);
}    

void set_led_red(void)
{
	nrf_gpio_pin_clear(PIN_LED_A);
	nrf_gpio_pin_set(PIN_LED_B);
}

void set_led_green(void)
{
	nrf_gpio_pin_clear(PIN_LED_B);
	nrf_gpio_pin_set(PIN_LED_A);
}

void clear_led(void)
{
	nrf_gpio_pin_clear(PIN_LED_A);
	nrf_gpio_pin_clear(PIN_LED_B);
}

void blink_led_green_start(uint16_t interval_ms)
{
	uint32_t err_code;
	uint32_t interval_ticks;

	// Determine # of ticks based on ms.
	interval_ticks = APP_TIMER_TICKS(interval_ms, APP_TIMER_PRESCALER);

	// Stop any current LED flash.
	clear_led();

	// Start the timer.
	err_code = app_timer_start(m_led_blink_timer_id, interval_ticks, NULL);
	APP_ERROR_CHECK(err_code);
}

void blink_led_green_stop(void)
{
	uint32_t err_code;

	err_code = app_timer_stop(m_led_blink_timer_id);
	APP_ERROR_CHECK(err_code);

	clear_led();
}

/**@brief 	Returns the count of 1/2048th seconds (2048 per second) since the
 *			the counter started.
 *
 * @note	This value rolls over at 32 seconds.
 */
uint16_t seconds_2048_get()
{
	// Get current tick count.
	uint32_t ticks = NRF_RTC1->COUNTER;

	// Based on frequence of ticks, calculate 1/2048 seconds.
	// freq (hz) = times per second.
	uint16_t seconds_2048 = ROUNDED_DIV(ticks, (TICK_FREQUENCY / 2048));

	return seconds_2048;
}

void peripheral_init(peripheral_evt_t *p_on_peripheral_evt)
{
	uint32_t err_code;
	mp_on_peripheral_evt = p_on_peripheral_evt;
	
    irt_gpio_init();
    accelerometer_init();
	temperature_init();

	uint32_t val = nrf_gpio_pin_read(PIN_SHAKE);
	if (val == 1)
		set_led_red();
	else
		set_led_green();

	// Create the timer for blinking led.
	err_code = app_timer_create(&m_led_blink_timer_id,
		APP_TIMER_MODE_REPEATED,
		blink_timeout_handler);

	APP_ERROR_CHECK(err_code);
}
