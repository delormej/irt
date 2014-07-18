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
#include "debug.h"
#include "battery.h"

/**@brief Debug logging for main module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define PH_LOG debug_log
#else
#define PH_LOG(...)
#endif // ENABLE_DEBUG_LOG

// RTC1 is based on a 32.768khz crystal, or in other words it oscillates
// 32768 times per second.  The PRESCALER determins how often a tick gets
// counted.  With a prescaler of 0, there are 32,768 ticks in 1 second
// 1/2048th of a second would be 16 ticks (32768/2048)
// # of 2048th's would then be ticks / 16.
#define	TICK_FREQUENCY	(32768 / (NRF_RTC1->PRESCALER + 1))

static peripheral_evt_t *mp_on_peripheral_evt;
static app_timer_id_t m_led1_blink_timer_id;
//static app_timer_id_t m_led2_blink_timer_id;
static app_gpiote_user_id_t mp_user_id;

/**@brief Function for handling interrupt events.
 *
 * @param[in]   pin_no   The pin number of the button pressed.
 */
static void interrupt_handler(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low)
{
	//event_pins_low_to_high
	if (event_pins_high_to_low & (1 << PIN_SHAKE))
	{
		// testing - Turn off pin sense for a moment.
		//app_gpiote_user_disable(mp_user_id);
		mp_on_peripheral_evt->on_accelerometer_evt();
	}
#ifdef IRT_REV_2A_H
	// TODO: This button should be debounced.
	else if (event_pins_high_to_low & (1 << PIN_PBSW))
	{
		mp_on_peripheral_evt->on_button_pbsw();
	}
	else if (event_pins_high_to_low & (1 << PIN_PG_N))
	{
		// Detects when the power adapter is plugged in.
		mp_on_peripheral_evt->on_power_plug();
	}
#endif
}

static void blink_timeout_handler(void * p_context)
{
	UNUSED_PARAMETER(p_context);

	// Toggle the green LED on/off.
//	nrf_gpio_pin_toggle(PIN_LED_A);
	nrf_gpio_pin_toggle(PIN_LED_B);
}

/**@brief	Returns 0 = adapter power, 1= no adapter.
 */
static __INLINE uint32_t ac_adapter_off(void)
{
	return nrf_gpio_pin_read(PIN_PG_N);
}

/**@brief Initialize all peripherial pins.
 */
static void irt_gpio_init()
{
	uint32_t err_code;
	uint32_t pins_low_to_high_mask, pins_high_to_low_mask;

	// Initialize the LED pins.
	nrf_gpio_cfg_output(PIN_LED_A);		// Green
	nrf_gpio_cfg_output(PIN_LED_B);		// Red

#ifdef IRT_REV_2A_H
	nrf_gpio_cfg_output(PIN_LED_C);		// Red #2
	nrf_gpio_cfg_output(PIN_LED_D);		// Green #2

	// User push button on the board.
	// TODO: this needs to be debounced.
	nrf_gpio_cfg_input(PIN_PBSW, NRF_GPIO_PIN_NOPULL);

	// Enable servo / LED.
	nrf_gpio_cfg_output(PIN_EN_SERVO_PWR);
	nrf_gpio_pin_set(PIN_EN_SERVO_PWR);

	#ifdef USE_BATTERY_CHARGER
		// Configure pin for enabling or disabling battery charger. 0=on, 1=off
		nrf_gpio_cfg_output(PIN_CHG_EN_N);

		// Enable the power regulator if the board is built with power regulator.
		nrf_gpio_cfg_output(PIN_SLEEP_N);
		nrf_gpio_pin_set(PIN_SLEEP_N);

		// Configure pin to read if there is a power adapter.
		nrf_gpio_cfg_input(PIN_PG_N, NRF_GPIO_PIN_NOPULL);

		// Configure pins for battery charger status.
		nrf_gpio_cfg_input(PIN_STAT1, NRF_GPIO_PIN_NOPULL);
		nrf_gpio_cfg_input(PIN_STAT2, NRF_GPIO_PIN_NOPULL);
	#endif

	#ifdef USE_BATTERY_READ_PIN
		// When set enables the device to read battery voltage (on specific board builds - not all).
		nrf_gpio_cfg_output(PIN_ENBATT);
	#endif // USE_BATTERY_READ_PIN

	// Configure input for the battery AIN.
	// This is available whether or not the board is built with battery charge support.
	NRF_GPIO->PIN_CNF[PIN_BATT_VOLT] = (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos);
#endif //IRT_REV_2A_H

	// Initialize the pin to wake the device on movement from the accelerometer.
	nrf_gpio_cfg_sense_input(PIN_SHAKE, NRF_GPIO_PIN_NOPULL, GPIO_PIN_CNF_SENSE_Low);

	pins_low_to_high_mask =
#ifdef USE_BATTERY_CHARGER
		1 << PIN_STAT1;				// On battery charger status changing.
#else
		0;
#endif

	pins_high_to_low_mask = ( 1 << PIN_SHAKE
#ifdef IRT_REV_2A_H
			| 1 << PIN_PBSW			// On user push button switch
			| 1 << PIN_PG_N			// On power adapter plugged in
#endif
			);

	APP_GPIOTE_INIT(1);

	err_code = app_gpiote_user_register(&mp_user_id,
		pins_low_to_high_mask,
		pins_high_to_low_mask,
		interrupt_handler);
	APP_ERROR_CHECK(err_code);

	err_code = app_gpiote_user_enable(mp_user_id);
	APP_ERROR_CHECK(err_code);
}    

void set_led_red(uint8_t led_mask)
{
	// led_mask =
	// 0 led 1 only (backwards compat)
	// 1 led 1 only
	// 2 led 2 only
	// 3 both
	if (led_mask <= 1)
	{
		nrf_gpio_pin_clear(PIN_LED_A);
		nrf_gpio_pin_set(PIN_LED_B);
	}
#ifdef IRT_REV_2A_H
	if ((led_mask | 2) == led_mask)
	{
		nrf_gpio_pin_clear(PIN_LED_C);
		nrf_gpio_pin_set(PIN_LED_D);
	}
#endif
}

void set_led_green(uint8_t led_mask)
{
	nrf_gpio_pin_clear(PIN_LED_B);
	nrf_gpio_pin_set(PIN_LED_A);

#ifdef IRT_REV_2A_H
	nrf_gpio_pin_clear(PIN_LED_C);
	nrf_gpio_pin_set(PIN_LED_D);
#endif
}

void clear_led(uint8_t led_mask)
{
	if (led_mask <= 1)
	{
		nrf_gpio_pin_set(PIN_LED_A);
		nrf_gpio_pin_set(PIN_LED_B);
	}
#ifdef IRT_REV_2A_H
	if ((led_mask | 2) == led_mask)
	{
		nrf_gpio_pin_set(PIN_LED_C);
		nrf_gpio_pin_set(PIN_LED_D);
	}
#endif
}

void blink_led_green_start(uint8_t led_mask, uint16_t interval_ms)
{
	uint32_t err_code;
	uint32_t interval_ticks;

	// Determine # of ticks based on ms.
	interval_ticks = APP_TIMER_TICKS(interval_ms, APP_TIMER_PRESCALER);

	// Stop any current LED flash.
	clear_led(0);

	// Start the timer.
	err_code = app_timer_start(m_led1_blink_timer_id, interval_ticks, NULL);
	APP_ERROR_CHECK(err_code);
}

void blink_led_green_stop(uint8_t led_mask)
{
	uint32_t err_code;

	err_code = app_timer_stop(m_led1_blink_timer_id);
	APP_ERROR_CHECK(err_code);

	clear_led(0);
}

/*
void blink_led_red_start(uint8_t led_mask, uint16_t interval_ms)
{

}

void blink_led_red_stop(uint8_t led_mask)
{

}
*/

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

//
// Cuts power to servo and optical sensor and other peripherals.
// Optionally will put the accelerometer in standby and it will
// not wake the device.
//
void peripheral_powerdown(bool accelerometer_off)
{
	PH_LOG("[PH] Powering down peripherals.\r\n");

	// Disconnect all input buffers to save power on shutdown.
    NRF_GPIO->PIN_CNF[PIN_FLYWHEEL] = (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos);

#ifdef SIMPLE_UART
    // Disable UART and disconnect input buffers to save power.
    NRF_UART0->ENABLE = 0;
	NRF_GPIO->PIN_CNF[PIN_UART_RXD] = (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos);
	NRF_GPIO->PIN_CNF[PIN_UART_CTS] = (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos);
#endif // SIMPLE_UART

	if (accelerometer_off)
	{
		// Put accelerometer to sleep so we don't get awaken by it's AUTO-SLEEP/AUTO-WAKE interrupts.
		accelerometer_standby();
	}

#ifdef IRT_REV_2A_H
	// Disable servo / LED.
	nrf_gpio_pin_clear(PIN_EN_SERVO_PWR);

	#ifdef USE_BATTERY_CHARGER
	// Disable the power regulator if the board is configured for.
	nrf_gpio_pin_clear(PIN_SLEEP_N);

	// Check if AC power is plugged in.  If not, cut power to charger.
	if (ac_adapter_off())
	{
		battery_charge_set(false);
	}
	#endif // USE_BATTERY_CHARGER
#endif // IRT_REV_2A_H

	// Shut down the leds.
	clear_led(3);
}

void peripheral_init(peripheral_evt_t *p_on_peripheral_evt)
{
	uint32_t err_code;
	mp_on_peripheral_evt = p_on_peripheral_evt;
	
    irt_gpio_init();
    accelerometer_init();
	temperature_init();

	// Blink both LEDS
	set_led_green(3);
	nrf_delay_ms(10);
	clear_led(3);

	// Create the timer for blinking led #1.
	err_code = app_timer_create(&m_led1_blink_timer_id,
		APP_TIMER_MODE_REPEATED,
		blink_timeout_handler);
	APP_ERROR_CHECK(err_code);

	/*
	// Create the timer for blinking led #2.
	err_code = app_timer_create(&m_led2_blink_timer_id,
		APP_TIMER_MODE_REPEATED,
		blink_timeout_handler);
	APP_ERROR_CHECK(err_code); */

#if USE_BATTERY_CHARGER
	battery_init(PIN_ENBATT, PIN_CHG_EN_N, p_on_peripheral_evt->on_battery_result);
	PH_LOG("[PH] Initialized battery.\r\n");
#endif

}
