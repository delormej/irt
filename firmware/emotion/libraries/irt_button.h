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

// Length of button presses.
typedef enum 
{
	press_delay_short = 0,
	press_delay_2_sec,
	press_delay_4_sec,
	press_delay_8_sec
} press_delay_e;

// Callback type.
typedef void (*on_button_pbsw_t)(press_delay_e press_delay);

uint32_t irt_button_init(on_button_pbsw_t handler);

#endif /* IRT_BUTTON_H_ */


