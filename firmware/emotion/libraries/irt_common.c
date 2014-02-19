/*
 * irt_common.c
 *
 *  Created on: Feb 18, 2014
 *      Author: jasondel
 *
 */
#include "irt_common.h"
#include "app_fifo.h"
#include "nrf_error.h"

static irt_power_meas_t* 	mp_buf_power_meas;
static uint8_t*				mp_buf_dummy;
static app_fifo_t			m_fifo;
static uint8_t				m_buf_size;

uint32_t irt_power_meas_fifo_init(uint8_t size)
{
	uint32_t err_code;

	m_buf_size = size;

	// Allocate buffer on the heap for events.
	mp_buf_power_meas = (irt_power_meas_t*)calloc(size, sizeof(irt_power_meas_t));
	mp_buf_dummy = (uint8_t*)calloc(size, sizeof(uint8_t));

	if (mp_buf_power_meas == 0 || mp_buf_dummy == 0)
		return NRF_ERROR_NO_MEM;

	// Initialize fifo.
	err_code = app_fifo_init(&m_fifo, mp_buf_dummy, (uint16_t)(sizeof(uint8_t)*size));

	if (err_code != NRF_SUCCESS)
		return err_code;

	return IRT_SUCCESS;
}

void irt_power_meas_fifo_free()
{
	// Free up the heap.
	free(mp_buf_power_meas);
	free(mp_buf_dummy);
}

/*
 *  Implements a FIFO queue of power events.  This function abstracts interaction
 *  with the queue.  first returns a pointer to first event put in the buffer
 *  next returns a pointer to use for the next write.
 *
 *  The FIFO is used to maintain the index in a buffer holding the events.
 */
uint32_t irt_power_meas_fifo_op(irt_power_meas_t** first, irt_power_meas_t** next)
{
	uint32_t err_code;
	uint8_t dummy_byte;	// Use a dummy byte for the FIFO queue.

	// Is buffer full?
	if (m_fifo.write_pos == (m_fifo.read_pos + m_buf_size))
	{
		// Set pointer of the first event in the FIFO.
		*first = &mp_buf_power_meas[m_fifo.read_pos];

		// Pop off the first event to make room for a write.
		err_code = app_fifo_get(&m_fifo, &dummy_byte);

		if (err_code != NRF_SUCCESS)
		{
			return err_code;
		}
	}
	else
	{
		// FIFO isn't full, so don't return a first one yet.
		*first = 0x0;
	}

	// Set pointer of the current event to write.
	*next = &mp_buf_power_meas[m_fifo.write_pos];

	// Advance the index of where to write the next event.
	err_code = app_fifo_put(&m_fifo, dummy_byte);

	if (err_code == NRF_ERROR_NO_MEM)
	{
		// Something went wrong because we caught this earlier.
	}

	return err_code;
}

