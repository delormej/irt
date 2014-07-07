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

/**@brief Debug logging for module.
 *
 */
//#ifdef ENABLE_DEBUG_LOG
//#define PW_LOG debug_log
//#else
#define PW_LOG(...)
//#endif // ENABLE_DEBUG_LOG

static float m_rr_force;

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
 */
static float inline slope_calc(float y, float slope, float intercept)
{
	float x;
	 x = y * slope + intercept;

	return x;
}

/**@brief	Calculates the force applied by the servo at a given position.
 */
static float servo_force(uint16_t servo_pos)
{
	float force;

	//
	// Manual multiple linear regression hack.
	//
	if (servo_pos > 1500)
	{
		force = 0.0f;
	}
	else if (servo_pos > 1300)
	{
		// 1,500 - 1,300
		force = slope_calc(servo_pos, -0.02098f, 31.9982f);
	}
	else if (servo_pos > 900)
	{
		// 1,300 - 900
		force = slope_calc(servo_pos, -0.09138f, 123.0521f);
	}
	else if (servo_pos >= 699)
	{
		// 900 - 700
		force = slope_calc(servo_pos, -0.02238f, 61.1305f);
	}

	return force;
}

/**@brief	Calculates the required servo position given force needed.
 */
uint16_t power_servo_pos_calc(float force_needed)
{
	int16_t servo_pos;

	//
	// Manual multiple linear regression hack.
	//
	if (force_needed < 0.832566f)
	{
		servo_pos = 1500;
	}
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
	}

	// Protect min/max.
	if (servo_pos < 700)
		servo_pos = 700;
	else if (servo_pos > 1500)
		servo_pos = 1500;

	return servo_pos;
}

/**@brief 	Returns the force of rolling resistance using profile crr and weight.
 * @returns Force in Newtons typical range 13.0 : 30.0
 */
float power_rr_force()
{
	return m_rr_force;
}


/**@brief	Initializes power module with a profile pointer that contains things like
 * 			the total rider weight, calibration details, etc...
 *
 */
void power_init(uint16_t total_weight_kg, uint16_t calibrated_crr)
{
	float crr;

	// Convert to the right unit.
	crr = (float)(calibrated_crr / 100000.0f);

	m_rr_force = (GRAVITY * (total_weight_kg / 100.0f) * crr);
}

/**@brief	Calculates and records current power measurement relative to last measurement.
 *
 */
uint32_t power_calc(irt_power_meas_t * p_current, irt_power_meas_t * p_last)
{
	uint16_t torque;
	uint16_t period_diff;
	float servo;

	servo = servo_force(p_current->servo_position);

	// Calculate power.
	p_current->instant_power = ( (m_rr_force + servo) * p_current->instant_speed_mps );

	PW_LOG("[PW] rr: %.2f, servo: %.2f, watts: %i\r\n", m_rr_force, servo, p_current->instant_power);

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
