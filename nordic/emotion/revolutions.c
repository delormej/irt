#include "revolutions.h"
#include "nrf_sdm.h"
#include "app_error.h"


void revs_init_gpiote(uint32_t pin_drum_rev)
{
	// Configure GPIO input from drum revolution pin and create an event on channel. 
	nrf_gpio_cfg_input(pin_drum_rev, NRF_GPIO_PIN_NOPULL);
	
	nrf_gpiote_event_config(REVS_CHANNEL_TASK_TOGGLE, 
													pin_drum_rev, 
													NRF_GPIOTE_POLARITY_HITOLO);
}

void revs_init_ppi()
{
	uint32_t err_code; 
	
	// Using hardcoded channel 3.
	err_code = sd_ppi_channel_assign(3, 
																	&NRF_GPIOTE->EVENTS_IN[REVS_CHANNEL_TASK_TOGGLE], 
																	&REVS_TIMER->TASKS_COUNT);
	
	if (err_code == NRF_ERROR_SOC_PPI_INVALID_CHANNEL)
		APP_ERROR_HANDLER(NRF_ERROR_SOC_PPI_INVALID_CHANNEL);


	// Enable PPI channel 3, combined with previous settings.
	sd_ppi_channel_enable_set(PPI_CHEN_CH3_Enabled << PPI_CHEN_CH3_Pos);
}

void revs_init_timer()
{
	REVS_TIMER->MODE				=	TIMER_MODE_MODE_Counter;
	REVS_TIMER->BITMODE   	= TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos;
	REVS_TIMER->TASKS_CLEAR = 1;
	
	REVS_TIMER->CC[0] 			= REVS_TO_TRIGGER;

	// Clear the counter every time we hit the trigger value.
	REVS_TIMER->SHORTS 			= TIMER_SHORTS_COMPARE0_CLEAR_Msk;
	
	// Interrupt setup.
  REVS_TIMER->INTENSET 		= (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos);	
}

void init_revolutions(uint32_t pin_drum_rev)
{
	revs_init_gpiote(pin_drum_rev);
	revs_init_ppi();
	revs_init_timer();
	
	// Enable interrupt handler which will internally map to REVS_IRQHandler.
	NVIC_EnableIRQ(REVS_IRQn);
	//__enable_irq();
	
	// Start the counter.
	REVS_TIMER->TASKS_START = 1;
}
