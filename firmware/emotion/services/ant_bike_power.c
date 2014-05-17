/* Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#include <stdint.h>
#include "stdio.h"
#include "ant_bike_power.h"
#include "ant_parameters.h"
#include "ant_interface.h"
#include "ant_error.h"
#include "app_error.h"
#include "debug.h"

#define POWER_PAGE_INTERLEAVE_COUNT			5u
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
#define EXTRA_INFO_FLYWHEEL_REVS		1u
#define EXTRA_INFO_SERVO_POS_LSB		2u
#define EXTRA_INFO_SERVO_POS_MSB		3u
#define EXTRA_INFO_TARGET_LSB			4u
#define EXTRA_INFO_TARGET_MSB			5u
#define EXTRA_INFO_TEMP					6u

// Standard Wheel Torque Main Data Page (0x11)
#define WHEEL_TICKS_INDEX				2u
#define	WHEEL_PERIOD_LSB_INDEX			4u
#define	WHEEL_PERIOD_MSB_INDEX			5u
#define ACCUMMULATED_TORQUE_LSB_INDEX	6u
#define ACCUMMULATED_TORQUE_MSB_INDEX	7u

#define BP_PAGE_1               	 0x01u   /**< Calibration message main data page. */
#define BP_PAGE_STANDARD_POWER_ONLY  0x10u   /**< Standard Power only main data page. */
#define BP_PAGE_TORQUE_AT_WHEEL		 0x11u   /**< Power reported as torque at wheel data page, which includes speed. */
#define BP_PAGE_EXTRA_INFO			 0x24u   // TODO: My custom page.  Look for better page no for this.

#define ANT_BP_CHANNEL_TYPE          0x10                                         /**< Channel Type TX. */
#define ANT_BP_DEVICE_TYPE           0x0B                                         /**< Channel ID device type. */
#define ANT_BP_TRANS_TYPE            0xA5 // is how we indicate WAHOO KICKR, normal is: 0x5                                         /**< Transmission Type. */
#define ANT_BP_MSG_PERIOD            0x1FF6                                     	 /**< Message Periods, decimal 8182 (~4.00Hz) data is transmitted every 8182/32768 seconds. */
#define ANT_BP_EXT_ASSIGN            0	                                          /**< ANT Ext Assign. */

#define ANT_BURST_MSG_ID_SET_RESISTANCE		0x48																				 /** Message ID used when setting resistance via an ANT BURST. */

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
static rc_evt_handler_t m_on_set_resistance;
static ant_bp_evt_dfu_enable m_on_enable_dfu_mode;

// TODO: Implement required calibration page.

static __INLINE uint32_t broadcast_message_transmit(const uint8_t * p_buffer)
{
	uint32_t err_code;

	err_code = sd_ant_broadcast_message_tx(ANT_BP_TX_CHANNEL, TX_BUFFER_SIZE, (uint8_t*)p_buffer);

	if (ANT_ERROR_AS_WARN(err_code))
	{
		BP_LOG("[BP]:broadcast_message_transmit WARN:%#.8x\r\n", err_code);
		err_code = NRF_SUCCESS;
	}
	
	return err_code;
}

static __INLINE uint32_t acknolwedge_message_transmit(const uint8_t * p_buffer)
{
	uint32_t err_code;

	err_code = sd_ant_acknowledge_message_tx(ANT_BP_TX_CHANNEL, TX_BUFFER_SIZE, (uint8_t*)p_buffer);

	if (ANT_ERROR_AS_WARN(err_code))
	{
		BP_LOG("[BP]:acknolwedge_message_transmit WARN: %#.8x\r\n", err_code);
		err_code = NRF_SUCCESS;
	}
	
	return err_code;
}

static uint32_t torque_transmit(uint16_t accumulated_torque, uint16_t last_wheel_period_2048, uint8_t wheel_ticks)
{
	// If the wheel is not moving, wheel ticks AND wheel period should not increment.
	// We determine if the wheel moved by seeing if wheel_ticks has changed.
	if (m_torque_tx_buffer[WHEEL_TICKS_INDEX] != wheel_ticks)
	{
		// Event-synchronous model.  Wheel ticks = event count.
		m_torque_tx_buffer[EVENT_COUNT_INDEX] = wheel_ticks;

		m_torque_tx_buffer[WHEEL_TICKS_INDEX] = wheel_ticks;
		m_torque_tx_buffer[WHEEL_PERIOD_LSB_INDEX] = LOW_BYTE(last_wheel_period_2048);
		m_torque_tx_buffer[WHEEL_PERIOD_MSB_INDEX] = HIGH_BYTE(last_wheel_period_2048);
		m_torque_tx_buffer[ACCUMMULATED_TORQUE_LSB_INDEX] = LOW_BYTE(accumulated_torque);
		m_torque_tx_buffer[ACCUMMULATED_TORQUE_MSB_INDEX] = HIGH_BYTE(accumulated_torque);
	}

	return broadcast_message_transmit(m_torque_tx_buffer);
}

static uint32_t power_transmit(uint16_t watts)
{	
	static uint16_t accumulated_power                 = 0;            
	accumulated_power                                += watts;
		
	++(m_power_tx_buffer[EVENT_COUNT_INDEX]);
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

// Transmits extra info embedded in the power measurement.
// TODO: Need a formal message/methodology for this.
static uint32_t extra_info_transmit(irt_power_meas_t * p_power_meas)
{
	uint8_t buffer[TX_BUFFER_SIZE];

	buffer[PAGE_NUMBER_INDEX]			= BP_PAGE_EXTRA_INFO;
	buffer[EXTRA_INFO_FLYWHEEL_REVS]	= (uint8_t)(p_power_meas->accum_flywheel_revs);
	buffer[EXTRA_INFO_SERVO_POS_LSB]	= LOW_BYTE(p_power_meas->servo_position);
	buffer[EXTRA_INFO_SERVO_POS_MSB]	= HIGH_BYTE(p_power_meas->servo_position);
	buffer[EXTRA_INFO_TARGET_LSB]		= LOW_BYTE(p_power_meas->resistance_level);
	buffer[EXTRA_INFO_TARGET_MSB]		= encode_resistance_level(p_power_meas);
	buffer[EXTRA_INFO_TEMP]				= (uint8_t)(p_power_meas->temp);
	buffer[7]							= 0xFF;

	return sd_ant_broadcast_message_tx(ANT_BP_TX_CHANNEL, TX_BUFFER_SIZE, (uint8_t*)&buffer);
}

static void handle_move_servo(ant_evt_t * p_ant_evt)
{
	rc_evt_t evt;

	evt.operation = RESISTANCE_SET_SERVO_POS;
	evt.pBuffer = &(p_ant_evt->evt_buffer[ANT_BP_COMMAND_OFFSET+2]);
	m_on_set_resistance(evt);
}

static void handle_set_weight(ant_evt_t * p_ant_evt)
{
	// TODO: currently this is in the resistance command.
	rc_evt_t evt;

	evt.operation = RESISTANCE_SET_WEIGHT;
	evt.pBuffer = &(p_ant_evt->evt_buffer[ANT_BP_COMMAND_OFFSET+2]);
	m_on_set_resistance(evt);
}

// Right now all this method does is handle resistance control messages.
static void handle_burst(ant_evt_t * p_ant_evt)
{
	static bool 							receiving_burst_resistance = false;
	static resistance_mode_t	resistance_mode = RESISTANCE_SET_STANDARD;

	// TODO: there is probably a more defined way to deal with burst data, but this
	// should work for now.  i.e. use  some derivation of sd_ant_burst_handler_request
	// Although that method looks as though it's for sending bursts, not receiving.
	uint8_t message_sequence_id = p_ant_evt->evt_buffer[2];			// third byte.
	uint8_t message_id = p_ant_evt->evt_buffer[3];			// forth byte.

	if (message_sequence_id == 0x01 && message_id == ANT_BURST_MSG_ID_SET_RESISTANCE)
	{
		// Burst has begun, fifth byte has the mode, need to wait for subsequent messages
		// to parse the level.
		receiving_burst_resistance = true;
		resistance_mode = (resistance_mode_t) p_ant_evt->evt_buffer[4];
	}
	else if (message_sequence_id == 0x21)
	{
		// do nothing, not sure what this message is used for.
	}
	else if (message_sequence_id == 0xC1 && receiving_burst_resistance)
	{
		// Value for the operation exists in this message sequence.  

		rc_evt_t evt;
		evt.operation = resistance_mode;
		evt.pBuffer = &(p_ant_evt->evt_buffer[3]);

		// Reset state.
		receiving_burst_resistance = false;

		BP_LOG("[BP]:handle_burst [%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\r\n",
				p_ant_evt->evt_buffer[0],
				p_ant_evt->evt_buffer[1],
				p_ant_evt->evt_buffer[2],
				p_ant_evt->evt_buffer[3],
				p_ant_evt->evt_buffer[4],
				p_ant_evt->evt_buffer[5],
				p_ant_evt->evt_buffer[6],
				p_ant_evt->evt_buffer[7],
				p_ant_evt->evt_buffer[8]);

		m_on_set_resistance(evt);
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
	else if (p_ant_evt->evt_buffer[ANT_BUFFER_INDEX_MESG_ID] == MESG_ACKNOWLEDGED_DATA_ID &&
		p_ant_evt->evt_buffer[3] == WF_ANT_RESPONSE_PAGE_ID)
	{
		// Determine the "command". 
		switch (p_ant_evt->evt_buffer[ANT_BP_COMMAND_OFFSET])
		{
			case ANT_BP_SET_WEIGHT_COMMAND:	// Set Weight
				handle_set_weight(p_ant_evt);
				break;
			
			case ANT_BP_ENABLE_DFU_COMMAND:	// Invoke device firmware update mode.
				m_on_enable_dfu_mode();
				break;

			case ANT_BP_MOVE_SERVO_COMMAND: // Move the servo to a specific position.
				handle_move_servo(p_ant_evt);
				break;

			default:
				break;
		}
	}
}

void ant_bp_tx_init(rc_evt_handler_t on_set_resistance, ant_bp_evt_dfu_enable on_enable_dfu_mode)
{
    uint32_t err_code;
    
	// Assign callback for when resistance message is processed.	
	m_on_set_resistance = on_set_resistance;
	m_on_enable_dfu_mode = on_enable_dfu_mode;
		
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
	m_power_tx_buffer[PAGE_NUMBER_INDEX]              = BP_PAGE_STANDARD_POWER_ONLY;
	m_power_tx_buffer[EVENT_COUNT_INDEX]              = 0;
	m_power_tx_buffer[PEDAL_POWER_INDEX]              = BP_PAGE_RESERVE_BYTE;
	m_power_tx_buffer[INSTANT_CADENCE_INDEX]          = BP_PAGE_RESERVE_BYTE;
	m_power_tx_buffer[ACCUMMULATED_POWER_LSB_INDEX]   = 0;
	m_power_tx_buffer[ACCUMMULATED_POWER_MSB_INDEX]   = 0;
	m_power_tx_buffer[INSTANT_POWER_LSB_INDEX]        = 0;
	m_power_tx_buffer[INSTANT_POWER_MSB_INDEX]        = 0;

	// Initialize torque transmit buffer.
	m_torque_tx_buffer[PAGE_NUMBER_INDEX]              = BP_PAGE_TORQUE_AT_WHEEL;
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
	const uint8_t power_page_interleave = POWER_PAGE_INTERLEAVE_COUNT;
	const uint8_t product_page_interleave 			= PRODUCT_PAGE_INTERLEAVE_COUNT;
	const uint8_t manufacturer_page_interleave 	= MANUFACTURER_PAGE_INTERLEAVE_COUNT;

	static uint16_t event_count = 0;
	uint32_t err_code = 0;		

	//
	// Using an EVENT-syncronous update methodology.  The last wheel time is 
	// adjusted to when the last wheel rotation occured.  See section 8 of ANT+ 
	// Bicycle Power Device Profile.
	//

	// # Default broadcast message is torque.
	err_code = torque_transmit(p_power_meas->accum_torque,
		p_power_meas->last_wheel_event_time,
		(uint8_t) p_power_meas->accum_wheel_revs);
	APP_ERROR_CHECK(err_code);

	event_count++;		// Always increment event counter.

	// DEBUG info, only send every 2 seconds.
	if (event_count % 8 == 0)
	{
		extra_info_transmit(p_power_meas);
	}

	if (event_count % power_page_interleave == 0)
	{
		// # Only transmit standard power message every 5th power message. 
		err_code = power_transmit(p_power_meas->instant_power);
		APP_ERROR_CHECK(err_code);

		event_count++;		// Always increment event counter.
	}
	else if (event_count % product_page_interleave == 0)
	{			
		// # Figures out which common message to submit at which time.
		ANT_COMMON_PAGE_TRANSMIT(ANT_BP_TX_CHANNEL, ant_product_page);
		event_count++;		// Always increment event counter.
	}
	else if (event_count % manufacturer_page_interleave == 0)
	{
		ANT_COMMON_PAGE_TRANSMIT(ANT_BP_TX_CHANNEL, ant_manufacturer_page);
		event_count++;		// Always increment event counter.
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

	err_code = acknolwedge_message_transmit(tx_buffer);

	return err_code;
}
