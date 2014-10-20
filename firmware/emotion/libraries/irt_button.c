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

/* called from a timer
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
*/

#define CHECK_MSEC 5 // Read hardware every 5 msec
#define PRESS_MSEC 10 // Stable time before registering pressed
#define RELEASE_MSEC 100 // Stable time before registering released
// This function reads the key state from the hardware.


inline bool RawKeyPressed()
{
	return !nrf_gpio_pin_read(PIN_PBSW);
}


// This holds the debounced state of the key.
bool DebouncedKeyPress = false;
// Service routine called every CHECK_MSEC to
// debounce both edges
void DebounceSwitch1(bool *Key_changed, bool *Key_pressed)
{
static uint8_t Count = RELEASE_MSEC / CHECK_MSEC;
bool RawState;
*Key_changed = false;
*Key_pressed = DebouncedKeyPress;
RawState = RawKeyPressed();
if (RawState == DebouncedKeyPress) {
// Set the timer which allows a change from current state.
if (DebouncedKeyPress) Count = RELEASE_MSEC / CHECK_MSEC;
else Count = PRESS_MSEC / CHECK_MSEC;
} else {
// Key has changed - wait for new state to become stable.
if (--Count == 0) {
// Timer expired - accept the change.
DebouncedKeyPress = RawState;
*Key_changed=true;
*Key_pressed=DebouncedKeyPress;
// And reset the timer.
if (DebouncedKeyPress) Count = RELEASE_MSEC / CHECK_MSEC;
else Count = PRESS_MSEC / CHECK_MSEC;
}
}
}
