/*
 * irt_adc.c
 *
 *  Created on: Sep 9, 2014
 *      Author: jasondel
 */

#include <stdint.h>
#include "nrf51.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "nrf51_bitfields.h"
#include "irt_adc.h"
#include "debug.h"

static on_adc_result_t m_on_adc_result;

/**@brief Function for handling the ADC interrupt.
 * @details  This function will fetch the conversion result from the ADC, convert the value into
 *           percentage and send it to peer.
 */
void ADC_IRQHandler(void)
{
    if (NRF_ADC->EVENTS_END != 0)
    {
        uint32_t	adc_result;

        NRF_ADC->EVENTS_END     = 0;
        adc_result              = NRF_ADC->RESULT;
        NRF_ADC->TASKS_STOP     = 1;

        NRF_ADC->ENABLE     	= ADC_ENABLE_ENABLE_Disabled;

		if (m_on_adc_result != NULL)
		{
			m_on_adc_result(adc_result);
		}
    }
}

/**@brief	Initiate a read of the ADC.
 *
 */
void irt_adc_start(void)
{
	uint32_t err_code;

	// Stop any running conversions.
	NRF_ADC->EVENTS_END = 0;

	// Enable the ADC and start it.
	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;
	NRF_ADC->TASKS_START = 1;
}

/**@brief	Initialize and configure with ADC configuration.
 *
 */
void irt_adc_init(uint32_t nrf_adc_config, on_adc_result_t on_adc_result)
{
	m_on_adc_result = on_adc_result;
	// Configure ADC
	NRF_ADC->INTENSET = ADC_INTENSET_END_Msk;
	NRF_ADC->CONFIG = nrf_adc_config;
}
