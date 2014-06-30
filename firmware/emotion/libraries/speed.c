/*
*******************************************************************************
* 
* (c) Jason De Lorme <jason@insideridetech.com>
* Inside Ride Technologies, LLC
*
********************************************************************************/

#include <string.h>
#include <math.h>
#include "speed.h"
#include "nrf_sdm.h"
#include "app_error.h"
#include "app_util.h"
#include "debug.h"

/**@brief Debug logging for resistance control module.
 */
#ifdef ENABLE_DEBUG_LOG
#define SP_LOG debug_log
#else
#define SP_LOG(...)
#endif // ENABLE_DEBUG_LOG

/*
 * Flywheel is actually 40cm in circumference, but transfers via belt to the drum (virtual road surface).
 * Flywheel to drum ratio creates a virtual road distance travelled of 11.176cm per revolution of the flywheel.
 * 1 meter of travel is equal to 8.9477452 rotations (1 / 0.11176)
 * Flywheel generates 2 ticks per revolution == 1 meter of travel is equal to 17.89549 flywheel ticks.
 */
#define FLYWHEEL_SIZE				0.11176f					// Size in meters.
#define FLYWHEEL_TICK_PER_METER		(1 / FLYWHEEL_SIZE) * 2 	// 2 ticks per rev.

static uint16_t m_wheel_size;									// Wheel diameter size in mm.
static float m_flywheel_to_wheel_revs;							// Ratio of flywheel revolutions for 1 wheel revolution.

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
static uint32_t flywheel_ticks_get()
{
// DEBUG ONLY CODE
#if defined(SIM_SPEED)
	// DEBUG PURPOSES ONLY. Simulates speed for 1/4 second.
	// 
	// ((((speed_kmh / 3600) * 250) / wheel_size_m) * 18.5218325f);
	// Where speed_kmh = 28.0f, ~17 revolutions per 1/4 of a second.
	//
	static uint32_t r = 0;
	return r+=16;  // ~8mph
#endif

	uint32_t revs = 0;

	REVS_TIMER->TASKS_CAPTURE[0] = 1;
	revs = REVS_TIMER->CC[0]; 
	
	return revs;
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

void set_wheel_size(uint16_t wheel_size_mm)
{
	m_wheel_size = wheel_size_mm;
	/*
		For example, assuming a 2.07m wheel circumference:
		 0.01 miles : 144 flywheel_revs 
		 0.01 miles = 16.09344 meters
		 1 servo_rev = 0.11176 distance_meters (FLYWHEEL_SIZE)
	*/
	// For every 1 wheel revolution, the flywheel revolves this many times.
	m_flywheel_to_wheel_revs = (wheel_size_mm / 1000.0f) / FLYWHEEL_SIZE;
}

void speed_init(uint32_t pin_flywheel_rev, uint16_t wheel_size_mm)
{
	set_wheel_size(wheel_size_mm);
	
	revs_init_gpiote(pin_flywheel_rev);
	revs_init_ppi();
	revs_init_timer();
	
	// Enable interrupt handler which will internally map to REVS_IRQHandler.
	NVIC_EnableIRQ(REVS_IRQn);
	//__enable_irq();	// <-- TODO: not sure what this call does, leaving it commented.
	
	// Start the counter.
	REVS_TIMER->TASKS_START = 1;
}

/**@brief	Calculates and records current speed measurement relative to last measurement
 *
 */
uint32_t speed_calc(irt_power_meas_t * p_current, irt_power_meas_t * p_last)
{
	// Store delta between last event and current.
	uint32_t flywheel_ticks;

	float distance_m;
	float wheel_revs;

	// Get the flywheel ticks (2 per rev).
	p_current->accum_flywheel_ticks = flywheel_ticks_get();

	// TODO: Handle rollover, but this will be rare given 32 bit #.
	// Ticks in the event period.
	flywheel_ticks = p_current->accum_flywheel_ticks - p_last->accum_flywheel_ticks;

	// Only calculate speed if the flywheel has rotated since last.
	if (flywheel_ticks > 0)
	{
		// Handle time rollover.
		if (p_current->event_time_2048 < p_last->event_time_2048)
		{
			p_current->wheel_period_2048 = (p_last->event_time_2048 ^ 0xFFFF) +
					p_current->event_time_2048;
		}
		else
		{
			p_current->wheel_period_2048 = p_current->event_time_2048 - p_last->event_time_2048;
		}

		// Distance in meters.
		distance_m = flywheel_ticks / FLYWHEEL_TICK_PER_METER;

		// Calculate speed in meters per second.
		p_current->instant_speed_mps = distance_m / (p_current->wheel_period_2048 / 2048.0f);

		// Calculate complete bicycle wheel revs based on wheel size and truncate to int.
		p_current->accum_wheel_revs = (uint32_t)(p_current->accum_flywheel_ticks / m_flywheel_to_wheel_revs);

		p_current->accum_wheel_period = p_last->accum_wheel_period + p_current->wheel_period_2048;

		SP_LOG("[SP] accum_wheel_revs:%i, flywheel_ticks:%i, instant_mps:%.2f\r\n",
				p_current->accum_wheel_revs,
				flywheel_ticks,
				p_current->instant_speed_mps);
	}
	else
	{
		// From the ANT spec:
		// To indicate zero rotational velocity, do not increment the accumulated wheel period and do not increment the wheel ticks.
		// The update event count continues incrementing to indicate that updates are occurring, but since the wheel is not rotating
		// the wheel ticks do not increase.
		p_current->accum_wheel_revs = p_last->accum_wheel_revs;
		p_current->accum_wheel_period = p_last->accum_wheel_period;

		p_current->instant_speed_mps = 0.0f;

		SP_LOG("[SP] Coasting... %i : %i\r\n", p_current->accum_flywheel_ticks, p_last->accum_flywheel_ticks);
	}

	// TODO: do we really need this? There is no error condition produced.
	return IRT_SUCCESS;
}


