/*
 * irt_common.c
 *
 *  Created on: Feb 18, 2014
 *      Author: jasondel
 *
 */
#include <stdbool.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include "speed_event_fifo.h"
#include "irt_common.h"
#include "nrf_error.h"
#include "nrf_soc.h"
#include "debug.h"

/**@brief Debug logging for resistance control module.
 */
#ifdef ENABLE_DEBUG_LOG
#define CN_LOG debug_log
#else
#define CN_LOG(...)
#endif // ENABLE_DEBUG_LOG

static speed_event_t		mp_buf_speed_events[SPEED_EVENT_CACHE_SIZE] = {0};
static uint8_t				m_buf_size = SPEED_EVENT_CACHE_SIZE;
static uint8_t				m_fifo_index;

/*
 * Copies the speed event to the queue and returns a pointer.
 */
speed_event_t* speed_event_fifo_put(speed_event_t speed_event)
{
	uint8_t idx_write;

	// Determine index to write.
	idx_write = m_fifo_index % m_buf_size;

	// Clear the bytes for the next write.
	memset(&mp_buf_speed_events[idx_write], 0, sizeof(speed_event_t));

	// Increment the index.
	m_fifo_index++;

	//CN_LOG("[CN] _next %i \r\n", idx_write);

	// Set pointer of the current event to write.
	speed_event_t* p_event = &mp_buf_speed_events[idx_write];

	// Copy the current event into the buffer.
	memcpy(p_event, &speed_event, sizeof(speed_event_t));

	return p_event;
}

/*
 * Returns a pointer to oldest event in the queue.
 */
speed_event_t* speed_event_fifo_oldest()
{
	// Determine index to read.
	uint8_t idx_read;

	idx_read = (m_fifo_index+1) % m_buf_size;
	//CN_LOG("[CN] _first %i \r\n", idx_read);

	// Return the pointer to the oldest event in the stack.
	return &mp_buf_speed_events[idx_read];
}

/*
 * Returns the last item written to the queue.
 */
speed_event_t* speed_event_fifo_get()
{
	uint8_t idx_read;

	idx_read = (m_fifo_index + m_buf_size - 2) % m_buf_size;
	//CN_LOG("[CN] _last %i \r\n", idx_read);

	return &mp_buf_speed_events[idx_read];
}
