/*
 * irt_button.c
 *
 * Debounce buttons
 *
 *  Created on: August 8, 2017
 *      Author: Jason De Lorme
 */
#include <stdbool.h>
#include "irt_button.h"
#include "boards.h"
#include "nrf_gpio.h"
#include "app_timer.h"
#include "app_gpiote.h"
#include "debug.h"

/**@brief Debug logging for button module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define BTN_LOG debug_log
#else
#define BTN_LOG(...)
#endif // ENABLE_DEBUG_LOG

// How often to try to debounce (in ms) 
#define DEBOUNCE_CHECK_MS				APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)

// Seconds * ticks per second.  Amount of time the button was held to determine if it's a long press.
#define PRESS_DELAY_2_SEC				(2 * TICK_FREQUENCY)
#define PRESS_DELAY_4_SEC				(4 * TICK_FREQUENCY)
#define PRESS_DELAY_8_SEC				(8 * TICK_FREQUENCY)

static app_gpiote_user_id_t           m_gpiote_user_id;             /**< GPIOTE user id for buttons module. */
static app_timer_id_t                 m_debounce_timer_id;  		/**< Timer called to every 'n' to debounce button. */
static on_button_pbsw_t				  m_on_button_pressed;			// Function pointer to be invoked when a button press and release are fully debounced.
static bool 						  delay_exceeded = false;		// Hangs on to whether we've exceeded max delay. 

// Tracks when the button press was initiated.
static uint32_t m_press_start = 0; 

/*

Button is active low, i.e. 0 when pressed, 1 when not pressed.

Check button state every 5ms
#define CHECK_MSEC	5		// Read hardware every 5 msec


1. in init:
	- save reference button press callback 
	- initialize timer
	- register gpio event callback when button is pressed.
		// this will require a seperate user object for gpiote, with a bitmask to just PBSW_PIN
		// or we can just do it directly with: NRF_GPIO->PIN_CNF[pin_no] .. but there is a lot more to this, might be better off using app_gpiote

2. in gpiote_callback:
	- disable sense on the pin, so this doesn't fire again until fully debounced.
	- start a timer with a callback

3. in timer_callback (Debounce Method)
	- if debounced and it wasn't actually pushed
		- reset (stop timer, enable pin sense)
	- if debounced and still pushed
		- accumulate time it's been pushed ()
	- if debounced and released
		- reset (stop timer, enable pin sense)
		- raise button_press callback with the amount of time it was pressed for

// Keep calling debouncer from timer until it returns false? 

	0xE000 	1110000000000000
	0xF000  1111000000000000
*/

//
// Returns the current raw pin state (0 pressed, 1 not pressed).
//
static inline uint32_t irt_button_raw() 
{
	return nrf_gpio_pin_read(PIN_PBSW);
}

static inline void irt_debounce_sense_disable()
{
	app_gpiote_user_disable(m_gpiote_user_id);
	BTN_LOG("[BTN] irt_debounce_sense_disable\r\n");
	/*
	// Reset the SENSE bits
	NRF_GPIO->PIN_CNF[PIN_PBSW] &= ~GPIO_PIN_CNF_SENSE_Msk;
	// Disable interrupt
	NRF_GPIO->PIN_CNF[PIN_PBSW] |= GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos;
	*/
}

static inline void irt_debounce_sense_enable()
{
	app_gpiote_user_enable(m_gpiote_user_id);
	BTN_LOG("[BTN] irt_debounce_sense_enable\r\n");
	/*
	// Reset the SENSE bits
	NRF_GPIO->PIN_CNF[PIN_PBSW] &= ~GPIO_PIN_CNF_SENSE_Msk;
	// Enable interrput
	NRF_GPIO->PIN_CNF[PIN_PBSW] |= GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos;
	*/
}

static uint32_t irt_debounce_timer_start()
{
	BTN_LOG("[BTN] button timer starting.\r\n");			
    return app_timer_start(m_debounce_timer_id,
							DEBOUNCE_CHECK_MS,
							NULL);	
}

static uint32_t irt_debounce_timer_stop()
{
	return app_timer_stop(m_debounce_timer_id);
	BTN_LOG("[BTN] button timer stopped.\r\n");			
}

//
// Caculates the delta between start and current counter, accomodating for rollover.
//
static uint32_t get_delay()
{
	volatile uint32_t delta = 0;
	volatile uint32_t current = NRF_RTC1->COUNTER;

	if (current < m_press_start)
	{
		// handle rollover
		delta = (0xFFFFFFFF ^ m_press_start) + current;
	}
	else
	{
		delta = current - m_press_start;
	}

	// BTN_LOG("[BTN] get_delay: %i, %i, %i \r\n", m_press_start, current, delta);

	return delta;
}

//
// 
// @note	
// 	
// This method effecively detects a stable change in state from button pressed 
// to not pressed.  It will return true when a button push is detected and 
// 11 occurences sequential of being called when it is not pushed.
static inline bool debounce()
{
	// Current debounce status.
	static uint16_t state = 0; 	

	// Progressively shift least significant bit left, OR it with 1 if pushed, 0 if not pushed.
	// 	0xE000 	1110000000000000
	state = (state << 1) | !irt_button_raw() | 0xE000;

	//	0xF000  1111000000000000	
	if (state == 0xF000)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//
// Determines the length of the press and invokes the button callback after it's been debouned.
//
static void on_irt_button_released(uint32_t press_delay)
{
	if (press_delay >= PRESS_DELAY_8_SEC)
	{
		BTN_LOG("[BTN] button released (PRESS_DELAY_8_SEC): %u \r\n", press_delay);
		m_on_button_pressed(press_delay_8_sec);
	}
	else if (press_delay >= PRESS_DELAY_4_SEC)
	{
		BTN_LOG("[BTN] button released (PRESS_DELAY_4_SEC): %u \r\n", press_delay);
		m_on_button_pressed(press_delay_4_sec);
	}
	else if (press_delay >= PRESS_DELAY_2_SEC)
	{
		BTN_LOG("[BTN] button released (PRESS_DELAY_2_SEC): %u \r\n", press_delay);
		m_on_button_pressed(press_delay_2_sec);
	}
	else
	{
		BTN_LOG("[BTN] button released (PRESS_DELAY_SHORT): %u \r\n", press_delay);			
		m_on_button_pressed(press_delay_short);
	}
}

static void irt_debounce_reset()
{
	// stop the timer, so we're not debouncing any longer.
	irt_debounce_timer_stop();	

	BTN_LOG("[BTN] debounce_reset \r\n");			
	delay_exceeded = false;
	// re-enable sensing
	irt_debounce_sense_enable();			
	// reset starting point.
	m_press_start = 0;	
}

//
// Called by debounce_timer every CHECK_MSEC (5ms)
//
static void irt_debounce_timer_handler()
{
	// Tracks how long the button was pressed for.
	volatile uint32_t press_delay = 0;
	press_delay = get_delay();

	// Debounce the press and release.
	if (debounce())
	{
		// if we're in here, we've succesfully debounced a PRESS or a RELEASE
		irt_debounce_reset();

		// If > 8 sec, then we've already invoked and we're just waiting to reset 
		// button state.
		if (!delay_exceeded)
		{
			BTN_LOG("[BTN] debounced! was pressed and released: %i.\r\n", press_delay);			
			// Invoke button callback.
			on_irt_button_released(press_delay);
		}

		
	}
	// otherwise, we have not debounced.
	else
	{
		// If button is still down invoke callback, but keep debounce timer going until
		// user eventually let's go, at which point reset state.  
		if (!delay_exceeded && press_delay >= PRESS_DELAY_8_SEC && !irt_button_raw())
		{
			// Keep track so that we only signal once.
			delay_exceeded = true;
			on_irt_button_released(press_delay);
		}
	}
}

//
// Called when pin changes.
//
static void irt_gpio_handler()
{
	m_press_start = NRF_RTC1->COUNTER;

	// Disable pin sense so that we don't get invoked while debouncing.
	irt_debounce_sense_disable();

	// Start the timer.
	irt_debounce_timer_start();

	BTN_LOG("[BTN] Got signaled: %i.\r\n", m_press_start);
}

//
// Initialization function.
//
uint32_t irt_button_init(on_button_pbsw_t handler)
{
	uint32_t err_code;
	BTN_LOG("[BTN] initializing irt_button\r\n");
	
	// Saves the button handler callback.
	m_on_button_pressed = handler;
	
	// Enables pin sense on the pin.

	// Configure pin for input.
	nrf_gpio_cfg_input(PIN_PBSW, NRF_GPIO_PIN_NOPULL);
	
    // Register callback for button pin as new gpiote user.
    err_code = app_gpiote_user_register(&m_gpiote_user_id,
                                        0,  				// low to high (don't detect this)
                                        (1 << PIN_PBSW),  	// high to low transition mask
                                        irt_gpio_handler);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Create debounce timer.
    err_code = app_timer_create(&m_debounce_timer_id,
                            APP_TIMER_MODE_REPEATED,
                            irt_debounce_timer_handler);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

	BTN_LOG("[BTN] initalized irt_button, 2 sec delay == %i, prescaler == %i\r\n", PRESS_DELAY_2_SEC,
		 NRF_RTC1->PRESCALER);

    return app_gpiote_user_enable(m_gpiote_user_id);
}
