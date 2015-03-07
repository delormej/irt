/*
 * bp_response_queue.c
 *
 *  Created on: Dec 18, 2014
 *      Author: Jason
 */


#include <stdbool.h>
#include <stdint.h>
#include "ble_ant.h"
#include "bp_response_queue.h"
#include "nordic_common.h"
#include "debug.h"

/**@brief Debug logging for main module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define RQ_LOG debug_log
#else
#define RQ_LOG(...)
#endif // ENABLE_DEBUG_LOG

#define ANT_RESPONSE_LIMIT													4				// DO NOT CHANGE -- see code dependency with counter.  Only send max 4 sequential response messages before interleaving real power message.
static ant_request_data_page_t	m_request_data_pending[ANT_RESPONSE_LIMIT];					// Queue of ANT responses.
static uint8_t m_queue_idx = 0;																// Position counter for the next ANT response queue.

/**@brief	Queues up the last resistance control acknowledgment.
 *
 */
void bp_queue_resistance_ack(uint8_t op_code, uint16_t value)
{
	ant_request_data_page_t resistance_ack;

	resistance_ack.data_page = WF_ANT_RESPONSE_PAGE_ID;
	resistance_ack.descriptor[0] = op_code;
	resistance_ack.reserved[0] = LSB(value);
	resistance_ack.reserved[1] = MSB(value);
	resistance_ack.tx_response = 0x80; // acknowledge

	bp_queue_data_response(resistance_ack);
}

/**@brief	Queue an ANT data response message to be dequeued
 * 			in the appropriate ANT time window to send.
 *
 */
void bp_queue_data_response(ant_request_data_page_t request)
{
	// Limit index to the size of the queue (0,1,2,3) allowing the queue to overflow.
	m_request_data_pending[m_queue_idx & (ANT_RESPONSE_LIMIT-1u)] = request;

	// Advance the queue.
	m_queue_idx++;
}

/**@brief	Last in, first out queue.  Returns false if nothing to dequeue.
 *
 * 			LIFO is done for simplicity.  In reality, we should never really get more than 2
 * 			requests concurrently, 3 should be overkill.
 */
bool bp_dequeue_ant_response(ant_request_data_page_t* p_request)
{
	uint8_t ix;

	if (m_queue_idx == 0)
	{
		// No data.
		return false;
	}
	else
	{
		ix = (m_queue_idx -1) % (ANT_RESPONSE_LIMIT-1u);

		// byte 1 of the buffer contains the flag for either acknowledged (0x80) or a value
		// indicating how many times to send the message.
		if (m_request_data_pending[ix].tx_response == 0x80 ||
				(m_request_data_pending[ix].tx_response & 0x7F) <= 1)
		{
			// Move to the next message.
			m_queue_idx--;
			//RQ_LOG("[RQ] Moving to next message: %i\r\n", m_queue_idx);
		}
		else
		{
			// Decrement the count, we'll need to send again.
			m_request_data_pending[ix].tx_response--;
		}

		// dequeue
		*p_request = m_request_data_pending[ix];

		return true;
	}
}
