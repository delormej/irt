/*
*******************************************************************************
* 
* By Jason De Lorme <jason@insideride.com>
* (c) Inside Ride Technologies, LLC
*
********************************************************************************/

#ifndef __BP_RESPONSE_QUEUE_H__
#define __BP_RESPONSE_QUEUE_H__

#include <stdint.h>
#include <stdbool.h>

/**@brief	Queues up the last resistance control acknowledgment.
 *
 */
void bp_queue_resistance_ack(uint8_t op_code, uint16_t value);

/**@brief	Queue an ANT data response message to be dequeued
 * 			in the appropriate ANT time window to send.
 *
 */
void bp_queue_data_response(ant_request_data_page_t request);

/**@brief	Last in, first out queue.  Returns false if nothing to dequeue.
 *
 * 			LIFO is done for simplicity.  In reality, we should never really get more than 2
 * 			requests concurrently, 3 should be overkill.
 */
bool bp_dequeue_ant_response(ant_request_data_page_t* p_request);

#endif // __BP_RESPONSE_QUEUE_H__
