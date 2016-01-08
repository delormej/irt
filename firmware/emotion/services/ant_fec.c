/* Copyright (c) 2016 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ant_fec.h"
#include "ble_ant.h"
#include "ant_stack_handler_types.h"
#include "ant_parameters.h"
#include "ant_interface.h"
#include "ant_error.h"
#include "app_error.h"
#include "app_util.h"
#include "nordic_common.h"
#include "irt_common.h"
#include "user_profile.h"
#include "resistance.h"
#include "debug.h"

/**@brief Debug logging for module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define FE_LOG debug_log
#else
#define FE_LOG(...)
#endif // ENABLE_DEBUG_LOG

#define ANT_FEC_CHANNEL_TYPE          0x10                                          /**< Channel Type TX. */
#define ANT_FEC_DEVICE_TYPE           0x11                                          /**< Channel ID device type. */
#define ANT_FEC_TRANS_TYPE            0x05 	                                        /**< Transmission Type. */
#define ANT_FEC_MSG_PERIOD            0x2000                                     	/**< Message Periods, decimal 8192 (~4.00Hz) data is transmitted every 8192/32768 seconds. */
#define ANT_FEC_EXT_ASSIGN            0	                                            /**< ANT Ext Assign. */

#define HR_DATA_SOURCE 				0	
#define DISTANCE_TRAVELED_ENABLED	1	
#define EQUIPMENT_TYPE				25
#define CADENCE_INVALID				0xFF
#define HEARTRATE_INVALID			0xFF
#define INCLINE_INVALID				0x7FFF
#define RESISTANCE_INVALID			0xFF

/**@ brief	Macro to set the value of FE State bit field (nibble) from irt_context_t. 	
				fe_state 			// bits 0-2 
				lap_toggle 			// bit 3 
 */
#define FESTATE_CONTEXT(context) \
		(context->fe_state | (context->lap_toggle << 3))			

/**@ brief	Macro to set the value of FE capabilities bit field (nibble) from irt_context_t.
		HR_DATA_SOURCE  						// bits 0-1
		DISTANCE_TRAVELED_ENABLED 				// bit 2
		virtual_speed_flag   					// bit 3
 */
#define CAPABILITIES_CONTEXT(context) \
		(HR_DATA_SOURCE | \	
		(DISTANCE_TRAVELED_ENABLED << 2) | \	
		(context->virtual_speed_flag << 3))   	

static ant_ble_evt_handlers_t* mp_evt_handlers;
static user_profile_t* mp_user_profile;

/**@brief	Converts speed in mps as float into a dword.
 */
static uint16_t speed_mps_to_int16(float f) {
	// Speed is sent in 0.001 m/s, removing the decimal point to represent int.
	uint16_t i = (uint16_t)(f * 1000.0f);
	return i;
}

/**@brief 	Builds and transmits General FE Data page (Page 16) from irt_context_t.
 */
static uint32_t GeneralFEDataPage_Send(irt_context_t* context)
{
	static FEC_Page16 page = { 
        .DataPageNumber = GENERAL_FE_DATA_PAGE, 
        .EquipmentType = EQUIPMENT_TYPE }; 

	uint16_t speed_int = speed_mps_to_int16(context->instant_speed_mps);
	
	page.ElapsedTime = context->elapsed_time;
	page.Distance = context->distance_m;
	page.SpeedLSB = LOW_BYTE(speed_int);
	page.SpeedMSB = HIGH_BYTE(speed_int);
	page.HeartRate = HEARTRATE_INVALID;
	page.Capabilities = CAPABILITIES_CONTEXT(context); 
	page.FEState = FESTATE_CONTEXT(context);
	
	return sd_ant_broadcast_message_tx(ANT_FEC_TX_CHANNEL, TX_BUFFER_SIZE, 
		(uint8_t*)&page);
}

/**@brief   Builds and transmits General Settings page (Page 17) from irt_context_t.
 */ 
static uint32_t GeneralSettingsPage_Send(irt_context_t* context) 
{
	static FEC_Page17 page = { 
        .DataPageNumber = GENERAL_SETTINGS_PAGE, 
        .Reserved[0] = 0xFF, 
        .Reserved[1] = 0xFF, 
        .Incline = INCLINE_INVALID };
    
	page.CycleLength = (uint8_t)(mp_user_profile->wheel_size_mm / 10);	// Convert wheel centimeters.
	
	// In basic modes, calculate percentage.
	switch (context->resistance_mode) 
	{
		case RESISTANCE_SET_STANDARD:
		case RESISTANCE_SET_PERCENT:
			page.ResistanceLevelFEC = resistance_pct_get(context->servo_position);
            break;
		default:
			page.ResistanceLevelFEC = RESISTANCE_INVALID;
            break;
	} 

    //FE_LOG("[FE]:resistance pct: %i, %i\r\n",context->servo_position, resistance_pct_get(context->servo_position));

	page.Capabilities = CAPABILITIES_CONTEXT(context);
	page.FEState = FESTATE_CONTEXT(context);
	
	return sd_ant_broadcast_message_tx(ANT_FEC_TX_CHANNEL, TX_BUFFER_SIZE, 
		(uint8_t*)&page);    
}

static uint32_t SpecificTrainerDataPage_Send(irt_context_t* context) 
{
	static uint8_t event_count = 0;
	static uint16_t accumulated_power = 0;
	static FEC_Page25 page = { 
        .DataPageNumber = SPECIFIC_TRAINER_PAGE,
        .InstantCadence = CADENCE_INVALID };

	// Increment static fields.
	accumulated_power += context->instant_power;
	
	// Update the page.
	page.UpdateEventCount = event_count++;
	page.AccumulatedPowerLSB = LOW_BYTE(accumulated_power);
	page.AccumulatedPowerMSB = HIGH_BYTE(accumulated_power);
	/* 1.5 bytes used for instantaneous power.  Full 8 bits for byte LSB
	   only 4 bits (0-3) used for the MSB. */
	page.InstantPowerLSB = LOW_BYTE(context->instant_power);
	page.InstantPowerMSB = HIGH_BYTE(context->instant_power) & 0x0F;
	
	/* MSB (bit 3) reserved, 3 bits used for calibration required flags */
	page.TrainerStatusBit = 
		context->bike_power_calibration_required | 			// bit 0
		(context->resistance_calibration_required << 1) |	// bit 1
		(context->user_configuration_required << 2) |		// bit 2
		(0 << 3);											// bit 3 - reserved 
	page.Flags = context->target_power_limits;
	page.FEState = FESTATE_CONTEXT(context);
	
	return sd_ant_broadcast_message_tx(ANT_FEC_TX_CHANNEL, TX_BUFFER_SIZE, 
		(uint8_t*)&page);   
}
static uint32_t SpecificTrainerTorqueDataPage_Send() {return 0;}

static uint32_t FECapabilitiesDataPage_Send()
{
    static FEC_Page54 page = { 
        .DataPageNumber = FE_CAPABILITIES_PAGE,
        .FECapabilities = FE_CAPABILITIES_BIT_FIELD
    };
    
    // TODO: Calculate maximum resistance for the current speed.
    page.MaxResistanceLSB = 0xFF;
    page.MaxResistanceMSB = 0xFF;
    
	return sd_ant_broadcast_message_tx(ANT_FEC_TX_CHANNEL, TX_BUFFER_SIZE, 
		(uint8_t*)&page);       
}

static void HandleResistancePages(uint8_t* buffer)
{
	rc_evt_t resistance_evt; 
    memset(&resistance_evt, 0, sizeof(rc_evt_t));

    // Operation is the page number, which is the first byte.
    resistance_evt.operation = buffer[0]; 
    
    switch (resistance_evt.operation) 
    {
        case BASIC_RESISTANCE_PAGE:
            // Decode 0.5% increments, range 0 - 100%.
            resistance_evt.total_resistance = (float)(buffer[7] / 200.0f);
            FE_LOG("[FE] total_resistance: %.2f\r\n", 
                resistance_evt.total_resistance);
            break;
            
        case TARGET_POWER_PAGE:
            // Decode 0.25w increments, range 0 - 4000 watts.
            resistance_evt.target_power = uint16_decode(&buffer[6]) / 4;
            FE_LOG("[FE] target_power: %i\r\n",resistance_evt.target_power);            
            break;
        
        case WIND_RESISTANCE_PAGE:
        case TRACK_RESISTANCE_PAGE:
            break;
        default:
            break;   
    };
                
    mp_evt_handlers->on_set_resistance(resistance_evt);
}

/**@brief	Initialize the ANT+ FE-C profile and register callbacks.
 */
void ant_fec_tx_init(ant_ble_evt_handlers_t * evt_handlers)
{
    uint32_t err_code;

	// Assign callback for when resistance message is processed.	
    mp_evt_handlers = evt_handlers;

    // Get a pointer to the user profile object.
    mp_user_profile = user_profile_get();


    // Configure ANT channel.
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
    static uint8_t count = 0; 
    
    // xor , then flip ~, and with a bunch of 00s
    /* Binary patterns that match last 2 bits (*):
        0   0000 *
        1   0001 **
        2   0010 ***
        3   0011 -
        4   0100 *
        5   0101 **
        6   0110 ***
        7   0111 --
        
        Message Pattern:
        16 26 25 17  16 26 25 255 .{64 messages}. 80 80 .{64 messages}. 81 81
    */
    if (count == 128 || count == 129)
    {
        // Send this page twice consecutively.
        ant_common_page_transmit(ANT_FEC_TX_CHANNEL, ant_product_page);
    }
    else if (count == 64 || count == 65)
    {
        // Send this page twice consecutively.
        ant_common_page_transmit(ANT_FEC_TX_CHANNEL, ant_manufacturer_page);
    }
    else if (  (~(0b01 ^ count) & 3) == 3  )
    {
        // Message sequence 1 & 5: Page 26.
        SpecificTrainerTorqueDataPage_Send();
    }
    else if (  (~(0b10 ^ count) & 3) == 3  )
    {
        // Message sequence 2 & 6: Page 25.
        SpecificTrainerDataPage_Send(p_power_meas);
    }
    else if (  (~(0b111 ^ count) & 7) == 7  )
    {
        // Message sequence 7: Manufacturer specific page.
        // Send IRT specific Extra_info_page.
    }       
    else if (  (~(0b11 ^ count) & 3) == 3  )
    {
        // Message sequence 3: Page 17.
        GeneralSettingsPage_Send(p_power_meas);
    }     
    else 
    {
        // Default message, page 16.
        GeneralFEDataPage_Send(p_power_meas);    
    }
    
    // Reset after two complete, or increment.
    if (count == 129)
        count = 0;
    else 
        count++;
}

/**@brief	Handle messages sent from ANT+ FE-C display.
 */
void ant_fec_rx_handle(ant_evt_t * p_ant_evt)
{
	// Deal with all messages sent from the FE-C display such as resistance 
	// control and calibration.
	// Only interested in BURST events right now for processing resistance control.
    if (p_ant_evt->evt_buffer[ANT_BUFFER_INDEX_MESG_ID] == MESG_ACKNOWLEDGED_DATA_ID)
	{
		// TODO: remove these hard coded array position values and create defines.
		switch (p_ant_evt->evt_buffer[3])  // Switch on the page number.
		{
            case BASIC_RESISTANCE_PAGE:
            case TARGET_POWER_PAGE:
            case WIND_RESISTANCE_PAGE:
            case TRACK_RESISTANCE_PAGE:           
                HandleResistancePages(&p_ant_evt->evt_buffer[3]);
                break;
            
			case ANT_PAGE_REQUEST_DATA:
				FE_LOG("[FE]:requesting data page: [%.2x]\r\n", p_ant_evt->evt_buffer[9]);
                mp_evt_handlers->on_request_data(&(p_ant_evt->evt_buffer[3]));				
				break;            
            
			default:
				FE_LOG("[FE]:unrecognized message [%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\r\n",
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
