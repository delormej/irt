#include "resistance.h"
#include "nrf_pwm.h"

bool m_initialized = false;

static uint32_t RESISTANCE_LEVEL[MAX_RESISTANCE_LEVELS] = { 
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

void set_resistance(uint32_t level)
{
		if (!m_initialized) 
			init_resistance();
			
		pwm_set_servo(RESISTANCE_LEVEL[level]);
}
