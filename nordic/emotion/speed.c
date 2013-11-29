/*
*******************************************************************************
* 
* By Jason De Lorme <jjdelorme@yahoo.com>
* http://www.roadacious.com
*
********************************************************************************/

#include "speed.h"
#include "nrf_sdm.h"
#include "app_error.h"
#include "app_util.h"

// Wheel diameter size in mm.  A road wheel is typically 2,070 mmm.
static uint16_t m_wheel_size;

/**@brief	Configure GPIO input from flywheel revolution pin and create an 
 *				event on achannel. 
 */
static void revs_init_gpiote(uint32_t pin_drum_rev)
{
	nrf_gpio_cfg_input(pin_drum_rev, NRF_GPIO_PIN_NOPULL);
	
	nrf_gpiote_event_config(REVS_CHANNEL_TASK_TOGGLE, 
													pin_drum_rev, 
													NRF_GPIOTE_POLARITY_HITOLO);
}

/**@brief		Enable PPI channel 3, combined with previous settings.
 *
 */
static void revs_init_ppi()
{
	uint32_t err_code; 
	
	// Using hardcoded channel 3.
	err_code = sd_ppi_channel_assign(3, 
																&NRF_GPIOTE->EVENTS_IN[REVS_CHANNEL_TASK_TOGGLE], 
																&REVS_TIMER->TASKS_COUNT);
	
	if (err_code == NRF_ERROR_SOC_PPI_INVALID_CHANNEL)
		APP_ERROR_HANDLER(NRF_ERROR_SOC_PPI_INVALID_CHANNEL);

	sd_ppi_channel_enable_set(PPI_CHEN_CH3_Enabled << PPI_CHEN_CH3_Pos);
}

/**@brief 	Function called when a specified number of flywheel revolutions occur.
 *
 */
static void REVS_IRQHandler()
{
	REVS_TIMER->EVENTS_COMPARE[0] = 0;	// This stops the IRQHandler from getting called indefinetly.
	/*
	uint32_t revs = 0;

	REVS_TIMER->TASKS_CAPTURE[0] = 1;
	revs = REVS_TIMER->CC[0]; */
}

/**@brief		Initializes the counter which tracks the # of flywheel revolutions.
 *
 */
static void revs_init_timer()
{
	REVS_TIMER->MODE				=	TIMER_MODE_MODE_Counter;
	REVS_TIMER->BITMODE   	= TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos;
	REVS_TIMER->TASKS_CLEAR = 1;
	
	/**
		Uncomment this code to set up a trigger that will call REVS_IRQHandler after a 
		certain number of revolutions.
	 **

	REVS_TIMER->CC[0] 			= REVS_TO_TRIGGER;

	// Clear the counter every time we hit the trigger value.
	REVS_TIMER->SHORTS 			= TIMER_SHORTS_COMPARE0_CLEAR_Msk;
	
	// Interrupt setup.
  REVS_TIMER->INTENSET 		= (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos);	
	*/
}
static uint16_t revs_get_count()
{
	uint16_t revs = 0;

	REVS_TIMER->TASKS_CAPTURE[0] = 1;
	revs = REVS_TIMER->CC[0]; 
	
	return revs;
}

float get_speed_kmh(uint16_t wheel_revolutions, uint16_t seconds_2048)
{
	if (wheel_revolutions == 0 || seconds_2048 == 0)
		return 0.0f;
	
	// Convert mm to meters, divide by revs, multiply by 1,000 for km.
	float km = ((m_wheel_size*1000) / wheel_revolutions)*1000;
	// Divide distance by time (multiple by  2048 to get seconds, then seconds in hour).
	float speed = km / (seconds_2048 * 2048 * 3600);
	
	return speed;
}

float get_speed_mph(uint16_t wheel_revolutions, uint16_t seconds_2048)
{
	// Convert km/h to mp/h.
	return get_speed_kmh(wheel_revolutions, seconds_2048) * 1.609344;
}

uint16_t get_wheel_revolutions()
{
	/*
		For example, assuming a 2.07m wheel circumference:
		 0.01 miles : 144 s_revs 
		 0.01 miles = 16.09344 meters
		 1 servo_rev = 0.11176 distance_meters
	*/
	uint16_t revs = revs_get_count();
	
	if (revs == 0)
		return 0;
	
	// TODO: Use m_wheel_size to more accurately calculate flywheel to wheel revs.
	uint16_t flywheel_to_wheel_revs = 19;
	return ROUNDED_DIV(revs, flywheel_to_wheel_revs);
}

void init_speed(uint32_t pin_drum_rev, uint16_t wheel_size_mm)
{
	m_wheel_size = wheel_size_mm;
	
	revs_init_gpiote(pin_drum_rev);
	revs_init_ppi();
	revs_init_timer();
	
	// Enable interrupt handler which will internally map to REVS_IRQHandler.
	NVIC_EnableIRQ(REVS_IRQn);
	//__enable_irq();	// <-- TODO: not sure what this call does, leaving it commented.
	
	// Start the counter.
	REVS_TIMER->TASKS_START = 1;
}
