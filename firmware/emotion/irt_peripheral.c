//
// This module wraps all peripheral interaction by the device.
//

#include <stdbool.h>
#include <stdint.h>
#include "irt_peripheral.h"
#include "nordic_common.h"
#include "app_util.h"
#include "app_gpiote.h"
#include "app_timer.h"
#include "irt_button.h"
#include "nrf_gpio.h"
#include "accelerometer.h"
#include "temperature.h"
#include "boards.h"
#include "debug.h"
#include "battery.h"
#include "irt_led.h"
#include "app_scheduler.h"

/**@brief Debug logging for main module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define PH_LOG debug_log
#else
#define PH_LOG(...)
#endif // ENABLE_DEBUG_LOG


static peripheral_evt_t 				*mp_on_peripheral_evt;
static app_gpiote_user_id_t 			mp_user_id;

/**@brief	Callback used by the app scheduler to move processing of accelerometer
 * 			events off of the interrupt handler.
 */
static void accelerometer_handler(void)
{
	mp_on_peripheral_evt->on_accelerometer_evt();
}

/**@brief Function for handling interrupt events.
 *
 * @param[in]   pin_no   The pin number of the button pressed.
 */
static void interrupt_handler(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low)
{
	uint32_t err_code;

	//event_pins_low_to_high
	if (event_pins_high_to_low & (1 << PIN_SHAKE))
	{
        // Move processing off the interrupt handler.
        err_code = app_sched_event_put(NULL, 0, accelerometer_handler);
        APP_ERROR_CHECK(err_code);
	}
#ifdef IRT_REV_2A_H
	else if (event_pins_low_to_high & (1 << PIN_PG_N))
	{
		// Detects when the power adapter is unplugged.
		mp_on_peripheral_evt->on_power_plug(false);

		// If we were charging, we stopped.  Check and report status.
		irt_charger_status_t status = battery_charge_status();
		mp_on_peripheral_evt->on_charge_status(status);
	}
	else if (event_pins_high_to_low & (1 << PIN_PG_N))
	{
		// Detects when the power adapter is plugged in.
		mp_on_peripheral_evt->on_power_plug(true);
	}
	else if ( event_pins_high_to_low & (1 << PIN_STAT1) ||
			event_pins_high_to_low & (1 << PIN_STAT2) )
	{
		irt_charger_status_t status = battery_charge_status();
		mp_on_peripheral_evt->on_charge_status(status);
	}
#endif
}

/**@brief Initialize all peripherial pins.
 */
static void irt_gpio_init()
{
	uint32_t err_code;
	uint32_t pins_low_to_high_mask, pins_high_to_low_mask;

	if (HW_REVISION >= 2)	// IRT_V2_REV_A
	{
		// Enable servo / optical sensor.
		nrf_gpio_cfg_output(PIN_EN_SERVO_PWR);
		nrf_gpio_pin_set(PIN_EN_SERVO_PWR);

		if (FEATURE_AVAILABLE(FEATURE_BATTERY_CHARGER))
		{
			PH_LOG("[PH] irt_gpio_init enabling charger and power regulator.\r\n");

			// Configure pin for enabling or disabling battery charger. 0=on, 1=off
			nrf_gpio_cfg_output(PIN_CHG_EN_N);
			nrf_gpio_pin_clear(PIN_CHG_EN_N);

			// Enable the power regulator if the board is built with power regulator.
			nrf_gpio_cfg_output(PIN_SLEEP_N);
			nrf_gpio_pin_set(PIN_SLEEP_N);

			// Configure pin to read if there is a power adapter.
			nrf_gpio_cfg_input(PIN_PG_N, NRF_GPIO_PIN_NOPULL);

			// Configure pins for battery charger status.
			nrf_gpio_cfg_input(PIN_STAT1, NRF_GPIO_PIN_NOPULL);
			nrf_gpio_cfg_input(PIN_STAT2, NRF_GPIO_PIN_NOPULL);
		}

		#ifdef PIN_ENBATT
		if (FEATURE_AVAILABLE(FEATURE_BATTERY_READ_PIN))
		{
			// When set enables the device to read battery voltage (on specific board builds - not all).
			nrf_gpio_cfg_output(PIN_ENBATT);
		}
		#endif

		// TODO: I don't think we need to configure Analog input pins, the ADC should just take control.  Verify, then remove.
		// Configure input for the battery AIN.
		// This is available whether or not the board is built with battery charge support.
		NRF_GPIO->PIN_CNF[PIN_BATT_VOLT] = (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos);
	}

	if (HW_REVISION >= 3)	// IRT_V2_REV_A1
	{
		nrf_gpio_cfg_output(PIN_3VPWR_DIS);
		nrf_gpio_cfg_input(PIN_AC_PWR, NRF_GPIO_PIN_NOPULL);
	}

	// Initialize the pin to wake the device on movement from the accelerometer.
	nrf_gpio_cfg_input(PIN_SHAKE, NRF_GPIO_PIN_NOPULL);

	//
	// Setup pin masks for low to high and high to low
	//
	pins_low_to_high_mask = 0;
	pins_high_to_low_mask = 0;

	pins_low_to_high_mask = 1 << PIN_PG_N;			// On power adapter unplugged.

	if (FEATURE_AVAILABLE(FEATURE_BATTERY_CHARGER))
	{
		PH_LOG("[PH] irt_gpio_init: configuring charger status.\r\n");
		pins_high_to_low_mask |= 1 << PIN_STAT1;	// Charge status 1. Indicates charge in process.
		pins_high_to_low_mask |= 1 << PIN_STAT2;	// Charge status 2. Indicates charge complete.
	}

	pins_high_to_low_mask |= ( 1 << PIN_SHAKE
#ifdef IRT_REV_2A_H
			//| 1 << PIN_PBSW			// On user push button switch
			| 1 << PIN_PG_N			// On power adapter plugged in
#endif
			);

	APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);

	err_code = app_gpiote_user_register(&mp_user_id,
		pins_low_to_high_mask,
		pins_high_to_low_mask,
		interrupt_handler);
	APP_ERROR_CHECK(err_code);

	err_code = app_gpiote_user_enable(mp_user_id);
	APP_ERROR_CHECK(err_code);
}    


/**@brief	Turn J7-6 power off/on.
 */
void peripheral_aux_pwr_set(bool disable)
{
	if (HW_REVISION >= 3) // IRT_V2_REV_A1
	{
		if (disable)
		{
			nrf_gpio_pin_set(PIN_3VPWR_DIS);
		}
		else
		{
			nrf_gpio_pin_clear(PIN_3VPWR_DIS);
		}
	}
}

/**@brief	Cuts power to all non-critical components.
 *
 */
void peripheral_low_power_set()
{
	PH_LOG("[PH] peripheral_low_power_set cutting power to all non-critical components.\r\n");

    // Shutdown temperature sensor.
	temperature_shutdown();

	if (HW_REVISION >= 2) // IRT_REV_2A_H
	{
		// Disable servo / Optical sensor.
		nrf_gpio_pin_clear(PIN_EN_SERVO_PWR);

		if (FEATURE_AVAILABLE(FEATURE_BATTERY_CHARGER))
		{
			// Disable the power regulator if the board is configured for.
			nrf_gpio_pin_clear(PIN_SLEEP_N);

			// Check if AC power is plugged in.  If not, cut power to charger.
			if (!peripheral_plugged_in())
			{
				battery_charge_set(false);
			}
		}
	} // IRT_REV_2A_H

	peripheral_aux_pwr_set(true);
}


/**@brief Returns true if the ac adapter is plugged in, otherwise false.
 *
 */
bool peripheral_plugged_in()
{
#ifdef PIN_PG_N
	// Pin will read 0 when plugged in, 1 when not.  Flip the bit for boolean response.
	return !(nrf_gpio_pin_read(PIN_PG_N));
#else // PIN_PG_N
	return false;
#endif // PIN_PG_N
}

//
// Cuts power to servo and optical sensor and other peripherals.
// Optionally will put the accelerometer in standby and it will
// not wake the device.
//
void peripheral_powerdown(bool accelerometer_off)
{
	PH_LOG("[PH] Powering down peripherals.\r\n");

	if (accelerometer_off)
	{
		// Put accelerometer to sleep by reseting AUTO-WAKE interrupts and putting the device in standby.
		accelerometer_reset();

		PH_LOG("[PH] Disabled accelerometer AUTO-WAKE interrupt.\r\n");
	}

	peripheral_low_power_set();

	// Shut down the leds.
	led_set(LED_POWER_OFF);
}

/**@brief	Sets the pins to wake the device from sleep (button push and accelerometer).
 *
 */
void peripheral_wakeup_set()
{
	// Accelerometer interrupt signal
	NRF_GPIO->PIN_CNF[PIN_SHAKE] = (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
            | (NRF_GPIO_PIN_NOPULL << GPIO_PIN_CNF_PULL_Pos)
            | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
            | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
	// SENSE transition from high to LOW.  Configure the pin sense separately per PANv2.1 #8
	NRF_GPIO->PIN_CNF[PIN_SHAKE] |= (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);

	if (HW_REVISION >= 2) // IRT_REV_2A_H
	{
		// Push button switch
		NRF_GPIO->PIN_CNF[PIN_PBSW] = (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
	            | (NRF_GPIO_PIN_NOPULL << GPIO_PIN_CNF_PULL_Pos)
	            | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
	            | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
		// SENSE transition from high to LOW.  Configure the pin sense separately per PANv2.1 #8
		NRF_GPIO->PIN_CNF[PIN_PBSW] |= (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);

		/* Indicates device was plugged in.
		 *
		 * This is commented out because it causes an immediate wake up when plugged-in, not sure
		 * why, but we really don't need because accelerometer will wake it anyways in most cases.
		 *
		NRF_GPIO->PIN_CNF[PIN_PG_N] = (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
				| (NRF_GPIO_PIN_NOPULL << GPIO_PIN_CNF_PULL_Pos)
				| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
				| (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
		// SENSE transition from high to LOW.  Configure the pin sense separately per PANv2.1 #8
		NRF_GPIO->PIN_CNF[PIN_PG_N] |= (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos); */

		if (FEATURE_AVAILABLE(FEATURE_BATTERY_CHARGER))
		{
			// Charge status 2. Indicates charge complete.
			NRF_GPIO->PIN_CNF[PIN_STAT2] = (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
					| (NRF_GPIO_PIN_NOPULL << GPIO_PIN_CNF_PULL_Pos)
					| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
					| (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
			// Configure the pin sense separately per PANv2.1 #8
			NRF_GPIO->PIN_CNF[PIN_STAT2] |= (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
		}
	}
}

/**@brief 	Returns the count of 1/2048th seconds (2048 per second) since the
 *			the counter started.
 *
 * @note	This value rolls over at 32 seconds.
 */
uint32_t timestamp_get()
{
	// Get current tick count.
	uint32_t ticks = NRF_RTC1->COUNTER;

	// Based on frequence of ticks, calculate 1/2048 seconds.
	// freq (hz) = times per second.
	uint32_t seconds_2048 = ROUNDED_DIV(ticks, (TICK_FREQUENCY / 2048));

	return seconds_2048;
}

void peripheral_init(peripheral_evt_t *p_on_peripheral_evt)
{
	uint32_t err;
	mp_on_peripheral_evt = p_on_peripheral_evt;
	
	led_init();

	// Turn on the power light.
	led_set(LED_POWER_ON);

    irt_gpio_init();
    err = irt_button_init(mp_on_peripheral_evt->on_button_pbsw);
	APP_ERROR_CHECK(err);
    accelerometer_init();
	temperature_init();

	PH_LOG("[PH] peripheral_init: power plugged in? %i\r\n", peripheral_plugged_in());

	// Ensure aux power is off for right now, we're not using.
	peripheral_aux_pwr_set(true);

#ifndef PIN_ENBATT
#define PIN_ENBATT -1
#endif

	if (FEATURE_AVAILABLE(FEATURE_BATTERY_CHARGER))
	{
		battery_init(PIN_ENBATT, PIN_CHG_EN_N, p_on_peripheral_evt->on_battery_result);

		irt_charger_status_t status = battery_charge_status();
		mp_on_peripheral_evt->on_charge_status(status);
	}
	else
	{
		battery_init(PIN_ENBATT, -1, p_on_peripheral_evt->on_battery_result);
	}

	PH_LOG("[PH] Initialized battery.\r\n");
}
