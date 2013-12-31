/* Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#include <stdint.h>
#include "ant_bike_power.h"
#include "app_error.h"
#include "ant_interface_ds.h"
#include "irt_emotion.h"

#define HIGH_BYTE(word)              		(uint8_t)((word >> 8u) & 0x00FFu)           /**< Get high byte of a uint16_t. */
#define LOW_BYTE(word)               		(uint8_t)(word & 0x00FFu)                   /**< Get low byte of a uint16_t. */

#define POWER_PAGE_INTERLEAVE_COUNT			5u
#define MANUFACTURER_PAGE_INTERLEAVE_COUNT	121u
#define PRODUCT_PAGE_INTERLEAVE_COUNT		121u*2u
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

#define ANT_BP_TX_CHANNEL     	 		 1                                            /**< Bicycle Speed TX ANT Channel. */
#define ANT_BP_CHANNEL_TYPE          0x10                                         /**< Channel Type TX. */
#define ANT_BP_DEVICE_TYPE           0x0B                                         /**< Channel ID device type. */
#define ANT_DEVICE_NUMBER         	 0x01                                         /**< TODO: need a unique Device Number derived from device serial no. */
#define ANT_BP_TRANS_TYPE            0x5                                         /**< Transmission Type. */
#define ANT_BP_MSG_PERIOD            0x1FF6                                     	 /**< Message Periods, decimal 8182 (~4.00Hz) data is transmitted every 8182/32768 seconds. */
#define ANT_BP_EXT_ASSIGN            0	                                          /**< ANT Ext Assign. */

static uint8_t 		m_power_tx_buffer[TX_BUFFER_SIZE];
static uint8_t 		m_torque_tx_buffer[TX_BUFFER_SIZE];

// TODO: Implement required calibration page.

static __INLINE uint32_t broadcast_message_transmit(const uint8_t * p_buffer)
{
		return sd_ant_broadcast_message_tx(ANT_BP_TX_CHANNEL, TX_BUFFER_SIZE, (uint8_t*)p_buffer);
}

static uint32_t torque_transmit(uint16_t accumulated_torque, uint16_t last_wheel_period_2048, uint8_t wheel_ticks)
{
		static uint16_t last_accumulated_torque           = 0;            
		uint16_t torque																		= accumulated_torque - last_accumulated_torque;
		last_accumulated_torque														= accumulated_torque;

		// NOTE: Torque message uses it's OWN event count, not to be confused with other message
		// event counts.
    ++(m_torque_tx_buffer[EVENT_COUNT_INDEX]);
    m_torque_tx_buffer[WHEEL_TICKS_INDEX]   					= wheel_ticks;
    m_torque_tx_buffer[WHEEL_PERIOD_MSB_INDEX]   			= HIGH_BYTE(accumulated_torque);   
    m_torque_tx_buffer[ACCUMMULATED_TORQUE_LSB_INDEX] = LOW_BYTE(torque);            
    m_torque_tx_buffer[ACCUMMULATED_TORQUE_MSB_INDEX] = HIGH_BYTE(torque);                

		return broadcast_message_transmit(m_torque_tx_buffer);
}

static uint32_t power_transmit(uint16_t watts)
{	
    static uint16_t accumulated_power                 = 0;            
    
    ++(m_power_tx_buffer[EVENT_COUNT_INDEX]);
    accumulated_power                                += watts;
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

void ant_bp_tx_init(void)
{
    uint32_t err_code;
    
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
    m_torque_tx_buffer[PAGE_NUMBER_INDEX]              = BP_PAGE_STANDARD_POWER_ONLY;
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

void ant_bp_tx_send(ble_cps_meas_t * p_cps_meas)
{
		uint32_t err_code;
	
		uint16_t total_event_count;
		
		total_event_count = m_torque_tx_buffer[EVENT_COUNT_INDEX] +
										m_power_tx_buffer[EVENT_COUNT_INDEX];
	
		// # Always send torque message.
		err_code = torque_transmit(p_cps_meas->accum_torque, 
										p_cps_meas->last_wheel_event_time, 
										p_cps_meas->accum_wheel_revs);
	
		
	
		APP_ERROR_CHECK(err_code);
	
		if (total_event_count % MANUFACTURER_PAGE_INTERLEAVE_COUNT == 0)
		{
			// # Only transmit standard power message every 5th power message. 
			err_code = power_transmit(p_cps_meas->instant_power);
		}
		else if (total_event_count % PRODUCT_PAGE_INTERLEAVE_COUNT == 0)
		{			
			// # Figures out which common message to submit at which time.
			err_code = product_page_transmit();
		}
		else if (total_event_count % MANUFACTURER_PAGE_INTERLEAVE_COUNT == 0)
		{
			err_code = manufacturer_page_transmit();
		}
		
		APP_ERROR_CHECK(err_code);
}
