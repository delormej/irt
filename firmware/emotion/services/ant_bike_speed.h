/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#ifndef ANT_BIKE_SPEED_H__
#define ANT_BIKE_SPEED_H__

#define ANT_SP_CHANNEL_TYPE          0x10                                       /**< Channel Type TX. */
#define ANT_SP_DEVICE_TYPE           0x7B                                       /**< Channel ID device type. */
#define ANT_SP_TRANS_TYPE            0x01 										/**< Transmission Type. */
#define ANT_SP_MSG_PERIOD            0x1FB6                                     /**< Message Periods, decimal 8182 (~4.00Hz) data is transmitted every 8182/32768 seconds. */
#define ANT_SP_EXT_ASSIGN            0	                                        /**< ANT Ext Assign. */

/**@brief	Initialize the speed channel.
 *
 */
void ant_sp_tx_init(void);

/**@brief	Sends the required ANT speed message.
 *			Determines when to send MANUF and PROD pages.
 */
uint32_t ant_sp_tx_send(uint16_t event_time_1024, uint16_t cum_rev_count);

#endif /* ANT_BIKE_SPEED_H__ */
