#ifndef __REVOLUTIONS_H__
#define __REVOLUTIONS_H__

#define REVS_TIMER 				NRF_TIMER1
#define REVS_IRQHandler		TIMER1_IRQHandler
#define REVS_IRQn					TIMER1_IRQn
#define REVS_TO_TRIGGER		3UL			// Number of revolutions before iterrupt is triggered.

#define REVS_CHANNEL_TASK_TOGGLE	2

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_assert.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_pwm.h"

/**@brief 	Initializes the drum photo sensor that reports revolutions.*/
void init_revolutions(uint32_t pin_drum_rev);

/**@brief 	Gets the current count of drum revolutions since the counter 
 * 					was cleared or rolled over. 
 */
uint16_t revs_get_count(void);

#endif
