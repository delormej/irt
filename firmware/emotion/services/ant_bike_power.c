/* Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#include <stdbool.h>
#include <stdint.h>
#include "stdio.h"
#include "app_util.h"
#include "ant_bike_power.h"
#include "ant_parameters.h"
#include "ant_interface.h"
#include "ant_error.h"
#include "app_error.h"
#include "nordic_common.h"
#include "debug.h"

#define POWER_PAGE_INTERLEAVE_COUNT			5u
#define EXTRA_INFO_PAGE_INTERLEAVE_COUNT	8u
#define MANUFACTURER_PAGE_INTERLEAVE_COUNT	121u
#define PRODUCT_PAGE_INTERLEAVE_COUNT		242u
#define BATTERY_PAGE_INTERLEAVE_COUNT		61u

#define PAGE_NUMBER_INDEX               0                   /**< Index of the data page number field.  */
#define EVENT_COUNT_INDEX               1u                  /**< Index of the event count field in the power-only main data page. */
#define PEDAL_POWER_INDEX               2u                  /**< Index of the pedal power field in the power-only main data page. */
#define INSTANT_CADENCE_INDEX           3u                  /**< Index of the instantaneous cadence field in the power-only main data page. */
#define ACCUMMULATED_POWER_LSB_INDEX    4u                  /**< Index of the accumulated power LSB field in the power-only main data page. */
#define ACCUMMULATED_POWER_MSB_INDEX    5u                  /**< Index of the accumulated power MSB field in the power-only main data page. */
#define INSTANT_POWER_LSB_INDEX         6u                  /**< Index of the instantaneous power LSB field in the power-only main data page. */
#define INSTANT_POWER_MSB_INDEX         7u                  /**< Index of the instantaneous power MSB field in the power-only main data page. */

// Custom message fields.
#define EXTRA_INFO_SERVO_POS_LSB		1u
#define EXTRA_INFO_SERVO_POS_MSB		2u
#define EXTRA_INFO_TARGET_LSB			3u
#define EXTRA_INFO_TARGET_MSB			4u
#define EXTRA_INFO_FLYWHEEL_REVS_LSB	5u
#define EXTRA_INFO_FLYWHEEL_REVS_MSB	6u
#define EXTRA_INFO_TEMP					7u

// Standard Wheel Torque Main Data Page (0x11)
#define WHEEL_TICKS_INDEX				2u
#define	WHEEL_PERIOD_LSB_INDEX			4u
#define	WHEEL_PERIOD_MSB_INDEX			5u
#define ACCUMMULATED_TORQUE_LSB_INDEX	6u
#define ACCUMMULATED_TORQUE_MSB_INDEX	7u

#define ANT_BP_CHANNEL_TYPE          0x10                                         /**< Channel Type TX. */
#define ANT_BP_DEVICE_TYPE           0x0B                                         /**< Channel ID device type. */
#define ANT_BP_TRANS_TYPE            0xA5 	// is how we indicate WAHOO KICKR, normal is: 0x5                                         /**< Transmission Type. */
#define ANT_BP_MSG_PERIOD            0x1FF6                                     	 /**< Message Periods, decimal 8182 (~4.00Hz) data is transmitted every 8182/32768 seconds. */
#define ANT_BP_EXT_ASSIGN            0	                                          /**< ANT Ext Assign. */

// Battery Status page.
#define ANT_BAT_ID_INDEX		 	 2
#define ANT_BAT_TIME_LSB_INDEX	 	 3
#define ANT_BAT_TIME_INDEX	 	 	 4
#define ANT_BAT_TIME_MSB_INDEX	 	 5
#define ANT_BAT_FRAC_VOLT_INDEX	 	 6
#define ANT_BAT_DESC_INDEX	 	 	 7


#define ANT_BURST_MSG_ID_SET_RESISTANCE	0x48									/** Message ID used when setting resistance via an ANT BURST. */
#define ANT_BURST_MSG_ID_SET_POSITIONS	0x59									/** Message ID used when setting servo button stop positions via an ANT BURST. */

static const uint8_t 				ACK_MESSAGE_RETRIES = 3;
static const uint8_t 				ACK_MESSAGE_RETRY_DELAY = 5; // milliseconds

/**@brief Debug logging for module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define BP_LOG debug_log
#else
#define BP_LOG(...)
#endif // ENABLE_DEBUG_LOG

/******************************************************************************
		
		Trainer Road specific, from Wahoo Fitness

 ******************************************************************************/
// Manufacturer-Specific pages (0xF0 - 0xFF).
#define WF_ANT_RESPONSE_PAGE_ID              0xF0

//
#define WF_ANT_RESPONSE_FLAG                 0x80

// This is used by Trainer Road, so we'll use the same format to acknowledge a set resistance command.
typedef struct  
{
    uint8_t dataPage;
    uint8_t commandId;
    uint8_t responseStatus;
    uint8_t commandSequence;
    uint8_t responseData0;
    uint8_t responseData1;
    uint8_t responseData2;
    uint8_t responseData3;
    
} ANTMsgWahoo240_t;
/*****************************************************************************/

static uint8_t m_power_tx_buffer[TX_BUFFER_SIZE];
static uint8_t m_torque_tx_buffer[TX_BUFFER_SIZE];
static ant_ble_evt_handlers_t* mp_evt_handlers;

// Shared event counter for all ANT BP messages.
static uint8_t m_event_count;

// TODO: Implement required calibration page.

static __INLINE uint32_t broadcast_message_transmit(const uint8_t * p_buffer)
{
	uint32_t err_code;

	err_code = sd_ant_broadcast_message_tx(ANT_BP_TX_CHANNEL, TX_BUFFER_SIZE, (uint8_t*)p_buffer);

	if (ANT_ERROR_AS_WARN(err_code))
	{
		//BP_LOG("[BP]:broadcast_message_transmit WARN:%#.8x\r\n", err_code);
		BP_LOG("[BP]:broadcast_message_transmit WARN:%#.8x\r\n\t[%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\r\n",
				err_code,
				p_buffer[0],
				p_buffer[1],
				p_buffer[2],
				p_buffer[3],
				p_buffer[4],
				p_buffer[5],
				p_buffer[6],
				p_buffer[7],
				p_buffer[8]);
		err_code = NRF_SUCCESS;
	}
	
	return err_code;
}

static __INLINE uint32_t acknolwedge_message_transmit(const uint8_t * p_buffer)
{
	uint32_t err_code;
	uint8_t retries = 0;

	while (retries++ < ACK_MESSAGE_RETRIES)
	{
		err_code = sd_ant_acknowledge_message_tx(ANT_BP_TX_CHANNEL, TX_BUFFER_SIZE, (uint8_t*)p_buffer);
		if (ANT_ERROR_AS_WARN(err_code))
		{
			BP_LOG("[BP]:acknolwedge_message_transmit retry: %i, %#.8x\r\n", retries, err_code);

			// Sleep and try again.
			nrf_delay_ms(ACK_MESSAGE_RETRY_DELAY);
			continue;
		}
		else
		{
			// No need to retry, either success or hard fail.
			break;
		}
	}

	if (ANT_ERROR_AS_WARN(err_code))
	{
		BP_LOG("[BP]:acknolwedge_message_transmit WARN: %#.8x\r\n", err_code);
		err_code = NRF_SUCCESS;
	}
	
	return err_code;
}

static uint32_t torque_transmit(uint16_t accum_torque, uint16_t accum_wheel_period_2048, uint8_t wheel_ticks)
{

	// Time-synchronous model.  Increment the event count.
	m_torque_tx_buffer[EVENT_COUNT_INDEX] = m_event_count;

	m_torque_tx_buffer[WHEEL_TICKS_INDEX] = wheel_ticks;
	m_torque_tx_buffer[WHEEL_PERIOD_LSB_INDEX] = LOW_BYTE(accum_wheel_period_2048);
	m_torque_tx_buffer[WHEEL_PERIOD_MSB_INDEX] = HIGH_BYTE(accum_wheel_period_2048);
	m_torque_tx_buffer[ACCUMMULATED_TORQUE_LSB_INDEX] = LOW_BYTE(accum_torque);
	m_torque_tx_buffer[ACCUMMULATED_TORQUE_MSB_INDEX] = HIGH_BYTE(accum_torque);

	return broadcast_message_transmit(m_torque_tx_buffer);
}

static uint32_t power_transmit(uint16_t watts)
{	
	static uint16_t accumulated_power                 = 0;            
	accumulated_power                                += watts;
		
	m_power_tx_buffer[EVENT_COUNT_INDEX]			  = m_event_count;
	m_power_tx_buffer[ACCUMMULATED_POWER_LSB_INDEX]   = LOW_BYTE(accumulated_power);        
	m_power_tx_buffer[ACCUMMULATED_POWER_MSB_INDEX]   = HIGH_BYTE(accumulated_power);   
	m_power_tx_buffer[INSTANT_POWER_LSB_INDEX]        = LOW_BYTE(watts);            
	m_power_tx_buffer[INSTANT_POWER_MSB_INDEX]        = HIGH_BYTE(watts);                
			
	return broadcast_message_transmit(m_power_tx_buffer);
}

// Encodes the resistance mode into the 2 most significant bits.
static uint8_t encode_resistance_level(irt_power_meas_t * p_power_meas)
{
	uint8_t target_msb;
	uint8_t mode;

	// Subtract 64 (0x40) from mode to use only 2 bits.
	// Modes only go from 0x40 - 0x45 or so.
	mode = p_power_meas->resistance_mode - 64u;

	// Grab the most significant bits of the resistance level.
	target_msb = HIGH_BYTE(p_power_meas->resistance_level);

	// Use the 2 most significant bits for the mode and stuff them in the
	// highest 2 bits.  Level should never need to use these 2 bits.
	target_msb |= mode << 6;

	return target_msb;
}

uint32_t ant_bp_battery_tx_send(irt_battery_status_t status)
{
	uint8_t buffer[TX_BUFFER_SIZE];

	buffer[PAGE_NUMBER_INDEX]			= ANT_PAGE_BATTERY_STATUS;
	buffer[1]							= 0xFF;
	buffer[ANT_BAT_ID_INDEX]			= 0b00010001;	// bits 0:3 = Number of batteries, 4:7 = Identifier.
	buffer[ANT_BAT_TIME_LSB_INDEX]	 	= (uint8_t)status.operating_time;
	buffer[ANT_BAT_TIME_INDEX]	 		= (status.operating_time & 0x0000FF00) >> 8;
	buffer[ANT_BAT_TIME_MSB_INDEX]		= (status.operating_time & 0x00FF0000) >> 16;
	buffer[ANT_BAT_FRAC_VOLT_INDEX]		= status.fractional_volt;
	buffer[ANT_BAT_DESC_INDEX]			= status.coarse_volt |
											status.status << 4 |
											status.resolution << 7;

	return sd_ant_broadcast_message_tx(ANT_BP_TX_CHANNEL, TX_BUFFER_SIZE, (uint8_t*)&buffer);
}

// Transmits extra info embedded in the power measurement.
// TODO: Need a formal message/methodology for this.
static uint32_t extra_info_transmit(irt_power_meas_t * p_power_meas)
{
	uint8_t buffer[TX_BUFFER_SIZE];
	uint16_t flywheel;

	buffer[PAGE_NUMBER_INDEX]			= ANT_BP_PAGE_EXTRA_INFO;
	buffer[EXTRA_INFO_SERVO_POS_LSB]	= LOW_BYTE(p_power_meas->servo_position);
	buffer[EXTRA_INFO_SERVO_POS_MSB]	= HIGH_BYTE(p_power_meas->servo_position);
	buffer[EXTRA_INFO_TARGET_LSB]		= LOW_BYTE(p_power_meas->resistance_level);
	buffer[EXTRA_INFO_TARGET_MSB]		= encode_resistance_level(p_power_meas);
	flywheel 							= (uint16_t)p_power_meas->accum_flywheel_ticks;
	buffer[EXTRA_INFO_FLYWHEEL_REVS_LSB]= LOW_BYTE(flywheel);
	buffer[EXTRA_INFO_FLYWHEEL_REVS_MSB]= HIGH_BYTE(flywheel);
	buffer[EXTRA_INFO_TEMP]				= (uint8_t)(p_power_meas->temp);

	return sd_ant_broadcast_message_tx(ANT_BP_TX_CHANNEL, TX_BUFFER_SIZE, (uint8_t*)&buffer);
}

static void handle_move_servo(ant_evt_t * p_ant_evt)
{
	rc_evt_t evt;

	evt.operation = RESISTANCE_SET_SERVO_POS;
	evt.pBuffer = &(p_ant_evt->evt_buffer[ANT_BP_COMMAND_OFFSET+2]);
	mp_evt_handlers->on_set_resistance(evt);
}

static void handle_set_weight(ant_evt_t * p_ant_evt)
{
	// TODO: currently this is in the resistance command.
	// TODO: Change this to  use data page 2 get/set params.
	rc_evt_t evt;

	evt.operation = RESISTANCE_SET_WEIGHT;
	evt.pBuffer = &(p_ant_evt->evt_buffer[ANT_BP_COMMAND_OFFSET+2]);
	mp_evt_handlers->on_set_resistance(evt);
}

/**@brief	Manages the state of a burst sequence which has 3 messages
 *			and starts with a byte that represent the type of message.
 */
static void handle_burst(ant_evt_t * p_ant_evt)
{
	static bool in_burst = false;
	static uint8_t message_id = 0;

	static rc_evt_t	resistance_evt; // resistance message

	if (BURST_SEQ_FIRST_PACKET(p_ant_evt->evt_buffer[2]))
	{
		// Flag that we'll be receiving multiple messages.
		in_burst = true;
		message_id = p_ant_evt->evt_buffer[3];

		switch (message_id)
		{
			case ANT_BURST_MSG_ID_SET_RESISTANCE:
				memset(&resistance_evt, 0, sizeof(rc_evt_t));
				resistance_evt.operation = (resistance_mode_t) p_ant_evt->evt_buffer[4];
				break;

			case ANT_BURST_MSG_ID_SET_POSITIONS:
				break;

			default:
				BP_LOG("[BP] handle_burst WARN: unrecognized message_id: %i\r\n", message_id);
				break;
		}
	}
	else if (BURST_SEQ_LAST_PACKET(p_ant_evt->evt_buffer[2]) && in_burst)
	{
		// Reset state.
		in_burst = false;

		switch (message_id)
		{
			case ANT_BURST_MSG_ID_SET_RESISTANCE:
				// Value for the operation exists in this message sequence.
				resistance_evt.pBuffer = &(p_ant_evt->evt_buffer[3]);
				mp_evt_handlers->on_set_resistance(resistance_evt);
				break;

			case ANT_BURST_MSG_ID_SET_POSITIONS:
				//
				break;

			default:
				BP_LOG("[BP] handle_burst WARN: unrecognized message_id: %i\r\n", message_id);
				break;
		}
	}
	else
	{
		// Middle message.
	}

	/*BP_LOG("[BP]:handle_burst [%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\r\n",
			p_ant_evt->evt_buffer[0],
			p_ant_evt->evt_buffer[1],
			p_ant_evt->evt_buffer[2],
			p_ant_evt->evt_buffer[3],
			p_ant_evt->evt_buffer[4],
			p_ant_evt->evt_buffer[5],
			p_ant_evt->evt_buffer[6],
			p_ant_evt->evt_buffer[7],
			p_ant_evt->evt_buffer[8]);*/
}

/**@brief	Handles WAHOO API specific commands.
 * 			Note: A couple of custom hack commands are here too.
 */
static void handle_wahoo_page(ant_evt_t * p_ant_evt)
{
	// Determine the "command".
	switch (p_ant_evt->evt_buffer[ANT_BP_COMMAND_OFFSET])
	{
		case ANT_BP_SET_WEIGHT_COMMAND:	// Set Weight
			handle_set_weight(p_ant_evt);
			break;

		case ANT_BP_ENABLE_DFU_COMMAND:	// Invoke device firmware update mode.
			mp_evt_handlers->on_enable_dfu_mode();
			break;

		case ANT_BP_MOVE_SERVO_COMMAND: // Move the servo to a specific position.
			handle_move_servo(p_ant_evt);
			break;

		default:
			break;
	}
}

// TODO: need to implement calibration requests as well.
void ant_bp_rx_handle(ant_evt_t * p_ant_evt)
{
	// Only interested in BURST events right now for processing resistance control.
	if (p_ant_evt->evt_buffer[ANT_BUFFER_INDEX_MESG_ID] == MESG_BURST_DATA_ID)
	{
		handle_burst(p_ant_evt);
	}
	// TODO: remove these hard coded array position values and create defines.
	else if (p_ant_evt->evt_buffer[ANT_BUFFER_INDEX_MESG_ID] == MESG_ACKNOWLEDGED_DATA_ID)
	{
		switch (p_ant_evt->evt_buffer[3])  // Switch on the page number.
		{
			case WF_ANT_RESPONSE_PAGE_ID:
				handle_wahoo_page(p_ant_evt);
				break;

			case ANT_PAGE_GETSET_PARAMETERS:
				BP_LOG("[BP]:ANT GET/SET Page Sent.\r\n");
				mp_evt_handlers->on_set_parameter(&(p_ant_evt->evt_buffer[3]));
				break;

			case ANT_PAGE_REQUEST_DATA:
				BP_LOG("[BP]:ANT Data Request Page.\r\n");
				mp_evt_handlers->on_request_data(&(p_ant_evt->evt_buffer[3]));
				break;

			default:
				BP_LOG("[BP]:unrecognized message [%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\r\n",
						p_ant_evt->evt_buffer[0],
						p_ant_evt->evt_buffer[1],
						p_ant_evt->evt_buffer[2],
						p_ant_evt->evt_buffer[3],
						p_ant_evt->evt_buffer[4],
						p_ant_evt->evt_buffer[5],
						p_ant_evt->evt_buffer[6],
						p_ant_evt->evt_buffer[7],
						p_ant_evt->evt_buffer[8]);
				break;
		}
	}
}

void ant_bp_tx_init(ant_ble_evt_handlers_t * evt_handlers)
{
    uint32_t err_code;
    
	// Assign callback for when resistance message is processed.	
    mp_evt_handlers = evt_handlers;
		
    err_code = sd_ant_network_address_set(ANTPLUS_NETWORK_NUMBER, (uint8_t *)m_ant_network_key);
    APP_ERROR_CHECK(err_code);
    
    err_code = sd_ant_channel_assign(ANT_BP_TX_CHANNEL,
                                     ANT_BP_CHANNEL_TYPE,
                                     ANTPLUS_NETWORK_NUMBER,
                                     ANT_BP_EXT_ASSIGN);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ant_channel_id_set(ANT_BP_TX_CHANNEL,
                                     ANT_DEVICE_NUMBER,
                                     ANT_BP_DEVICE_TYPE,
                                     ANT_BP_TRANS_TYPE);
    APP_ERROR_CHECK(err_code);
    
    err_code = sd_ant_channel_radio_freq_set(ANT_BP_TX_CHANNEL, ANTPLUS_RF_FREQ);
    APP_ERROR_CHECK(err_code);
    
    err_code = sd_ant_channel_period_set(ANT_BP_TX_CHANNEL, ANT_BP_MSG_PERIOD);
    APP_ERROR_CHECK(err_code);
		
	// Initialize power transmit buffer.
	m_power_tx_buffer[PAGE_NUMBER_INDEX]              = ANT_BP_PAGE_STANDARD_POWER_ONLY;
	m_power_tx_buffer[EVENT_COUNT_INDEX]              = 0;
	m_power_tx_buffer[PEDAL_POWER_INDEX]              = BP_PAGE_RESERVE_BYTE;
	m_power_tx_buffer[INSTANT_CADENCE_INDEX]          = BP_PAGE_RESERVE_BYTE;
	m_power_tx_buffer[ACCUMMULATED_POWER_LSB_INDEX]   = 0;
	m_power_tx_buffer[ACCUMMULATED_POWER_MSB_INDEX]   = 0;
	m_power_tx_buffer[INSTANT_POWER_LSB_INDEX]        = 0;
	m_power_tx_buffer[INSTANT_POWER_MSB_INDEX]        = 0;

	// Initialize torque transmit buffer.
	m_torque_tx_buffer[PAGE_NUMBER_INDEX]              = ANT_BP_PAGE_TORQUE_AT_WHEEL;
	m_torque_tx_buffer[EVENT_COUNT_INDEX]              = 0;
	m_torque_tx_buffer[WHEEL_TICKS_INDEX]              = 0;
	m_torque_tx_buffer[INSTANT_CADENCE_INDEX]          = BP_PAGE_RESERVE_BYTE;
	m_torque_tx_buffer[WHEEL_PERIOD_LSB_INDEX]         = 0;
	m_torque_tx_buffer[WHEEL_PERIOD_MSB_INDEX]   	   = 0;
	m_torque_tx_buffer[ACCUMMULATED_TORQUE_LSB_INDEX]  = 0;
	m_torque_tx_buffer[ACCUMMULATED_TORQUE_MSB_INDEX]  = 0;
}

void ant_bp_tx_start(void)
{
    uint32_t err_code = sd_ant_channel_open(ANT_BP_TX_CHANNEL);
    APP_ERROR_CHECK(err_code);
}

void ant_bp_tx_send(irt_power_meas_t * p_power_meas)
{
	const uint8_t power_page_interleave 		= POWER_PAGE_INTERLEAVE_COUNT;
	const uint8_t product_page_interleave 		= PRODUCT_PAGE_INTERLEAVE_COUNT;
	const uint8_t manufacturer_page_interleave 	= MANUFACTURER_PAGE_INTERLEAVE_COUNT;
	const uint8_t extra_info_page_interleave 	= EXTRA_INFO_PAGE_INTERLEAVE_COUNT;
	const uint8_t battery_page_interleave 		= BATTERY_PAGE_INTERLEAVE_COUNT;

	uint32_t err_code = 0;		

	// Increment event counter.
	m_event_count++;

	if (m_event_count % product_page_interleave == 0)
	{			
		// # Figures out which common message to submit at which time.
		ANT_COMMON_PAGE_TRANSMIT(ANT_BP_TX_CHANNEL, ant_product_page);
	}
	else if (m_event_count % manufacturer_page_interleave == 0)
	{
		ANT_COMMON_PAGE_TRANSMIT(ANT_BP_TX_CHANNEL, ant_manufacturer_page);
	}
	else if (m_event_count % battery_page_interleave == 0)
	{
		ant_bp_battery_tx_send(p_power_meas->battery_status);
	}
	else if (m_event_count % extra_info_page_interleave == 0)
	{
		// Send DEBUG info.
		extra_info_transmit(p_power_meas);
	}
	else if (m_event_count % power_page_interleave == 0)
	{
		// # Only transmit standard power message every 5th power message.
		err_code = power_transmit(p_power_meas->instant_power);
		APP_ERROR_CHECK(err_code);
	}
	else
	{
		// # Default broadcast message is torque.
		err_code = torque_transmit(p_power_meas->accum_torque,
			p_power_meas->accum_wheel_period,
			(uint8_t) p_power_meas->accum_wheel_revs);
		APP_ERROR_CHECK(err_code);
	}
}

uint32_t ant_bp_resistance_tx_send(resistance_mode_t mode, uint16_t value)
{
	// State required to be managed.
	static uint8_t resistance_sequence = 0;

	uint32_t err_code;

	// TODO: should we use this struct instead? ANTMsgWahoo240_t
	uint8_t tx_buffer[TX_BUFFER_SIZE] =
	{
		WF_ANT_RESPONSE_PAGE_ID,
		mode,
		WF_ANT_RESPONSE_FLAG,
		++resistance_sequence,
		LOW_BYTE(value),
		HIGH_BYTE(value),
		0x01, 	// Again, not sure why, but KICKR responds with this.
		0x00
	};

	err_code = broadcast_message_transmit(tx_buffer);
	BP_LOG("[BP]:acknowledged OP:[%.2x] VAL:[%.2x][%.2x]\r\n",
			mode, LOW_BYTE(value), HIGH_BYTE(value));

	return err_code;
}

/**@brief	Sends get/set parameters page.  This is sent in response to a
 * 			Page 0x46 (Request Data Page) message.  Returns a broadcast message.
 *
 *@note		Transmission type: (from 16.2.1.2 in the spec) - not fully implemented here.
 * 			The power meter shall be able to support all requested transmission response types;
 * 			however, the ANT+ bicycle power device profile further stipulates that the display
 * 			shall only request broadcast messages from a power meter sensor.
 *
 *			tx_type Describes transmission characteristics of the data requested.
 *			Bit 0-6: Number of times to transmit requested page.
 *			Bit 7: Setting the MSB means the device replies using acknowledged messages if possible.
 *			Special Values: 0x80 - Transmit until a successful acknowledge is received. 0x00 – Invalid
 *
 */
void ant_bp_page2_tx_send(uint8_t subpage, uint8_t buffer[6], uint8_t tx_type)
{
	uint32_t err_code;

	uint8_t tx_buffer[TX_BUFFER_SIZE] =
	{
		ANT_PAGE_GETSET_PARAMETERS,
		subpage,
		buffer[0],
		buffer[1],
		buffer[2],
		buffer[3],
		buffer[4],
		buffer[5]
	};

	/*BP_LOG("[BP]:Sending page 2 response [%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\r\n",
			tx_buffer[0],
			tx_buffer[1],
			tx_buffer[2],
			tx_buffer[3],
			tx_buffer[4],
			tx_buffer[5],
			tx_buffer[6],
			tx_buffer[7]);*/

	if (tx_type == 0x80)
	{
		// Send Acknowledged.
		err_code = acknolwedge_message_transmit(tx_buffer);
	}
	else
	{
		// Send Broadcast.
		err_code = broadcast_message_transmit(tx_buffer);
	}

	APP_ERROR_CHECK(err_code);
}

/**@brief	Sends Measurement Output Data Page (0x03).  Normally sent only during calibration.
 *@note		The measurement output data page shall only be sent as a broadcast message from the
 *@note		sensor to the display.
 */
void ant_bp_page3_tx_send(
		uint8_t meas_count,						// Number of data types currently being rotated through.
		ant_output_meas_data_type data_type,
		int8_t scale_factor,
		uint16_t timestamp,
		int16_t value)
{
	uint32_t err_code;

	if (meas_count > 15)	// Only 3 bits are used, upper 4 bits are reserved for future.
	{
		// Throw an error -- for now log
		BP_LOG("[BP] Attempt to send more than 15 measurement data types, truncating.\r\n");
		meas_count = 15;
	}

	uint8_t tx_buffer[TX_BUFFER_SIZE] =
	{
		ANT_PAGE_MEASURE_OUTPUT,
		meas_count,
		(uint8_t)data_type,
		scale_factor,
		LSB(timestamp),
		MSB(timestamp),
		LSB(value),
		MSB(value)
	};

	// Send Broadcast.
	err_code = broadcast_message_transmit(tx_buffer);

	BP_LOG("[BP] Sent temperature.\r\n");

	APP_ERROR_CHECK(err_code);
}

