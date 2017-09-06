/* Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#include <stdbool.h>
#include <stdint.h>
#include "stdio.h"
#include <stddef.h>
#include <math.h>
#include "string.h"
#include "app_util.h"
#include "ant_bike_power.h"
#include "ant_parameters.h"
#include "ant_interface.h"
#include "ant_error.h"
#include "app_error.h"
#include "nordic_common.h"
#include "irt_common.h"
#include "speed_event_fifo.h"
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

/**@brief Callback to invoke when power data arrives.
 *
 */
static bp_evt_handler_t m_on_bp_power_data;

/**@brief Standard Bike Power Only page structure.
 *
 */
typedef struct 
{
	uint8_t			page_number;
	uint8_t			event_count;
	uint8_t			pedal_power;
	uint8_t			instant_cadence;
	uint8_t			accum_power_lsb;
	uint8_t			accum_power_msb;
	uint8_t			instant_power_lsb;
	uint8_t			instant_power_msb;	
} ant_bp_standard_power_only_t;

/**@brief	Crank Torque Frequence Message Format.
 *
 */
typedef struct
{
	uint8_t			page_number;
	uint8_t			event_count;
	uint8_t			slope_msb;
	uint8_t			slope_lsb;
	uint8_t			timestamp_msb;
	uint8_t			timestamp_lsb;
	uint8_t			torque_ticks_msb;
	uint8_t			torque_ticks_lsb;
} ant_bp_ctf_t;

typedef struct
{
	uint8_t 		event_count;
	uint16_t 		accum_power;
} power_event_t;

/**@brief Maintain a buffer of events.
 *
 */
static power_event_t m_power_only_buffer[SPEED_EVENT_CACHE_SIZE];
static event_fifo_t m_power_only_fifo;
static uint16_t m_ant_power_meter_id = 0; // tracks when a power meter is connected.
static uint8_t m_last_power_page[8] = { 0xFF }; // buffer for holding last power message received.
static uint16_t m_ctf_offset = 0; // Crank Torque Frequency Offset.

/**@brief	Calculates average power in watts between two events.
 *
 */
static float CalcAveragePower(power_event_t first, power_event_t last)
{
	// Deltas between first and last events.
	uint16_t event_count = 0;
	uint16_t accum_power = 0;
	float average_power = 0.0f;

	// Deal with rollover.
	if (first.event_count > last.event_count)
	{
		// Add 1 because there is an event at count = 0.
		event_count = 1 + (0xFF ^ first.event_count) + last.event_count;
	}
	else
	{
		event_count = last.event_count - first.event_count;
	}

	//
	// Only calculate power if events have occurred.
	//
	if (event_count == 0)
	{
		return 0.0f;
	}

	//
	// Calculate delta in accum power between events.
	//
	if (first.accum_power > last.accum_power)
	{
		// Handle power rollover.
		accum_power = (0xFFFF ^ first.accum_power) + last.accum_power;
	}
	else
	{
		accum_power = last.accum_power - first.accum_power;
	}

	// Calculate average power.
	// distance_m = flywheel_ticks / FLYWHEEL_TICK_PER_METER;
	average_power = (float)accum_power / (float)event_count;

	return average_power;
}

#define CTF_SUCCESS				0
#define CTF_NO_EVENTS			1
#define CTF_CADENCE_TIMEOUT		2

/**@brief	Calculates power(watts) from Crank Tourque Frequency page.
 *			Returns 0 if succesful, 1 if no new events, 2 if cadence time out.
 *
 */
static uint32_t CalcCTFWatts(ant_bp_ctf_t* p_page, ant_bp_ctf_t* p_last_page, float* p_watts)
{
	static uint8_t cadence_time_out = 0;

	// Parse current message (N)
	uint16_t slope = p_page->slope_msb << 8 | p_page->slope_lsb;
	uint16_t current_timestamp = p_page->timestamp_msb << 8 | p_page->timestamp_lsb;
	uint16_t current_torque_ticks = p_page->torque_ticks_msb << 8 | p_page->torque_ticks_lsb;

	// Parse prior message (N-1)
	uint16_t last_timestamp = p_last_page->timestamp_msb << 8 | p_last_page->timestamp_lsb;
	uint16_t last_torque_ticks = p_last_page->torque_ticks_msb << 8 | p_last_page->torque_ticks_lsb;

	uint16_t elapsed_time = DELTA_ROLLOVER_16(last_timestamp, current_timestamp);
	uint16_t events = DELTA_ROLLOVER_16(p_last_page->event_count, p_page->event_count);

	if (events < 1) 
	{
		// If no new events, keep track until we have a cadence time out.

		if (++cadence_time_out >=12)
		{
			// Cadence timed out.
			p_watts = 0;
			return CTF_CADENCE_TIMEOUT;
		}
		else 
		{
			return CTF_NO_EVENTS;
		}
	}
	else
	{
		cadence_time_out = 0;
	}

	float cadence_period = (elapsed_time / events) * 0.0005f; // Seconds
	uint16_t cadence = (uint16_t) ROUNDED_DIV(60, cadence_period); // RPMs
	uint16_t torque_ticks = DELTA_ROLLOVER_16(last_torque_ticks, current_torque_ticks);

	// The average torque per revolution of the pedal is calculated using the calibrated Offset parameter.
	float torque_frequency = (1.0f / ( (elapsed_time* 0.0005f) /torque_ticks)) - m_ctf_offset; // hz

	// Torque in Nm is calculated from torque rate (Torque Frequency) using the calibrated sensitivity Slope
	float torque = torque_frequency / (slope/10);

	// Finally, power is calculated from the cadence and torque.
	*p_watts = torque * cadence * (MATH_PI/30); // watts

	/*
	BP_LOG("[BP] CalcCTFWatts, %i, %i \r\n", last_timestamp, current_timestamp);
	BP_LOG("[BP] CalcCTFWatts, events: %i, elapsed_time:%i, torque_ticks:%i, slope:%i\r\n",
		events, elapsed_time, torque_ticks, slope);
	BP_LOG("[BP] CalcCTFWatts, torque:%.2f, torque_frequency:%.2f cadence: %i, power:%.2f \r\n",
		torque, torque_frequency, cadence, *p_watts);
	*/

	return CTF_SUCCESS;
}

/**@brief Parses ant data page to combine bits for power in watts.
 *
 */
static uint16_t GetWatts(uint8_t msb, uint8_t lsb)
{
	//									    LSB MSB
	// Example Tx: [10][44][FF][5A][2C][4B][1B][01] // 283 watts
	uint16_t watts = msb << 8 | lsb;
	return  watts;
}

/**@brief Ask ANT for the channelID.
 *
 */
static uint32_t GetPowerMeterId() 
{
	uint8_t channelType, transmissionType;
	uint32_t err_code = sd_ant_channel_id_get(ANT_BP_RX_CHANNEL, 
		&m_ant_power_meter_id, &channelType, &transmissionType);

	APP_ERROR_CHECK(err_code);
	
	BP_LOG("[BP] Got Power Meter ANT ID: %i!\r\n", m_ant_power_meter_id);

	if (m_ant_power_meter_id > 0) {
		// Channel ID message, which means we've connected.
		// Make a callback here which sets irt_context_t.power_meter_paired = true;
		m_on_bp_power_data( BP_MSG_DEVICE_CONNECTED, m_ant_power_meter_id );
		BP_LOG("[BP] DEVICE CONNECTED!\r\n");
	}

	return err_code;
}

static void HandleStandardPowerOnlyPage(uint8_t* p_payload)
{
	power_event_t event;

	// Parse page.
	ant_bp_standard_power_only_t* p_page = (ant_bp_standard_power_only_t*)p_payload;
	event.event_count = p_page->event_count;
	event.accum_power = GetWatts(p_page->accum_power_msb, p_page->accum_power_lsb);

	// Add to the queue for later averaging.
	speed_event_fifo_put(&m_power_only_fifo, (uint8_t*)&event);

	// Make a copy of the last power page.
	memcpy((uint8_t*)m_last_power_page, (uint8_t*)p_page, sizeof(m_last_power_page));

	// Raise event with instantnew power in watts.
	m_on_bp_power_data(BP_MSG_POWER_DATA,
		GetWatts(p_page->instant_power_msb, p_page->instant_power_lsb));
}

static void HandleCTFPage(uint8_t* p_payload)
{
	// m_on_bp_power_data(BP_MSG_POWER_DATA,
	ant_bp_ctf_t* p_page = (ant_bp_ctf_t*)p_payload;

	float power = 0.0f;
	uint16_t watts = 0;
	uint32_t err = CalcCTFWatts(p_page, (ant_bp_ctf_t*)m_last_power_page, &power);
	
	switch (err)
	{
		case CTF_CADENCE_TIMEOUT:
			// Register no power as there is no cadence.	
			watts = 0;
			break;
	
		case CTF_NO_EVENTS:
			// Exit out, there is nothing to do.
			return;

		case CTF_SUCCESS:
			// Round float power into integer watts.
			watts = roundl(power);
			break;

		default:
			break;
	}

	//
	// Averaging needs to be accounted for, accumulate our own power and event count.
	// Create new event from the old.
	//
	power_event_t* p_previous_event = (power_event_t*)speed_event_fifo_get(
		&m_power_only_fifo);

	power_event_t new_event;
	new_event.event_count = p_previous_event->event_count + 1;
	new_event.accum_power = p_previous_event->accum_power + watts;

	//
	// TODO: These methods are duplicated with HandleStandardPowerOnlyPage 
	// (do not repeat yourself) -- let's fix this.
	//
	speed_event_fifo_put(&m_power_only_fifo, (uint8_t*)&new_event);
	
	// Make a copy of the last power page.
	memcpy((uint8_t*)m_last_power_page, (uint8_t*)p_page, sizeof(m_last_power_page));

	m_on_bp_power_data(BP_MSG_POWER_DATA, watts);
}

static void HandleCTFOffset(uint8_t* p_payload)
{
	// Byte 0 == page number (0x01)
	// Byte 1 == calibration ID (0x10) CTF defined message
	// Byte 2 == CTF Defined ID (0x01) Zero Offset
	// Byte 3-5 0xFF (reserved)
	// Byte 6 == Offset MSB
	// Byte 7 == Offset LSB

	BP_LOG("[BP] CTF OFFSET? 0x10: %i, 0x01: %i, Offset: %i\r\n", 
		p_payload[1], p_payload[2], p_payload[6] << 8 | p_payload[7]);

	m_ctf_offset = 0; // Hopefully set this here.
}

/**@brief Returns the connected power meter ID or 0 if not connected.
 *
 */
uint16_t ant_bp_power_meter_id_get()
{
	return m_ant_power_meter_id;
}

/**@brief Invoked when a event occurs on the ant_bp channel.
 *
 */
void ant_bp_rx_handle(ant_evt_t * p_ant_evt)
{
	ANT_MESSAGE* p_mesg = (ANT_MESSAGE*)p_ant_evt->evt_buffer;

	switch (p_ant_evt->event)
	{
		case EVENT_RX_SEARCH_TIMEOUT:
		case EVENT_RX_FAIL_GO_TO_SEARCH:
			// Timed out looking for a power meter.
			m_on_bp_power_data(BP_MSG_DEVICE_SEARCH_TIME_OUT, 0); 
			BP_LOG("[BP] SEARCH TIMEOUT!\r\n");
			break;

		case EVENT_CHANNEL_CLOSED:
			// Channel was closed.
			// Make a callback here which sets irt_context_t.power_meter_paired = false;
			m_ant_power_meter_id = 0;
			m_on_bp_power_data(BP_MSG_DEVICE_CLOSED, 0); 
			BP_LOG("[BP] CHANNEL CLOSED!\r\n");
			break;

		case EVENT_RX:
			// If this is the first message, grab the power meter Id.
			if (m_ant_power_meter_id == 0) 
			{
				GetPowerMeterId();
			}

			// Switch on page number.
			switch (p_mesg->ANT_MESSAGE_aucPayload[0])
			{
				case ANT_BP_PAGE_CALIBRATION:
					// Try to parse CTF offset.
					HandleCTFOffset(p_mesg->ANT_MESSAGE_aucPayload);
					break;

				case ANT_BP_PAGE_STANDARD_POWER_ONLY:
					HandleStandardPowerOnlyPage(p_mesg->ANT_MESSAGE_aucPayload);
					break;

				case ANT_BP_PAGE_CTF_MAIN:
					// Calculate power based on Crank Torque Frequency.
					HandleCTFPage(p_mesg->ANT_MESSAGE_aucPayload);
					break;

				default:
					/*
					BP_LOG("[BP]:unrecognized message [%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\r\n",
							p_mesg->ANT_MESSAGE_aucPayload[0],
							p_mesg->ANT_MESSAGE_aucPayload[1],
							p_mesg->ANT_MESSAGE_aucPayload[2],
							p_mesg->ANT_MESSAGE_aucPayload[3],
							p_mesg->ANT_MESSAGE_aucPayload[4],
							p_mesg->ANT_MESSAGE_aucPayload[5],
							p_mesg->ANT_MESSAGE_aucPayload[6],
							p_mesg->ANT_MESSAGE_aucPayload[7],
							p_mesg->ANT_MESSAGE_aucPayload); */
					break;
			}
			break;

		default:
			break;
	}
}

/**@brief	Close & unassign if the channel is open or in use.
 *
 */
uint32_t ant_bp_rx_close()
{
	uint8_t status;
	uint32_t err;
	
	// Reset power meter id state.
	m_ant_power_meter_id = 0;

	err = sd_ant_channel_status_get(ANT_BP_RX_CHANNEL, &status);
	// throw a real exception here if this fails because it shouldn't.
	APP_ERROR_CHECK(err);

	switch (status)
	{
		case STATUS_SEARCHING_CHANNEL:
		case STATUS_TRACKING_CHANNEL:
			BP_LOG("[BP] Closing previously opened bp channel.\r\n");
			err = sd_ant_channel_close(ANT_BP_RX_CHANNEL);
			if (err != NRF_SUCCESS) 
			{
				// Disregard any error, but log it here.  By recalling this method
				// it should change state. The case to protect against is infinite
				// recursion.  ANT should eventually change state out of search/track
				// at which point it would no longer fall into this case.

				BP_LOG("[BP] Error attempt to close previously opened bp channel.%i\r\n",
					err);
			}
			// Recursively call this function to further unassign or try again.
			return ant_bp_rx_close();

			// TODO: Guard against indefinite recursion, i.e. max 4 calls? 
			// TODO: Decrement a recursion counter here? 
			break;
			
		case STATUS_ASSIGNED_CHANNEL:
			BP_LOG("[BP] Unassigning previously used bp channel.\r\n");
			err = sd_ant_channel_unassign(ANT_BP_RX_CHANNEL);
			if (err != NRF_SUCCESS) 
			{
				BP_LOG("[BP] Error unassigning previously used bp channel: %i.\r\n",
					err);
				return err;
			}
			break;

		case STATUS_UNASSIGNED_CHANNEL:
			// Do nothing.
			break;

		default:
			// do nothing if the channel status is not one of the above states.
			BP_LOG("[BP] Unexpected channel state: %i.\r\n", status);	
			break;
	}

	// If we got this far, it was a success.
	return NRF_SUCCESS;
}

/**@brief Initialize the module with callback and power meter device id to search for.
 *
 */
uint32_t ant_bp_rx_init(bp_evt_handler_t on_bp_power_data, uint16_t device_id)
{
    uint32_t err_code;
    
	// Assign callback for when resistance message is processed.	
    m_on_bp_power_data = on_bp_power_data;
    
	// Because this function can be called multiple times with new power meter connections,
	// unassign any previous use.
	ant_bp_rx_close();

    err_code = sd_ant_channel_assign(ANT_BP_RX_CHANNEL,
                                     ANT_BP_CHANNEL_TYPE,
                                     ANTPLUS_NETWORK_NUMBER,
                                     ANT_BP_EXT_ASSIGN);
    
	if (err_code != NRF_SUCCESS)
	{
		// If in the wrong state, just exit.
		// NRF_ANT_ERROR_CHANNEL_IN_WRONG_STATE
		return err_code;
	}

    err_code = sd_ant_channel_id_set(ANT_BP_RX_CHANNEL,
                                     device_id,
                                     ANT_BP_DEVICE_TYPE,
                                     ANT_BP_TRANS_TYPE);
	
	if (err_code != NRF_SUCCESS)
	{
		// If in the wrong state, just exit.
		// NRF_ANT_ERROR_CHANNEL_IN_WRONG_STATE
		return err_code;
	}
    
	//
	// Errors in these two methods indicate invalid params, so throw exception as 
	// we shouldn't see these conditions.
	//

    err_code = sd_ant_channel_radio_freq_set(ANT_BP_RX_CHANNEL, ANTPLUS_RF_FREQ);
    APP_ERROR_CHECK(err_code);
    
    err_code = sd_ant_channel_period_set(ANT_BP_RX_CHANNEL, ANT_BP_MSG_PERIOD);
    APP_ERROR_CHECK(err_code);

	BP_LOG("[BP] Attempting to open Bike Power channel with ANT device id: %i\r\n",
		device_id);

	return NRF_SUCCESS;
}

/**@brief Opens the channel and begins to search.
 *
 */
void ant_bp_rx_start(void)
{
    uint32_t err_code = sd_ant_channel_open(ANT_BP_RX_CHANNEL);
    APP_ERROR_CHECK(err_code);

	// Initialize fifo for collecting power events to average.
	m_power_only_fifo = speed_event_fifo_init((uint8_t*)m_power_only_buffer, 
		sizeof(power_event_t));

}

/**@brief Calculates average power for the period in seconds.
 *
 */
float ant_bp_avg_power(uint8_t seconds)
{
	float average_power = 0.0f;
	uint8_t events = seconds * 2; // 2 events per second, oldest event is seconds * 2.

	power_event_t* p_oldest = 
		(power_event_t*)speed_event_fifo_oldest(&m_power_only_fifo, events);
	power_event_t* p_current = 
		(power_event_t*)speed_event_fifo_get(&m_power_only_fifo);
	
	average_power = CalcAveragePower(*p_oldest, *p_current);

	BP_LOG("[BP] %i:%i, %i:%i == Average Power: %.2f (events:%i)\r\n", 
		p_oldest->event_count, p_current->event_count,
		p_oldest->accum_power, p_current->accum_power, 
		average_power, events);

	return average_power;	
}
