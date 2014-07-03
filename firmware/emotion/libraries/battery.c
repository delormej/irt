/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 */

#include <stdint.h>
#include <string.h>
#include "battery.h"
#include "irt_peripheral.h"
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf51_bitfields.h"
#include "softdevice_handler.h"
#include "app_util.h"
#include "debug.h"

#define ADC_REF_VOLTAGE_IN_MILLIVOLTS        1200                                      /**< Reference voltage (in milli volts) used by ADC while doing conversion. */
#define ADC_PRE_SCALING_COMPENSATION         3                                         /**< The ADC is configured to use VDD with 1/3 prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.*/
#define AIN_BATT_VOLT						 ADC_CONFIG_PSEL_AnalogInput2
#define DIODE_FWD_VOLT_DROP_MILLIVOLTS       270                                       /**< Typical forward voltage drop of the diode (Part no: SD103ATW-7-F) that is connected in series with the voltage supply. This is the voltage drop when the forward current is 1mA. Source: Data sheet of 'SURFACE MOUNT SCHOTTKY BARRIER DIODE ARRAY' available at www.diodes.com. */

#ifdef ENABLE_DEBUG_LOG
#define BY_LOG debug_log
#else
#define BY_LOG(...)
#endif // ENABLE_DEBUG_LOG


/**@brief Macro to convert the result of ADC conversion in millivolts.
 *
 * @param[in]  ADC_VALUE   ADC result.
 * @retval     Result converted to millivolts.
 */
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
        ((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / 255) * ADC_PRE_SCALING_COMPENSATION)


static on_battery_result_t m_on_battery_result;
static uint8_t m_pin_battery_read;

/**@brief Function for handling the ADC interrupt.
 * @details  This function will fetch the conversion result from the ADC, convert the value into
 *           percentage and send it to peer.
 */
void ADC_IRQHandler(void)
{
    if (NRF_ADC->EVENTS_END != 0)
    {
        uint8_t     adc_result;
        uint16_t    batt_lvl_in_milli_volts;
        uint8_t     percentage_batt_lvl;
        uint32_t    err_code;

        NRF_ADC->EVENTS_END     = 0;
        adc_result              = NRF_ADC->RESULT;
        NRF_ADC->TASKS_STOP     = 1;

        batt_lvl_in_milli_volts = ADC_RESULT_IN_MILLI_VOLTS(adc_result) +
                                  DIODE_FWD_VOLT_DROP_MILLIVOLTS;
        percentage_batt_lvl     = battery_level_in_percent(batt_lvl_in_milli_volts);

        BY_LOG("[BY] Battery result %i, millivolts: %i, percent: %i\r\n", adc_result, batt_lvl_in_milli_volts, percentage_batt_lvl);

		if (m_on_battery_result != NULL)
		{
			m_on_battery_result(adc_result); // batt_lvl_in_milli_volts);
		}
    }
}

// Starts a battery read operation.  on_battery_result() will be called with result.
void battery_read_start()
{
    uint32_t err_code;

    // Enable reading voltage.
	nrf_gpio_pin_set(m_pin_battery_read);

    // Configure ADC
    NRF_ADC->INTENSET   = ADC_INTENSET_END_Msk;
    NRF_ADC->CONFIG     = (ADC_CONFIG_RES_8bit                        		<< ADC_CONFIG_RES_Pos)     |
                          (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling	<< ADC_CONFIG_INPSEL_Pos)  |
                          (ADC_CONFIG_REFSEL_VBG							<< ADC_CONFIG_REFSEL_Pos)  |
						  (AIN_BATT_VOLT							  		<< ADC_CONFIG_PSEL_Pos) |
                          (ADC_CONFIG_EXTREFSEL_None                  		<< ADC_CONFIG_EXTREFSEL_Pos);
    NRF_ADC->EVENTS_END = 0;
    NRF_ADC->ENABLE     = ADC_ENABLE_ENABLE_Enabled;

    // Enable ADC interrupt
    err_code = sd_nvic_ClearPendingIRQ(ADC_IRQn);
    APP_ERROR_CHECK(err_code);

    err_code = sd_nvic_SetPriority(ADC_IRQn, NRF_APP_PRIORITY_LOW);
    APP_ERROR_CHECK(err_code);

    err_code = sd_nvic_EnableIRQ(ADC_IRQn);
    APP_ERROR_CHECK(err_code);

    NRF_ADC->EVENTS_END  = 0;    // Stop any running conversions.
    NRF_ADC->TASKS_START = 1;

	nrf_gpio_pin_clear(m_pin_battery_read); // Stop reading voltage.
}

// Ensures the ADC is enabled.
// TODO: Should this happen only when we initiate a read? Sample seems to do this once and only once in main.
void battery_init(on_battery_result_t on_battery_result, uint8_t pin_battery_read)
{
	// Save callback.
	m_on_battery_result = on_battery_result;
	m_pin_battery_read = pin_battery_read;

	// Enable the ADC.
	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled << ADC_ENABLE_ENABLE_Pos;
}

/**
 * @}
 */
