/* Copyright (c) 2016 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#include "calibration.h"
#include "irt_common.h"
#include "irt_led.h"

static calibration_status_t calibration_state;

/**@brief 	Flags to start sending calibration messages.
 */
void calibration_start(void)
{
    calibration_in_progress = true; // legacy flag for indicating in calibration mode.

    // Flag state that a spin down has been requested.
    calibration_state.spin_down = 1;

	led_set(LED_CALIBRATION_ENTER);
}

/**@brief 	Stops calibration and resumes normal activity.
 */
void calibration_stop(void)
{
	calibration_in_progress = false;

	led_set(LED_CALIBRATION_EXIT);
}

// Call to update progress and determine state.
calibration_status_t* calibration_progress(irt_context_t* p_context)
{
    // If calibration is requested, flag start time.
    // Maintain a 32 position cyclical buffer for speed values.  
    // They'll always be at 1/4 second intervals.
    
    // Analyze how long coastdowns are taking most? 7-9 seconds typically. 
    
    // Calcluates spin-down time.
    
    // Flag complete if speed drops below a certain speed. 
    
    // If below threshold speed entirely, exit calibration mode.

    return &calibration_state;
}
