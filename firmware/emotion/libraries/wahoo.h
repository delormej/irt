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

/**@brief	Helper for decoding a float from a 2 byte buffer.*/
#define DECODE_FLOAT(P_BUF, SCALE)	uint16_decode((const uint8_t*)P_BUF) / SCALE

float wahoo_decode_crr(uint8_t *p_buffer);

float wahoo_decode_c(uint8_t *p_buffer);

// Parses the simulation grade.
float wahoo_sim_grade_decode(int16_t value);

// Parses the simulation wind speed.
float wahoo_sim_wind_decode(uint16_t value);

// Parses the wahoo resistance percentage.
float wahoo_resistance_pct_decode(uint16_t value);

#endif // __WAHOO_H__
