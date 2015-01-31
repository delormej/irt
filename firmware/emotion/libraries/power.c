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
static user_profile_t* mp_profile;


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

	mp_profile = p_profile;

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
uint32_t power_calc(irt_power_meas_t * p_current, irt_power_meas_t * p_last, float* p_rr_force)
{
	uint16_t torque;
	uint16_t period_diff;
	float mag_watts;
	float magoff_watts;

	if (!isnan(mp_profile->ca_drag) && !isnan(mp_profile->ca_rr))
	{
		//PW_LOG("[PW] power_calc: We have ca_drag %.7f, ca_rr: %.7f\r\n",
		//		mp_profile->ca_drag, mp_profile->ca_rr);
		/*
		 * Power equation = (K * v^2) + (v * rr)
		 */
		magoff_watts = (mp_profile->ca_drag * pow(p_current->instant_speed_mps, 2)) +
				p_current->instant_speed_mps * mp_profile->ca_rr;

		*p_rr_force = (magoff_watts / p_current->instant_speed_mps);
	}
	else if (m_ca_slope != 0xFFFF)
	{
		// return a calibrated value.

		// First calculate power in watts, then back out to force.
		// Calculated by taking slope of change in watts (y) for each 1 mps (x) in speed.
		magoff_watts =
				(p_current->instant_speed_mps * m_ca_slope - m_ca_intercept);

		// Calculate force of rolling resistance alone.
		*p_rr_force = (magoff_watts / p_current->instant_speed_mps);
	}
	else
	{
		// DEFAULT behavior is to return a statically calculated value regardless
		// of speed.  This is returned if user did not override with a calibration
		// value.
		// This seems like overhead now, but in the future we shouldn't use a static
		// RR value anyways - we'll come up with a default calibration value based on
		// weight.
		*p_rr_force = m_rr_force;
	}

	// Calculate watts added by magnet.
	mag_watts = magnet_watts(p_current->instant_speed_mps, p_current->servo_position);

	// Calculate power.
	p_current->instant_power = ( *p_rr_force * p_current->instant_speed_mps ) + mag_watts;

	//PW_LOG("[PW] rr: %.2f, servo: %.2f, watts: %i\r\n", *p_rr_force, servo, p_current->instant_power);

	// Handle time rollover.
	// Accum wheel period is a calculated period representing the average time it takes
	// in 1/2048s for a wheel revolution at the current speed.
	if (p_current->accum_wheel_period < p_last->accum_wheel_period)
	{
		period_diff = (p_last->accum_wheel_period ^ 0xFFFF) +
				p_current->accum_wheel_period;
	}
	else
	{
		period_diff = p_current->accum_wheel_period - p_last->accum_wheel_period;
	}

	// Calculate torque.
	torque = power_torque_calc(p_current->instant_power, period_diff);

	// Accumulate torque from last measure.
	p_current->accum_torque = p_last->accum_torque + torque;

	return IRT_SUCCESS;
}
