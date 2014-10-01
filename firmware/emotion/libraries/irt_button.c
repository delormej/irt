/*
 * irt_button.c
 *
 * Debounce buttons
 *
 *  Created on: Oct 1, 2014
 *      Author: Jason
 */
#include <stdbool.h>
#include "irt_button.h"
#include "boards.h"
#include "nrf_gpio.h"
#include "debug.h"

#define LONG_BUTTON_COUNT		40U
#define SHORT_BUTTON_COUNT		1U

/**@brief Debug logging for main module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define BTN_LOG debug_log
#else
#define BTN_LOG(...)
#endif // ENABLE_DEBUG_LOG

static uint32_t m_button_pin;

void irt_button_init(uint32_t button_pin)
{
	m_button_pin = button_pin;
}

// called from a timer every 50ms.
irt_button_state_t irt_button_debounce()
{
	static uint8_t down_counter = 0;
	bool button_down = false;
	irt_button_state_t state = NoPress;

	// Read the button; 0 for depressed, 1 for not pressed.
	button_down = !nrf_gpio_pin_read(m_button_pin);

	if (button_down)
	{
		down_counter++;
		if (down_counter == LONG_BUTTON_COUNT)
		{
			// long detected.
			state = LongPress;
		}
	}
	// When the button is released.
	else
	{
		if (down_counter > SHORT_BUTTON_COUNT && down_counter < LONG_BUTTON_COUNT)
		{
			state = ShortPress;
		}

		// Reset counter
		down_counter = 0;
	}

	return state;
}

