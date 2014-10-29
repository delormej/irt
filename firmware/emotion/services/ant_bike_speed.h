/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#ifndef ANT_BIKE_SPEED_H__
#define ANT_BIKE_SPEED_H__

/**@brief	Sends the required ANT speed message.
 *			Determines when to send MANUF and PROD pages.
 */
uint32_t ant_sp_tx_send(uint16_t event_time_1024, uint16_t cum_rev_count);

#endif /* ANT_BIKE_SPEED_H__ */
