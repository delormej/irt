/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#include <string.h>
#include "ant_ctrl.h"
#include "ble_ant.h"
#include "app_error.h"
#include "irt_emotion.h"
#include "ant_interface.h"

//
// Module specific defines.
//
#define ANT_CTRL_CHANNEL_TYPE		0x10                                         
#define ANT_CTRL_DEVICE_TYPE		0x10
#define ANT_CTRL_TRANS_TYPE			0x05	
#define ANT_CTRL_MSG_PERIOD			0x0020		// 8192/32768 (4hz)
#define ANT_CTRL_EXT_ASSIGN			0

#define GENERIC_CONTROL_SUPPORT		(0x01 << 4)	// Generic control supported

#define CTRL_COMMAND_PAGE			0x49		// Page Number 73

//
// Data pages.
//
typedef struct 
{
	uint8_t			page_number;
	uint8_t			notifications;
	uint8_t			reserved[5];
	uint8_t			capabilities;
} ant_ctrl_data_page2_t;

typedef struct 
{
	uint8_t			page_number;
	uint8_t			slave_serial[2];				// Serial number of the remote control
	uint8_t			slave_manufacturer[2];
	uint8_t			sequence;
	ctrl_command_e	command_lsb;					// All commands currently used are < 0xFF (1 byte), so our command is here.
	uint8_t			command_msb;					// Most significant byte of command (unused).
} ant_ctrl_data_page73_t;

//
// Module specific initialized state.
//
static uint8_t						m_channel_id;
static ant_ctrl_data_page2_t		m_data_page2;
static ctrl_evt_handler_t			m_on_ctrl_command;

void ant_ctrl_tx_init(uint8_t channel_id, ctrl_evt_handler_t on_ctrl_command)
{
	uint32_t err_code;

	// Set the channel id to use.
	m_channel_id = channel_id;

	// Assign callback for when command message is processed.
	m_on_ctrl_command = on_ctrl_command;

	// Configure data page 2.
	memset(&m_data_page2, 0, sizeof(ant_ctrl_data_page2_t));
	
	m_data_page2.page_number = 0x02;
	m_data_page2.capabilities = GENERIC_CONTROL_SUPPORT;

	err_code = sd_ant_network_address_set(ANTPLUS_NETWORK_NUMBER, m_ant_network_key);
	APP_ERROR_CHECK(err_code);

	err_code = sd_ant_channel_assign(m_channel_id,
		ANT_CTRL_CHANNEL_TYPE,
		ANTPLUS_NETWORK_NUMBER,
		ANT_CTRL_EXT_ASSIGN);
	
	APP_ERROR_CHECK(err_code);

	err_code = sd_ant_channel_id_set(m_channel_id,
		ANT_DEVICE_NUMBER,
		ANT_CTRL_DEVICE_TYPE,
		ANT_CTRL_TRANS_TYPE);
	
	APP_ERROR_CHECK(err_code);

	err_code = sd_ant_channel_radio_freq_set(m_channel_id, ANTPLUS_RF_FREQ);
	APP_ERROR_CHECK(err_code);

	err_code = sd_ant_channel_period_set(m_channel_id, ANT_CTRL_MSG_PERIOD);
	APP_ERROR_CHECK(err_code);
}

void ant_ctrl_tx_start(void)
{
	// Open the ANT channel.
	uint32_t err_code = sd_ant_channel_open(m_channel_id);
	APP_ERROR_CHECK(err_code);
}

void ant_ctrl_tx_stop(void)
{
	uint32_t err_code;

	// Close the ANT channel.
	err_code = sd_ant_channel_close(m_channel_id);
	APP_ERROR_CHECK(err_code);
}

void ant_ctrl_device_available(uint8_t notifications)
{
	uint32_t err_code;

	// Set notification status.
	m_data_page2.notifications = notifications;

	err_code = sd_ant_broadcast_message_tx(m_channel_id,
										TX_BUFFER_SIZE, 
										(uint8_t*) &m_data_page2);
	
	APP_ERROR_CHECK(err_code);
}

void ant_ctrl_rx_handle(ant_evt_t * p_ant_evt)
{
	ant_ctrl_data_page73_t *p_page;
	ctrl_evt_t evt;

	// Only process page 73.
	if (p_ant_evt->evt_buffer[0] != CTRL_COMMAND_PAGE)
		return;

	// Make it easier to work with the data structure.
	p_page = (ant_ctrl_data_page73_t*)p_ant_evt->evt_buffer;

	// Assign the event data payload.
	evt.sequence = p_page->sequence;
	evt.command = p_page->command_lsb;

	// Raise the control command event.
	m_on_ctrl_command(evt);
}
