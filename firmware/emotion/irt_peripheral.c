#include "irt_peripheral.h"
#include "app_util.h"
#include "app_gpiote.h"
#include "app_timer.h"
#include "app_button.h"
#include "accelerometer.h"

static app_button_handler_t m_on_button_evt;

/**@brief LEDs initialization.
 *
 * @details Initializes all LEDs used by this application.
 */
static void leds_init(void)
{
    nrf_gpio_cfg_output(PIN_LED_A);
    nrf_gpio_cfg_output(PIN_LED_B);
}

/**@brief Function for handling button events.
 *
 * @param[in]   pin_no   The pin number of the button pressed.
 */
static void button_event_handler(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low)
{
		uint8_t pin_no;
		
		if (event_pins_low_to_high & (1 << PIN_BUTTON_I)) 
			pin_no = PIN_BUTTON_I;
		else if (event_pins_low_to_high & (1 << PIN_BUTTON_II)) 
			pin_no = PIN_BUTTON_II;
		else if (event_pins_low_to_high & (1 << PIN_BUTTON_III)) 
			pin_no = PIN_BUTTON_III;
		else if (event_pins_low_to_high & (1 << PIN_BUTTON_IV)) 
			pin_no = PIN_BUTTON_IV;
		else if (event_pins_high_to_low & (1 << PIN_SHAKE))
			pin_no = PIN_SHAKE;
			
		m_on_button_evt(pin_no);
}

/**@brief Initialize buttons.
 */
static void buttons_init()
{
	nrf_gpio_cfg_input(PIN_BUTTON_I, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(PIN_BUTTON_II, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(PIN_BUTTON_III, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg_input(PIN_BUTTON_IV, NRF_GPIO_PIN_NOPULL);

	APP_GPIOTE_INIT(1);
	
	uint32_t err_code;
	uint32_t  pins_low_to_high_mask, pins_high_to_low_mask;

	pins_low_to_high_mask = (1 << PIN_BUTTON_I | 1 << PIN_BUTTON_II | 1 << PIN_BUTTON_III | 1 << PIN_BUTTON_IV);
	pins_high_to_low_mask = 0;
	app_gpiote_user_id_t p_user_id;

	err_code = app_gpiote_user_register(&p_user_id,
		pins_low_to_high_mask,
		pins_high_to_low_mask,
		button_event_handler);

	APP_ERROR_CHECK(err_code);

	err_code = app_gpiote_user_enable(p_user_id);
	
	APP_ERROR_CHECK(err_code);
}    

/**@brief Initialize the pin to wake the device on movement from the accelerometer */
static void wake_init(void)
{
	nrf_gpio_cfg_sense_input(PIN_SHAKE, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);
	accelerometer_init();
}

void peripheral_init(app_button_handler_t on_button_evt)
{
	m_on_button_evt = on_button_evt;
	
    leds_init();
	wake_init();
    buttons_init();
}
