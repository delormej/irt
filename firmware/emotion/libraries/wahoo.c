/*
*******************************************************************************
*
* By Jason De Lorme <jjdelorme@yahoo.com>
* http://www.roadacious.com
*
* This module has a few functions for interacting with Wahoo KICKR commands.
*
********************************************************************************/
#include "wahoo.h"
#include "app_util.h"
#include "debug.h"

/**@brief Debug logging for main module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define WH_LOG debug_log
#else
#define WH_LOG(...)
#endif // ENABLE_DEBUG_LOG

/**@brief	Helper for decoding a float from buffer.*/
#define DECODE_FLOAT(BUF, SCALE)	uint16_decode((const uint8_t*)BUF) / SCALE


float wahoo_decode_crr(uint8_t *buffer)
{
	return DECODE_FLOAT(buffer, 10000.0f);
}

float wahoo_decode_c(uint8_t *buffer)
{
	return DECODE_FLOAT(buffer, 1000.0f);
}

/**@brief Parses the resistance percentage out of the KICKR command.
 *
 */
float wahoo_resistance_pct_decode(uint8_t *buffer)
{
	/*	Not exactly sure why it is this way, but it seems that 2 bytes hold a
	value that is a percentage of the MAX which seems arbitrarily to be 16383.
	Use that value to calculate the percentage, for example:

	10.7% example:
	(16383-14630) / 16383 = .10700 = 10.7%
	*/
	static const float PCT_100 = 16383.0f;
	uint16_t value = buffer[0] | buffer[1] << 8u;

	float percent = (PCT_100 - value) / PCT_100;

	return percent;
}

/**@brief Parses the simulated wind speed out of the KICKR command.
 * @note	This is the headwind in meters per second. A negative headwind
 *				represents a tailwind. The range for mspWindSpeed is -30.0:30.0.
 */
float wahoo_sim_wind_decode(uint8_t *buffer)
{
	// Note this is a signed int, change the endianness.
	int16_t value = buffer[0] | buffer[1] << 8u;

	// First bit is the sign.
	bool negative = value >> 15u;

	if (negative)
	{
		// Remove the negative sign.
		value = value & 0x7FFF;
	}
	else
	{
		value = (32768 - value) *-1;
	}

	// Set the right scale.
	return value / 1000.0f;
}


/**@brief 	Parses the simulated slope out of the KICKR command.
 * 			Initial value sent on the wire from KICKR ranges from -1.0 to 1.0 and
 * 			represents a percentage of 45 degrees up/downhill.
 *
 */
float wahoo_sim_grade_decode(uint8_t *buffer)
{
	int16_t value;

	// Note this is a signed int, change the endianness.
	value = buffer[0] | buffer[1] << 8u;

	// For some reason the value seems to come across the wire backwards?
	// FLIP the sign bit (negative == positive, and vice versa).
	value ^= 1 << 15u;

	WH_LOG("[WH]:wahoo_sim_grade_decode = %i / 32786\r\n", value);

	return value / 32768.0f;
}
