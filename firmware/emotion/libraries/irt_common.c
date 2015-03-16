/*
 * irt_common.c
 *
 *  Created on: Feb 18, 2014
 *      Author: jasondel
 *
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <float.h>
#include <math.h>
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

static irt_power_meas_t* 	mp_buf_power_meas;
static uint8_t				m_buf_size;
static uint8_t				m_fifo_index;

uint32_t irt_power_meas_fifo_init(uint8_t size)
{
	m_buf_size = size;

	// Allocate buffer on the heap for events and clear all bytes to 0.
	mp_buf_power_meas = (irt_power_meas_t*)calloc(size, sizeof(irt_power_meas_t));

	if (mp_buf_power_meas == 0)
		return NRF_ERROR_NO_MEM;

	return IRT_SUCCESS;
}

void irt_power_meas_fifo_free()
{
	// Free up the heap.
	free(mp_buf_power_meas);
}

/*
 * Returns a pointer to use for the next write.
 */
irt_power_meas_t* irt_power_meas_fifo_next()
{
	uint8_t idx_write;

	// Determine index to write.
	idx_write = m_fifo_index % m_buf_size;

	// Clear the bytes for the next write.
	memset(&mp_buf_power_meas[idx_write], 0, sizeof(irt_power_meas_t));

	// Increment the index.
	m_fifo_index++;

	//CN_LOG("[CN] _next %i \r\n", idx_write);

	// Set pointer of the current event to write.
	return &mp_buf_power_meas[idx_write];
}

/*
 * Returns a pointer to oldest event in the queue.
 */
irt_power_meas_t* irt_power_meas_fifo_first()
{
	// Determine index to read.
	uint8_t idx_read;

	idx_read = (m_fifo_index+1) % m_buf_size;
	//CN_LOG("[CN] _first %i \r\n", idx_read);

	// Return the pointer to the oldest event in the stack.
	return &mp_buf_power_meas[idx_read];
}

/*
 * Returns the last item written to the queue.
 */
irt_power_meas_t* irt_power_meas_fifo_last()
{
	uint8_t idx_read;

	idx_read = (m_fifo_index + m_buf_size - 2) % m_buf_size;
	//CN_LOG("[CN] _last %i \r\n", idx_read);

	return &mp_buf_power_meas[idx_read];
}
