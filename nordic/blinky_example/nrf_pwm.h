#ifndef __NRF_PWM_H__
#define __NRF_PWM_H__

#include <stdint.h>

#define PWM_PERIOD_WIDTH_US		20*1000		// 20ms
#define PWM_DURATION_US				500*1000	// 500ms is how long we should pulse for.
#define PWM_TIMER_PRESCALER 	4U 				// Prescaler 4 results in 1 tick == 1 microsecond 
#define PWM_PULSE_MIN					699
#define PWM_PULSE_MAX					2107

void pwm_init(uint32_t pwm_pin_output_number);
void pwm_set_servo(uint32_t pulse_width_us);
void pwm_stop_servo(void);

#endif
