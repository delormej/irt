/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * @brief	Defines port pins and what they do for the IRT REV_A board.
 *
 */
#include "board_irt_v2a.h"

#ifndef IRT_REV_2A1_H
#define IRT_REV_2A1_H

#undef	PIN_ENBATT					// no longer uses pin 6 for reading battery voltage.
#undef	USE_BATTERY_READ_PIN

#undef 	PIN_SERVO_SIGNAL
#define	PIN_SERVO_SIGNAL			25	// Move servo signal from pin 1 to pin 25.

#define PIN_ANALOG_READ				1	// J7-4 which connects to pin 1, used for reading analog voltage.

#define	PIN_3VPWR_DIS				6	// Disables 3v power to pin J7-6.

#define	PIN_AC_PWR					PIN_TP19 // monitors the status of the AC Power. P0.20 monitors a similar signal from a different source with the inverse logic sense.

/* Redefine support for 2 TWI device instances. */ 
#undef PIN_SDA
#undef PIN_SCL
#define PIN_TWI_0_SDA				PIN_SERVO_SIGNAL	// Bidirectional signal from I2C bus
#define PIN_TWI_0_SCL				PIN_UART_RTS	// Clock signal for I2C bus.
#define PIN_TWI_1_SDA				14	// Bidirectional signal from I2C bus
#define PIN_TWI_1_SCL				15	// Clock signal for I2C bus.
static const uint8_t PIN_SDA[] = {PIN_TWI_0_SDA, PIN_TWI_1_SDA};
static const uint8_t PIN_SCL[] = {PIN_TWI_0_SCL, PIN_TWI_1_SCL};

#endif /* IRT_REV_2A1_H */
