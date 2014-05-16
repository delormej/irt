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
#include "power.h"
#include "resistance.h"
#include "nrf_pwm.h"
#include "math.h"
#include "debug.h"

#define	GRAVITY				9.8f	// Co-efficent of gravity for Earth.  Change for other planets.
#define MIN_THRESHOLD_MOVE	3		// Minimum threshold for a servo move.

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

static uint16_t		m_servo_pos;		// State of current servo position.

uint16_t resistance_init(uint32_t servo_pin_number)
{
	// Initialize pulse-width-modulation.
	pwm_init(servo_pin_number);

	// Always start off with resistance at level 0.
	return resistance_level_set(0);
}

uint16_t resistance_position_get()
{
	return m_servo_pos;
}

/**@brief	Determines if there is a move and move accordingly.
 *
 */
uint16_t resistance_position_set(uint16_t position)
{
	if (m_servo_pos != position && ABOVE_TRESHOLD(position) )
	{
		RC_LOG("[RC]:SET_SERVO %i\r\n", position);
		pwm_set_servo(position);
		m_servo_pos = position;
	}

	return m_servo_pos;
}

uint16_t resistance_level_set(uint8_t level)
{
	// Sets the resistance to a standard 0-9 level.
	if (level >= MAX_RESISTANCE_LEVELS)
	{
		level = MAX_RESISTANCE_LEVELS - 1;
	}

	return resistance_position_set(RESISTANCE_LEVEL[level]);
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
		position = RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS-1];
	}
	else
	{
		// Calculate the difference between easiest and hardest positions.
		position = (uint16_t) (MIN_RESISTANCE_LEVEL -(
							(MIN_RESISTANCE_LEVEL-RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS-1]) *
							percent));
	}

	return resistance_position_set(position);
}

/**@brief		Sets mag resistance to simulate desired erg watts.
 * @returns 	Servo position.
 */
uint16_t resistance_erg_set(float speed_mps, float weight_kg, rc_sim_forces_t *p_sim_forces)
{
	int16_t mag0_watts;
	float mag0_force;
	float needed_force;
	uint16_t position;
	
	// TODO: who is going to handle calculating calibration? Whoever is, should keep track of 
	// current reporting watts, vs. adjusted watts?

	// TODO: since we already do this in the calculation, we should store this value
	// in p_power_meas instead of recalcuating here.
	// Calculate the current watts with no mag.
	mag0_watts = power_watts_calc(speed_mps,
			weight_kg,
			RESISTANCE_LEVEL[0]);	// mag 0 (off) position

	// Calculate mag0 force.
	mag0_force = ((float)mag0_watts) / speed_mps;
	
	// Calculate the incremental force required.
	needed_force = (((float)p_sim_forces->erg_watts) / speed_mps) - mag0_force;
	
	// Determine the correct servo position for that force given speed & weight.
	position = power_servo_pos_calc(weight_kg, speed_mps, needed_force);
								
	//
	// Ensure we don't move the servo beyond it's min and max.
	// NOTE: min/max are reversed on the servo; max is around 699, off is 2107 
	//
	if (position < RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS-1])
	{
		position = RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS-1];
	}
	else if (position > RESISTANCE_LEVEL[0])
	{
		position = RESISTANCE_LEVEL[0];
	}
	
	return resistance_position_set(position);
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
uint16_t resistance_sim_set(float speed_mps, float weight_kg, rc_sim_forces_t *p_sim_forces)
{
	// sim is going to calculate the estimated watts required at grade + wind for
	// the current speed and rider total weight.  It will then hand this off to
	// the same functions that 'erg' mode uses to find the right servo position.

	// p_sim_forces->c is equal to A*Cw*Rho where A is effective frontal area (m^2); 
	// Cw is drag coefficent (unitless); and Rho is the air density (kg/m^3). 
	// The default value for A*Cw*Rho is 0.60.
	float wind = 0.5f * (p_sim_forces->c * pow(speed_mps, 2));

	// Weight * GRAVITY * Co-efficient of rolling resistance.
	float rolling = weight_kg * GRAVITY * p_sim_forces->crr;

	// fGrade is the slope of the hill (slope = rise / run). Should be from -1.0 : 1.0, 
	// where -1.0 is a 45 degree downhill slope, 0.0 is flat ground, and 1.0 is a 45 
	// degree uphil slope. 
	float gravitational = weight_kg * GRAVITY *
							p_sim_forces->grade;

	// Total power required, which is rounded to nearest int for watts and assign.
	p_sim_forces->erg_watts = (int16_t)((wind + rolling + gravitational) * speed_mps);
	/*
	RC_LOG("[RC]:grade: %.2f\r\n", p_sim_forces->grade);
	RC_LOG("[RC]:speed: %.2f\r\n", speed_mps);
	RC_LOG("[RC]:rolling: %.2f\r\n", rolling);
	RC_LOG("[RC]:weight_kg: %.2f\r\n", weight_kg);
	RC_LOG("[RC]:slope force: %.2f\r\n", gravitational);
	RC_LOG("[RC]:watts: %i\r\n", p_sim_forces->erg_watts);
	*/

	// Same as erg mode now, set to a specific watt level.
	return resistance_erg_set(speed_mps, weight_kg, p_sim_forces);
}
