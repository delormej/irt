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
#include "speed_event_fifo.h"
#include "nrf.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_sdm.h"
#include "app_error.h"
#include "app_util.h"
#include "app_gpiote.h"
#include "user_profile.h"
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

static speed_event_t				m_lastSpeedEvent;												// Last flywheel tick event.
static speed_event_t 				m_speed_buffer[SPEED_EVENT_CACHE_SIZE];							// Buffer of speed events.
static event_fifo_t				  	m_speed_fifo;

static app_gpiote_user_id_t 		mp_user_id;
static user_profile_t*				mp_user_profile;

/**@brief	Calculates the ratio of flywheel revolutions for 1 wheel revolution.
 *
 */
static float flywheel_to_wheel_revs()
{
	float ratio;

	/*
		For example, assuming a 2.07m wheel circumference:
		 0.01 miles : 144 flywheel_revs
		 0.01 miles = 16.09344 meters
		 1 servo_rev = 0.11176 distance_meters (FLYWHEEL_SIZE)
	*/
	// For every 1 wheel revolution, the flywheel ticks this many times.
	ratio = (((mp_user_profile->wheel_size_mm / 1000.0f) / FLYWHEEL_ROAD_DISTANCE)  	// Convert wheel size in mm to m, divide by the virtual road distance of 1 flywheel rev.
			* FLYWHEEL_TICK_PER_REV);													// Multiple by the # of ticks in a single flywheel rev.
			
	return ratio;
}


/**@brief	Invoked each time there is a tick of the flywheel.  This records the last tick
 * 			time and a running count of the number of ticks.
 *
 */
static void on_flywheel_tick(uint32_t event_pins_low_to_high, uint32_t event_pins_high_to_low)
{
	UNUSED_PARAMETER(event_pins_low_to_high);
	UNUSED_PARAMETER(event_pins_high_to_low);

	m_lastSpeedEvent.event_time_2048 = timestamp_get();
	m_lastSpeedEvent.accum_flywheel_ticks++;
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
static uint32_t last_wheel_time_calc(float wheel_revs, float avg_wheel_period, uint32_t event_time)
{
	uint32_t time_to_full_rev_2048 = 0;
	float partial_wheel_rev;
	
	// Difference between calculated partial wheel revs and the last full wheel rev.
	partial_wheel_rev = fmod(wheel_revs, 1);

	// How long ago in 1/2048's of a second would the last full wheel rev have occurred?
	time_to_full_rev_2048 = round(partial_wheel_rev * avg_wheel_period);
	
	return (event_time - time_to_full_rev_2048);
}

/**@brief	Calculates average meters per second between two events.
 *
 */
static float speed_calc_mps(speed_event_t first, speed_event_t last)
{
	// Deltas between first and last events.
	uint16_t flywheel_ticks;
	uint16_t event_period;
	float distance_m;

	//
	// Calculate ticks in the event period.
	//
	if (last.accum_flywheel_ticks < first.accum_flywheel_ticks)
	{
		// Handle ticks rollover.
		flywheel_ticks = (0xFFFF ^ first.accum_flywheel_ticks) +
				last.accum_flywheel_ticks;

		SP_LOG("[SP] speed_calc had a accum tick rollover.\r\n");
	}
	else
	{
		flywheel_ticks = last.accum_flywheel_ticks - first.accum_flywheel_ticks;
	}

	//
	// Only calculate speed if the flywheel has rotated.
	//
	if (flywheel_ticks == 0)
	{
		return 0.0f;
	}

	//
	// Calculate delta in time between events.
	//
	if (last.event_time_2048 < first.event_time_2048)
	{
		// Handle time rollover.
		event_period = (UINT32_MAX ^ first.event_time_2048) +
			last.event_time_2048;
	}
	else
	{
		event_period = last.event_time_2048 - first.event_time_2048;
	}

	// Virtual road distance traveled in meters.
	distance_m = flywheel_ticks / FLYWHEEL_TICK_PER_METER;

	// Calculate speed in meters per second.
	return (distance_m / (event_period / 2048.0f));
}

/*****************************************************************************
*
* Public functions, see function descriptions in header.
*
*****************************************************************************/

void speed_init(uint32_t pin_flywheel_rev, user_profile_t* p_profile)
{
	mp_user_profile = p_profile;
	 
	revs_init_gpiote(pin_flywheel_rev);
 	m_speed_fifo = speed_event_fifo_init((uint8_t*)m_speed_buffer, sizeof(speed_event_t));

	SP_LOG("[SP] Initialized speed.\r\n");
}

/**@brief	Returns a running smoothed average of speed.
 *
 */
float speed_average_mps(uint8_t seconds)
{
	uint8_t events = seconds*2; // 2 events per second.

	speed_event_t* p_oldest = (speed_event_t*)speed_event_fifo_oldest(&m_speed_fifo, events);
	speed_event_t* p_current = (speed_event_t*)speed_event_fifo_get(&m_speed_fifo);

	SP_LOG("[SP] %i:%i, %i:%i, events=%i\r\n", 
		p_oldest->event_time_2048, p_oldest->accum_flywheel_ticks,
		p_current->event_time_2048, p_current->accum_flywheel_ticks,
		events);

	return speed_calc_mps(*p_oldest, *p_current);
}

/**@brief	Calculates and records current speed measurement relative to last
 * 			measurement.
 *
 * @note	This method modifies the state of the speed by recording history
 * 			of speed events.
 */
uint32_t speed_calc(irt_context_t * p_meas)
{
	// Retatined state of accumulate wheel period.
	static uint16_t accumWheelPeriod = 0;

	float fractional_wheel_revs;
	speed_event_t* p_current;
	speed_event_t* p_previous;

	// Get handle to previous speed event.
	p_previous = (speed_event_t*)speed_event_fifo_get(&m_speed_fifo);

	// Make a copy of the current speed event to work on.
	p_current = (speed_event_t*)speed_event_fifo_put(&m_speed_fifo, 
		(uint8_t*)&m_lastSpeedEvent);

	// Get the flywheel ticks and calculate speed.
	p_meas->accum_flywheel_ticks = p_current->accum_flywheel_ticks;
	p_meas->instant_speed_mps = speed_calc_mps(*p_previous, *p_current);

	if (p_meas->instant_speed_mps > 0.0f)
	{
		// Calculate complete bicycle wheel revs based on wheel size and truncate to int.
		fractional_wheel_revs = p_meas->accum_flywheel_ticks / flywheel_to_wheel_revs();
		p_meas->accum_wheel_revs = (uint32_t)fractional_wheel_revs;

		// Calculate average wheel period; the amount of time (1/2048s) it takes for a complete wheel rev.
		p_meas->wheel_period = ((1 / (p_meas->instant_speed_mps / mp_user_profile->wheel_size_mm)) / 1000) * 2048;

		// Accumulate the wheel period.
		accumWheelPeriod += p_meas->wheel_period;
		p_meas->accum_wheel_period = accumWheelPeriod;

		// Calculate when the last wheel revolution would have occurred.
		p_meas->last_wheel_event_2048 = last_wheel_time_calc(
				fractional_wheel_revs,
				p_meas->wheel_period,
				p_current->event_time_2048);
	}

	// TODO: do we really need this? There is no error condition produced.
	return IRT_SUCCESS;
}

/**@brief 	Returns the accumulated count of flywheel ticks (2x revolutions).
 *
 */
uint16_t flywheel_ticks_get()
{
	return m_lastSpeedEvent.accum_flywheel_ticks;
}

/**@brief 	Returns the last tick event time in 1/2048's of a second.
 *
 */
uint16_t seconds_2048_get(void)
{
	return m_lastSpeedEvent.event_time_2048;
}

/**@brief 	Returns the distance traveled in meters.  Rolls over at 256m.
 *
 */
uint8_t speed_distance_get(void) 
{
    // Convert flywheel ticks to meters.
    return (uint8_t)
        (m_lastSpeedEvent.accum_flywheel_ticks / FLYWHEEL_TICK_PER_METER);     
}