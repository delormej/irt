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
#include "app_timer.h"
#include "math_private.h"
#include "magnet.h"
#include "resistance.h"
#include "nrf_pwm.h"
#include "speed.h"
#include "ant_bike_power.h"
#include "power.h"
#include "simulation.h"
#include <math.h>
#include "debug.h"

#define MIN_THRESHOLD_MOVE		2		// Minimum threshold for a servo move (in servo positions)

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
#define ABOVE_TRESHOLD(MOVE_POS, CURRENT_POS)										\
		((MOVE_POS - CURRENT_POS) > MIN_THRESHOLD_MOVE || 			\
				(CURRENT_POS - MOVE_POS > MIN_THRESHOLD_MOVE))		\

// MACRO for getting to the array.
#define RESISTANCE_LEVEL 	mp_user_profile->servo_positions.positions

/**@brief  Timeout for adjusting resistance when in erg/sim mode.
 */
#define ADJUSTMENT_INTERVAL		APP_TIMER_TICKS(mp_user_profile->power_adjust_seconds * 1000, APP_TIMER_PRESCALER) // Defaults to 5 seconds.
static app_timer_id_t			m_adjust_timer_id;						  // Timer used to invoke resistance adjustments in erg/sim mode.

static user_profile_t* 			mp_user_profile;
static irt_context_t*			mp_current_state;
static irt_resistance_state_t	m_resistance_state;

/**@brief Function for adjusting resistance on timer timeout.
 *
 * @details This function will be called each timer expiration.  
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void adjustment_timeout_handler(void * p_context)
{
	resistance_adjust();
}

static inline bool adjustment_timer_in_use()
{
	return ( ADJUSTMENT_INTERVAL > 0 && 
		(m_resistance_state.mode == RESISTANCE_SET_ERG || 
			m_resistance_state.mode == RESISTANCE_SET_SIM) );
}

/**@brief	Initializes the resistance module which controls the servo.
 *
 */
irt_resistance_state_t* resistance_init(uint32_t servo_pin_number, 
	user_profile_t* p_user_profile, irt_context_t* p_current_state)
{
	mp_user_profile = p_user_profile;
	mp_current_state = p_current_state;

	memset(&m_resistance_state, 0, sizeof(m_resistance_state));

	 /*	fCrr is the coefficient of rolling resistance (unitless). Default value is 0.004.
	 *
	 *	fC is equal to A*Cw*Rho where A is effective frontal area (m^2); Cw is drag
	 *	coefficent (unitless); and Rho is the air density (kg/m^3). The default value
	 *	for A*Cw*Rho is 0.60.
	 */
	m_resistance_state.crr = DEFAULT_CRR;
	m_resistance_state.c = SIM_C;
    m_resistance_state.drafting_factor = SIM_DRAFTING_FACTOR;

	// Initialize pulse-width-modulation.
	pwm_init(servo_pin_number);

	// Always start off with resistance at level 0.
	resistance_level_set(0);

    // Timer set to interval to re-adjust in erg/sim mode based.
    uint32_t err_code = app_timer_create(&m_adjust_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                adjustment_timeout_handler);
	APP_ERROR_CHECK(err_code);

	// Return the state object.
	return &m_resistance_state;
}

/**@brief		Gets the current user friendly magnet position, 
 * 				integrating any servo offset.
 */
uint16_t resistance_servo_position()
{
	uint16_t raw_servo_position = pwm_get_servo_position();
	if (raw_servo_position > 0)
		return raw_servo_position - mp_user_profile->servo_offset;
	else
		return MAGNET_POSITION_OFF;
	}
	
/**@brief		Gets the current resistance state object.
 *
 */
irt_resistance_state_t* resistance_state_get(void)
{
	// TODO: Should this return an immutable object to enforce all state changes
	// happen in this module? 
	return &m_resistance_state;
}

/**@brief	Determines if there is a move and move accordingly.
 *
 */
uint16_t resistance_position_set(uint16_t servo_pos, uint8_t smooth_steps)
{
	uint32_t err_code;
	// Actual servo position after calibration.
	uint16_t actual_servo_pos;
	uint16_t current_servo_pos = resistance_servo_position();

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
	if (servo_pos != MAGNET_POSITION_MIN_RESISTANCE &&
		(servo_pos + mp_user_profile->servo_offset) > MAGNET_POSITION_OFF)
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
	if ( (current_servo_pos != servo_pos) 
		&& ABOVE_TRESHOLD(servo_pos, current_servo_pos) )
	{
		// Issue a command to move the servo.
		err_code = pwm_set_servo(actual_servo_pos, smooth_steps);
		APP_ERROR_CHECK(err_code);

		RC_LOG("[RC]:SET_SERVO %i\r\n", actual_servo_pos);
	}

	return current_servo_pos;
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
	return resistance_position_set(RESISTANCE_LEVEL[level], 0);
}

/**@brief		Gets the levels of standard resistance available.
  */
uint8_t resistance_level_count()
{
	return mp_user_profile->servo_positions.count;
}

/**@brief		Gets the percentage of total resistance at a given servo position.
 * @returns		Percentage of maximum resistance (Units: 0.5%, Range: 0-100%).
 */
uint8_t resistance_pct_get(uint16_t position)
{
	float resistance_pct = 0.0f;
    
    if (position != MAGNET_POSITION_OFF)
    {
        // Subtract position from min position, divide by full range,
        // multiply by 200 to remove the decimal and represent units of 0.5%.
        resistance_pct = 200 * (  
            (float)((MAGNET_POSITION_MIN_RESISTANCE - position) /
            (float)(MAGNET_POSITION_MIN_RESISTANCE - MAGNET_POSITION_MAX_RESISTANCE)));
    }	
    
	return (uint8_t)resistance_pct;
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

    RC_LOG("[RC] resistance_pct_set: %i\r\n", percent * 100);
	return resistance_position_set(position, 0);
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

	return magnet_position(speed_mps, mag_watts, &m_resistance_state.power_limit);
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
	float total_watts = simulation_watts(speed_mps, mp_user_profile->total_weight_kg, 
		&m_resistance_state);
	// remove the existing watts we have.
	float mag_watts = total_watts - magoff_watts; 
	// No support for negative force.
	if (mag_watts < 0.0f)
		mag_watts = 0.0f;

	// Get the servo to the required position.
	return magnet_position(speed_mps, mag_watts, &m_resistance_state.power_limit);
}

static float calc_avg_magoff_watts(uint8_t seconds)
{
	float magoff_watts = 0.0f;
	if (mp_current_state->power_meter_paired) // Paired to a power meter..
	{
		float average_power = ant_bp_avg_power(seconds); 
		float mag_watts = magnet_watts(speed_mps, resistance_servo_position());
		
		// Take actual power, remove estimated watts from magnet.
		magoff_watts = average_power - mag_watts;
	}
	else
	{
		// Calculate estimated power without the magnet force.
		magoff_watts = power_magoff(speed_mps);
	}	

	return magoff_watts;
}

static void adjust_to_target(resistance_mode_t mode, float speed_mps, float magoff_watts)
{
	uint16_t servo_pos = 0;
	uint8_t smoothing_steps = 0;

	if (speed_mps < RESISTANCE_MIN_SPEED_ADJUST)
    {
        m_resistance_state.power_limit = TARGET_SPEED_TOO_LOW;
		// Move magnet to the min position, as it will be tough for rider to overcome
		// magnet force from a slow speed.
		resistance_position_set(MAGNET_POSITION_MIN_RESISTANCE, 0);
		return;
    }

	if (mode == RESISTANCE_SET_ERG)
	{
		servo_pos = resistance_erg_position(speed_mps, magoff_watts);	
		// If recovering from a speed too low event, smooth into the position.
		if (m_resistance_state.power_limit == TARGET_SPEED_TOO_LOW) 
			smoothing_steps = DEFAULT_SMOOTHING_STEPS;		
	}
	else if (mode == RESISTANCE_SET_SIM)
	{
		servo_pos = resistance_sim_position(speed_mps, magoff_watts);
		smoothing_steps = mp_user_profile->servo_smoothing_steps;
	}
	else
	{
		RC_LOG("[RC] resistance_adjust: WARNING called with invalid resistance mode.\r\n");
		return;
	}

	// Move the servo, with smoothing only if in sim mode.
	resistance_position_set(servo_pos, smoothing_steps);
}

/**@brief		Adjusts dynamic magnetic resistance control based on current
 * 				speed and watts, for erg/sim mode.
 *
 */
static void resistance_adjust()
{
	// Stop the timer if we were in erg/sim mode, regardless of whether
	// adjustment interval is 0 or not, it will not error if timer was
	// never started.  This guarantees that timer will stop even if user
	// changed the adjust timeout to 0 after a timer already started (possible case).
	app_timer_stop(m_adjust_timer_id);

	float speed_mps = speed_average_mps(mp_user_profile->power_average_seconds);
	float magoff_watts = calc_avg_magoff_watts(mp_user_profile->power_average_seconds);
	adjust_to_target(m_resistance_state.mode, speed_mps, magoff_watts);
	if (adjustment_timer_in_use())
		// Start the timer for next resistance adjustment.
		APP_ERROR_CHECK(app_timer_start(m_adjust_timer_id, ADJUSTMENT_INTERVAL, NULL));
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

	if (m_resistance_state.mode != RESISTANCE_SET_ERG ||
		m_resistance_state.erg_watts != watts)
	{
		m_resistance_state.mode = RESISTANCE_SET_ERG;
		m_resistance_state.erg_watts = watts;
		m_resistance_state.unadjusted_erg_watts = watts;
		resistance_adjust();
	}
}

/**@brief		Sets simulation grade.
 *				Range is -2.0 - +2.0.  E.g. 6% grade is 0.06.
 *
 */
void resistance_grade_set(float grade)
{
	bool dirty = false;
	if (grade > 2.0 || grade < -2.0)
	{
		RC_LOG("[RC]:Grade out of range: %.2f\r\n", grade);
		return; //APP_ERROR_CHECK(NRF_ERROR_INVALID_PARAM);
	}
	
	if (m_resistance_state.mode != RESISTANCE_SET_SIM)
	{
		m_resistance_state.mode = RESISTANCE_SET_SIM;
		m_resistance_state.grade = grade;
		resistance_adjust();
	}
	else if (m_resistance_state.grade != grade)
	{
		m_resistance_state.grade = grade;
		if (ADJUSTMENT_INTERVAL == 0)
			resistance_adjust();
	}
}

/**@brief		Sets simulation wind speed.
 *
 */
void resistance_windspeed_set(float wind_speed_mps)
{
	if (m_resistance_state.mode != RESISTANCE_SET_SIM)
	{
		m_resistance_state.mode = RESISTANCE_SET_SIM;
		m_resistance_state.wind_speed_mps = wind_speed_mps;
		resistance_adjust();
	}
	else if (m_resistance_state.wind_speed_mps != wind_speed_mps)
	{
		m_resistance_state.wind_speed_mps = wind_speed_mps;
		if (ADJUSTMENT_INTERVAL == 0)
			resistance_adjust();
	}
}

/**@brief		Sets simulation coefficient of rolling resistance.
 *
 *@remarks      The drafting factor scales the total wind resistance depending 
 *              on the position of the user relative to other virtual competitors. 
 *              The drafting scale factor ranges from 0.0 to 1.0, where 0.0 removes
 *              all air resistance from the simulation, and 1.0 indicates no drafting
 *              effects (e.g. cycling alone, or in the lead of a pack). 
 */
void resistance_drafting_set(float factor)
{
	if (m_resistance_state.mode != RESISTANCE_SET_SIM)
	{
		m_resistance_state.mode = RESISTANCE_SET_SIM;
		m_resistance_state.drafting_factor = factor;
		resistance_adjust();
	}
	else if (m_resistance_state.factor != factor)
	{
		m_resistance_state.drafting_factor = factor;
		if (ADJUSTMENT_INTERVAL == 0)
			resistance_adjust();
	}
}

/**@brief		Sets simulation coefficient of drag.
 *
 */
void resistance_c_set(float drag)
{
	m_resistance_state.c = drag;
}

/**@brief		Sets simulation coefficient of rolling resistance.
 *
 */
void resistance_crr_set(float crr)
{
	m_resistance_state.crr = crr;
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
			resistance_adjust();

			err_code = NRF_SUCCESS;
			break;

		default:
			break;
	}


	return err_code;
}
