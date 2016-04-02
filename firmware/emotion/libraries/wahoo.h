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

// Defines specific to Wahoo commands.
#define ANT_BP_COMMAND_OFFSET			4u
#define ANT_BP_SET_WEIGHT_COMMAND		0x60
#define ANT_BP_MOVE_SERVO_COMMAND		0x61
#define ANT_BP_ENABLE_DFU_COMMAND		0x64

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
