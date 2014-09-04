/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * @brief	Defines port pins and what they do for the IRT REV_A board.
 *
 */
#include "board_irt_v2a.h"

#ifndef BOARD_IRT_V2B_H_
#define BOARD_IRT_V2B_H_

#undef	PIN_ENBATT					// no longer uses pin 6 for reading battery voltage.
#undef	USE_BATTERY_READ_PIN

#undef 	PIN_SERVO_SIGNAL
#define	PIN_SERVO_SIGNAL			25	// Move servo signal from pin 1 to pin 25.

#define PIN_ANALOG_READ				1	// J7-4 which connects to pin 1, used for reading analog voltage.

#define	PIN_3VPWR_EN				6	// Enables 3.3v power to pin J7-6.

#define	PIN_AC_PWR					PIN_TP19 // monitors the status of the AC Power. P0.20 monitors a similar signal from a different source with the inverse logic sense.

#endif /* BOARD_IRT_V2B_H_ */
