/*
*******************************************************************************
*
* By Jason De Lorme <jjdelorme@yahoo.com>
* http://www.roadacious.com
*
* This module has a few functions for interacting with Wahoo KICKR commands.
*
********************************************************************************/

#ifndef __WAHOO_H__
#define __WAHOO_H__

#include <stdint.h>

float wahoo_decode_crr(uint8_t *buffer);

float wahoo_decode_c(uint8_t *buffer);

// Parses the simulation grade.
float wahoo_sim_grade_decode(uint8_t *buffer);

// Parses the simulation wind speed.
float wahoo_sim_wind_decode(uint8_t *buffer);

// Parses the wahoo resistance percentage.
float wahoo_resistance_pct_decode(uint8_t *buffer);

#endif // __WAHOO_H__
