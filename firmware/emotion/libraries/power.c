/*
*******************************************************************************
*
* By Jason De Lorme <jjdelorme@yahoo.com>
* http://www.roadacious.com
*
********************************************************************************/

#include "power.h"
#include "app_error.h"
#include "resistance.h"
#include "debug.h"
#include "math.h"
#include "nrf_delay.h"

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

/**@brief	Helper function to calculate x = y * slope + intercept.

static float inline slope_calc(float y, float slope, float intercept)
{
	float x;
	 x = y * slope + intercept;

	return x;
}*/

/**@brief 	Returns the force of rolling resistance using profile crr and weight.
 * @returns Force in Newtons typical range 13.0 : 30.0
 */
static float power_rr_force(float speed_mps)
{
	if (m_ca_slope != 0xFFFF)
	{
		// return a calibrated value.
		return (speed_mps * m_ca_slope + m_ca_intercept);
	}
	else
	{
		return m_rr_force;
	}
}

/**@brief	Calculates the force applied by the servo at a given position.
 */
static float servo_force(uint16_t servo_pos)
{
	float force;

	if (servo_pos >= MIN_RESISTANCE_LEVEL)
	{
		// Magnet OFF
		force = 0.0f;
	}
	else
	{
		if (FEATURE_IS_SET(FEATURE_SMALL_MAG))
		{
			force = (
					-0.00000000000033469583 * pow(servo_pos,5)
					+0.00000000202071048200 * pow(servo_pos,4)
					-0.00000466916875500000 * pow(servo_pos,3)
					+0.00513145135800000000 * pow(servo_pos,2)
					-2.691480529 * servo_pos
					+562.4577135);
		}
		else // BIG_MAG
		{
			force = (
					-0.0000000000012401 * pow(servo_pos,5)
					+0.0000000067486647 * pow(servo_pos,4)
					-0.0000141629606351 * pow(servo_pos,3)
					+0.0142639827784839 * pow(servo_pos,2)
					-6.92836459712442 * servo_pos
					+1351.463567618);
		}
	}

	if (force < 0.0f)
	{
		force = 0.0f;
	}

	return force;
}

/**@brief	Calculates the required servo position given force needed.
 */
uint16_t power_servo_pos_calc(float force)
{
	int16_t servo_pos;

	//
	// Manual multiple linear regression hack.
	//
	if (force < 0.8f)
	{
		servo_pos = MIN_RESISTANCE_LEVEL;
	}
	/*
	else if (force_needed < 5.028207f)
	{
		// 1,500 - 1,300
		servo_pos = (uint16_t)slope_calc(force_needed, -44.8841f, 1517.988f);
	}
	else if (force_needed < 40.51667f)
	{
		// 1,300 - 900
		servo_pos = (uint16_t)slope_calc(force_needed, -10.9038f, 1345.708f);
	}
	else if (force_needed < 44.9931f)
	{
		// 900 - 700
		servo_pos = (uint16_t)slope_calc(force_needed, -39.4606f, 2505.677f);
	}
	else
	{
		// Max
		servo_pos = 700;
	}*/
	else
	{
		if (FEATURE_IS_SET(FEATURE_SMALL_MAG))
		{
			servo_pos = (
					0.001461686  * pow(force,5)
					-0.076119976 * pow(force,4)
					+1.210189005 * pow(force,3)
					-5.221468861 * pow(force,2)
					-37.59134617 * force
					+1526.614724);
		}
		else // BIG_MAG
		{
			servo_pos = (
					-0.0000940913669469  * pow(force,5)
					+ 0.0108240213514885 * pow(force,4)
					-0.46173964201648 	 * pow(force,3)
					+8.9640144624266 	 * pow(force,2)
					-87.5217493343533 	 * force
					+1558.47782198543);
		}
	}

	// Protect min/max.
	if (servo_pos < MIN_RESISTANCE_LEVEL)
	{
		servo_pos = MIN_RESISTANCE_LEVEL;
	}
	else if (servo_pos > MAX_RESISTANCE_LEVEL)
	{
		servo_pos = MAX_RESISTANCE_LEVEL;
	}

	return servo_pos;
}

/**@brief	Initializes power module with a profile pointer that contains things like
 * 			the total rider weight, calibration details, etc...
 * 			Takes a default co-efficient for rolling resistance (i.e. 0.03).
 * 			This is only used if profile doesn't contain a slope/intercept override.
 *
 */
void power_init(user_profile_t* p_profile, uint16_t default_crr)
{
	if (p_profile->ca_slope != 0xFFFF)
	{
		m_ca_slope = (p_profile->ca_slope / 10000.0f);
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
	float servo;

	servo = servo_force(p_current->servo_position);
	*p_rr_force = power_rr_force(p_current->instant_speed_mps);

	// Calculate power.
	p_current->instant_power = ( *p_rr_force + servo ) * p_current->instant_speed_mps;

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
