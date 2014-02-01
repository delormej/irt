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
#include "resistance.h"
#include "nrf_pwm.h"
#include "math.h"

static user_profile_t 			*mp_user_profile = 0; 
static rc_sim_forces_t			*mp_sim_forces = 0;
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
		INIT_RESISTANCE();
		pwm_set_servo(RESISTANCE_LEVEL[level]);
		
		return RESISTANCE_LEVEL[level];
}

uint16_t set_resistance_pct(float percent)
{
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

/*
trainerSetSimMode:   (float)  fWeight 
rollingResistance:  (float)  fCrr 
windResistance:  (float)  fC  

Puts the trainer in Sim Mode. 

Sim Mode is used to simulate real world riding situations. This mode will adjust the brake resistance based on the effects of gravity, rolling resistance, and wind resistance. In order to creat an accurate simulation of real world conditions the following variables must be set: rider & bike weight, coefficient of rolling resistance, coefficient of wind resistance, wind speed, wheel circumference, and grade. If these variables are not set, they will default to an "average" value.
Note:IMPORTANT: the following parameters are set when this function is called; however, the remaining parameters can only be set after the trainer is put in Sim Mode.Parameters:
fWeight represents the weight of the combined rider and bicycle in kilograms. The default value for fWeight is 85.0kg. This parameter can not be adjusted without calling trainerSetSimMode again. 
fCrr is the coefficient of rolling resistance (unitless). Can be reset later by calling trainerSetRollingResistance. Default value is 0.004. 
fC is equal to A*Cw*Rho where A is effective frontal area (m^2); Cw is drag coefficent (unitless); and Rho is the air density (kg/m^3). The default value for A*Cw*Rho is 0.60. 


*/

// TODO: Future implementations.
uint16_t set_resistance_erg(uint16_t watts) 
{
/*
Puts the trainer in Resistance Mode. 

Resistance Mode will directly control the strength of the brake and will stay constant regardless of the rider's speed. This mode is similar to a spin bike where the user can increase or decrease the difficulty of their workout.
Parameters:
fpScale a float from 0.0 to 1.0 that represents the percentage the brake is turned on (0.0 = brake turned off; 0.256 = 25.6% of brake; 1.0 = 100% brake force). 
*/
};
uint16_t set_resistance_slope(float slope) // should be a float fGrade.
{
// fGrade is the slope of the hill (slope = rise / run). Should be from -1.0 : 1.0, where -1.0 is a 45 degree downhill slope, 0.0 is flat ground, and 1.0 is a 45 degree uphil slope. 

};
uint16_t set_resistance_wind(float wind) {};

void set_resistance_sim(user_profile_t *p_user_profile, rc_sim_forces_t *p_sim_forces)
{
	mp_user_profile = p_user_profile;
	mp_sim_forces = p_sim_forces;
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

static bool isSimModeSet()
{
	return (mp_user_profile != 0);
}

// Returns resistance 
static float windResistance(float speed)
{
	// need to calculate wind resistnace regardless of whether this gets set.
	// If you set ADDITIONAL wind resistance it could add / remove from this
	
	
}
