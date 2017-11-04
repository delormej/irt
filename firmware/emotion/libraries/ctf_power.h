/* Copyright (c) 2017 Inside Ride Technologies, LLC. All Rights Reserved.
*/
#ifndef CTF_POWER_H__
#define CTF_POWER_H__

#include <stdint.h>
#include "ctf_offset.h"

// Response codes for CTF power message
#define CTF_SUCCESS				0
#define CTF_NO_EVENTS			1
#define CTF_CADENCE_TIMEOUT		2

/**@brief	Crank Torque Frequence Message Format.
 *
 */
typedef struct
{
	uint8_t			page_number;
	uint8_t			event_count;
	uint8_t			slope_msb;
	uint8_t			slope_lsb;
	uint8_t			timestamp_msb;
	uint8_t			timestamp_lsb;
	uint8_t			torque_ticks_msb;
	uint8_t			torque_ticks_lsb;
} ant_bp_ctf_t;

/**@brief	Crank Torque Frequency Calibration for Zero Offset.
 *
 */
typedef struct
{
	uint8_t			page_number;
	uint8_t			calibration_id;
	uint8_t			ctf_defined_id;
	uint8_t			reserved[3];
	uint8_t			offset_msb;
	uint8_t			offset_lsb;
} ant_bp_ctf_calibration_t;

uint32_t ctf_get_power(int16_t* p_watts);
int16_t ctf_get_average_power(uint8_t seconds);
bool ctf_power_in_use();
void ctf_set_main_page(ant_bp_ctf_t* p_page);
void ctf_set_calibration_page(ant_bp_ctf_calibration_t* p_page);

#endif // CTF_POWER_H__
