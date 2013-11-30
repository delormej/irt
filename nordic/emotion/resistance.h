/*
*******************************************************************************
* 
* By Jason De Lorme <jjdelorme@yahoo.com>
* http://www.roadacious.com
*
********************************************************************************/

#ifndef __RESISTANCE_H__
#define __RESISTANCE_H__

#include <stdint.h>

#define PIN_SERVO_SIGNAL			3							// GPIO pin that the servo signal line is connected to. (P3 - P0.03)
#define MAX_RESISTANCE_LEVELS 10						// Maximum resistance levels available.

/**@brief		Sets the resistance to a specific level by moving the servo to the 
 *					corresponding position.
 *
 *@note			Levels 0-9 allowable.
 */
void set_resistance(uint8_t level);

/**@brief		Sets the resistance to a value 0-100 percent.
 *
 */
void set_resistance_pct(uint8_t percent);

// Future implementations.
void set_resistance_erg(uint16_t watts);
void set_resistance_slope(uint16_t slope);
void set_resistance_wind(uint16_t wind);

#endif
