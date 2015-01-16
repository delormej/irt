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

/**@brief	Parses a float from a byte stream with scale factor.
 * 			This uses the standard IEEE 754 specification for binary32.
 *
 * 			NOTE: ANT doc section 15.3 for details on Scale Factor which
 * 			is also supported by this same method, although the scale
 * 			factor is sent in a different byte per the ANT spec, this
 * 			supports each individual value have it's own exponent as per
 * 			IEEE 754.
 */
float float_from_buffer(uint32_t* p_encoded)
{
	// Signed int contains the fraction, starting at byte 2.
	// IEEE754 binary32 format for float.
	// Sign 1 bit, exponent (8 bits), fraction (23 bits)
	// 0 00000000 00000000000000000000000
	bool sign = *p_encoded >> 31;

	// mask out 23 bits for the fractional value.
	uint32_t fraction = *p_encoded & 0x7FFFFF;

	// mask out the exponent and shift into an 8 bit int
	// exponent is transmitted with binary offset of 127
	uint8_t exponent = (*p_encoded >> 23)-127;

	// calculate the float value.
	float value = fraction / pow(2, exponent);

	if (sign)
	{
		value *= -1;
	}

	CN_LOG("[CN] float_from_buffer encoded:%u, exp:%i, sign:%i, fraction:%i, value:%.7f\r\n",
			*p_encoded, exponent, sign, fraction, value);

	return value;
}


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

/**@brief Stores features value on flash. */
uint32_t features_store(uint32_t* p_buffer)
{
	uint32_t err_code;
	uint32_t value = 0;

	// Make a local copy.
	memcpy(&value, p_buffer, sizeof(uint32_t));

	err_code = sd_flash_write((uint32_t *)(FACTORY_SETTINGS_BASE),
                            &value,
                            sizeof(uint32_t));

	CN_LOG("[CN] features_store storing value: %i, returned: %i.\r\n",
			value, err_code);

	return err_code;
}


