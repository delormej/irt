/*
 * speed_event_fifo.c
 *
 *  Created on: Feb 18, 2014
 *      Author: jasondel
 *
 */
#include <stdint.h>
#include <string.h>
#include "speed_event_fifo.h"

/*
 * Copies the speed event to the queue and returns a pointer.
 */
uint8_t* speed_event_fifo_put(event_fifo_t* p_fifo, uint8_t* speed_event)
{
	uint8_t idx_write;

	// Determine index to write.
	idx_write = p_fifo->write_index % SPEED_EVENT_CACHE_SIZE;

	// Clear the bytes for the next write.
	memset(&p_fifo->speed_buffer[idx_write * p_fifo->size], 0, p_fifo->size);

	// Increment index for next write.
	p_fifo->write_index++;

	// Set pointer of the current event to write.
	uint8_t* p_event = &p_fifo->speed_buffer[idx_write * p_fifo->size];

	// Copy the current event into the buffer.
	memcpy(p_event, speed_event, p_fifo->size);

	// Returns the pointer into the new buffer location.
	return p_event;
}

/*
 * Returns a pointer to oldest event in the queue, by the # of events ago.
 */
uint8_t* speed_event_fifo_oldest(event_fifo_t* p_fifo, uint8_t events)
{
	// Determine index to read.
	uint8_t idx_read;

	idx_read = ( (p_fifo->write_index - 1 + SPEED_EVENT_CACHE_SIZE - events) 
		% SPEED_EVENT_CACHE_SIZE );

	// Return the pointer to the oldest event in the stack.
	return &p_fifo->speed_buffer[idx_read * p_fifo->size];
}

/*
 * Returns the last item written to the queue.
 */
uint8_t* speed_event_fifo_get(event_fifo_t* p_fifo)
{
	uint8_t idx_read;

	idx_read = ( (p_fifo->write_index + SPEED_EVENT_CACHE_SIZE - 1) 
		% SPEED_EVENT_CACHE_SIZE );
	//CN_LOG("[CN] _last %i \r\n", idx_read);

	return &p_fifo->speed_buffer[idx_read * p_fifo->size];
}

event_fifo_t speed_event_fifo_init(uint8_t* p_buffer, uint8_t size)
{
	// Clear buffer, size of each object * # of objects to cache.
	memset(p_buffer, 0, size * SPEED_EVENT_CACHE_SIZE);
	event_fifo_t fifo = { .size = size, .speed_buffer = p_buffer, .write_index = 0 };

	return fifo;
}