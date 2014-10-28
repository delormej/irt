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

#include "irt_peripheral.h"
#include "app_error.h"
#include "power.h"
#include "resistance.h"
#include "nrf_pwm.h"
#include "math.h"
#include "debug.h"

#define MIN_THRESHOLD_MOVE	2		// Minimum threshold for a servo move.
#define MIN_SERVO_RANGE		699		// Defined spec for servo is between 2ms and 1ms, but we have a little legacy where we were setting to 699 - this should be eliminated.
#define MAX_SERVO_RANGE		2000

#ifdef KURT
#include "irt_adc.h"
#include "nrf_delay.h"

#define AIN_SLIDE_POT	ADC_CONFIG_PSEL_AnalogInput2	// P0.01 AIN2 (pin J7-4 on board)
#define MAX_ADC_READ	938								// See explanation in method below (3.3f {Vout} / 3.6f {VBG*3} * 1024 {10bit})
#define MIN_ADC_READ	1

static int16_t m_target_servo_pos = 0;

#endif // KURT

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
		((POS - m_servo_pos) > MIN_THRESHOLD_MOVE || 			\
				(m_servo_pos - POS > MIN_THRESHOLD_MOVE))		\

// MACRO for getting to the array.
#define RESISTANCE_LEVEL 	mp_user_profile->servo_positions.positions

static uint16_t	m_servo_pos;		// State of current servo position.
static user_profile_t* mp_user_profile;

/**@brief	Sets the servo by specifying magnet force required.
 */
static uint16_t position_set_by_force(float mag_force)
{
	uint16_t servo_pos;

	// Determine the required servo position for desired watts.
	servo_pos = power_servo_pos_calc(mag_force);

	// Move the servo.
	return resistance_position_set(servo_pos);
}

#ifdef KURT
static void on_adc_result(uint16_t result)
{
	static uint16_t count;

	uint32_t retry = 0;
	m_servo_pos = result;

	// Every once in a while report out.
	if (count++ % 1024 == 0)
	{
		RC_LOG("[RC] on_adc_result: %i\r\n", result);
	}

	// ****
	// TODO: FIX as this doesn't handle when you OVERSHOOT the goal.
	// ****

	if (result >= MAX_ADC_READ || result <= MIN_ADC_READ ||
			result == m_target_servo_pos)
	{
		// Clear target.
		m_target_servo_pos = 0;

		// Stop the servo.
		pwm_continuous_servo(0);
	}
	else
	{
		// TODO: check recursion here.
		resistance_position_set(m_target_servo_pos);
	}
}
#endif


/**@brief	Initializes the resistance module which controls the servo.
 */
uint16_t resistance_init(uint32_t servo_pin_number, user_profile_t* p_user_profile)
{
	mp_user_profile = p_user_profile;

	// Initialize pulse-width-modulation.
	pwm_init(servo_pin_number);

#ifdef KURT
	RC_LOG("[RC] resistance_init\r\n");

	// Initialize the ADC for our purposes.
    uint32_t config     = (ADC_CONFIG_RES_10bit                        		<< ADC_CONFIG_RES_Pos)     |
                          (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling	<< ADC_CONFIG_INPSEL_Pos)  |
                          (ADC_CONFIG_REFSEL_VBG							<< ADC_CONFIG_REFSEL_Pos)  |
						  (AIN_SLIDE_POT							  		<< ADC_CONFIG_PSEL_Pos) |
						  (ADC_CONFIG_EXTREFSEL_None                  		<< ADC_CONFIG_EXTREFSEL_Pos);

    irt_adc_init(config, on_adc_result);

	// TODO: Hard coded to return 2,000 - fix this.
	m_servo_pos = 2000;

	return m_servo_pos;
#else
	// Always start off with resistance at level 0.
	return resistance_level_set(0);
#endif // KURT
}

/**@brief	Returns the last known position of the servo.
 */
uint16_t resistance_position_get()
{
	return m_servo_pos;
}

/**@brief	Determines if there is a move and move accordingly.
 *
 */
uint16_t resistance_position_set(uint16_t servo_pos)
{
#ifdef KURT

	/* NOTE: the range with a 10bit ADC is 1-939.  This is because we're referencing against
	 * 1.2VBG and doing 1/3 pre-scaling.  The voltage off the pin is 3.3V, thus:
	 * 		3.3 / 3.6 = 0.9167 * 1024 = 938.7008
	 */
	if (servo_pos > MAX_ADC_READ)
	{
		servo_pos = MAX_ADC_READ;
		RC_LOG("[RC] resistance_position_set using MAX, servo_pos too high: %i\r\n", servo_pos);
	}
	else if (servo_pos == 0)
	{
		servo_pos = MIN_ADC_READ;
		RC_LOG("[RC] resistance_position_set using MIN, servo_pos can't be 0.\r\n");
	}


	if (m_target_servo_pos == 0)
	{
		//
		// TODO: Handle situation when we get an OVERRIDE call, move to another position
		// before the first time was called.
		//
		if (m_servo_pos == servo_pos)
		{
			RC_LOG("[RC] resistance_position_set already at position: %i\r\n", servo_pos);
			return m_servo_pos;
		}

		// Record the target
		m_target_servo_pos = servo_pos;

		// First time we're being called to set a position.
		RC_LOG("[RC] resistance_position_set setting target pos: %i\r\n", servo_pos);

		if (m_servo_pos > m_target_servo_pos)
		{
			// issue reverse
			pwm_continuous_servo(-100);
		}
		else
		{
			// issue forward
			pwm_continuous_servo(100);
		}
	}
	else
	{
		// TODO:
		// We're being called from the adc handler.
		// Check here if we overshot?
	}

	// Read the adc, all the magic happens in on_adc_result
	// TODO: yeah... lots to do, fix this so that it doesn't sit & loop indefinitely potentially.
	if (irt_adc_start() == NRF_ERROR_BUSY)
	{
		RC_LOG("[RC] resistance_position_set adc is busy.\r\n");
		// TODO: raise an error, retry, etc...
	}

	// Just return where we know we are for now.
	return m_servo_pos;
#else
	uint32_t err_code;
	// Actual servo position after calibration.
	uint16_t actual_servo_pos;
	//
	// Ensure we don't move the servo beyond it's min and max.
	// NOTE: min/max are reversed on the servo; max is around 699, off is 2107
	//
	if (servo_pos < RESISTANCE_LEVEL[RESISTANCE_LEVELS-1])
	{
		servo_pos = RESISTANCE_LEVEL[RESISTANCE_LEVELS-1];
	}
	else if (servo_pos > RESISTANCE_LEVEL[0])
	{
		servo_pos = RESISTANCE_LEVEL[0];
	}

	if ( (m_servo_pos != servo_pos) )// && ABOVE_TRESHOLD(servo_pos) )
	{
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
		actual_servo_pos = ACTUAL_SERVO_POS(servo_pos);

		// Using offset, guard to acceptable range of 2000-1000
		if (actual_servo_pos > MAX_SERVO_RANGE)
		{
			actual_servo_pos = MAX_SERVO_RANGE;
		}
		else if (actual_servo_pos < MIN_SERVO_RANGE)
		{
			actual_servo_pos = MIN_SERVO_RANGE;
		}

		err_code = pwm_set_servo(actual_servo_pos);
		APP_ERROR_CHECK(err_code);

		m_servo_pos = servo_pos;
		RC_LOG("[RC]:SET_SERVO %i\r\n", actual_servo_pos);
	}
#endif // KURT
	return m_servo_pos;
}

/**@brief		Gets the minimum (magnet off) resistance position.
 */
uint16_t resistance_position_min(void)
{
	return RESISTANCE_LEVEL[0];
}

/**@brief 		Gets the maximum resistance position.
 */
uint16_t resistance_position_max(void)
{
	return RESISTANCE_LEVEL[RESISTANCE_LEVELS-1];
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
		if (positions->positions[index] == MAX_SERVO_RANGE)
			continue;

		value = ACTUAL_SERVO_POS(positions->positions[index]);
		if (value > MAX_SERVO_RANGE || value < MIN_SERVO_RANGE)
		{
			RC_LOG("[RC] resistance_positions_validate adjusted value invalid: %i\r\n", value);
			return false;
		}
	} while (++index < positions->count);

	return true;
}

uint16_t resistance_level_set(uint8_t level)
{
	/*RC_LOG("[RC] resistance_level_set: %i, max: %i\r\n",
			level, RESISTANCE_LEVELS);*/

	// Sets the resistance to a standard 0-9 level.
	if (level >= RESISTANCE_LEVELS)
	{
		level = RESISTANCE_LEVELS - 1;
	}

	return resistance_position_set(RESISTANCE_LEVEL[level]);
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

	if (percent == 0.0f)
	{
		position = RESISTANCE_LEVEL[0];
	}
	else if (percent > 99.9f)
	{
		position = RESISTANCE_LEVEL[RESISTANCE_LEVELS-1];
	}
	else
	{
		// Calculate the difference between easiest and hardest positions.
		position = (uint16_t) (MIN_RESISTANCE_LEVEL -(
							(MIN_RESISTANCE_LEVEL-RESISTANCE_LEVEL[RESISTANCE_LEVELS-1]) *
							percent));
	}

	return resistance_position_set(position);
}

/**@brief		Sets mag resistance to simulate desired erg watts.
 * @returns 	Servo position.
 */
static uint16_t resistance_erg_set(int16_t target_watts, float speed_mps, float rr_force)
{
	float mag_force;

	//
	// Calculate the required incremental magnet force required (if any).
	// By finding the total force required at current speed and backing
	// out the baseline force of rolling resistance.
	//
	// TODO: We could get smarter here and deal with 'erg-ing out' or if the user
	// stops pedaling deal with them starting back up.
	//
	mag_force = ( (((float)target_watts) / speed_mps) - rr_force );

	return position_set_by_force(mag_force);
}

/**@brief	Puts the trainer in simulation mode.
 * @note 	Sim Mode is used to simulate real world riding situations. This mode 
 *				will adjust the brake resistance based on the effects of gravity, 
 *				rolling resistance, and wind resistance. In order to create an accurate 
 *				simulation of real world conditions the following variables must be set: 
 *				rider & bike weight, coefficient of rolling resistance, coefficient of 
 *				wind resistance, wind speed, wheel circumference, and grade. 
 *				If these variables are not set, they will default to an "average" value.
 */
static uint16_t resistance_sim_set(float speed_mps, rc_sim_forces_t *p_sim_forces, float rr_force)
{
	float mag_force;

	// sim is going to calculate the estimated watts required at grade + wind for
	// the current speed and rider total weight.  It will then hand this off to
	// the same functions that 'erg' mode uses to find the right servo position.

	// p_sim_forces->c is equal to A*Cw*Rho where A is effective frontal area (m^2); 
	// Cw is drag coefficent (unitless); and Rho is the air density (kg/m^3). 
	// The default value for A*Cw*Rho is 0.60.

	// Note that we add HEAD and subtract TAIL wind speed in the speed calc here.
	float wind = 0.5f * (p_sim_forces->c * pow((speed_mps + p_sim_forces->wind_speed_mps), 2));

	// Weight * GRAVITY * Co-efficient of rolling resistance.
	float rolling = (mp_user_profile->total_weight_kg / 100.0f) * GRAVITY * p_sim_forces->crr;

	// fGrade is the slope of the hill (slope = rise / run). Should be from -1.0 : 1.0, 
	// where -1.0 is a 45 degree downhill slope, 0.0 is flat ground, and 1.0 is a 45 
	// degree uphil slope. 
	float gravitational = (mp_user_profile->total_weight_kg / 100.0f) * GRAVITY *
							p_sim_forces->grade;

	// Determine the additional force required from the magnet if necessary.
	mag_force = ( (wind + rolling + gravitational) - rr_force );

	RC_LOG("[RC]:resistance_sim_set: mag_force: %.2f\r\n", mag_force);
	/*
	RC_LOG("[RC]:rr_force: %.2f\r\n", rr_force);
	RC_LOG("[RC]:grade: %.2f\r\n", p_sim_forces->grade);
	RC_LOG("[RC]:gravitational: %.2f\r\n", gravitational);
	RC_LOG("[RC]:rolling: %.2f\r\n", rolling);
	RC_LOG("[RC]:wind_speed: %.2f\r\n", p_sim_forces->wind_speed_mps);
	 */

	// No support for negative force.
	if (mag_force < 0.0f)
	{
		mag_force = 0.0f;
	}

	// Move the servo to the required position.
	return position_set_by_force(mag_force);
}

/**@brief	Adjusts the magnetic resistance accordingly for erg & sim modes.
 *
 */
void resistance_adjust(irt_power_meas_t* p_power_meas_first,
		irt_power_meas_t* 	p_power_meas_current,
		rc_sim_forces_t* 	p_sim_forces,
		resistance_mode_t 	resistance_mode,
		float 				rr_force)
{
	float speed_avg;

	// If we have a range of events between first and current, we're able to do a moving average of speed.
	if (p_power_meas_first != NULL)
	{
		// Average the speed.
		speed_avg = ( (p_power_meas_current->instant_speed_mps +
				p_power_meas_first->instant_speed_mps) / 2.0f );
	}
	else
	{
		speed_avg = p_power_meas_current->instant_speed_mps;
	}

	// Don't attempt to adjust if stopped or going too slow.
	if (speed_avg < 2.0f)
	{
		RC_LOG("[RC] resistance_adjust: not adjusting, too slow: %.2f \r\n",
				p_power_meas_current->instant_speed_mps);
		return;
	}

	// If in erg or sim mode, adjust resistance accordingly.
	switch (resistance_mode)
	{
		case RESISTANCE_SET_ERG:
			resistance_erg_set(p_sim_forces->erg_watts, speed_avg, rr_force);
			break;

		case RESISTANCE_SET_SIM:
			resistance_sim_set(speed_avg, p_sim_forces, rr_force);
			break;

		default:
			break;
	}
}

