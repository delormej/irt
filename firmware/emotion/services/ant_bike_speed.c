/* Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
 */

#include "ant_bike_speed.h"
#include "ble_ant.h"
#include "nrf_error.h"
#include "debug.h"

/**@brief Debug logging for module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define BS_LOG debug_log
#else
#define BS_LOG(...)
#endif // ENABLE_DEBUG_LOG

typedef struct ant_sp_page_s {
	uint8_t page_change:1 = 0;
	uint8_t page_number:7 = 0;
	uint8_t reserved1 = 0xFF;
	uint8_t reserved2 = 0xFF;
	uint8_t reserved3 = 0xFF;
	uint8_t event_time_lsb = 0;
	uint8_t event_time_msb = 0;
	uint8_t rev_count_lsb = 0;
	uint8_t rev_count_msb = 0;
} ant_sp_page_t;

static ant_sp_page_t page0;
static uint8_t message_count = 0;

/**@brief	Sends the required ANT speed message.
 *			Determines when to send MANUF and PROD pages.
 */
uint32_t ant_sp_tx_send(uint16_t event_time_1024, uint16_t cum_rev_count)
{
	uint32_t err_code;

	if (message_count % 4 == 0)
	{
		// Toggle page change.
		page0.page_change = ~page0.page_change;
	}

	page0.event_time_lsb = LOW_BYTE(event_time_1024);
	page0.event_time_msb = HIGH_BYTE(event_time_1024);
	page0.rev_count_lsb = LOW_BYTE(cum_rev_count);
	page0.rev_count_msb = HIGH_BYTE(cum_rev_count);

	err_code = sd_ant_broadcast_message_tx(ANT_SP_TX_CHANNEL, TX_BUFFER_SIZE, (uint8_t*)&page0);

	if (ANT_ERROR_AS_WARN(err_code))
	{
		err_code = NRF_SUCCESS;
		BS_LOG("[BS] ant_sp_tx_send WARN: %i\r\n", err_code);
	}

	message_count++;

	return err_code;
}
