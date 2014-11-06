#ifndef __NRF_PWM_H__
#define __NRF_PWM_H__

#include <stdint.h>
#include <stdbool.h>
#include "nrf.h"
#include "nrf_assert.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_pwm.h"

void pwm_init(uint32_t pwm_pin_output_number);
uint32_t pwm_set_servo(uint32_t pulse_width_us);
uint32_t pwm_stop_servo(void);

#ifdef KURT

#define PWM_FULL_FORWARD		100
#define PWM_FULL_REVERSE		-100
#define	PWM_STOP				0

/**@brief	Moves a continuous rotation servo forward / backward / stop.
 * 			Speed is a percentage of speed -100...+100
 * 			- = reverse, 0 = stop + = forward
 */
uint32_t pwm_continuous_servo(int8_t speed);
#endif

#endif
