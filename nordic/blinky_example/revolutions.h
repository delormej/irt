#ifndef __REVOLUTIONS_H__
#define __REVOLUTIONS_H__

#define REVS_TIMER NRF_TIMER1

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_assert.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_pwm.h"

// Initializes the counter and iterrupt handler.
void init_revolutions(void);

#endif