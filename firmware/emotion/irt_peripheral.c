#include "irt_peripheral.h"
#include "app_util.h"
#include "app_gpiote.h"
#include "app_timer.h"
#include "app_button.h"

static app_button_handler_t m_on_button_evt;

/**@brief LEDs initialization.
 *
 * @details Initializes all LEDs used by this application.
 */
static void leds_init(void)
{
    GPIO_LED_CONFIG(ADVERTISING_LED_PIN_NO);
    GPIO_LED_CONFIG(CONNECTED_LED_PIN_NO);
    GPIO_LED_CONFIG(ASSERT_LED_PIN_NO);
}


/**@brief Function for handling button events.
 *
 * @param[in]   pin_no   The pin number of the button pressed.
 */
static void button_event_handler(uint8_t pin_no)
{
		m_on_button_evt(pin_no);
}

/**@brief Function for initializing the GPIOTE module.
 */
static void gpiote_init(void)
{
    APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);
}

/**@brief Initialize buttons.
 */
static void buttons_init()
{
		// This declaration needs to be compile time constant, so we need to pass
		// this intermediary button_event_handler.
    static app_button_cfg_t buttons[] =
    {
				//{PIN_BUTTON_I,  false, NRF_GPIO_PIN_PULLUP, button_event_handler},
				{PIN_BUTTON_II, false, NRF_GPIO_PIN_PULLUP, button_event_handler},
				{PIN_BUTTON_III,false, NRF_GPIO_PIN_PULLUP, button_event_handler}
				//{PIN_BUTTON_IV, false, NRF_GPIO_PIN_PULLUP, button_event_handler}
    };
    
		// TODO: move away from using app_button as it's a huge memory hog
		// and does more than what we need.  It also uses a valuable timer
		// which we don't need, etc...
    APP_BUTTON_INIT(buttons, sizeof(buttons) / sizeof(buttons[0]), BUTTON_DETECTION_DELAY, false);
		app_button_enable();
}    

void peripheral_init(app_button_handler_t on_button_evt)
{
		m_on_button_evt = on_button_evt;
	
    leds_init();
		gpiote_init();
		// TODO: move away from using app_button as it's a huge memory hog
		// and does more than what we need.  It also uses a valuable timer
		// which we don't need, etc...
    buttons_init();
}
