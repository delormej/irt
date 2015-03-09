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


float wahoo_decode_crr(uint8_t *p_buffer)
{
	float value;

	value = DECODE_FLOAT(p_buffer, 10000.0f);
	WH_LOG("[WH]:wahoo_decode_crr [%.2x][%.2x]: %.4f\r\n",
			p_buffer[0], p_buffer[1], value);

	return value;
}

float wahoo_decode_c(uint8_t *p_buffer)
{
	float value;

	value = DECODE_FLOAT(p_buffer, 1000.0f);
	WH_LOG("[WH]:wahoo_decode_c [%.2x][%.2x]: %.4f\r\n",
			p_buffer[0], p_buffer[1], value);

	return value;
}

/**@brief Parses the resistance percentage out of the KICKR command.
 *
 */
float wahoo_resistance_pct_decode(uint16_t value)
{
	/*	Not exactly sure why it is this way, but it seems that 2 bytes hold a
	value that is a percentage of the MAX which seems arbitrarily to be 16383.
	Use that value to calculate the percentage, for example:

	10.7% example:
	(16383-14630) / 16383 = .10700 = 10.7%
	*/
	static const float PCT_100 = 16383.0f;
	float percent = (PCT_100 - value) / PCT_100;

	return percent;
}

/**@brief 	Parses the simulated wind speed out of the KICKR command.
 * @note	This is the headwind in meters per second. A negative headwind
 *			represents a tailwind. The range for WindSpeed is -30.0:30.0.
 */
float wahoo_sim_wind_decode(uint16_t value)
{
	float wind_mps;

	// NOTE: The WAHOO app as of 1.4.2.3 displays MPH, but the setting is really KMH
	wind_mps = ((value - 32768) / 1000.0f);

	WH_LOG("[WH]:wahoo_sim_wind_decode: set wind_speed_mps %.2f\r\n",
			wind_mps);

	// Set the right scale.
	return wind_mps;
}


/**@brief 	Parses the simulated slope out of the KICKR command.
 * 			Initial value sent on the wire from KICKR ranges from -1.0 to 1.0 and
 * 			represents a percentage of 45 degrees up/downhill.
 *
 */
float wahoo_sim_grade_decode(int16_t value)
{
	float grade;

	// For some reason the value seems to come across the wire backwards?
	// FLIP the sign bit (negative == positive, and vice versa).
	value ^= 1 << 15u;
	grade = value / 32768.0f;

	WH_LOG("[WH]:wahoo_sim_grade_decode %#04x = %.4f\r\n", value, grade);

	return grade;
}
