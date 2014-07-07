/*
Battery

Copyright (c) Inside Ride Technologies, LLC 2013
All rights reserved.
*/

#ifndef BATTERY_H
#define BATTERY_H

#include "boards.h"	// Included for status pins.

/* Battery status indicators, uses 2 GPIO pins combined to get status.
 * 			STAT2	STAT1
			0		1				Charge-in-progress
			1		0				Charge complete
			1		1				Fault = No battery, charger off, overvoltage, timer fault, suspended
 */
#define BATTERY_CHARGING			1u
#define BATTERY_CHARGE_COMPLETE		2u
#define BATTERY_CHARGE_FAULT		3u	//

typedef void (*on_battery_result_t)(uint16_t battery_level);

// Reads the charger status.
// TODO: Connect this with PPI to one of the LEDs.
uint8_t battery_charge_status(void);

// Begins a read of the battery voltage.
void battery_read_start(void);

// Enables the Analog to Digital Converter and registers a callback for results. 
void battery_init(uint8_t pin_battery_enable, on_battery_result_t on_battery_result);

#endif // BATTERY_H
