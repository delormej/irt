/*
*******************************************************************************
* 
* By Jason De Lorme <jason@insideride.com>
* Inside Ride Technologies, LLC
*
* This module is responsible for interacting with the e*Motion servo to control
* magnetic resistance on the flywheel.  
*
********************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "irt_peripheral.h"
#include "app_error.h"
#include "magnet.h"
#include "resistance.h"
#include "nrf_pwm.h"
#include <math.h>
#include "debug.h"

#define MIN_THRESHOLD_MOVE	2		// Minimum threshold for a servo move.

#define ACTUAL_SERVO_POS(POS)	POS + mp_user_profile->servo_offset

/**@brief Debug logging for resistance control module.
 */
#ifdef ENABLE_DEBUG_LOG
#define RC_LOG debug_log
#else
#define RC_LOG(...)
#endif // ENABLE_DEBUG_LOG

/**@brief	Determines if the move is above the threshold for a servo move.
 */
#define ABOVE_TRESHOLD(POS)										\
		((POS - m_resistance_state.servo_position) > MIN_THRESHOLD_MOVE || 			\
				(m_resistance_state.servo_position - POS > MIN_THRESHOLD_MOVE))		\

// MACRO for getting to the array.
#define RESISTANCE_LEVEL 	mp_user_profile->servo_positions.positions
#define SIM_CRR							0.0033f										// Default crr for typical outdoor rolling resistance (not the same as above).
#define SIM_C							0.60f										// Default co-efficient for drag.  See resistance sim methods.

static user_profile_t* 			mp_user_profile;
static irt_resistance_state_t	m_resistance_state;


/**@brief	Initializes the resistance module which controls the servo.
 *
 */
irt_resistance_state_t* resistance_init(uint32_t servo_pin_number, user_profile_t* p_user_profile)
{
	mp_user_profile = p_user_profile;

	memset(&m_resistance_state, 0, sizeof(m_resistance_state));

	 /*	fCrr is the coefficient of rolling resistance (unitless). Default value is 0.004.
	 *
	 *	fC is equal to A*Cw*Rho where A is effective frontal area (m^2); Cw is drag
	 *	coefficent (unitless); and Rho is the air density (kg/m^3). The default value
	 *	for A*Cw*Rho is 0.60.
	 */
	m_resistance_state.crr = SIM_CRR;
	m_resistance_state.c = SIM_C;

	// Initialize pulse-width-modulation.
	pwm_init(servo_pin_number);

	// Always start off with resistance at level 0.
	resistance_level_set(0);

	// Return the state object.
	return &m_resistance_state;
}

/**@brief		Gets the current resistance state object.
 *
 */
irt_resistance_state_t* resistance_state_get(void)
{
	return &m_resistance_state;
}

/**@brief	Determines if there is a move and move accordingly.
 *
 */
uint16_t resistance_position_set(uint16_t servo_pos, bool smooth)
{
	uint32_t err_code;
	// Actual servo position after calibration.
	uint16_t actual_servo_pos;

	/*
	 * NOTE: SERVO OFFSET LOGIC
	 *
	 * Only record offset position internally, don't expose beyond the module
	 * as certain servo positions have bearing on position.
	 *
	 * Goal is to use the range 2,000 - 1,000 as per servo specs.  Instead of
	 * the original 2,107 - 699 range which our power curve testing was done against.
	 *
	 * Factory Baseline is where Jennifer's jig is set.  This used to be "position 2" or 1150.
	 * Initial power curve testing was based on a servo that would consider position 2 as 1100
	 * (had -50 offset).
	 *
	 * Adjusted offset for the 2,000 - 1,000 range is -301, but since testing was done at -50,
	 * the new baseline offset is 351 for a servo that is factory calibrated to 1,451.
	*/
	if ((servo_pos + mp_user_profile->servo_offset) > MAGNET_POSITION_OFF)
	{
		// Don't go beyond max magnet position, even after adjusting for servo offset.
		servo_pos = MAGNET_POSITION_OFF;
		actual_servo_pos = MAGNET_POSITION_OFF;
	}
	else
	{
		if (servo_pos < MAGNET_POSITION_MAX_RESISTANCE)
		{
			// Adjust to the maximum resistance position.
			servo_pos = MAGNET_POSITION_MAX_RESISTANCE;
		}

		// Adjust to the actual position based on servo offset.
		actual_servo_pos = ACTUAL_SERVO_POS(servo_pos);
	}

	// Check if we actually need to move.
	if ( (m_resistance_state.servo_position != servo_pos) && ABOVE_TRESHOLD(servo_pos) )
	{
		// Issue a command to move the servo.
		err_code = pwm_set_servo(actual_servo_pos, smooth);
		APP_ERROR_CHECK(err_code);

		// Save module state for next adjustment.
		m_resistance_state.servo_position = servo_pos;

		RC_LOG("[RC]:SET_SERVO %i\r\n", actual_servo_pos);
	}

	return m_resistance_state.servo_position;
}

/**@brief		Validates the values of positions are in range.
 *
 */
bool resistance_positions_validate(servo_positions_t* positions)
{
	uint8_t index = 0;
	uint16_t value = 0;

	if (positions->count > MAX_RESISTANCE_LEVEL_COUNT)
	{
		RC_LOG("[RC] resistance_positions_validate too many: %i\r\n", positions->count);
		return false;
	}

	do
	{
		// If it's the home position 2,000 it's valid.
		if (positions->positions[index] == MAGNET_POSITION_OFF)
			continue;

		if (positions->positions[index] > MAGNET_POSITION_OFF ||
				positions->positions[index] < MAGNET_POSITION_MAX_RESISTANCE)
		{
			RC_LOG("[RC] resistance_positions_validate Servo position value invalid: %i\r\n", value);
			return false;
		}
	} while (++index < positions->count);

	return true;
}

/**@brief		Sets to a standard resistance level (0-9).
 *
 */
uint16_t resistance_level_set(uint8_t level)
{
	RC_LOG("[RC] resistance_level_set: %i, max: %i\r\n",
			level, RESISTANCE_LEVELS);

	// Ensures the resistance to a standard 0-9 level.
	if (level >= RESISTANCE_LEVELS)
	{
		// Set to the highest level.
		level = RESISTANCE_LEVELS - 1;
	}

	m_resistance_state.mode = RESISTANCE_SET_STANDARD;
	m_resistance_state.level = level;

	return resistance_position_set(RESISTANCE_LEVEL[level], false);
}

/**@brief		Gets the levels of standard resistance available.
  */
uint8_t resistance_level_count()
{
	return mp_user_profile->servo_positions.count;
}

uint16_t resistance_pct_set(float percent)
{
	/*
	Puts the trainer in Resistance Mode.

	Resistance Mode will directly control the strength of the brake and will stay
	constant regardless of the rider's speed. This mode is similar to a spin bike
	where the user can increase or decrease the difficulty of their workout.
	Parameters:
	fpScale a float from 0.0 to 1.0 that represents the percentage the brake is
	turned on (0.0 = brake turned off; 0.256 = 25.6% of brake; 1.0 = 100% brake force).
	*/
	uint16_t position;

	m_resistance_state.mode = RESISTANCE_SET_PERCENT;

	if (percent == 0.0f)
	{
		position = MAGNET_POSITION_OFF;
	}
	else if (percent > 99.9f)
	{
		position = MAGNET_POSITION_MAX_RESISTANCE;
	}
	else
	{
		// Calculate the difference between easiest and hardest positions.
		position = (uint16_t) (  MAGNET_POSITION_MIN_RESISTANCE -
				( (MAGNET_POSITION_MIN_RESISTANCE - MAGNET_POSITION_MAX_RESISTANCE) *
						percent )  );
	}

	return resistance_position_set(position, false);
}

/**@brief		Gets magnet position resistance to simulate desired erg watts.
 * @returns 	Servo position.
 */
static uint16_t resistance_erg_position(float speed_mps, int16_t magoff_watts)
{
	float mag_watts;

	// Handle adjustment.
	if (m_resistance_state.adjust_pct > 0)
	{
		// Adjust erg target.
		m_resistance_state.erg_watts = m_resistance_state.unadjusted_erg_watts * 
			(m_resistance_state.adjust_pct / 100.0f);
	}

	//
	// Calculate the required incremental magnet force required (if any).
	//
	// TODO: We could get smarter here and deal with 'erg-ing out' or if the user
	// stops pedaling deal with them starting back up.
	//
	mag_watts = m_resistance_state.erg_watts - magoff_watts;

	return magnet_position(speed_mps, mag_watts);
}

/**@brief	Get the position of the servo based on simulation mode forces.
 * @note 	Sim Mode is used to simulate real world riding situations. This mode 
 *				will adjust the brake resistance based on the effects of gravity, 
 *				rolling resistance, and wind resistance. In order to create an accurate 
 *				simulation of real world conditions the following variables must be set: 
 *				rider & bike weight, coefficient of rolling resistance, coefficient of 
 *				wind resistance, wind speed, wheel circumference, and grade. 
 *				If these variables are not set, they will default to an "average" value.
 */
static uint16_t resistance_sim_position(float speed_mps, int16_t magoff_watts)
{
	float rr_force = (float)magoff_watts / speed_mps;
	float mag_watts = 0;

	// sim is going to calculate the estimated watts required at grade + wind for
	// the current speed and rider total weight.  It will then hand this off to
	// the same functions that 'erg' mode uses to find the right servo position.

	// p_sim_forces->c is equal to A*Cw*Rho where A is effective frontal area (m^2); 
	// Cw is drag coefficent (unitless); and Rho is the air density (kg/m^3). 
	// The default value for A*Cw*Rho is 0.60.

	// Note that we add HEAD and subtract TAIL wind speed in the speed calc here.
	float wind = 0.5f * (m_resistance_state.c * pow((speed_mps + m_resistance_state.wind_speed_mps), 2));

	// Weight * GRAVITY * Co-efficient of rolling resistance.
	float rolling = (mp_user_profile->total_weight_kg / 100.0f) * GRAVITY * m_resistance_state.crr;

	// fGrade is the slope of the hill (slope = rise / run). Should be from -1.0 : 1.0, 
	// where -1.0 is a 45 degree downhill slope, 0.0 is flat ground, and 1.0 is a 45 
	// degree uphil slope. 
	float gravitational = (mp_user_profile->total_weight_kg / 100.0f) * GRAVITY *
			m_resistance_state.grade;

	// Determine the additional force required from the magnet if necessary.
	mag_watts = ( (wind + rolling + gravitational) - rr_force ) * speed_mps;

	/* If we have some wildly large force number returned, log what caused it.
	if (mag_watts > 1500.0f)
	{
		RC_LOG("[RC]:resistance_sim_set mag_force seems to high: %.2f\r\n", mag_force);
		RC_LOG("[RC]:rr_force: %.2f\r\n", rr_force);
		RC_LOG("[RC]:grade: %.2f\r\n", p_sim_forces->grade);
		RC_LOG("[RC]:gravitational: %.2f\r\n", gravitational);
		RC_LOG("[RC]:rolling: %.2f\r\n", rolling);
		RC_LOG("[RC]:wind_speed: %.2f\r\n", p_sim_forces->wind_speed_mps);
	}*/

	// No support for negative force.
	if (mag_watts < 0.0f)
	{
		mag_watts = 0.0f;
	}

	// Get the servo to the required position.
	return magnet_position(speed_mps, mag_watts);
}

/**@brief		Adjusts dynamic magnetic resistance control based on current
 * 				speed and watts.
 *
 */
void resistance_adjust(float speed_mps, int16_t magoff_watts)
{
	uint16_t servo_pos = 0;
	bool use_smoothing;

	if (speed_mps < RESISTANCE_MIN_SPEED_ADJUST)
		return;

	// If in erg or sim mode, destermine the magnet position.
	switch (m_resistance_state.mode)
	{
		case RESISTANCE_SET_ERG:
			servo_pos = resistance_erg_position(speed_mps, magoff_watts);
			break;

		case RESISTANCE_SET_SIM:
			servo_pos = resistance_sim_position(speed_mps, magoff_watts);
			break;

		default:
			RC_LOG("[RC] resistance_adjust: WARNING called with invalid resistance mode.\r\n");
			break;
	}

	// Only smooth out servo transition if in simulation mode.
	use_smoothing = (m_resistance_state.mode == RESISTANCE_SET_SIM);

	// Move the servo, with smoothing only if in sim mode.
	resistance_position_set(servo_pos, use_smoothing);
}

/**@brief		Sets erg mode with a watt target.  If 0, resumes last setting.
 * 				If no last setting, uses configured default.
 *
 */
void resistance_erg_set(uint16_t watts)
{
	// If watts == 0, use the last setting.
	if (watts == 0)
	{
		if (m_resistance_state.unadjusted_erg_watts == 0)
		{
			// If last setting is 0, use the default.
			watts = DEFAULT_ERG_WATTS;
		}
		else
		{
			watts = m_resistance_state.unadjusted_erg_watts;
		}
	}

	m_resistance_state.erg_watts = watts;
	m_resistance_state.unadjusted_erg_watts = watts;
	m_resistance_state.mode = RESISTANCE_SET_ERG;
}

/**@brief		Sets simulation grade.
 *
 */
void resistance_grade_set(float grade)
{
	m_resistance_state.grade = grade;
	m_resistance_state.mode = RESISTANCE_SET_SIM;
}

/**@brief		Sets simulation wind speed.
 *
 */
void resistance_windspeed_set(float windspeed_mps)
{
	m_resistance_state.wind_speed_mps = windspeed_mps;
	m_resistance_state.mode = RESISTANCE_SET_SIM;
}

/**@brief		Sets simulation coefficient of drag.
 *
 */
void resistance_c_set(float drag)
{
	m_resistance_state.c = drag;
	m_resistance_state.mode = RESISTANCE_SET_SIM;
}

/**@brief		Sets simulation coefficient of rolling resistance.
 *
 */
void resistance_crr_set(float crr)
{
	m_resistance_state.crr = crr;
	m_resistance_state.mode = RESISTANCE_SET_SIM;
}

/**@brief		Adjusts resistance +/- by either a step or % (erg mode).
 * 				If major step for standard, maxes resistance.
 * 				If minor step for standard, turns resistance off.
 *
 */
uint32_t resistance_step(bool increment, bool minor_step)
{
	uint32_t err_code = NRF_ERROR_INVALID_STATE;
	int8_t step;

	switch (m_resistance_state.mode)
	{
		case RESISTANCE_SET_STANDARD:
			// If incrementing, ensure we're not at the top.
			// If decrementing, ensure we're not at the bottom.
			if ( (increment && m_resistance_state.level < (RESISTANCE_LEVELS-1)) ||
					(!increment && m_resistance_state.level > 0) )
			{
				//
				// Increment or decrement by one for minor step, major step is to MIN or MAX.
				//
				if (increment)
				{
					step = minor_step ? (m_resistance_state.level + 1) :
							(RESISTANCE_LEVELS - 1);
				}
				else
				{
					step = minor_step ? (m_resistance_state.level - 1) : 0;
				}

				resistance_level_set(step);
				err_code = NRF_SUCCESS;
			}
			break;

		case RESISTANCE_SET_ERG:
			// If adjusting in erg mode, we keep this value around for the session.
			if (m_resistance_state.adjust_pct == 0)
			{
				// Set initial value to 100%
				m_resistance_state.adjust_pct = 100;
			}

			if (increment)
			{
				// Adjust by minor or major increment.
				step = m_resistance_state.adjust_pct + (minor_step ? ERG_ADJUST_MINOR :
						ERG_ADJUST_MAJOR);

				// Ensure we won't overflow as a result.
				if (m_resistance_state.adjust_pct < (UINT8_MAX - step))
				{
					m_resistance_state.adjust_pct = step;
				}
				else
				{
					// Otherwise set to max.
					m_resistance_state.adjust_pct = UINT8_MAX;
				}
			}
			else // decrement
			{
				step = m_resistance_state.adjust_pct - (minor_step ? ERG_ADJUST_MINOR :
						ERG_ADJUST_MAJOR);

				// Never go below the step change of ERG_ADJUST_MAJOR.
				if (step > ERG_ADJUST_MAJOR)
				{
					m_resistance_state.adjust_pct = step;
				}
				else
				{
					step = ERG_ADJUST_MAJOR;
				}
			}

			RC_LOG("[RC]:resistance_step erg adjust %i\r\n", m_resistance_state.adjust_pct);

			err_code = NRF_SUCCESS;
			break;

		default:
			break;
	}


	return err_code;
}
