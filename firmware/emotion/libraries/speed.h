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

#define REVS_TIMER 					NRF_TIMER1
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
void speed_wheel_size_set(uint16_t wheel_size_mm);

/**@brief	Calculates and records current speed measurement relative to last measurement
 *
 */
uint32_t speed_calc(irt_power_meas_t * current, irt_power_meas_t * last);

/**@brief 	Returns the accumulated count of flywheel revolutions since the
 *					counter started.
 *
 */
inline uint16_t flywheel_ticks_get()
{
	REVS_TIMER->TASKS_CAPTURE[0] = 1;
	return REVS_TIMER->CC[0];
}

#ifdef SIM_SPEED
// Declaration of
// # of ticks to simulate in debug mode.
extern uint8_t  speed_debug_ticks;
#endif

#endif
