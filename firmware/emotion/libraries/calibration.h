/* Copyright (c) 2016 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "irt_common.h"
#include <stdbool.h>

// Global variable indicating when calibration is in progress.
bool calibration_in_progress;

// Called when calibration is invoked.
void calibration_start();

// Call when calibration is 
void calibration_end();

// Call to update progress and determine state.
calibration_status_t* calibration_progress(irt_context_t* p_context);

#endif // CALIBRATION_H 