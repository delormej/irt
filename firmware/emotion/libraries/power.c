/*
*******************************************************************************
*
* By Jason De Lorme <jjdelorme@yahoo.com>
* http://www.roadacious.com
*
********************************************************************************/

#include <float.h>
#include <math.h>
#include "math_private.h"
#include "power.h"
#include "nrf_error.h"
#include "app_error.h"
#include "resistance.h"
#include "debug.h"
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
//static float m_ca_slope;			// calibration parameters
//static float m_ca_intercept;
static user_profile_t* mp_profile;
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
void power_init(user_profile_t* p_profile)
{
	//m_use_big_mag = FEATURE_AVAILABLE(FEATURE_BIG_MAG);
	//PW_LOG("[PW] Use small mag?: %i\r\n", m_use_big_mag);
	mp_profile = p_profile;
}

/**@brief	Determines the right power calculation method (linear vs. bicycling science)
 * 			and returns estimated power based on speed (no magnet).
 */
float power_magoff(float speed_mps) 
{
	float watts = 0.0f;

	/* There are currently 2 ways to calculate base resistance as shown below.
		* The goal is to get to 1, but for legacy reasons we support both until we  
		* know which one is best.
		*/
	if (!isnan(mp_profile->ca_drag) && !isnan(mp_profile->ca_rr))
	{
		/*
			* Bicycling Science Power equation = ((K * v^2) + rr) * v
			*/
		watts = ( (mp_profile->ca_drag * pow(speed_mps, 2)) +
				mp_profile->ca_rr ) * speed_mps ;
	}
	/*else if (m_ca_slope != 0xFFFF)
	{
		*
			* Calibrated linear power equation.
			*
		p_meas->magoff_power =
				(p_meas->instant_speed_mps * m_ca_slope - m_ca_intercept);
	}*/
	else
	{
		/*
			* Default linear power equation.
			*/
		m_rr_force = (GRAVITY * (mp_profile->total_weight_kg / 100.0f) *
				(DEFAULT_CRR / 1000.0f));		 
		watts = m_rr_force * speed_mps;
	}

	//PW_LOG("[PW] power_magoff: %.2f\r\n", watts);
	return watts;
}

/**@brief	Calculates and records current power measurement relative to last measurement.
 *
 */
uint32_t power_calc(irt_context_t* p_meas)
{
	uint16_t torque = 0;
	float mag_watts = 0.0f;
	float magoff_power = 0.0f;

	// Calculate estimated power with no magnet.
	magoff_power = power_magoff(p_meas->instant_speed_mps);

	// Calculate watts added by magnet.
	mag_watts = magnet_watts(p_meas->instant_speed_mps, p_meas->servo_position);

	// Calculate power.
	p_meas->instant_power = magoff_power + mag_watts;

	// PW_LOG("[PW] rr: %.2f, servo: %.2f, watts: %i\r\n", *p_rr_force, servo, p_current->instant_power);
	// PW_LOG("[PW] Estimated power, speed: %.2f, watts: %i\r\n", p_meas->instant_speed_mps,
	// 	p_meas->instant_power);

	// Accumulate torque from last measure.	
	torque = power_torque_calc(p_meas->instant_power, p_meas->wheel_period);
	p_meas->accum_torque += torque;

	return IRT_SUCCESS;
}
