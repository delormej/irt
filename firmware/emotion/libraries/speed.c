/*
*******************************************************************************
* 
* (c) Jason De Lorme <jason@insideridetech.com>
* Inside Ride Technologies, LLC
*
********************************************************************************/

#include <string.h>
#include <math.h>
#include "nordic_common.h"
#include "speed.h"
#include "nrf_sdm.h"
#include "app_error.h"
#include "app_util.h"
#include "app_gpiote.h"
#include "debug.h"

/**@brief Debug logging for resistance control module.
 */
#ifdef ENABLE_DEBUG_LOG
#define SP_LOG debug_log
#else
#define SP_LOG(...)
#endif // ENABLE_DEBUG_LOG

/*
 * Flywheel is 40cm in circumference, but transfers via belt to the drum (virtual road surface)
 * at ratio of 10cm (pully circumference) : 22.5cm (drum belt slot circumference).
 *
 * Virtual road distance traveled is 11.5cm per revolution of the flywheel.
 * 1 drum rotation = 22.5/10=2.25 flywheel rotations
 *
 * 1 meter of travel is equal to 8.695652174 rotations (1 / 0.115)
 * Flywheel generates 2 ticks per revolution == 1 meter of travel is equal to 17.3913 flywheel ticks.
 *
 * NOTE: if magnet power is a function of a static force per position * flywheel speed, we should make sure
 * to calculate based on actual flywheel speed and not virtual road speed.
 *
 */
#define FLYWHEEL_TICK_PER_REV		2																// # of ticks per flywheel revolution.
#define FLYWHEEL_ROAD_DISTANCE		0.115f															// Virtual road distance traveled in meters per complete flywheel rev.
#define FLYWHEEL_TICK_PER_METER		((1.0f / FLYWHEEL_ROAD_DISTANCE) * FLYWHEEL_TICK_PER_REV)		// # of flywheel ticks per meter

static uint16_t 					m_flywheel_ticks = 0;											// Accumulated count of flywheel ticks.
static uint16_t						m_last_event_2048 = 0;
static uint16_t 					m_wheel_size;													// Wheel diameter size in mm.
static float 						m_flywheel_to_wheel_revs;										// Ratio of flywheel revolutions for 1 wheel revolution.

static app_gpiote_user_id_t 		mp_user_id;


/**@brief 	Returns the count of 1/2048th seconds (2048 per second) since the
 *			the counter started.
 *
 * @note	This value rolls over at 32 seconds.
 */
static uint16_t timestamp_get()
{
	// Get current tick count.
	uint32_t ticks = NRF_RTC1->COUNTER;

	// Based on frequence of ticks, calculate 1/2048 seconds.
	// freq (hz) = times per second.
	uint16_t seconds_2048 = ROUNDED_DIV(ticks, (TICK_FREQUENCY / 2048));

	return seconds_2048;
}

/**@brief	Invoked each time there is a tick of the flywheel.  This records the last tick
 * 			time and a running count of the number of ticks.
 *
 */
static void on_flywheel_tick(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low)
{
	UNUSED_PARAMETER(event_pins_low_to_high);
	UNUSED_PARAMETER(event_pins_high_to_low);

	m_last_event_2048 = timestamp_get();
	m_flywheel_ticks++;
}

/**@brief	Configure GPIO input from flywheel revolution pin and create an 
 *				event on achannel. 
 */
static void revs_init_gpiote(uint32_t pin_flywheel_rev)
{
	uint32_t err_code;

	nrf_gpio_cfg_input(pin_flywheel_rev, NRF_GPIO_PIN_NOPULL);
	
	err_code = app_gpiote_user_register(&mp_user_id,
		1UL << pin_flywheel_rev,
		1UL << pin_flywheel_rev,
		on_flywheel_tick);
	APP_ERROR_CHECK(err_code);

	err_code = app_gpiote_user_enable(mp_user_id);
	APP_ERROR_CHECK(err_code);
}

/**@brief		Calculates how long it would have taken since the last complete 
 *					wheel revolution given current speed (in meters per second).  
 *					Returns a value in 1/2048's of a second.
 *
 */
static uint16_t last_wheel_time_calc(float wheel_revs, float avg_wheel_period, uint16_t event_time)
{
	uint16_t time_to_full_rev_2048 = 0;
	float partial_wheel_rev;
	
	// Difference between calculated partial wheel revs and the last full wheel rev.
	partial_wheel_rev = fmod(wheel_revs, 1);

	// How long ago in 1/2048's of a second would the last full wheel rev have occurred?
	time_to_full_rev_2048 = round(partial_wheel_rev * avg_wheel_period);
	
	return (event_time - time_to_full_rev_2048);
}

/*****************************************************************************
*
* Public functions, see function descriptions in header.
*
*****************************************************************************/

void speed_wheel_size_set(uint16_t wheel_size_mm)
{
	m_wheel_size = wheel_size_mm;
	/*
		For example, assuming a 2.07m wheel circumference:
		 0.01 miles : 144 flywheel_revs 
		 0.01 miles = 16.09344 meters
		 1 servo_rev = 0.11176 distance_meters (FLYWHEEL_SIZE)
	*/
	// For every 1 wheel revolution, the flywheel ticks this many times.
	m_flywheel_to_wheel_revs = (((wheel_size_mm / 1000.0f) / FLYWHEEL_ROAD_DISTANCE)  	// Convert wheel size in mm to m, divide by the virtual road distance of 1 flywheel rev.
			* FLYWHEEL_TICK_PER_REV);													// Multiple by the # of ticks in a single flywheel rev.
}

void speed_init(uint32_t pin_flywheel_rev, uint16_t wheel_size_mm)
{
	// TODO: remove this and figure another way.
	speed_wheel_size_set(wheel_size_mm);
	
	revs_init_gpiote(pin_flywheel_rev);

	SP_LOG("[SP] Initialized speed.\r\n");
}

/**@brief	Calculates and records current speed measurement relative to last measurement
 *
 */
uint32_t speed_calc(irt_power_meas_t * p_current, irt_power_meas_t * p_last)
{
	// Store delta between last event and current.
	uint16_t flywheel_ticks;

	float distance_m;
	float fractional_wheel_revs;
	uint16_t avg_wheel_period;
	uint16_t event_period;

	// Get the flywheel ticks.
	p_current->accum_flywheel_ticks = flywheel_ticks_get();

	// Ticks in the event period.
	if (p_current->accum_flywheel_ticks < p_last->accum_flywheel_ticks)
	{
		// Handle ticks rollover.
		flywheel_ticks = (p_last->accum_flywheel_ticks ^ 0xFFFF) +
				p_current->accum_flywheel_ticks;

		SP_LOG("[SP] speed_calc had a accum tick rollover.\r\n");
	}
	else
	{
		flywheel_ticks = p_current->accum_flywheel_ticks - p_last->accum_flywheel_ticks;
	}

	// Only calculate speed if the flywheel has rotated since last.
	if (flywheel_ticks > 0)
	{
		// Handle time rollover.
		if (p_current->event_time_2048 < p_last->event_time_2048)
		{
			event_period = (p_last->event_time_2048 ^ 0xFFFF) +
					p_current->event_time_2048;
		}
		else
		{
			event_period = p_current->event_time_2048 - p_last->event_time_2048;
		}

		// Virtual road distance traveled in meters.
		distance_m = flywheel_ticks / FLYWHEEL_TICK_PER_METER;

		// Calculate speed in meters per second.
		p_current->instant_speed_mps = distance_m / (event_period / 2048.0f);

		// Calculate complete bicycle wheel revs based on wheel size and truncate to int.
		fractional_wheel_revs = p_current->accum_flywheel_ticks / m_flywheel_to_wheel_revs;
		p_current->accum_wheel_revs = (uint32_t)fractional_wheel_revs;

		// Calculate average wheel period; the amount of time (1/2048s) it takes for a complete wheel rev.
		avg_wheel_period = ((1 / (p_current->instant_speed_mps / m_wheel_size)) / 1000) * 2048;

		p_current->accum_wheel_period = p_last->accum_wheel_period + avg_wheel_period;

		// Calculate when the last wheel revolution would have occurred.
		p_current->last_wheel_event_2048 = last_wheel_time_calc(
				fractional_wheel_revs,
				avg_wheel_period,
				p_current->event_time_2048);

		/*SP_LOG("[SP] flywheel:%i, speed:%.1f, ratio:%.4f\r\n",
				p_current->accum_flywheel_ticks,
				p_current->instant_speed_mps,
				m_flywheel_to_wheel_revs);*/
	}

	// TODO: do we really need this? There is no error condition produced.
	return IRT_SUCCESS;
}

/**@brief 	Returns the accumulated count of flywheel ticks (2x revolutions).
 *
 */
uint16_t flywheel_ticks_get()
{
	return m_flywheel_ticks;
}

/**@brief 	Returns the last tick event time in 1/2048's of a second.
 *
 */
uint16_t seconds_2048_get(void)
{
	return m_last_event_2048;
}
