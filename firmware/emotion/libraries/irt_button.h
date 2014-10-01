/*
 * irt_button.h
 *
 *  Created on: Oct 1, 2014
 *      Author: Jason
 */

#ifndef IRT_BUTTON_H_
#define IRT_BUTTON_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	NoPress,
	ShortPress,
	LongPress
} irt_button_state_t;

void irt_button_init(uint32_t button_pin);

irt_button_state_t irt_button_debounce();

#endif /* IRT_BUTTON_H_ */
