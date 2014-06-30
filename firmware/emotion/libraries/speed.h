/*
*******************************************************************************
* 
* (c) Inside Ride Technologies, LLC
* By Jason De Lorme <jason@insideridetech.com>
* http://www.insideridetech.com
*
* This module is responsible for interacting with the photo sensor measuring 
* revolutions on the fly wheel and reporting speed/distance based on this.
* 
* Speed is calculated by polling this module.  There is a margin of error 
* when you poll the flywheel could be anywhere from just recording a rev to
* just about to record a new revolution.  Assuming the user polls this module
* once per second at 18mph there is a 1.39% margin of error:
*
*		Wheel Size = 2.07m, Flywheel revolutions per second @ 18mph = 71.99982
*		Margin of error = 1/71.99982 = 1.39% or 0.25 mph +/-.
*
* Getting more exact is possible if we record the time stamp of every flywheel
* revolution, but that comes at a CPU expense.  This could be evaluated in the
* future based on need.
*
********************************************************************************/

#ifndef __REVOLUTIONS_H__
#define __REVOLUTIONS_H__

#define REVS_TIMER 				NRF_TIMER1
#define REVS_IRQHandler		TIMER1_IRQHandler
#define REVS_IRQn					TIMER1_IRQn

#define REVS_CHANNEL_TASK_TOGGLE	2

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_assert.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "irt_common.h"


/**@brief 	Initializes the flywheel photo sensor that reports revolutions.
*
*/
void speed_init(uint32_t pin_flywheel_rev, uint16_t wheel_size_mm);

/**@brief 	Set's the wheel size.  Defaults to DEFAULT_WHEEL_SIZE_MM if not set.
*
*/
void set_wheel_size(uint16_t wheel_size_mm);

/**@brief	Calculates and records current speed measurement relative to last measurement
 *
 */
uint32_t speed_calc(irt_power_meas_t * current, irt_power_meas_t * last);

/**@brief		Converts speed from meters per second to kilometers per hour.
 *					
 */
float get_speed_kmh(float speed_mps);

/**@brief		Converts speed from meters per second to miles per hour.
 *					
 */
float get_speed_mph(float speed_mps);

#endif
