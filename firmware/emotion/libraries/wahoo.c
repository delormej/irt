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
#include <stdbool.h>

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
	// Note this is a signed int.
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


/**@brief Parses the simulated slope out of the KICKR command.
 *
 */
float wahoo_sim_grade_decode(uint8_t *buffer)
{
	uint16_t value = buffer[0] | buffer[1] << 8u;

	// First bit is the sign.
	bool negative = value >> 15u;

	float percent = 0.0f;

	if (negative)
	{
		// Strip the negative sign bit.
		value = value & 0x7FFF;
		// results in a positive (uphill) grade.
		percent = 1 - ((32768.0f - value) / 32768.0f);
	}
	else
	{
		// results in a negative (downhill) grade.
		percent = ((value - 32768.0f) / 32768.0f);
	}

	// Initial value sent on the wire from KICKR ranges from -1.0 to 1.0 and
	// represents a percentage of 45 degrees up/downhill.
	// e.g. 1.0% = 45 degree uphill,
	// -1.0% = 45 degree downhill,
	// 0% = flat,
	// 0.10% = 4.5% uphill grade.
	return percent * 0.45;
}
