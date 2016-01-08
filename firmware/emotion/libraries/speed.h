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

#include <stdbool.h>
#include <stdint.h>
#include "irt_common.h"

typedef struct
{
	uint32_t	event_time_2048;											// Event time in 1/2048s.
	uint16_t	accum_flywheel_ticks;										// Currently 2 ticks per flywheel rev.
} speed_event_t;


/**@brief 	Initializes the flywheel photo sensor that reports revolutions.
*
*/
void speed_init(uint32_t pin_flywheel_rev, uint16_t wheel_size_mm);

/**@brief 	Set's the wheel size.  Defaults to DEFAULT_WHEEL_SIZE_MM if not set.
*
*/
void speed_wheel_size_set(uint16_t wheel_size_mm);

/**@brief	Calculates a running smoothed average of speed.
 *
 */
float speed_average_mps(void);

/**@brief	Calculates and records current speed measurement.
 *
 */
uint32_t speed_calc(irt_context_t * p_meas);

/**@brief 	Returns the accumulated count of flywheel ticks (2x revolutions).
 *
 */
uint16_t flywheel_ticks_get();

/**@brief 	Returns the last tick event time in 1/2048's of a second.
 *
 */
uint16_t seconds_2048_get(void);

/**@brief 	Returns the distance traveled in meters.  Rolls over at 256m.
 *
 */
uint8_t speed_distance_get(void);

#endif
