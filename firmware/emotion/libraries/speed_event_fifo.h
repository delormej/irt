/*
 * speed_event_fifo.h
 *
 *  Created on: Mar 17, 2015
 *      Author: Jason
 */

#ifndef SPEED_EVENT_FIFO_H_
#define SPEED_EVENT_FIFO_H_

#include <stdint.h>

#define SPEED_EVENT_CACHE_SIZE		8		// @ 2hz, this is 4 seconds of speed data.

typedef struct
{
    uint8_t*		speed_buffer;
    uint8_t			size;           // Size of an individual item.
    uint8_t			write_index;
} event_fifo_t;

// Abstracts a FIFO queue of events.
event_fifo_t speed_event_fifo_init(uint8_t* p_buffer, uint8_t size);
uint8_t* speed_event_fifo_put(event_fifo_t* p_fifo, uint8_t* p_speed_event);
uint8_t* speed_event_fifo_get(event_fifo_t* p_fifo);
uint8_t* speed_event_fifo_oldest(event_fifo_t* p_fifo);

#endif /* SPEED_EVENT_FIFO_H_ */
