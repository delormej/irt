/* Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
 */

#include <stdint.h>
#include "ant_bike_speed.h"
#include "ant_interface.h"
#include "ble_ant.h"
#include "nrf_error.h"
#include "irt_common.h"
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
	uint8_t page_change:1;
	uint8_t page_number:7;
	uint8_t reserved1;
	uint8_t reserved2;
	uint8_t reserved3;
	uint8_t event_time_lsb;
	uint8_t event_time_msb;
	uint8_t rev_count_lsb;
	uint8_t rev_count_msb;
} ant_sp_page_t;

static ant_sp_page_t page0 = {
		.page_change = 0,
		.page_number = 0,
		.reserved1 = 0xFF,
		.reserved2 = 0xFF,
		.reserved3 = 0xFF,
		.event_time_lsb = 0,
		.event_time_msb = 0,
		.rev_count_lsb = 0,
		.rev_count_msb = 0
};
static uint8_t message_count = 0;

/**@brief	Initialize the speed channel.
 *
 */
void ant_sp_tx_init()
{
    uint32_t err_code;

    err_code = sd_ant_channel_assign(ANT_SP_TX_CHANNEL,
                                     ANT_SP_CHANNEL_TYPE,
                                     ANTPLUS_NETWORK_NUMBER,
                                     ANT_SP_EXT_ASSIGN);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ant_channel_id_set(ANT_SP_TX_CHANNEL,
                                     ANT_DEVICE_NUMBER,
                                     ANT_SP_DEVICE_TYPE,
                                     ANT_SP_TRANS_TYPE);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ant_channel_radio_freq_set(ANT_SP_TX_CHANNEL, ANTPLUS_RF_FREQ);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ant_channel_period_set(ANT_SP_TX_CHANNEL, ANT_SP_MSG_PERIOD);
    APP_ERROR_CHECK(err_code);

    BS_LOG("[BS] ant_sp_tx_init bike speed initialized.\r\n");
}

/**@brief	Opens the channel to start sending speed data.
 *
 */
void ant_sp_tx_start()
{
	uint32_t err_code = sd_ant_channel_open(ANT_SP_TX_CHANNEL);
	APP_ERROR_CHECK(err_code);
}

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
