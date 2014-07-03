/*
Battery

Copyright (c) Inside Ride Technologies, LLC 2013
All rights reserved.
*/

#ifndef BATTERY_H
#define BATTERY_H

typedef void (*on_battery_result_t)(uint16_t battery_level);

// Begins a read of the battery voltage.
void battery_read_start(void);

// Enables the Analog to Digital Converter and registers a callback for results. 
void battery_init(on_battery_result_t on_battery_result, uint8_t pin_battery_read);

#endif // BATTERY_H
