/*
Battery

Copyright (c) Inside Ride Technologies, LLC 2013
All rights reserved.
*/

#ifndef BATTERY_H
#define BATTERY_H

#include "boards.h"	// Included for status pins.
#include "irt_common.h"

/* Battery status indicators, uses 2 GPIO pins combined to get status.
 *
 * 10k pull-up resistor is on these pins so: 1 == "OFF",  0 == "ON"
 *
 * 	2 bits (MSB)	(LSB)
 * 			STAT2	STAT1
			0		1				Charge complete
			1		0				Charge-in-progress
			1		1				Charge suspend, timer fault, overvoltage, sleep mode, battery absent
 */
typedef enum
{
	BATTERY_CHARGE_NONE			= 0u,	// No battery charger is present.
	BATTERY_CHARGE_COMPLETE		= 1u,
	BATTERY_CHARGING			= 2u,
	BATTERY_CHARGE_OFF			= 3u
} irt_charger_status_t;

/**@brief Returns true if battery charger is off.
 */
#define BATTERY_CHARGER_IS_OFF 			(battery_charge_status() == BATTERY_CHARGE_OFF)

typedef void (*on_battery_result_t)(uint16_t battery_level);

/**@brief	Battery statuses defined in ANT spec for common page 0x82.
 */
typedef enum 
{
	BAT_NEW = 0x01,
	BAT_GOOD,
	BAT_OK,
	BAT_LOW,
	BAT_CRITICAL,
	BAT_INVALID = 0x07
} irt_battery_level_t;

// Begins a read of the battery voltage.
void battery_read_start(void);

/**@brief	Converts millivolt reading into status structure.
 */
irt_battery_status_t battery_status(uint16_t millivolts, uint32_t operating_time);

// Reads the charger status.
// TODO: Connect this with PPI to one of the LEDs.
irt_charger_status_t battery_charge_status(void);

/**@brief	Starts or stops the battery charge process.
 */
void battery_charge_set(bool turn_on);

// Enables the Analog to Digital Converter and registers a callback for results. 
void battery_init(uint8_t pin_battery_enable, uint8_t pin_charge_stop, on_battery_result_t on_battery_result);

#endif // BATTERY_H
