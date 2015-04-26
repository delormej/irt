/*
*******************************************************************************
*
* By Jason De Lorme <jjdelorme@yahoo.com>
* http://www.roadacious.com
*
********************************************************************************/

#include <float.h>
#include "power.h"
#include "nrf_error.h"
#include "app_error.h"
#include "resistance.h"
#include "debug.h"
#include "math.h"
#include "irt_common.h"
#include "magnet.h"

/**@brief Debug logging for module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define PW_LOG debug_log
#else
#define PW_LOG(...)
#endif // ENABLE_DEBUG_LOG

static float m_rr_force;
static float m_ca_slope;			// calibration parameters
static float m_ca_intercept;
//static bool  m_use_big_mag;


/* Calculates angular velocity based on wheel ticks and time.
static float angular_vel_calc(uint8_t wheel_ticks, uint16_t period_2048)
{
	float value;

	value = (2.0f * MATH_PI * wheel_ticks) / (period_2048 / 2048.0f);

	return value;
}*/

/**@brief	Calculates estimated torque given watts and time.
 */
static uint16_t power_torque_calc(int16_t watts, uint16_t period_seconds_2048)
{
	uint16_t torque;

	if (watts == 0 || period_seconds_2048 == 0)
	{
		torque = 0;
	}
	else
	{
		torque = (watts * period_seconds_2048) / (128 * MATH_PI);
	}

	//PW_LOG("[PW] torque: %i\r\n", torque);

	return torque;
}

/**@brief	Initializes power module with a profile pointer that contains things like
 * 			the total rider weight, calibration details, etc...
 * 			Takes a default co-efficient for rolling resistance (i.e. 0.03).
 * 			This is only used if profile doesn't contain a slope/intercept override.
 *
 */
void power_init(user_profile_t* p_profile, uint16_t default_crr)
{
	//m_use_big_mag = FEATURE_AVAILABLE(FEATURE_BIG_MAG);
	//PW_LOG("[PW] Use small mag?: %i\r\n", m_use_big_mag);

	if (p_profile->ca_slope != 0xFFFF)
	{
		m_ca_slope = (p_profile->ca_slope / 1000.0f);
		m_ca_intercept = (p_profile->ca_intercept / 1000.0f);
		m_rr_force = 0;

		PW_LOG("[PW] Initializing power with slope: %.4f intercept %.3f \r\n",
				m_ca_slope, m_ca_intercept);
	}
	else
	{
		m_rr_force = (GRAVITY * (p_profile->total_weight_kg / 100.0f) *
				(default_crr / 1000.0f));
		m_ca_slope = 0xFFFF;
		m_ca_intercept = 0xFFFF;
	}
}

/**@brief	Calculates and records current power measurement relative to last measurement.
 *
 */
uint32_t power_calc(user_profile_t* p_profile, irt_context_t* p_meas)
{
	uint16_t torque = 0;
	float mag_watts;
	float magoff_watts;

	if (!isnan(p_profile->ca_drag) && !isnan(p_profile->ca_rr))
	{
		/*
		 * Power equation = ((K * v^2) + rr) * v
		 */
		magoff_watts = ( (p_profile->ca_drag * pow(p_meas->instant_speed_mps, 2)) +
				p_profile->ca_rr ) * p_meas->instant_speed_mps ;

		p_meas->rr_force = (magoff_watts / p_meas->instant_speed_mps);
	}
	else if (m_ca_slope != 0xFFFF)
	{
		// return a calibrated value.

		// First calculate power in watts, then back out to force.
		// Calculated by taking slope of change in watts (y) for each 1 mps (x) in speed.
		magoff_watts =
				(p_meas->instant_speed_mps * m_ca_slope - m_ca_intercept);

		// Calculate force of rolling resistance alone.
		p_meas->rr_force = (magoff_watts / p_meas->instant_speed_mps);
	}
	else
	{
		// DEFAULT behavior is to return a statically calculated value regardless
		// of speed.  This is returned if user did not override with a calibration
		// value.
		// This seems like overhead now, but in the future we shouldn't use a static
		// RR value anyways - we'll come up with a default calibration value based on
		// weight.
		p_meas->rr_force = m_rr_force;
	}

	// Calculate watts added by magnet.
	mag_watts = magnet_watts(p_meas->instant_speed_mps, p_meas->servo_position);

	// Calculate power.
	p_meas->instant_power = ( p_meas->rr_force * p_meas->instant_speed_mps ) + mag_watts;

	//PW_LOG("[PW] rr: %.2f, servo: %.2f, watts: %i\r\n", *p_rr_force, servo, p_current->instant_power);

	// Accumulate torque from last measure.
	torque = power_torque_calc(p_meas->instant_power, p_meas->wheel_period);
	p_meas->accum_torque += torque;

	return IRT_SUCCESS;
}
