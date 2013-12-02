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

/**@brief		Array representing the servo position in micrseconds (us) by 
 *					resistance level 0-9.
 *
 */
static uint16_t RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS] = { 
	2107, // 0 - no resistance
	1300,	// this was 1273
	1272,	
	1252,
	1212,
	1150,
	1068,
	966,
	843,
	699 };

static void init_resistance() 
{
	pwm_init(PIN_SERVO_SIGNAL);	
	m_initialized = true;
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


