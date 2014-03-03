/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#include "ant_ctrl.h"

//
// Module specific defines.
//
#define ANT_DEVICE_NUMBER			0x01		// TODO: this needs to be defined centraly (irt.h)
#define ANT_RF_FREQ					0x39		// TODO: this needs to be defined centraly as well.

#define ANT_CTRL_TX_CHANNEL			3u
#define ANT_CTRL_CHANNEL_TYPE		0x10                                         
#define ANT_CTRL_DEVICE_TYPE		0x10
#define ANT_CTRL_TRANS_TYPE			0x05	
#define ANT_CTRL_MSG_PERIOD			0x0020		// 8192/32768 (4hz)
#define ANT_CTRL_EXT_ASSIGN			0

#define GENERIC_CONTROL_SUPPORT		(0x1 < 4)	// Generic control supported

#define CTRL_COMMAND_PAGE			0x49		// Page Number 73

//
// Data pages.
//
typedef struct 
{
	uint8_t			page_number;
	uint8_t			notifications;
	uint8_t[5]		reserved;
	uint8_t			capabilities;
} ant_ctrl_data_page2_t;

typedef struct
{
	uint8_t			page_number;
	uint8_t[2]		slave_serial;					// Serial number of the remote control
	uint8_t[2]		slave_manufacturer;
	uint8_t			sequence;
	ctrl_command_t	command_lsb;					// All commands currently used are < 0xFF (1 byte), so our command is here.
	uint8_t			command_msb;					// Most significant byte of command (unused).
} ant_ctrl_data_page73_t;

//
// Module specific initialized state.
//
static ant_ctrl_data_page2_t		m_data_page2;
static ctrl_evt_handler_t			m_on_ctrl_command;

void ant_ctrl_tx_init(ctrl_evt_handler_t on_ctrl_command)
{
	uint32_t err_code;

	// Assign callback for when command message is processed.
	m_on_ctrl_command = on_ctrl_command;

	// Configure data page 2.
	memset(&m_data_page2, 0, sizeof(ant_ctrl_data_page2_t));
	
	m_data_page2.page_number = 0x02;
	m_data_page2.capabilities = GENERIC_CONTROL_SUPPORT;

	err_code = sd_ant_network_address_set(ANTPLUS_NETWORK_NUMBER, m_ant_network_key);
	APP_ERROR_CHECK(err_code);

	err_code = sd_ant_channel_assign(ANT_CTRL_TX_CHANNEL,
		ANT_CTRL_CHANNEL_TYPE,
		ANTPLUS_NETWORK_NUMBER,
		ANT_CTRL_EXT_ASSIGN);
	
	APP_ERROR_CHECK(err_code);

	err_code = sd_ant_channel_id_set(ANT_CTRL_TX_CHANNEL,
		ANT_DEVICE_NUMBER,
		ANT_CTRL_DEVICE_TYPE,
		ANT_CTRL_TRANS_TYPE);
	
	APP_ERROR_CHECK(err_code);

	err_code = sd_ant_channel_radio_freq_set(ANT_CTRL_TX_CHANNEL, ANTPLUS_RF_FREQ);
	APP_ERROR_CHECK(err_code);

	err_code = sd_ant_channel_period_set(ANT_CTRL_TX_CHANNEL, ANT_CTRL_MSG_PERIOD);
	APP_ERROR_CHECK(err_code);
}

void ant_ctrl_tx_start(void)
{
	// Open the ANT channel.
	uint32_t err_code = sd_ant_channel_open(ANT_CTRL_TX_CHANNEL);
	APP_ERROR_CHECK(err_code);
}

void ant_ctrl_tx_stop(void)
{
	uint32_t err_code;

	// Close the ANT channel.
	err_code = sd_ant_channel_close(ANT_CTRL_TX_CHANNEL);
	APP_ERROR_CHECK(err_code);
}

void ant_ctrl_device_available(uint8_t notifications)
{
	uint32_t err_code;

	// Set notification status.
	m_data_page2.notifications = notifications;

	err_code = sd_ant_broadcast_message_tx(ANT_CTRL_TX_CHANNEL, 
											TX_BUFFER_SIZE, 
											(uint8_t*) &m_data_page2);
	
	APP_ERROR_CHECK(err_code);
}

void ant_ctrl_rx_handle(ant_evt_t * p_ant_evt)
{
	ant_ctrl_data_page73_t page;
	ctrl_evt_t evt;

	// Only process page 73.
	if (p_ant_evt->evt_buffer[0] != CTRL_COMMAND_PAGE)
		return;

	// Make it easier to work with the data structure.
	page = (ant_ctrl_data_page73_t)p_ant_evt->evt_buffer;

	// Assign the event data payload.
	evt.sequence = page.sequence;
	evt.command = page.command_lsb;

	// Raise the control command event.
	m_on_ctrl_command(evt);
}
