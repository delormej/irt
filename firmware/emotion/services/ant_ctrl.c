/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#include <string.h>
#include "ant_ctrl.h"
#include "ble_ant.h"
#include "app_error.h"
#include "irt_common.h"
#include "ant_interface.h"
#include "ant_parameters.h"
#include "app_fifo.h"
#include "debug.h"

//
// Module specific defines.
//
#define ANT_CTRL_CHANNEL_TYPE		0x10                                         
#define ANT_CTRL_DEVICE_TYPE		0x10
#define ANT_CTRL_TRANS_TYPE			0x05	
#define ANT_CTRL_MSG_PERIOD			0x2000		// 8192/32768 (4hz)
#define ANT_CTRL_EXT_ASSIGN			0

#define GENERIC_CONTROL_SUPPORT		(0x01 << 4)	// Generic control supported

#define CTRL_DEVICE_AVAIL_PAGE		0x02		// Control Device Availability - Data Page 2
#define CTRL_CMD_REQ_DATA_PAGE		0x46		// Request Data - Common Data Page 70
#define CTRL_CMD_STATUS_PAGE		0x47		// Command status - Common Page 71
#define CTRL_CMD_PAGE				0x49		// Generic Command Page - Common Data Page 73

/**@brief Debug logging for module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define AC_LOG debug_log
#else
#define AC_LOG(...)
#endif // ENABLE_DEBUG_LOG

const uint8_t manufacturer_page_interleave = 65u;
const uint8_t product_page_interleave = 130u;

typedef enum 
{
	Req_Page		= 0x01,						// Request Data Page
	Req_Page_Slave	= 0x03						// Request Data Page from Slave
} req_cmd_type_e;

// Command status.
typedef enum 
{
	Pass = 0,
	Fail,
	Not_Supported,
	Rejected,
	Pending,
	// 5-254 reserved
	Uninitialized = 255							// Never received a command.
} ctrl_status_e;

//
// Relevant ANT data page formats.
//

// Device Availability Page.
typedef struct 
{
	uint8_t			page_number;
	uint8_t			notifications;					// Sets if the device can connect to additional remotes.
	uint8_t			reserved[5];
	uint8_t			capabilities;					// Bitmask of available capabilities.
} ant_ctrl_data_page2_t;

// Request Data Page.
typedef struct
{
	uint8_t			page_number;
	uint8_t			slave_serial[2];				// Serial number of the remote control
	uint8_t			descriptor1;					// Allows subpage number to be requested.
	uint8_t			descriptor2;					// Allows an additional subpage number to be requested.
	uint8_t			req_response;					// Requested transmission response characteristics.
	uint8_t			req_page_number;				// Requested page number.
	req_cmd_type_e	cmd_type;						// Requested data page or requested data page from slave.
} ant_ctrl_data_page70_t;

// Common Data Page. 
typedef struct
{
	uint8_t			page_number;
	uint8_t			last_command;					// Data page number of the last command page received.  255 indicates no command.
	uint8_t			sequence;						// Sequence # used by slave in last received command.  255 indicates no command.
	ctrl_status_e	status;							// Command status. 255 indicates no command.
	uint8_t			data[4];							// Response specific data.
} ant_ctrl_data_page71_t;

// Command Page.
typedef struct 
{
	uint8_t			page_number;
	uint8_t			slave_serial[2];				// Serial number of the remote control
	uint8_t			slave_manufacturer[2];
	uint8_t			sequence;
	ctrl_command_e	command_lsb;					// All commands currently used are < 0xFF (1 byte), so our command is here.
	uint8_t			command_msb;					// Most significant byte of command (unused).
} ant_ctrl_data_page73_t;

// Template for data page 2.  Using as a constant reduces stack size, we'll copy template when used.
const ant_ctrl_data_page2_t			m_data_page2 = { CTRL_DEVICE_AVAIL_PAGE, 0, { 0, 0, 0, 0, 0 }, GENERIC_CONTROL_SUPPORT };

//
// Module specific initialized state.
//
static ctrl_evt_handler_t			m_on_ctrl_command;
static uint8_t						m_channel_id;
static ant_ctrl_data_page71_t		m_ctrl_status;	// Keeps track of the last command and status.


//
// Circular buffer to queue for page requests.
//
#define FIFO_BUFFER_SIZE 4
static app_fifo_t request_queue;
static uint8_t m_fifo_buffer[FIFO_BUFFER_SIZE];

/**@brief   Queues a request to send a given page.
 */
static void queue_request(uint8_t page_number)
{
    uint32_t err_code;
    err_code = app_fifo_put(&request_queue, page_number);
    
    // If the queue was full, warn and move on.  We won't be able to respond to
    // that request. 
    if (err_code == NRF_ERROR_NO_MEM)
    {
        AC_LOG("[AC] queue_request : QUEUE FULL.\r\n"); 
    }
}

/**@brief   Checks if there are requested pages queued to send.
 *          If so, sends the requested page.
 */
static bool dequeue_request()
{
    uint8_t page_number = 0;
    if (app_fifo_get(&request_queue, &page_number) == NRF_ERROR_NOT_FOUND)
    {
        // No message to send.
        return false;
    } 
    
    switch (page_number)
    {
        case ANT_COMMON_PAGE_80:
            ant_common_page_transmit(m_channel_id, ant_manufacturer_page);
            break;
            
        case ANT_COMMON_PAGE_81:
            ant_common_page_transmit(m_channel_id, ant_product_page);
            break;

        case CTRL_CMD_STATUS_PAGE:
            // The slave isn't required to ask for this, but the master
            // is required to respond via broadcast if it does.
            sd_ant_broadcast_message_tx(m_channel_id, TX_BUFFER_SIZE,
                (uint8_t*) &m_ctrl_status);
            break;

        default:
            AC_LOG("[AC] dequeue_request, unrecognized page:%i\r\n", page_number);
            return false;
            break;            
    }
    
    return true;
}

static void on_command_page(ant_ctrl_data_page73_t* p_page)
{
	ctrl_evt_t evt;

	AC_LOG("[AC] on_command_page, seq:%i\r\n", m_ctrl_status.sequence);

	// Only process if a new sequence.
	if (p_page->sequence == m_ctrl_status.sequence)
		return;

	// Update internal state.
	m_ctrl_status.page_number = CTRL_CMD_PAGE;
	m_ctrl_status.last_command = p_page->command_lsb;
	m_ctrl_status.sequence = p_page->sequence;
	m_ctrl_status.status = Pending;
	
	// Assign the event data payload.
	evt.sequence = p_page->sequence;
	evt.command = p_page->command_lsb;
	evt.remote_serial_no = p_page->slave_serial[0] |
			(p_page->slave_serial[1] << 8);

	// Raise the control command event.
	m_on_ctrl_command(evt);							// TODO: should this return a value to indicate success?

	// Unless there was an error, we should pass here.
	m_ctrl_status.status = Pass; 
}

void ant_ctrl_tx_init(uint8_t channel_id, ctrl_evt_handler_t on_ctrl_command)
{
	uint32_t err_code;

	// Set the channel to use.
	m_channel_id = channel_id;

	// Assign callback for when command message is processed.
	m_on_ctrl_command = on_ctrl_command;

    // Initialize the queue.
    err_code = app_fifo_init(&request_queue, m_fifo_buffer, 
        (uint16_t)sizeof(m_fifo_buffer));
    APP_ERROR_CHECK(err_code);

	// Initialize status.
	memset(&m_ctrl_status, 0, sizeof(ant_ctrl_data_page71_t));
	m_ctrl_status.last_command = 0xFF;
	m_ctrl_status.sequence = 0xFF;
	m_ctrl_status.status = 0xFF; 

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
	uint32_t err_code;

	// Open the ANT channel.
	err_code = sd_ant_channel_open(m_channel_id);
	APP_ERROR_CHECK(err_code);
}

void ant_ctrl_tx_stop(void)
{
	uint32_t err_code;

	// Close the ANT channel.
	err_code = sd_ant_channel_close(m_channel_id);
	APP_ERROR_CHECK(err_code);
}

void ant_ctrl_device_avail_tx(uint8_t notifications)
{
	static uint8_t event_count;		// Event count for interleaving common pages.
	uint32_t err_code;

    // Check to see if we have any pending data page requests.
    // If so that message is sent, so return for this cycle.
    if (dequeue_request())
	{
        return;
	}

	event_count++;	// Increment event count.

	// Interleave common pages.
	if (event_count % product_page_interleave == 0)
	{
		// # Figures out which common message to submit at which time.
		ant_common_page_transmit(m_channel_id, ant_product_page);
	}
	else if (event_count % manufacturer_page_interleave == 0)
	{
		ant_common_page_transmit(m_channel_id, ant_manufacturer_page);
	}
	else
	{
		// Copy the template and set notification status.
		ant_ctrl_data_page2_t page = m_data_page2;
		page.notifications = notifications;
		
		err_code = sd_ant_broadcast_message_tx(m_channel_id,
			TX_BUFFER_SIZE, 
			(uint8_t*) &page);
		APP_ERROR_CHECK(err_code);		
	}
}

void ant_ctrl_rx_handle(ant_evt_t * p_ant_evt)
{
	if (p_ant_evt->event == EVENT_RX) 
	{
		ANT_MESSAGE* p_mesg;

		p_mesg = (ANT_MESSAGE*)p_ant_evt->evt_buffer;

		AC_LOG("ant_ctrl_rx_handle:%i\r\n", p_mesg->ANT_MESSAGE_aucPayload[0]);

		// Switch on page number.
		switch (p_mesg->ANT_MESSAGE_aucPayload[0])
		{
			case CTRL_CMD_REQ_DATA_PAGE:
				// Queue a request for a data page.
				queue_request(p_mesg->ANT_MESSAGE_aucPayload[0]);
				break;

			case CTRL_CMD_PAGE:
				on_command_page((ant_ctrl_data_page73_t*) p_mesg->ANT_MESSAGE_aucPayload);
				break;

			default:
				break;
		}
	}
}
