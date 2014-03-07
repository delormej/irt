/*
*******************************************************************************
* 
* (c) Jason De Lorme <jason@insideridetech.com>
* Inside Ride Technologies, LLC
*
********************************************************************************/

#include <string.h>
#include <math.h>
#include "irt_common.h"
#include "speed.h"
#include "nrf_sdm.h"
#include "app_error.h"
#include "app_util.h"

// RTC1 is based on a 32.768khz crystal, or in other words it oscillates
// 32768 times per second.  The PRESCALER determins how often a tick gets
// counted.  With a prescaler of 0, there are 32,768 ticks in 1 second
// 1/2048th of a second would be 16 ticks (32768/2048)
// # of 2048th's would then be ticks / 16.
#define	TICK_FREQUENCY	(32768 / (NRF_RTC1->PRESCALER + 1))

static uint16_t 	m_wheel_size;									// Wheel diameter size in mm.
static float 			m_flywheel_to_wheel_revs;			// Number of flywheel revolutions for 1 wheel revolution.

/**@brief	Configure GPIO input from flywheel revolution pin and create an 
 *				event on achannel. 
 */
static void revs_init_gpiote(uint32_t pin_flywheel_rev)
{
	nrf_gpio_cfg_input(pin_flywheel_rev, NRF_GPIO_PIN_NOPULL);
	
	nrf_gpiote_event_config(REVS_CHANNEL_TASK_TOGGLE, 
													pin_flywheel_rev, 
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

/**@brief 	Returns the accumulated count of flywheel revolutions since the
 *					counter started.
 *					
 */
static uint32_t get_flywheel_revs()
{
	uint32_t revs = 0;

	REVS_TIMER->TASKS_CAPTURE[0] = 1;
	revs = REVS_TIMER->CC[0]; 
	
	return revs;
}

/**@brief 	Returns the count of 1/2048th seconds (2048 per second) since the 
 *					the counter started.  
 *
 * @note		This value rolls over at 32 seconds.
 */
static uint16_t get_seconds_2048()
{
	// Get current tick count.
	uint32_t ticks = NRF_RTC1->COUNTER;

	// Based on frequence of ticks, calculate 1/2048 seconds.
	// freq (hz) = times per second.
	uint16_t seconds_2048 = ROUNDED_DIV(ticks, (TICK_FREQUENCY / 2048));

	return seconds_2048;
}

float get_speed_mps(float wheel_revolutions, uint16_t period_seconds_2048)
{
	if (wheel_revolutions == 0 || period_seconds_2048 == 0)
		return 0.0f;
	
	// Convert mm to meters, muliply by revs, multiply by 1,000 for km.
	float distance_m = (((float)m_wheel_size) / 1000.0f) * wheel_revolutions;
	// Get speed in meters per second.
	float mps = distance_m / (period_seconds_2048 / 2048);

	return mps;
}

/**@brief		Calculates how long it would have taken since the last complete 
 *					wheel revolution given current speed (in meters per second).  
 *					Returns a value in 1/2048's of a second.
 *
 */
static uint16_t fractional_wheel_rev_time_2048(float speed_mps, float accum_wheel_revs)
{
	uint16_t time_to_full_rev_2048 = 0;
	
	if (speed_mps > 0)
	{
		// Get the speed in meters per 1/2048s.
		float speed_2048 = speed_mps / 2048;
	
		// A single wheel rev at this speed would take this many 1/2048's of a second.	
		float single_wheel_rev_time = (m_wheel_size / 1000) / speed_2048;

		// Difference between calculated partial wheel revs and the last full wheel rev.
		float partial_wheel_rev = fmod(accum_wheel_revs, 1);

		// How long ago in 1/2048's of a second would the last full wheel rev have occurred?
		time_to_full_rev_2048 = round(partial_wheel_rev * single_wheel_rev_time);
	}
	
	return time_to_full_rev_2048;
}


/*****************************************************************************
*
* Public functions, see function descriptions in header.
*
*****************************************************************************/

float get_speed_kmh(float speed_mps)
{
	return speed_mps * 3.6;
}

float get_speed_mph(float speed_mps)
{
	// Convert km/h to mp/h.
	return get_speed_kmh(speed_mps) * 0.621371;
}

void calc_speed(speed_event_t* speed_event)
{
	static uint32_t last_accum_flywheel_revs 	= 0;
	static uint32_t last_accum_wheel_revs 		= 0;
	static uint16_t last_event_time_2048 			= 0;
	
	uint16_t time_delta;

	// Current time stamp.
	uint16_t current_2048 = get_seconds_2048();
		
	// Flywheel revolution count.
	uint32_t flywheel_revs = get_flywheel_revs();
	
	if (flywheel_revs > last_accum_flywheel_revs)
	{
		// Calculate accumlated fractional wheel revolutions.
		float fractional_wheel_revs = flywheel_revs / m_flywheel_to_wheel_revs;
		
		// Handle rollover situations.
		if (current_2048 < last_event_time_2048)
			time_delta = (last_event_time_2048 ^ 0xFFFF) + current_2048;
		else
			time_delta = current_2048 - last_event_time_2048;

		// Calculate the current speed in meters per second.
		speed_event->speed_mps = get_speed_mps(
												fractional_wheel_revs - last_accum_wheel_revs, 							// # of wheel revs in the period
												time_delta);								// Current time period in 1/2048 seconds.
		
		// Determine time since a full wheel rev in 1/2048's of a second at this speed.
		uint16_t time_since_full_rev_2048 = fractional_wheel_rev_time_2048(
																							speed_event->speed_mps,
																							fractional_wheel_revs);
		
		// Assign the speed event to the last calculated complete wheel revolution.
		speed_event->event_time_2048 			= current_2048 - time_since_full_rev_2048;
		speed_event->accum_wheel_revs 		= floor(fractional_wheel_revs);
		speed_event->period_2048 					= speed_event->event_time_2048 - 
																									last_event_time_2048;
	}
	else
	{
		// The flywheel hasn't moved, so we're stopped.
		speed_event->speed_mps 						= 0;
		speed_event->event_time_2048 			= current_2048;
		speed_event->accum_wheel_revs 		= last_accum_wheel_revs;
		speed_event->period_2048 					= current_2048 - last_event_time_2048;
	}
	
	speed_event->accum_flywheel_revs	= flywheel_revs;	// for debug purposes only.
	
	// Save state in static variables for next calculation.
	last_accum_wheel_revs 		= speed_event->accum_wheel_revs;
	last_event_time_2048 			= speed_event->event_time_2048;
	last_accum_flywheel_revs	= speed_event->accum_flywheel_revs;
}

void set_wheel_size(uint16_t wheel_size_mm)
{
	m_wheel_size = wheel_size_mm;
	
	// TODO: this is an assumed ratio, we need test/measure with other wheel sizes.
	/*
		For example, assuming a 2.07m wheel circumference:
		 0.01 miles : 144 s_revs 
		 0.01 miles = 16.09344 meters
		 1 servo_rev = 0.11176 distance_meters
	*/
	const float ratio 						= (2.07/0.11176)/2.07;
	// For every 1 wheel revolution the flywheel revolves this many times.
	m_flywheel_to_wheel_revs 	= (wheel_size_mm/1000)*ratio;
}

/**@brief Simulates the flywheel moving. */
void speed_simulate_flywheel_rev(uint8_t count)
{
	while (--count)
		REVS_TIMER->TASKS_COUNT = 1;
}

void init_speed(uint32_t pin_flywheel_rev)
{
	set_wheel_size(DEFAULT_WHEEL_SIZE_MM);	
	
	revs_init_gpiote(pin_flywheel_rev);
	revs_init_ppi();
	revs_init_timer();
	
	// Enable interrupt handler which will internally map to REVS_IRQHandler.
	NVIC_EnableIRQ(REVS_IRQn);
	//__enable_irq();	// <-- TODO: not sure what this call does, leaving it commented.
	
	// Start the counter.
	REVS_TIMER->TASKS_START = 1;
}
