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

static peripheral_evt_t *mp_on_peripheral_evt;
static app_timer_id_t m_led_blink_timer_id;

/**@brief Function for handling interrupt events.
 *
 * @param[in]   pin_no   The pin number of the button pressed.
 */
static void interrupt_handler(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low)
{
	if (event_pins_low_to_high & (1 << PIN_BUTTON_I))
		mp_on_peripheral_evt->on_button_i();
	else if (event_pins_low_to_high & (1 << PIN_BUTTON_II))
		mp_on_peripheral_evt->on_button_ii();
	else if (event_pins_low_to_high & (1 << PIN_BUTTON_III))
		mp_on_peripheral_evt->on_button_iii();
	else if (event_pins_low_to_high & (1 << PIN_BUTTON_IV))
		mp_on_peripheral_evt->on_button_iv();
	else if (event_pins_high_to_low & (1 << PIN_SHAKE))
	{
		// Always read the source as this will clear the buffer
		// so that the accelerometer will continue generating
		// events.
		uint8_t source = accelerometer_src();
		mp_on_peripheral_evt->on_accelerometer_evt(source);
	}
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
	uint32_t  pins_low_to_high_mask, pins_high_to_low_mask;

	// Initialize the LED pins.
	nrf_gpio_cfg_output(PIN_LED_A);		// Green
	nrf_gpio_cfg_output(PIN_LED_B);		// Red

	// Initialize the button inputs from the RXD4140.
	nrf_gpio_cfg_input(PIN_BUTTON_I, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(PIN_BUTTON_II, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(PIN_BUTTON_III, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(PIN_BUTTON_IV, NRF_GPIO_PIN_NOPULL);

	// Initialize the pin to wake the device on movement from the accelerometer.
	nrf_gpio_cfg_input(PIN_SHAKE, NRF_GPIO_PIN_NOPULL);
	//nrf_gpio_cfg_sense_input(PIN_SHAKE, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);

	APP_GPIOTE_INIT(1);
	
	pins_low_to_high_mask = (1 << PIN_BUTTON_I |
		1 << PIN_BUTTON_II |
		1 << PIN_BUTTON_III |
		1 << PIN_BUTTON_IV);
	
	pins_high_to_low_mask = 1 << PIN_SHAKE;

	app_gpiote_user_id_t p_user_id;

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

void blink_led_green_start(uint8_t interval_ms)
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
