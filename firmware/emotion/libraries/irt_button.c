/*
 * irt_button.c
 *
 * Debounce buttons
 *
 *  Created on: Oct 1, 2014
 *      Author: Jason
 */
#include "irt_button.h"
#include "stdbool.h"
#include "boards.h"
#include "nrf_gpio.h"
#include "debug.h"

/**@brief Debug logging for main module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define BTN_LOG debug_log
#else
#define BTN_LOG(...)
#endif // ENABLE_DEBUG_LOG

#define CHECK_MSEC 5 // Read hardware every 5 msec
#define PRESS_MSEC 10 // Stable time before registering pressed
#define RELEASE_MSEC 100 // Stable time before registering released

// This function reads the key state from the hardware.
extern bool RawKeyPressed();

// This holds the debounced state of the key.
bool DebouncedKeyPress = false;

// Service routine called every CHECK_MSEC to
// debounce both edges
void DebounceButton(bool *Key_changed, bool *Key_pressed)
{
	static uint8_t Count = RELEASE_MSEC / CHECK_MSEC;
	bool RawState;

	*Key_changed = false;
	*Key_pressed = DebouncedKeyPress;

	RawState = RawKeyPressed();

	if (RawState == DebouncedKeyPress)
	{
		// Set the timer which allows a change from current state.
		if (DebouncedKeyPress)
			Count = RELEASE_MSEC / CHECK_MSEC;
		else
			Count = PRESS_MSEC / CHECK_MSEC;
	}
	else
	{
		// Key has changed - wait for new state to become stable.
		if (--Count == 0)
		{
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

static uint8_t button_pin = PIN_PBSW;

static bool key_down()
{
	// Button reads 0 when pushed down.
	return !nrf_gpio_pin_read(button_pin);
}

// called from a timer every 50ms.
void button_debounce()
{
	static uint8_t down_counter = 0;
	static bool long_down = false;

	if (key_down())
	{
		down_counter++;
		if (down_counter == 20)
		{
			// long detected.
			long_down = true;
		}
	}
	else
	{
		if (down_counter > 1 && down_counter < 20)
		{
			// Short press
			BTN_LOG("SHORT press detected.\r\n");
		}
		else if (long_down)
		{
			long_down = false;
			BTN_LOG("LONG detected.\r\n");
		}
		// Reset counter
		down_counter = 0;
	}
}
