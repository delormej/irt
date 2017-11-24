/****************************************************************************** 
 * Copyright (c) 2017 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 */

#ifndef __SIMULATION_H_
#define __SIMULATION_H_

#include <stdint.h>
#include "user_profile.h"
#include "resistance.h"

#define DEFAULT_CRR     0.004f

/**@brief	Calculates rolling resistance based on weight and co-efficient of rolling resistance.
 *
 */
float simulation_rr_force(float weight_kg, float crr);

/**@brief	Calculates how many watts required for the simulation forces @ a speed.
 *
 */
float simulation_watts(float speed_mps, float weight_kg, 
    irt_resistance_state_t* p_resistance_state);

#endif // __SIMULATION_H_
