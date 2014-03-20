/*
*******************************************************************************
* 
* By Jason De Lorme <jjdelorme@yahoo.com>
* http://www.roadacious.com
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

#define	GRAVITY			9.8f

static bool 				m_initialized = false;

/**@brief 	Initializes the resistance object.
 *
 */
#define INIT_RESISTANCE()																											\
				do																																		\
				{																																			\
					if (!m_initialized) 																								\
						init_resistance();																								\
				} while (0)																																								

static void init_resistance() 
{
	// TODO: get the servo signal passed in during this method and remove
	// dependency on the hard linking here.  The desire is to have all 
	// hw dependencies in irt_peripheral instead of scattered through code
	// base.
	pwm_init(PIN_SERVO_SIGNAL);	
	m_initialized = true;
}

uint16_t resistance_level_set(uint8_t level)
{
	// Sets the resistance to a standard 0-9 level.
	INIT_RESISTANCE();
	if (level >= MAX_RESISTANCE_LEVELS)
	{
		level = MAX_RESISTANCE_LEVELS - 1;
	}

	pwm_set_servo(RESISTANCE_LEVEL[level]);

	return RESISTANCE_LEVEL[level];
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

	INIT_RESISTANCE();
	uint16_t position = 0;

	if (percent == 0.0f)
	{
		pwm_set_servo(RESISTANCE_LEVEL[0]);
		position = RESISTANCE_LEVEL[0];
	}
	else if (percent > 99.9f)
	{
		pwm_set_servo(RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS-1]);
		position = RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS-1];
	}
	else
	{
		// Calculate the difference between easiest and hardest positions.
		position = (uint16_t) (MIN_RESISTANCE_LEVEL -(
															(MIN_RESISTANCE_LEVEL-RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS-1]) *
															percent));
		
		pwm_set_servo(position);
	}

	return position;
}

/**@brief			Sets mag resistance to simulate desired erg watts.
 * @returns 	Servo position.
 */
uint16_t resistance_erg_set(float speed_mps, float weight_kg, uint16_t servo_pos,
		rc_sim_forces_t *p_sim_forces)
{
	int16_t mag0_watts;
	float mag0_force;
	float needed_force;
	uint16_t servo_position;
	
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
	servo_position = power_servo_pos_calc(weight_kg, speed_mps, needed_force);
								
	//
	// Ensure we don't move the servo beyond it's min and max.
	// NOTE: min/max are reversed on the servo; max is around 699, off is 2107 
	//
	if (servo_position < RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS-1])
	{
		servo_position = RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS-1];
	}
	else if (servo_position > RESISTANCE_LEVEL[0])
	{
		servo_position = RESISTANCE_LEVEL[0];
	}
	
	pwm_set_servo(servo_position);

#if defined(BLE_NUS_ENABLED)
		static const char format[] = "s,0f,fn,sp:%f,%f,%f,%i";
		char message[32];
		memset(&message, 0, sizeof(message));
		uint8_t length = sprintf(message, format,
								speed_mps,
								mag0_force,
								needed_force,
								servo_position);
		debug_send(message, sizeof(message));
#endif

	return servo_position;
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
uint16_t resistance_sim_set(float speed_mps, float weight_kg, uint16_t servo_pos,
		rc_sim_forces_t *p_sim_forces)
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
	p_sim_forces->erg_watts = (wind + rolling + gravitational) * speed_mps;
	
	// Same as erg mode now, set to a specific watt level.
	return resistance_erg_set(speed_mps, weight_kg, servo_pos, p_sim_forces);
}
