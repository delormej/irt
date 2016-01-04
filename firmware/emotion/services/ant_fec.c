/* Copyright (c) 2015 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#include <stdbool.h>
#include <stdint.h>
#include "ant_fec.h"
#include "ble_ant.h"
#include "ant_stack_handler_types.h"
#include "ant_parameters.h"
#include "ant_interface.h"
#include "ant_error.h"
#include "app_error.h"
#include "nordic_common.h"
#include "irt_common.h"
#include "debug.h"


#define ANT_FEC_CHANNEL_TYPE          0x10                                          /**< Channel Type TX. */
#define ANT_FEC_DEVICE_TYPE           0x11                                          /**< Channel ID device type. */
#define ANT_FEC_TRANS_TYPE            0x05 	                                        /**< Transmission Type. */
#define ANT_FEC_MSG_PERIOD            0x2000                                     	/**< Message Periods, decimal 8192 (~4.00Hz) data is transmitted every 8192/32768 seconds. */
#define ANT_FEC_EXT_ASSIGN            0	                                            /**< ANT Ext Assign. */

static ant_ble_evt_handlers_t* mp_evt_handlers;
static uint8_t m_event_count = 0;	

/**@brief	Converts speed in mps as float into a dword.
 */
static uint16_t speed_mps_to_int16(float f) {
	// Speed is sent in 0.001 m/s, removing the decimal point to represent int.
	uint16_t i = (uint16_t)(f * 1000.0f);
	return i;
}

/**@brief 	Builds and transmits General FE Dat page from irt_context_t.
 */
static uint32_t GeneralFEDataPage_Send(irt_context_t* context)
{
	static FEC_Page16 page;
	uint32_t err_code = 0;
	uint16_t speed_int = speed_mps_to_int16(context->instant_speed_mps);

	page.DataPageNumber = 16;
	page.EquipmentType = EQUIPMENT_TYPE;
	page.ElapsedTime = context->elapsed_time;
	page.Distance = context->distance_m;
	page.SpeedLSB = LOW_BYTE(speed_int);
	page.SpeedMSB = HIGH_BYTE(speed_int);
	page.HeartRate = HEARTRATE_INVALID;
	page.Capabilities = CAPABILITIES_CONTEXT(context); 
	page.FEState = FESTATE_CONTEXT(context);
	
	err_code = sd_ant_broadcast_message_tx(ANT_FEC_TX_CHANNEL, TX_BUFFER_SIZE, 
		(uint8_t*)&page);

	return err_code;
}

static uint32_t GeneralSettingsPage_Send();
static uint32_t SpecificTrainerDataPage_Send();
static uint32_t SpecificTrainerTorqueDataPage_Send();

/**@brief	Sets initial state for FE-C (Fitness Equipment-Control).
 */
static void state_init(irt_context_t* p_context)
{
	p_context->elapsed_time = 0;
	p_context->fe_state = FE_ASLEEP_OFF;
	p_context->lap_toggle = 0;
	p_context->virtual_speed_flag = 0;
	p_context->target_power_limits = TARGET_UNDETERMINED;
	p_context->bike_power_calibration_required = 0;
	p_context->resistance_calibration_required = 0;
	p_context->user_configuration_required = 0;
}

/**@brief	Initialize the ANT+ FE-C profile and register callbacks.
 */
void ant_fec_tx_init(ant_ble_evt_handlers_t * evt_handlers)
{
    uint32_t err_code;
    
	// Assign callback for when resistance message is processed.	
    mp_evt_handlers = evt_handlers;

    err_code = sd_ant_channel_assign(ANT_FEC_TX_CHANNEL,
                                     ANT_FEC_CHANNEL_TYPE,
                                     ANTPLUS_NETWORK_NUMBER,
                                     ANT_FEC_EXT_ASSIGN);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ant_channel_id_set(ANT_FEC_TX_CHANNEL,
                                     ANT_DEVICE_NUMBER,
                                     ANT_FEC_DEVICE_TYPE,
                                     ANT_FEC_TRANS_TYPE);
    APP_ERROR_CHECK(err_code);
    
    err_code = sd_ant_channel_radio_freq_set(ANT_FEC_TX_CHANNEL, ANTPLUS_RF_FREQ);
    APP_ERROR_CHECK(err_code);
    
    err_code = sd_ant_channel_period_set(ANT_FEC_TX_CHANNEL, ANT_FEC_MSG_PERIOD);
    APP_ERROR_CHECK(err_code);

}

/**@brief	Opens the ANT+ channel.
 */
void ant_fec_tx_start(void)
{
	// Open the channel.
    uint32_t err_code = sd_ant_channel_open(ANT_FEC_TX_CHANNEL);
    APP_ERROR_CHECK(err_code);    	
}

/**@brief	Send appropriate message based on current event count.
 */
void ant_fec_tx_send(irt_context_t * p_power_meas)
{
	// TODO: If this is the first time, call...
	state_init(p_power_meas);

	// Just keep sending page 16 for right now.
	GeneralFEDataPage_Send(p_power_meas);
}

/**@brief	Handle messages sent from ANT+ FE-C display.
 */
void ant_fec_rx_handle(ant_evt_t * p_ant_evt)
{
	// Deal with all messages sent from the FE-C display such as resistance 
	// control and calibration.
}
