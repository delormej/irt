/*
 * irt_adc.c
 *
 *  Created on: Sep 9, 2014
 *      Author: jasondel
 */

#include <stdint.h>
#include "nrf51.h"
#include "nrf_soc.h"
#include "app_error.h"
#include "nrf_error.h"
#include "nrf_gpio.h"
#include "nrf51_bitfields.h"
#include "irt_adc.h"
#include "debug.h"

/**@brief Debug logging for resistance control module.
 */
#ifdef ENABLE_DEBUG_LOG
#define ADC_LOG debug_log
#else
#define ADC_LOG(...)
#endif // ENABLE_DEBUG_LOG

static adc_result_t m_on_adc_result = NULL;

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

        //ADC_LOG("[ADC] received result: %i\r\n", adc_result);

		if (m_on_adc_result != NULL)
		{
			//ADC_LOG("[ADC] calling handler.\r\n");
			m_on_adc_result(adc_result);
		}
    }
}

/**@brief	Initiate a read of the ADC.
 *
 */
uint32_t irt_adc_start(void)
{
	//ADC_LOG("[ADC] irt_adc_start called.\r\n");

	// Stop any running conversions.
	NRF_ADC->EVENTS_END = 0;

	// Enable the ADC and start it.
	NRF_ADC->ENABLE     = ADC_ENABLE_ENABLE_Enabled;
	NRF_ADC->TASKS_START = 1;

	return NRF_SUCCESS;
}

/**@brief	Initialize and configure with ADC configuration.
 *
 */
void irt_adc_init(uint32_t nrf_adc_config, adc_result_t on_adc_result)
{
	uint32_t err_code;

	ADC_LOG("[ADC] irt_adc_init called.\r\n");

	m_on_adc_result = on_adc_result;
	// Configure ADC
	NRF_ADC->INTENSET = ADC_INTENSET_END_Msk;
	NRF_ADC->CONFIG = nrf_adc_config;

	// Enable ADC interrupt
	err_code = sd_nvic_ClearPendingIRQ(ADC_IRQn);
	APP_ERROR_CHECK(err_code);

	err_code = sd_nvic_SetPriority(ADC_IRQn, NRF_APP_PRIORITY_LOW);
	APP_ERROR_CHECK(err_code);

	err_code = sd_nvic_EnableIRQ(ADC_IRQn);
	APP_ERROR_CHECK(err_code);

}
