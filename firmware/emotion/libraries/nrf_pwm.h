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
uint32_t pwm_set_servo(uint16_t pulse_width_us, bool smooth);
uint32_t pwm_stop_servo(void);

#endif
