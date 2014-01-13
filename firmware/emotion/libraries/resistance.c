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

#include "resistance.h"
#include "nrf_pwm.h"
#include "math.h"

bool m_initialized = false;

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

void set_resistance(uint8_t level)
{
		INIT_RESISTANCE();
		pwm_set_servo(RESISTANCE_LEVEL[level]);
}

void set_resistance_pct(uint16_t percent)
{
	return;
	// NOT FUNCTIONING YET.
		INIT_RESISTANCE();
		if (percent == 0)
		{
			set_resistance(0);
			return;
		}
		else if (percent > 99)
		{
			set_resistance(MAX_RESISTANCE_LEVELS);
			return;
		}
		
		// Calculate the difference between easiest and hardest positions.
		uint16_t delta = RESISTANCE_LEVEL[0] - 
											RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS-1];
		
		// Position is the percentage of the delta less the easiest position.
		uint16_t position = (delta * percent) - RESISTANCE_LEVEL[0];
		
		pwm_set_servo(position);
}

// TODO: Future implementations.
void set_resistance_erg(uint16_t watts) {};
void set_resistance_slope(uint16_t slope) {};
void set_resistance_wind(uint16_t wind) {};


