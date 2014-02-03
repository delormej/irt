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

#define	GRAVITY				9.8f

static bool 								m_initialized = false;

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

//
// Calculates the desired servo position given speed in mps, weight in kg
// and additional needed force in newton meters.
//
static uint16_t calc_servo_pos(float weight_kg, float speed_mps, float force_needed)
{
#define SERVO_FORCE_A_SLOPE 			-24.66803762f
#define SERVO_FORCE_A_INTERCEPT 	1489.635063f
#define SERVO_FORCE_A_SPEED			 	8.94f
#define SERVO_FORCE_B_SLOPE 			-26.68991676f
#define SERVO_FORCE_B_INTERCEPT 	1468.153562f
#define SERVO_FORCE_B_SPEED			 	4.47f

	float value = (force_needed * SERVO_FORCE_A_SLOPE + SERVO_FORCE_A_INTERCEPT) -
		(((force_needed * SERVO_FORCE_A_SLOPE + SERVO_FORCE_A_INTERCEPT) -
		(force_needed * SERVO_FORCE_B_SLOPE + SERVO_FORCE_B_INTERCEPT)) /
		SERVO_FORCE_A_SPEED - SERVO_FORCE_B_SPEED)*(SERVO_FORCE_A_SPEED - speed_mps);

	// Round the position.
	uint16_t servo_pos = (uint16_t) value; //  ceil(value);

	// Enforce min/max position.
	if (servo_pos > RESISTANCE_LEVEL[0])
		servo_pos = RESISTANCE_LEVEL[0];
	else if (servo_pos < RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS - 1])
		servo_pos = RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS - 1];

	return servo_pos;
}

uint16_t set_resistance(uint8_t level)
{
		// Sets the resistance to a standard 0-9 level.
		INIT_RESISTANCE();
		pwm_set_servo(RESISTANCE_LEVEL[level]);
		
		return RESISTANCE_LEVEL[level];
}

uint16_t set_resistance_pct(float percent)
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
uint16_t set_resistance_erg(user_profile_t *p_user_profile, 
												rc_sim_forces_t *p_sim_forces,
												irt_power_meas_t *p_power_meas)
{
	int16_t mag0_watts = 0;
	uint16_t servo_position = 0;
	

	// TODO: who is going to handle calculating calibration? Whoever is, should keep track of 
	// current reporting watts, vs. adjusted watts?

	// TODO: since we already do this in the calculation, we should store this value
	// in p_power_meas instead of recalcuating here.
	// Calculate the current watts with no mag.
	calc_power2(p_power_meas->instant_speed_mps, 
							p_user_profile->total_weight_kg,
							RESISTANCE_LEVEL[0],	// mag 0 (off) position 
							&mag0_watts);

	// Calculate mag0 force.
	float mag0_force = ((float)mag0_watts) / 
										p_power_meas->instant_speed_mps;
	
	// Calculate the incremental force required.
	float needed_force = (((float)p_sim_forces->erg_watts) / 
										p_power_meas->instant_speed_mps) - mag0_force;
	
	// Determine the correct servo position for that force given speed & weight.
	servo_position = calc_servo_pos(p_user_profile->total_weight_kg, 
								p_power_meas->instant_speed_mps,
								needed_force);
								
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
uint16_t set_resistance_sim(user_profile_t *p_user_profile, 
												rc_sim_forces_t *p_sim_forces,
												irt_power_meas_t *p_power_meas)
{
	// sim is going to calculate the estimated watts required at grade + wind for
	// the current speed and rider total weight.  It will then hand this off to
	// the same functions that 'erg' mode uses to find the right servo position.

	// p_sim_forces->c is equal to A*Cw*Rho where A is effective frontal area (m^2); 
	// Cw is drag coefficent (unitless); and Rho is the air density (kg/m^3). 
	// The default value for A*Cw*Rho is 0.60.
	float wind = 0.5f * (p_sim_forces->c * pow(p_power_meas->instant_speed_mps, 2));

	// Weight * GRAVITY * Co-efficient of rolling resistance.
	float rolling = p_user_profile->total_weight_kg * GRAVITY * p_sim_forces->crr;

	// fGrade is the slope of the hill (slope = rise / run). Should be from -1.0 : 1.0, 
	// where -1.0 is a 45 degree downhill slope, 0.0 is flat ground, and 1.0 is a 45 
	// degree uphil slope. 
	float gravitational = p_user_profile->total_weight_kg * GRAVITY * 
							p_sim_forces->grade;

	// Total power required.
	float force = wind + rolling + gravitational; 
	
	// Round to nearest int for watts and assign.
	int16_t watts = force * p_power_meas->instant_speed_mps;
	p_sim_forces->erg_watts = watts;

	// Same as erg mode now, set to a specific watt level.
	return set_resistance_erg(p_user_profile, 
									p_sim_forces,
									p_power_meas);

	return 0;  // IRT_SUCCESS
}


// TODO: who is going to maintain responsibility for adjusting servo position
// every second when calculating power.
// Power is going to calculate current power based on servo pos + speed.
// it never needs to know about the additional forces other than weight and wheel circ (user_profile).
// Resistance module on the other hand does the virtual calculations of how
// many watts those forces should create and finds the appropriate servo position.

// Resistance module doesn't need to know how to parse messages from the KICKR
// because main needs to know in order to figure out setting wheel size and also
// weight.  Why not keep it there, or create a module with KICKR specific messages
// tihs may be the way you accomplish in the future to abstract the trainer device
// profile anyways.

/*
        protected double GravitationalForces(double gradient)
        {
            double value = _forces.TotalWeightKg * Conversion.GRAVITY * gradient;

            return value;
        }

        protected double RollingResistance()
        {
            double value = _forces.TotalWeightKg * Conversion.GRAVITY *
                _forces.RollingResistanceCoefficient;

            return value;
        }

        public double Calculate(Trackpoint dataPoint)
        {
            double value = dataPoint.Speed * (
                    WindResistance(dataPoint.Speed) +
                    RollingResistance() +
                    GravitationalForces(dataPoint.Gradient)
                    );
        }

        protected double WindResistance(double speed)
        {
            double value = 0.5d *
                (_forces.FrontalArea * _forces.AirDensity * _forces.DragCoefficient *
                Math.Pow(speed, 2));

            return value;
        }

				Forces forces = new Forces()
                {
                    DragCoefficient = 0.55,
                    AirDensity = 1.226,
                    FrontalArea = 0.5,
                    RollingResistanceCoefficient = 0.004,
                };


*/
