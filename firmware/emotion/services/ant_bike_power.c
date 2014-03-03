/* Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#include <stdint.h>
#include "stdio.h"
#include "ant_bike_power.h"
#include "app_error.h"
#include "ant_parameters.h"
#include "ant_interface.h"
#include "irt_emotion.h"

#define HIGH_BYTE(word)              		(uint8_t)((word >> 8u) & 0x00FFu)           /**< Get high byte of a uint16_t. */
#define LOW_BYTE(word)               		(uint8_t)(word & 0x00FFu)                   /**< Get low byte of a uint16_t. */

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

// Standard Wheel Torque Main Data Page (0x11)
#define WHEEL_TICKS_INDEX								2u
#define	WHEEL_PERIOD_LSB_INDEX					4u
#define	WHEEL_PERIOD_MSB_INDEX					5u
#define ACCUMMULATED_TORQUE_LSB_INDEX		6u
#define ACCUMMULATED_TORQUE_MSB_INDEX		7u

#define TX_BUFFER_SIZE               		8u               

#define BP_PAGE_1               		 0x01u   /**< Calibration message main data page. */
#define BP_PAGE_STANDARD_POWER_ONLY  0x10u   /**< Standard Power only main data page. */
#define BP_PAGE_TORQUE_AT_WHEEL		   0x11u   /**< Power reported as torque at wheel data page, which includes speed. */
#define COMMON_PAGE_80          		 0x50u   /**< Manufacturer's identification common data page. */
#define COMMON_PAGE_81          		 0x51u   /**< Product information common data page. */
//#define COMMON_PAGE_BATTERY_VOLTAGE 0x52u   /**< TODO: Optional battery voltage reporting. */

#define BP_PAGE_RESERVE_BYTE    		 0xFFu   /**< Page reserved value. */

#define ANT_BP_CHANNEL_TYPE          0x10                                         /**< Channel Type TX. */
#define ANT_BP_DEVICE_TYPE           0x0B                                         /**< Channel ID device type. */
#define ANT_BP_TRANS_TYPE            0xA5 // is how we indicate WAHOO KICKR, normal is: 0x5                                         /**< Transmission Type. */
#define ANT_BP_MSG_PERIOD            0x1FF6                                     	 /**< Message Periods, decimal 8182 (~4.00Hz) data is transmitted every 8182/32768 seconds. */
#define ANT_BP_EXT_ASSIGN            0	                                          /**< ANT Ext Assign. */

#define ANT_BURST_MSG_ID_SET_RESISTANCE		0x48																				 /** Message ID used when setting resistance via an ANT BURST. */
#define ANT_TRANSMIT_IN_PROGRESS 		 0x401F

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

static uint8_t 					m_power_tx_buffer[TX_BUFFER_SIZE];
static uint8_t 					m_torque_tx_buffer[TX_BUFFER_SIZE];
static rc_evt_handler_t m_on_set_resistance;

// TODO: Implement required calibration page.

static __INLINE uint32_t broadcast_message_transmit(const uint8_t * p_buffer)
{
		uint32_t err_code;
	
		err_code = sd_ant_broadcast_message_tx(ANT_BP_TX_CHANNEL, TX_BUFFER_SIZE, (uint8_t*)p_buffer);
		
		// TODO: this feels like a hack, but it works properly and the message does get sent.
		// This is likely because we're sending so many ACKS to so many set-resistance commands
		// from Trainer Road.  If we fix that, this condition shouldn't happen as often.
		if (err_code == ANT_TRANSMIT_IN_PROGRESS)
		{
			uint8_t data[] = "Warning: Pending Transmit";
			debug_send(data, sizeof(data));
			
			err_code = NRF_SUCCESS;
		}
		
		return err_code;
}

static __INLINE uint32_t acknolwedge_message_transmit(const uint8_t * p_buffer)
{
		uint32_t err_code;
	
		err_code = sd_ant_acknowledge_message_tx(ANT_BP_TX_CHANNEL, TX_BUFFER_SIZE, (uint8_t*)p_buffer);
		
		// TODO: this feels like a hack, but it works properly and the message does get sent.
		// This is likely because we're sending so many ACKS to so many set-resistance commands
		// from Trainer Road.  If we fix that, this condition shouldn't happen as often.
		if (err_code == ANT_TRANSMIT_IN_PROGRESS)
		{
			uint8_t data[] = "Warning: Pending Transmit";
			debug_send(data, sizeof(data));
			
			err_code = NRF_SUCCESS;
		}
		
		return err_code;
}

static uint32_t torque_transmit(uint16_t accumulated_torque, uint16_t last_wheel_period_2048, uint8_t wheel_ticks)
{
		// Only update the event count if a new event occurred.
		// NOTE: Torque message uses it's OWN event count, not to be confused with other message
		// event counts.
		if (m_torque_tx_buffer[WHEEL_TICKS_INDEX] != wheel_ticks)
		{
			++(m_torque_tx_buffer[EVENT_COUNT_INDEX]);
		}
    m_torque_tx_buffer[WHEEL_TICKS_INDEX]   					= wheel_ticks;
    m_torque_tx_buffer[WHEEL_PERIOD_LSB_INDEX]   			= LOW_BYTE(last_wheel_period_2048);   
		m_torque_tx_buffer[WHEEL_PERIOD_MSB_INDEX]   			= HIGH_BYTE(last_wheel_period_2048);   
		m_torque_tx_buffer[ACCUMMULATED_TORQUE_LSB_INDEX] = LOW_BYTE(accumulated_torque);            
    m_torque_tx_buffer[ACCUMMULATED_TORQUE_MSB_INDEX] = HIGH_BYTE(accumulated_torque);                

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

static __INLINE uint32_t product_page_transmit(void)
{
    static const uint8_t tx_buffer[TX_BUFFER_SIZE] = 
    {
        COMMON_PAGE_81, 
        BP_PAGE_RESERVE_BYTE, 
        BP_PAGE_RESERVE_BYTE, 
        SW_REVISION, 
        (uint8_t)(SERIAL_NUMBER), 
        (uint8_t)(SERIAL_NUMBER >> 8u), 
        (uint8_t)(SERIAL_NUMBER >> 16u), 
        (uint8_t)(SERIAL_NUMBER >> 24u)
    };       
        
    return broadcast_message_transmit(tx_buffer); 	
}

static __INLINE uint32_t manufacturer_page_transmit(void)
{    
    static const uint8_t tx_buffer[TX_BUFFER_SIZE] = 
    {
        COMMON_PAGE_80, 
        BP_PAGE_RESERVE_BYTE, 
        BP_PAGE_RESERVE_BYTE, 
        HW_REVISION, 
        LOW_BYTE(MANUFACTURER_ID), 
        HIGH_BYTE(MANUFACTURER_ID), 
        LOW_BYTE(MODEL_NUMBER), 
        HIGH_BYTE(MODEL_NUMBER)
    };       
    
    return broadcast_message_transmit(tx_buffer); 
}

//
// Send a message to indicate a manual set resistance override
//
static __INLINE uint32_t resistance_transmit(resistance_mode_t mode, uint16_t level)
{
		static uint8_t resistance_sequence = 0;
	
		// TODO: should we use this struct instead? ANTMsgWahoo240_t
    uint8_t tx_buffer[TX_BUFFER_SIZE] = 
    {
        WF_ANT_RESPONSE_PAGE_ID, 
        mode, 
        WF_ANT_RESPONSE_FLAG, 
        ++resistance_sequence, 								
        0x20, 	// Not sure why, but this is hardcoded to 0x20 for now.
        0x00,
        0x01, 	// Again, not sure why, but KICKR responds with this.
        0x00
    };       
    
    return acknolwedge_message_transmit(tx_buffer); 
}


// Right now all this method does is handle resistance control messages.
// TODO: need to implement calibration requests as well.
void ant_bp_rx_handle(ant_evt_t * p_ant_evt)
{
	static bool 							receiving_burst_resistance 	= false;
	static resistance_mode_t	resistance_mode = RESISTANCE_SET_STANDARD;
	
	// Only interested in BURST events right now for processing resistance control.
	if (p_ant_evt->evt_buffer[ANT_BUFFER_INDEX_MESG_ID] != MESG_BURST_DATA_ID)
	{
			return;
	}
	
	// TODO: there is probably a more defined way to deal with burst data, but this
	// should work for now.  i.e. use  some derivation of sd_ant_burst_handler_request
	// Although that method looks as though it's for sending bursts, not receiving.
	uint8_t message_sequence_id = p_ant_evt->evt_buffer[2];			// third byte.
	uint8_t message_id 					= p_ant_evt->evt_buffer[3];			// forth byte.
	
	if (message_sequence_id == 0x01 && message_id == ANT_BURST_MSG_ID_SET_RESISTANCE)
	{
		// Burst has begun, fifth byte has the mode, need to wait for subsequent messages
		// to parse the level.
		receiving_burst_resistance 		= true;
		resistance_mode								= (resistance_mode_t)p_ant_evt->evt_buffer[4];
	}
	else if (message_sequence_id == 0x21)
	{
		// do nothing, not sure what this message is used for.
	}
	else if (message_sequence_id == 0xC1 && receiving_burst_resistance)
	{
		// Value for the operation exists in this message sequence.  

		rc_evt_t evt;
		evt.operation 	= resistance_mode;
		evt.pBuffer			= &(p_ant_evt->evt_buffer[3]);

		// Reset state.
		receiving_burst_resistance = false;		
		
		m_on_set_resistance(evt);
	}
}

void ant_bp_tx_init(rc_evt_handler_t on_set_resistance)
{
    uint32_t err_code;
    
		// Assign callback for when resistance message is processed.	
		m_on_set_resistance = on_set_resistance;
		
    err_code = sd_ant_network_address_set(ANTPLUS_NETWORK_NUMBER, m_ant_network_key);
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
		m_torque_tx_buffer[WHEEL_PERIOD_MSB_INDEX]   			 = 0;        
    m_torque_tx_buffer[ACCUMMULATED_TORQUE_LSB_INDEX]  = 0;            
    m_torque_tx_buffer[ACCUMMULATED_TORQUE_MSB_INDEX]  = 0;
}

void ant_bp_tx_start(void)
{
    uint32_t err_code = sd_ant_channel_open(ANT_BP_TX_CHANNEL);
    APP_ERROR_CHECK(err_code);
}

void ant_bp_tx_send(irt_power_meas_t * p_cps_meas)
{
		static uint16_t event_count										= 0;
		static uint8_t power_page_interleave 					= POWER_PAGE_INTERLEAVE_COUNT;
		static uint8_t product_page_interleave 				= PRODUCT_PAGE_INTERLEAVE_COUNT;
		static uint8_t manufacturer_page_interleave 	= MANUFACTURER_PAGE_INTERLEAVE_COUNT;
		uint32_t err_code;		

		// Increment global event_count.
		event_count++;

		if (event_count % power_page_interleave == 0)
		{
			// # Only transmit standard power message every 5th power message. 
			err_code = power_transmit(p_cps_meas->instant_power);
		}
		else if (event_count % product_page_interleave == 0)
		{			
			// # Figures out which common message to submit at which time.
			err_code = product_page_transmit();
		}
		else if (event_count % manufacturer_page_interleave == 0)
		{
			err_code = manufacturer_page_transmit();
		}
		else if (event_count % 2 == 0) // TODO: Placeholder... use scheduler instead.
		{
			err_code = resistance_transmit(p_cps_meas->resistance_mode, 
																			p_cps_meas->resistance_level);
		}		
		else
		{
			// # Default broadcast message is torque.
			err_code = torque_transmit(p_cps_meas->accum_torque, 
											p_cps_meas->last_wheel_event_time, 
											p_cps_meas->accum_wheel_revs);
		}
		
		APP_ERROR_CHECK(err_code);
}

