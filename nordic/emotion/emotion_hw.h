/* eMotion Hardware abstraction.

Copyright (c) Inside Ride Technologies, LLC 2013
All rights reserved.
*/

#ifndef EMOTION_HW_H
#define EMOTION_HW_H

/*****************************************************************************
* NRF Defines
*****************************************************************************/


/*****************************************************************************
* eMotion Hardware Defines
*****************************************************************************/



/*****************************************************************************
* Hardware Abstraction Functions
*****************************************************************************/


void hw_set_servo(uint32_t pulse_width_us);
void hw_stop_servo(void);



#endif