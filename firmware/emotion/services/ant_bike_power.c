/* Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#include <stdbool.h>
#include <stdint.h>
#include "stdio.h"
#include <stddef.h>
#include "string.h"
#include "app_util.h"
#include "ant_bike_power.h"
#include "ant_parameters.h"
#include "ant_interface.h"
#include "ant_error.h"
#include "app_error.h"
#include "nordic_common.h"
#include "irt_common.h"
#include "wahoo.h"
#include "debug.h"

#define POWER_PAGE_INTERLEAVE_COUNT			5u
#define EXTRA_INFO_PAGE_INTERLEAVE_COUNT	8u
#define MANUFACTURER_PAGE_INTERLEAVE_COUNT	121u
#define PRODUCT_PAGE_INTERLEAVE_COUNT		242u
#define BATTERY_PAGE_INTERLEAVE_COUNT		61u

#define EVENT_COUNT_INDEX               1u                  /**< Index of the event count field in the power-only main data page. */
#define PEDAL_POWER_INDEX               2u                  /**< Index of the pedal power field in the power-only main data page. */
#define INSTANT_CADENCE_INDEX           3u                  /**< Index of the instantaneous cadence field in the power-only main data page. */
#define ACCUM_POWER_LSB_INDEX    		4u                  /**< Index of the accumulated power LSB field in the power-only main data page. */
#define ACCUM_POWER_MSB_INDEX    		5u                  /**< Index of the accumulated power MSB field in the power-only main data page. */
#define INSTANT_POWER_LSB_INDEX         6u                  /**< Index of the instantaneous power LSB field in the power-only main data page. */
#define INSTANT_POWER_MSB_INDEX         7u                  /**< Index of the instantaneous power MSB field in the power-only main data page. */

// Standard Wheel Torque Main Data Page (0x11)
#define WHEEL_TICKS_INDEX				2u
#define	WHEEL_PERIOD_LSB_INDEX			4u
#define	WHEEL_PERIOD_MSB_INDEX			5u
#define ACCUM_TORQUE_LSB_INDEX			6u
#define ACCUM_TORQUE_MSB_INDEX			7u

#define ANT_BP_CHANNEL_TYPE          0x00                   /**< Channel Type RX. */
#define ANT_BP_DEVICE_TYPE           0x0B                   /**< Channel ID device type. */
#define ANT_BP_TRANS_TYPE            0x00 					/**< Transmission Type. */
#define ANT_BP_MSG_PERIOD            0x1FF6                 /**< Message Periods, decimal 8182 (~4.00Hz) data is transmitted every 8182/32768 seconds. */
#define ANT_BP_EXT_ASSIGN            0	                    /**< ANT Ext Assign. */

// Battery Status page.
#define ANT_BAT_ID_INDEX		 	 2
#define ANT_BAT_TIME_LSB_INDEX	 	 3
#define ANT_BAT_TIME_INDEX	 	 	 4
#define ANT_BAT_TIME_MSB_INDEX	 	 5
#define ANT_BAT_FRAC_VOLT_INDEX	 	 6
#define ANT_BAT_DESC_INDEX	 	 	 7

#define ANT_BURST_MSG_ID_SET_RESISTANCE	0x48									/** Message ID used when setting resistance via an ANT BURST. */
#define ANT_BURST_MSG_ID_SET_POSITIONS	0x59									/** Message ID used when setting servo button stop positions via an ANT BURST. */
#define ANT_BURST_MSG_ID_SET_MAGNET_CA	0x60									/** Message ID used when setting magnet calibration via an ANT BURST. */

/**@brief Debug logging for module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define BP_LOG debug_log
#else
#define BP_LOG(...)
#endif // ENABLE_DEBUG_LOG

static bp_evt_handler_t m_on_bp_power_data;

/*
static uint32_t torque_transmit(uint16_t accum_torque, uint16_t accum_wheel_period_2048, uint8_t wheel_ticks)
{
	tx_buffer[PAGE_NUMBER_INDEX]            = ANT_BP_PAGE_TORQUE_AT_WHEEL;
	tx_buffer[EVENT_COUNT_INDEX] 			= m_event_count;
	tx_buffer[WHEEL_TICKS_INDEX] 			= wheel_ticks;
	tx_buffer[INSTANT_CADENCE_INDEX]        = BP_PAGE_RESERVE_BYTE;
	tx_buffer[WHEEL_PERIOD_LSB_INDEX] 		= LOW_BYTE(accum_wheel_period_2048);
	tx_buffer[WHEEL_PERIOD_MSB_INDEX] 		= HIGH_BYTE(accum_wheel_period_2048);
	tx_buffer[ACCUM_TORQUE_LSB_INDEX] 		= LOW_BYTE(accum_torque);
	tx_buffer[ACCUM_TORQUE_MSB_INDEX] 		= HIGH_BYTE(accum_torque);

	return broadcast_message_transmit();
}

static uint32_t power_transmit(uint16_t watts)
{	
	static uint16_t accumulated_power		= 0;
	accumulated_power                       += watts;
		
	tx_buffer[PAGE_NUMBER_INDEX]			= ANT_BP_PAGE_STANDARD_POWER_ONLY;
	tx_buffer[EVENT_COUNT_INDEX]			= m_event_count;
	tx_buffer[PEDAL_POWER_INDEX]			= BP_PAGE_RESERVE_BYTE;
	tx_buffer[INSTANT_CADENCE_INDEX]		= BP_PAGE_RESERVE_BYTE;
	tx_buffer[ACCUM_POWER_LSB_INDEX]		= LOW_BYTE(accumulated_power);
	tx_buffer[ACCUM_POWER_MSB_INDEX]		= HIGH_BYTE(accumulated_power);
	tx_buffer[INSTANT_POWER_LSB_INDEX]		= LOW_BYTE(watts);
	tx_buffer[INSTANT_POWER_MSB_INDEX]		= HIGH_BYTE(watts);
			
	return broadcast_message_transmit();
} */

uint16_t parseStandardPowerOnly(uint8_t* buffer)
{
	uint16_t watts = buffer[INSTANT_POWER_MSB_INDEX] << 8 | buffer[INSTANT_POWER_LSB_INDEX];
	return  watts;
}

void ant_bp_rx_handle(ant_evt_t * p_ant_evt)
{
	// TODO: remove these hard coded array position values and create defines.
	switch (p_ant_evt->evt_buffer[3])  // Switch on the page number.
	{
		case ANT_BP_PAGE_STANDARD_POWER_ONLY:
			// TOOD: parse standard power page.
			m_on_bp_power_data(parseStandardPowerOnly(p_ant_evt->evt_buffer));
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

void ant_bp_rx_init(bp_evt_handler_t on_bp_power_data, uint16_t device_id)
{
    uint32_t err_code;
    
	// Assign callback for when resistance message is processed.	
    m_on_bp_power_data = on_bp_power_data;
    
    err_code = sd_ant_channel_assign(ANT_BP_TX_CHANNEL,
                                     ANT_BP_CHANNEL_TYPE,
                                     ANTPLUS_NETWORK_NUMBER,
                                     ANT_BP_EXT_ASSIGN);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ant_channel_id_set(ANT_BP_TX_CHANNEL,
                                     device_id,
                                     ANT_BP_DEVICE_TYPE,
                                     ANT_BP_TRANS_TYPE);
    APP_ERROR_CHECK(err_code);
    
    err_code = sd_ant_channel_radio_freq_set(ANT_BP_TX_CHANNEL, ANTPLUS_RF_FREQ);
    APP_ERROR_CHECK(err_code);
    
    err_code = sd_ant_channel_period_set(ANT_BP_TX_CHANNEL, ANT_BP_MSG_PERIOD);
    APP_ERROR_CHECK(err_code);
}

// Opens the connection, looking for a specific power meter id.
void ant_bp_rx_start(void)
{
    uint32_t err_code = sd_ant_channel_open(ANT_BP_TX_CHANNEL);
    APP_ERROR_CHECK(err_code);
}
