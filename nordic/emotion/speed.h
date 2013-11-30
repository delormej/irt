/*
*******************************************************************************
* 
* By Jason De Lorme <jjdelorme@yahoo.com>
* http://www.roadacious.com
*
* This module is responsible for interacting with the photo sensor measuring 
* revolutions on the fly wheel and reporting speed/distance based on this.
*
********************************************************************************/

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
void init_speed(uint32_t pin_drum_rev, uint16_t wheel_size_mm);

/**@brief 	Gets the count of wheel revolutions since the counter started.
 */
uint16_t get_wheel_revolutions(void);

/**@brief		Returns speed in kilometers per hour (kph) given a period.  The 
 *					period is defined in 1/2048ths of a second.
 *					
 */
float get_speed_kmh(uint16_t wheel_revolutions, uint16_t period_seconds_2048);

/**@brief		Returns speed in miles per hour (kph) given a period.  The 
 *					period is defined in 1/2048ths of a second.
 *					
 */
float get_speed_mph(uint16_t wheel_revolutions, uint16_t period_seconds_2048);

#endif
