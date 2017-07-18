/*
 * speed_event_fifo.c
 *
 *  Created on: Feb 18, 2014
 *      Author: jasondel
 *
 */
#include <stdint.h>

#include "speed_event_fifo.h"

/*
 * Copies the speed event to the queue and returns a pointer.
 */
speed_event_t* speed_event_fifo_put(event_fifo_t* p_fifo, speed_event_t* speed_event)
{
	uint8_t idx_write;

	// Determine index to write.
	idx_write = p_fifo->write_index % p_fifo->size;

	// Clear the bytes for the next write.
	memset(&p_fifo->speed_buffer[idx_write], 0, sizeof(speed_event_t));

	// Increment index for next write.
	p_fifo->write_index++;

	// Set pointer of the current event to write.
	speed_event_t* p_event = &p_fifo->speed_buffer[idx_write];

	// Copy the current event into the buffer.
	memcpy(p_event, speed_event, sizeof(speed_event_t));

	// Returns the pointer into the new buffer location.
	return p_event;
}

/*
 * Returns a pointer to oldest event in the queue.
 */
speed_event_t* speed_event_fifo_oldest(event_fifo_t* p_fifo)
{
	// Determine index to read.
	uint8_t idx_read;

	idx_read = (p_fifo->write_index) % p_fifo->size;

	// Return the pointer to the oldest event in the stack.
	return &p_fifo->speed_buffer[idx_read];
}

/*
 * Returns the last item written to the queue.
 */
speed_event_t* speed_event_fifo_get(event_fifo_t* p_fifo)
{
	uint8_t idx_read;

	idx_read = (p_fifo->write_index + p_fifo->size - 1) % p_fifo->size;
	//CN_LOG("[CN] _last %i \r\n", idx_read);

	return &p_fifo->speed_buffer[idx_read];
}

event_fifo_t speed_event_fifo_init(speed_event_t* p_buffer, uint8_t size)
{
	event_fifo_t fifo = { .size = size, .speed_buffer = p_buffer, .write_index = 0 };
	// 0 out the buffer??
	// memset(...)
	return fifo;
}