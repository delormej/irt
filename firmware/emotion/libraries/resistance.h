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

/**@brief Bike types, used for predefined resistance coefficients. */
typedef enum
{
	BIKE_ROAD				= 0x21,
	BIKE_MOUNT			= 0x2E
} bike_type_t;

/**@brief Resistance modes. */
typedef enum 
{
	RESISTANCE_SET_PERCENT 		= 0x40,
	RESISTANCE_SET_STANDARD		= 0x41,
	RESISTANCE_SET_ERG				= 0x42,
	RESISTANCE_SET_BIKE_TYPE	= 0x44,
	RESISTANCE_SET_SLOPE			= 0x46,
	RESISTANCE_SET_WIND				= 0x47
} resistance_mode_t;

/**@brief Resistance control event payload. */
typedef struct
{
	resistance_mode_t 	mode;					/**< Data containing the resistance mode. */
	uint16_t 						level;				/**< Value to set the mode to. */
} rc_evt_t;


/**@brief Set resistance event handler type. */
typedef void(*rc_evt_handler_t) (rc_evt_t rc_evt);

/**@brief		Sets the resistance to a specific level by moving the servo to the 
 *					corresponding position.
 *
 *@note			Levels 0-9 allowable.
 */
void set_resistance(uint8_t level);

/**@brief		Sets the resistance to a value 0-100 percent.
 *
 */
void set_resistance_pct(uint16_t percent);

// Future implementations.
void set_resistance_erg(uint16_t watts);
void set_resistance_slope(uint16_t slope);
void set_resistance_wind(uint16_t wind);

#endif
