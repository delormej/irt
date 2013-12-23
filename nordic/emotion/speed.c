/*
*******************************************************************************
* 
* (c) Jason De Lorme <jason@insideridetech.com>
* Inside Ride Technologies, LLC
*
********************************************************************************/

#include <string.h>
#include "insideride.h"
#include "speed.h"
#include "nrf_sdm.h"
#include "app_error.h"
#include "app_util.h"
#include "math.h"

// Wheel diameter size in mm.  A road wheel is typically 2,070 mmm.
static uint16_t 			m_wheel_size;
static uint8_t 				m_flywheel_revs_trigger;			// Rounded down # of flywheel revolutions for a single wheel rev.
static float 					m_wheel_rev_increment;				// Fractional wheel revolution per flywheel revolution.
static float 					m_accum_wheel_revs = 0.0f;		// Keeps track of the actual accumulated calculated wheel revs.
static speed_event_t 	m_speed_event;								// The last recorded speed event.


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

/**@brief		Initializes the counter which tracks the # of flywheel revolutions.
 *
 */
static void revs_init_timer()
{
	REVS_TIMER->MODE				=	TIMER_MODE_MODE_Counter;
	REVS_TIMER->BITMODE   	= TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos;
	REVS_TIMER->TASKS_CLEAR = 1;
	
	/**
		Set up a trigger that will call REVS_IRQHandler after a certain number of revolutions.
	 **/

	REVS_TIMER->CC[0] 			= m_flywheel_revs_trigger;

	// Clear the counter every time we hit the trigger value.
	REVS_TIMER->SHORTS 			= TIMER_SHORTS_COMPARE0_CLEAR_Msk;
	
	// Interrupt setup.
  REVS_TIMER->INTENSET 		= (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos);	
}

static uint32_t revs_get_count()
{
	uint32_t revs = 0;
	/*
	REVS_TIMER->TASKS_CAPTURE[0] = 1;
	revs = REVS_TIMER->CC[0]; 
	*/
	return revs;
}

/**@brief 	Retuns the count of 1/2048th seconds (2048 per second) since the 
 *				the counter started.  
 *
 * @note		This value rolls over at 32 seconds.
 */
static uint16_t get_seconds_2048()
{
	// TODO: Optimize this out - only need to figure this out once.
	
	// RTC1 is based on a 32.768khz crystal, or in other words it oscillates
	// 32768 times per second.  The PRESCALER determins how often a tick gets
	// counted.  With a prescaler of 0, there are 32,768 ticks in 1 second
	// 1/2048th of a second would be 16 ticks (32768/2048)
	// # of 2048th's would then be ticks / 16.
	float freq = 32768/(NRF_RTC1->PRESCALER+1);

	// Get current tick count.
	uint32_t ticks = NRF_RTC1->COUNTER;

	// Based on frequence of ticks, calculate 1/2048 seconds.
	// freq (hz) = times per second.
	uint16_t seconds_2048 = ROUNDED_DIV(ticks, (freq / 2048));

	return seconds_2048;
}

static float get_speed_mps(float wheel_revolutions, uint16_t period_seconds_2048)
{
	if (wheel_revolutions == 0 || period_seconds_2048 == 0)
		return 0.0f;
	
	// Convert mm to meters, muliply by revs, multiply by 1,000 for km.
	float distance_m = (((float)m_wheel_size) / 1000.0f) * wheel_revolutions;
	// Get speed in meters per second.
	float mps = distance_m / (period_seconds_2048 / 2048);

	return mps;
}

/*
static float get_speed_kmh(uint16_t wheel_revolutions, uint16_t period_seconds_2048)
{
	return get_speed_mps(wheel_revolutions, period_seconds_2048) * 3.6;
}

float get_speed_mph(uint16_t wheel_revolutions, uint16_t period_seconds_2048)
{
	// Convert km/h to mp/h.
	return get_speed_kmh(wheel_revolutions, period_seconds_2048) * 0.621371;
}
*/

uint16_t get_wheel_revolutions()
{
	uint32_t revs = revs_get_count();
	
	if (revs == 0)
		return 0;
	
	uint16_t wheel_revs = (uint16_t)(ROUNDED_DIV(revs, m_flywheel_to_wheel_revs));
	
	return wheel_revs;
}

void init_speed(uint32_t pin_flywheel_rev, uint16_t wheel_size_mm)
{
	// Initialize tracking structure.
	memset(&m_speed_event, 0, sizeof(m_speed_event));
	m_speed_event.event_time_2048 = get_seconds_2048();
	
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
	float flywheel_to_wheel_revs 	= (wheel_size_mm/1000)*ratio;
	// Round down for the IRQ trigger value for approximately 1 wheel revolution.
	m_flywheel_revs_trigger 			= floor(flywheel_to_wheel_revs);
	m_wheel_rev_increment 				= flywheel_to_wheel_revs - m_flywheel_revs_trigger;
	
	revs_init_gpiote(pin_flywheel_rev);
	revs_init_timer();
	
	// Enable interrupt handler which will internally map to REVS_IRQHandler.
	NVIC_EnableIRQ(REVS_IRQn);
	//__enable_irq();	// <-- TODO: not sure what this call does, leaving it commented.
	
	// Start the counter.
	REVS_TIMER->TASKS_START = 1;
}


/**@brief		Function called when a specified number of flywheel revolutions occur.
 *					Populates the m_speed_event structure with the last recorded event.
 *
 */
static void on_flywheel_handler()
{
	// Increment the wheel revolutions based on the fraction associated with a whole
	// number of flywheel revolutions.
	m_accum_wheel_revs += m_wheel_rev_increment;

	uint16_t m_last_event_time_2048 = m_speed_event.event_time_2048;
	uint16_t current_2048 					= get_seconds_2048();
	
	// Get the actual current speed in meters per second.
	m_speed_event.speed_mps = get_speed_mps(
											m_accum_wheel_revs - m_speed_event.accum_wheel_revs, 	// # of wheel revs in the period
											current_2048 - m_speed_event.event_time_2048);					// Current time period in 1/2048 seconds.
	
	// Get the speed in meters per 1/2048s.
	float speed_2048 = m_speed_event.speed_mps / 2048;
	
	// A single wheel rev at this speed would take this many 1/2048's of a second.
	float single_wheel_rev_time = (m_wheel_size / 1000) / speed_2048;
	
	// Difference between calculated partial wheel revs and the last full wheel rev.
	float partial_wheel_rev = fmod(m_accum_wheel_revs, 1);

	// How long ago in 1/2048's of a second would the last full wheel rev have occurred?
	uint16_t time_to_full_rev_2048 = round(partial_wheel_rev * single_wheel_rev_time);
	
	//
	// Record the last speed event.
	// 
	m_speed_event.event_time_2048 	= current_2048 - time_to_full_rev_2048;
	m_speed_event.accum_wheel_revs 	= floor(m_accum_wheel_revs);
	m_speed_event.period_2048 			= m_speed_event.event_time_2048 - 
																					m_last_event_time_2048;
	
}

/**@brief 	Interrupt invoked when a specified number of flywheel revolutions occur.
 *
 */
static void REVS_IRQHandler()
{
	REVS_TIMER->EVENTS_COMPARE[0] = 0;	// This stops the IRQHandler from getting called indefinetly.
	/*
	// This *should* be equal to m_flywheel_revs_trigger, so no need for the extra step?
	uint32_t revs = 0;

	REVS_TIMER->TASKS_CAPTURE[0] = 1;
	revs = REVS_TIMER->CC[0]; 					
	*/
	
	on_flywheel_handler();
}