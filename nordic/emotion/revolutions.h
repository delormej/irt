#ifndef __REVOLUTIONS_H__
#define __REVOLUTIONS_H__

#define REVS_TIMER 				NRF_TIMER1
#define REVS_IRQHandler		TIMER1_IRQHandler
#define REVS_IRQn					TIMER1_IRQn
#define REVS_TO_TRIGGER		19UL			// Number of revolutions before iterrupt is triggered.

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_assert.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_pwm.h"

// Initializes the counter and iterrupt handler.
void init_revolutions(uint32_t pin_drum_rev);

#endif
