/*
 * speed_event_fifo.h
 *
 *  Created on: Mar 17, 2015
 *      Author: Jason
 */

#ifndef SPEED_EVENT_FIFO_H_
#define SPEED_EVENT_FIFO_H_

#include <stdint.h>
#include "speed.h"
#include "irt_common.h"

// Abstracts a FIFO queue of events.
speed_event_t* 	speed_event_fifo_put(speed_event_t speed_event);
speed_event_t* 	speed_event_fifo_get();
speed_event_t* 	speed_event_fifo_oldest();


#endif /* SPEED_EVENT_FIFO_H_ */
