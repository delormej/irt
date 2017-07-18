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

#define SPEED_EVENT_CACHE_SIZE		8		// @ 2hz, this is 4 seconds of speed data.

//
// If speed.h is not included (for test harness)
//
#ifndef __REVOLUTIONS_H__
typedef struct
{
	uint32_t	event_time_2048;											// Event time in 1/2048s.
	uint16_t	accum_flywheel_ticks;										// Currently 2 ticks per flywheel rev.
} speed_event_t;
#endif
//
//
//

typedef struct
{
    speed_event_t*		speed_buffer;
    uint8_t				size;
    uint8_t				write_index;
} event_fifo_t;

// Abstracts a FIFO queue of events.
event_fifo_t speed_event_fifo_init(speed_event_t* p_buffer, uint8_t size);
speed_event_t* speed_event_fifo_put(event_fifo_t* p_fifo, speed_event_t* p_speed_event);
speed_event_t* speed_event_fifo_get(event_fifo_t* p_fifo);
speed_event_t* speed_event_fifo_oldest(event_fifo_t* p_fifo);

#endif /* SPEED_EVENT_FIFO_H_ */
