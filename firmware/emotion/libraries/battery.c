/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "battery.h"
#include "irt_peripheral.h"
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf51_bitfields.h"
#include "softdevice_handler.h"
#include "app_util.h"
#include "nrf_delay.h"
#include "debug.h"

#define ADC_REF_VOLTAGE_IN_MILLIVOLTS        1200                           /**< Reference voltage (in milli volts) used by ADC while doing conversion. */
#define ADC_PRE_SCALING_COMPENSATION         3                              /**< The ADC is configured to use VDD with 1/3 prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.*/
#define AIN_BATT_VOLT						 ADC_CONFIG_PSEL_AnalogInput3
#define DIODE_FWD_VOLT_DROP_MILLIVOLTS       270                            /**< Typical forward voltage drop of the diode (Part no: SD103ATW-7-F) that is connected in series with the voltage supply. This is the voltage drop when the forward current is 1mA. Source: Data sheet of 'SURFACE MOUNT SCHOTTKY BARRIER DIODE ARRAY' available at www.diodes.com. */

#define CAPACITOR_CHARGE_TIME				5ul								// Wait 5ms for the capacitor to charge before reading voltage.

#ifdef ENABLE_DEBUG_LOG
#define BY_LOG debug_log
#else
#define BY_LOG(...)
#endif // ENABLE_DEBUG_LOG


/**@brief Macro to convert the result of ADC conversion in millivolts.
 *
 * @param[in]	ADC_VALUE   ADC result.
 * @retval     	Result converted to millivolts.
 * @notes		The current resistor values will multiply the battery voltage by 0.4
 * 				giving a voltage of 3.36 volts for a fully charged 8.4 volt battery.
 * 				Dividing by 1/3 (pre-scalar) should yield 1120 millivolts for a fully
 * 				charged battery which is about 93% of the 1.2 volt band gap reference.
 * 				100% charged (8.4v) should result in 238 returned from ADC.
 */
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
        (((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / 255) * \
        		ADC_PRE_SCALING_COMPENSATION) / 0.4)

static on_battery_result_t m_on_battery_result;		// Callback to invoke after reading voltage.
static uint8_t m_pin_battery_read;					// Separate pin used for enabling voltage read on Analog Input 2.
static uint8_t m_pin_charge_stop;					// Pin used to tell the charger to stop.

/**@brief Function for handling the ADC interrupt.
 * @details  This function will fetch the conversion result from the ADC, convert the value into
 *           percentage and send it to peer.
 */
void ADC_IRQHandler(void)
{
    if (NRF_ADC->EVENTS_END != 0)
    {
        uint32_t	adc_result;
        uint16_t    batt_lvl_in_milli_volts;
        //uint8_t     percentage_batt_lvl;

        NRF_ADC->EVENTS_END     = 0;
        adc_result              = NRF_ADC->RESULT;
        NRF_ADC->TASKS_STOP     = 1;

        NRF_ADC->ENABLE     	= ADC_ENABLE_ENABLE_Disabled;

#ifdef USE_BATTERY_READ_PIN
        // Stop reading voltage.
        nrf_gpio_pin_clear(m_pin_battery_read);
#endif // USE_BATTERY_READ_PIN

        batt_lvl_in_milli_volts = ADC_RESULT_IN_MILLI_VOLTS(adc_result);
        //percentage_batt_lvl     = battery_level_in_percent(batt_lvl_in_milli_volts);

        BY_LOG("[BY] Battery result: ADC %i, %imV \r\n", adc_result, batt_lvl_in_milli_volts);

		if (m_on_battery_result != NULL)
		{
			m_on_battery_result(batt_lvl_in_milli_volts);
		}
    }
}

/**@brief	Starts a battery read operation using the Analog to Digital Converter.
 * @note	on_battery_result() will be called with a result.
 */
void battery_read_start()
{
    uint32_t err_code;

    // Stop any running conversions.
    NRF_ADC->EVENTS_END  = 0;

#ifdef USE_BATTERY_READ_PIN
    // Enable reading voltage.
	nrf_gpio_pin_set(m_pin_battery_read);

	// Wait ~3-10ms after setting the pin to allow the capacitor to charge.
	nrf_delay_ms(CAPACITOR_CHARGE_TIME);
#endif // USE_BATTERY_READ_PIN

    // Enable ADC interrupt
    err_code = sd_nvic_ClearPendingIRQ(ADC_IRQn);
    APP_ERROR_CHECK(err_code);

    err_code = sd_nvic_SetPriority(ADC_IRQn, NRF_APP_PRIORITY_LOW);
    APP_ERROR_CHECK(err_code);

    err_code = sd_nvic_EnableIRQ(ADC_IRQn);
    APP_ERROR_CHECK(err_code);

    // Enable the ADC and start it.
    NRF_ADC->ENABLE     = ADC_ENABLE_ENABLE_Enabled;
    NRF_ADC->TASKS_START = 1;

    BY_LOG("[BY] Battery Charger status: %i \r\n", battery_charge_status());
}

/**@brief	Converts millivolt reading into status structure.
 */
irt_battery_status_t battery_status(uint16_t millivolts, uint32_t operating_time)
{
	irt_battery_status_t status;

	float volts = millivolts / 1000.0f;

	status.coarse_volt = (uint8_t)volts;
	status.fractional_volt = (uint8_t)((volts - status.coarse_volt) * 255);
	status.operating_time = operating_time;
	status.resolution = 0; // 0 = 16 second resolution.  1 = 2 second resolution.

	// TODO: implement actual rating of battery level more scientifically for
	// lithium ion battery.
	// TODO: also implement operating time of battery.

	if (millivolts > 7900)
	{
		status.status = BAT_NEW;		// <20% discharge
	}
	else if (millivolts > 7600)
	{
		status.status = BAT_GOOD;		// <40% discharge
	}
	else if (millivolts > 7300)
	{
		status.status = BAT_OK;			// <80% discharge
	}
	else if (millivolts > 6800)
	{
		status.status = BAT_LOW;
	}
	else
	{
		status.status = BAT_CRITICAL;	// start to shut things down!
	}

	return status;
}

/**@brief	Reads the current battery charge status.
 * @returns	BATTERY_CHARGE_COMPLETE
 * 			BATTERY_CHARGING
 * 			BATTERY_CHARGE_FAULT
 */
irt_charger_status_t battery_charge_status()
{
	irt_charger_status_t status;

    if (FEATURE_AVAILABLE(FEATURE_BATTERY_CHARGER))
    {
    	// 1 == "OFF",  0 == "ON"
		status = nrf_gpio_pin_read(PIN_STAT1);			// 0 = charge-in progress, 1 = complete OR other status combined with stat2
		status |= (nrf_gpio_pin_read(PIN_STAT2) << 1);	// 0 = charge complete, 1 = another status when combined with stat1
    }
    else
    {
    	status = BATTERY_CHARGE_NONE;
    }

    return status;
}

/**@brief	Starts or stops the battery charge process.
 */
void battery_charge_set(bool enable)
{
    if (FEATURE_AVAILABLE(FEATURE_BATTERY_CHARGER))
    {
		if (enable)
		{
			// Clear the pin to enable charger.
			nrf_gpio_pin_clear(m_pin_charge_stop);
		}
		else
		{
			// Set the pin to stop charger.
			nrf_gpio_pin_set(m_pin_charge_stop);
		}

		BY_LOG("[BY] Toggled charge, new status is: %i. \r\n", battery_charge_status());
    }
}

/**@brief	Initializes the battery module with a callback when a read is complete and
 * 			the pin used to enable reading (not the analog pin used for actual reading).
 */
void battery_init(uint8_t pin_battery_enable, uint8_t pin_charge_stop, on_battery_result_t on_battery_result)
{
	m_on_battery_result = on_battery_result;
	// TODO: This pin goes away, except for 1 test board.  We can remove in future build or ifdef.
	m_pin_battery_read = pin_battery_enable;

	m_pin_charge_stop = pin_charge_stop;

    // Configure ADC
    NRF_ADC->INTENSET   = ADC_INTENSET_END_Msk;
    NRF_ADC->CONFIG     = (ADC_CONFIG_RES_8bit                        		<< ADC_CONFIG_RES_Pos)     |
                          (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling	<< ADC_CONFIG_INPSEL_Pos)  |
                          (ADC_CONFIG_REFSEL_VBG							<< ADC_CONFIG_REFSEL_Pos)  |
						  (AIN_BATT_VOLT							  		<< ADC_CONFIG_PSEL_Pos) |
                          (ADC_CONFIG_EXTREFSEL_None                  		<< ADC_CONFIG_EXTREFSEL_Pos);
}

