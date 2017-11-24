/****************************************************************************** 
 * Copyright (c) 2017 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 */

#include "simulation.h"
#include "math_private.h"

#define	GRAVITY     9.81f // Coefficent of gravity.

static float wind_force(float speed_mps, irt_resistance_state_t* p_resistance_state)
{
    // p_sim_forces->c is equal to A*Cw*Rho where A is effective frontal area (m^2); 
	// Cw is drag coefficent (unitless); and Rho is the air density (kg/m^3). 
    // The default value for A*Cw*Rho is 0.51.
	// Note that we add HEAD and subtract TAIL wind speed in the speed calc here.
    
    /*
    The drafting factor
    scales the total wind resistance depending on the position of the user relative to other virtual competitors. The drafting
    scale factor ranges from 0.0 to 1.0, where 0.0 removes all air resistance from the simulation, and 1.0 indicates no drafting
    effects (e.g. cycling alone, or in the lead of a pack). 
    */    
    float wind = (0.5f * (p_resistance_state->c * 
        pow((speed_mps + p_resistance_state->wind_speed_mps), 2))) *
        p_resistance_state->drafting_factor;    
    return wind;
}

float gravitational_force(float weight_kg, float grade)
{
	// Grade is the slope of the hill (slope = rise / run). Should be from -1.0 : 1.0, 
	// where -1.0 is a 45 degree downhill slope, 0.0 is flat ground, and 1.0 is a 45 
	// degree uphil slope. 
	float gravitational = (weight_kg / 100.0f) * GRAVITY * grade;
    return gravitational;
}

float simulation_rr_force(float weight_kg, float crr)
{
	// Weight * GRAVITY * Co-efficient of rolling resistance.
    float rolling = (weight_kg / 100.0f) * GRAVITY * crr;
    return rolling;
}

float simulation_watts(float speed_mps, float weight_kg, 
    irt_resistance_state_t* p_resistance_state)
{
    float force = wind_force(speed_mps, p_resistance_state) + 
            simulation_rr_force(weight_kg, p_resistance_state->crr) + 
            gravitational_force(weight_kg, p_resistance_state->grade);
    float watts = force * speed_mps;
    return watts;
}
