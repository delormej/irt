/*
 * irt_led.h
 *
 *  Created on: Nov 12, 2014
 *      Author: Jason
 */

#ifndef IRT_LED_H_
#define IRT_LED_H_

#include <stdint.h>
#include "boards.h"

#define APP_TIMER_PRESCALER		0

// TODO: this module doesn't support HW_VERSION <2

#define LED_FRONT_GREEN			PIN_LED_D
#define LED_FRONT_RED			PIN_LED_C
#define LED_BACK_GREEN			PIN_LED_B
#define LED_BACK_RED			PIN_LED_A

//
// Defined LED patterns.
//
#define LED_MASK_POWER_ON		1UL << LED_FRONT_GREEN
#define LED_MASK_BLE_CONNECTED	1UL << LED_BACK_GREEN
#define LED_MASK_ERROR			(1UL << LED_BACK_RED | 1UL << LED_FRONT_RED)
#define LED_MASK_ALL 			(1UL << LED_FRONT_GREEN | 1UL << LED_FRONT_RED | 1UL << LED_BACK_GREEN | 1UL << LED_BACK_RED)

#define LED_BLINK_EMPTY			{ .interval_ms = 0 }

#define LED_BLINK_BLE_ADV		{ .interval_ms = 150, .pin_mask = 1UL << LED_BACK_GREEN, .count = 0, .pattern = 0b11110000, .repeated = 1 }
#define LED_BLINK_BUTTON_UP		{ .interval_ms = 150, .pin_mask = 1UL << LED_FRONT_GREEN, .count = 0, .pattern = 0b10101110, .repeated = 0 }
#define LED_BLINK_BUTTON_DOWN	{ .interval_ms = 150, .pin_mask = 1UL << LED_BACK_GREEN, .count = 0, .pattern = 0b10101110, .repeated = 0 }
#define LED_BLINK_BUTTON_MAX	{ .interval_ms = 150, .pin_mask = 1UL << LED_FRONT_RED, .count = 0, .pattern = 0b10101110, .repeated = 0 }
#define LED_BLINK_BUTTON_MIN	{ .interval_ms = 150, .pin_mask = 1UL << LED_BACK_RED, .count = 0, .pattern = 0b10101110, .repeated = 0 }
#define LED_BLINK_BUTTON_STD	{ .interval_ms = 200, .pin_mask = (1UL << LED_FRONT_GREEN | 1UL << LED_BACK_GREEN), .count = 0, .pattern = 0b10000000, .repeated = 0 }
#define LED_BLINK_BUTTON_ERG	{ .interval_ms = 200, .pin_mask = (1UL << LED_FRONT_GREEN | 1UL << LED_BACK_GREEN), .count = 0, .pattern = 0b10001000, .repeated = 0 }
#define LED_BLINK_CALIBRATION	{ .interval_ms = 150, .pin_mask = 1UL << LED_FRONT_RED, .count = 0, .pattern = 0b10101010, .repeated = 0 }

/**@brief	Structure for managing blink pattern.
 */
typedef struct blink_s
{
	uint32_t pin_mask; 			// Mask of pins to toggle
	uint8_t pattern; 			// each bit represents what to in 1 of 8 slots
	uint8_t repeated : 1;		// Determines whether this is a repeated blink or not
	uint8_t count : 3; 			// running count of where in the sequence
	uint16_t interval_ms;		// Interval time to repeat in ms.
} blink_t;


/**@brief	Turn one or more LEDs on.
 *
 */
void led_on(uint32_t pin_mask);

/**@brief	Turn one or more LEDs off.
 *
 */
void led_off(uint32_t pin_mask);

/**@brief	Stop blinking, shut off all LEDs.
 *
 */
void led_blink_stop();

/**@brief	Blink LED according to the pattern defined.
 *
 */
void led_blink(blink_t blink);

/**@brief	Initializes the LEDs vio GPIO and the timer use to control.
 *
 */
void led_init(void);

#endif /* IRT_LED_H_ */
