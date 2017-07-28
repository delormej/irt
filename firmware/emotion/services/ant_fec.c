    /* Copyright (c) 2016 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
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
#include "app_fifo.h"
#include "wahoo.h" // need a couple of defines from here.
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
#define GEAR_RATIO_INVALID          0x00

#define DEFAULT_BIKE_WEIGHT_KG		1000ul 										// Default weight 10kg as per FE-C spec.

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

//
// Circular buffer to queue for page requests.
//
#define FIFO_BUFFER_SIZE 4
static app_fifo_t request_queue;
static uint8_t m_fifo_buffer[FIFO_BUFFER_SIZE];

static ant_ble_evt_handlers_t* mp_evt_handlers;
static user_profile_t* mp_user_profile;

static void queue_request(uint8_t);

// Hold on to these pages to respond with when requested.
static FEC_Page50 m_page50;
static FEC_Page51 m_page51;
static FEC_Page55 m_page55 = {
    .DataPageNumber = USER_CONFIGURATION_PAGE    
};

// Manages state for the last command received.
static FEC_Page71 m_last_command = {           
    .DataPageNumber = COMMAND_STATUS_PAGE,
    .LastReceivedCommandID = 0xFF,
    .Sequence = 0xFF,
    .CommandStatus = FE_COMMAND_UNINITIALIZED,
    .Data = { 0xFF, 0xFF, 0xFF, 0xFF } 
    };

/**@brief	Converts speed in mps as float into a dword.
 */
static uint16_t speed_mps_to_int16(float f) {
	// Speed is sent in 0.001 m/s, removing the decimal point to represent int.
	uint16_t i = (uint16_t)(f * 1000.0f);
	return i;
}

/**@brief	Converts wheel cicrumference in mm i.e. (2107) or 2.107m into
 *          a byte for wheel diameter 0.01m and 1/2 a byte diameter offset (0-10mm).
 *          The return value is the offset (0-10mm), the diameter_cm parameter
 *          is referenced and updated with the calculated value.
 */
static uint8_t calc_wheel_diameter(uint16_t circ_mm, uint8_t* diameter_cm) {
    uint8_t diameter_offset_mm = 0;
    // Calculate diameter from circumference.
    float diameter = ((float)circ_mm / 31.4f); 
    // Truncate to 0.01m without decimnal, but essential it's the cm.
    *diameter_cm = (uint8_t)diameter; 
    
    // Calculate remainder, 0-10 mm.
    diameter_offset_mm = (uint8_t)((diameter - (*diameter_cm)) * 10.0f);
    
    return diameter_offset_mm;
}

/**@brief	Converts two part wheel diameter into cicrumference in mm.
 */
static uint16_t calc_wheel_circ(uint8_t diameter_cm, uint8_t diameter_offset_mm) {  
    //NOTE: from the Garmin Edge 520, this does not appear to be sent correctly.
    // Diameter comes in cm, convert to millimeters.
    float circ = 0.0f;
    int16_t diameter = diameter_cm * 10;
     
    if (diameter_offset_mm != 0xF)
    {
        // Add offset in millimeters.
        diameter += diameter_offset_mm;
    }
    
    // Calculate circumference from PI.
    circ = (float)diameter * 3.14f; 

    // TODO: use math rounding function.
    return (uint16_t)round(circ);
}

/**@brief   Helper method that parses wind resistance simulation.
 *
 */
static void wind_resistance_get(FEC_Page50* p_page, resistance_wind_t* p_wind)
{
    // Transported as 0.01 kg/m, range: 0.00 - 0.168.
    if (m_page50.WindResistanceCoeff > 0 && m_page50.WindResistanceCoeff <= 186) 
    {
        p_wind->wind_coefficient = m_page50.WindResistanceCoeff / 100.0f;
    }
    else
    {
        // Out of range, use default value.
        FE_LOG("[FE] wind_coefficient out of range: %i\r\n", 
            m_page50.WindResistanceCoeff);
        p_wind->wind_coefficient = 0.51f;
    }
    
    // NOTE: Message is in km/h, but we process in meters per second.  
    // Conversion is currently in main.c, but that should change.   
    // Cast to a SIGNED int8_t -127 - +127. 
    p_wind->wind_speed_km = (int8_t) m_page50.WindSpeed;
    
    // Range 0.00 - 1.00.
    if (m_page50.DraftingFactor <= 100)
    {
        p_wind->drafting_factor = m_page50.DraftingFactor / 100.0f;
    }
    else
    {
        // Out of range, use default value.
        FE_LOG("[FE] drafting factor out of range: %i\r\n", 
            m_page50.DraftingFactor);
        
        // Default is 1.0 (no drafting factor).
        p_wind->drafting_factor = 1.0f;
    } 
}

/**@brief   Helper method that parses simulation track grade.
 *          The grade of the simulated track is set as a percentage of vertical displacement to horizontal displacement. Fitness equipment shall assume the default grade of 0% (flat track) if the open display sets the grade field to invalid (0xFF). Fitness equipment that is capable of adjusting the *           incline directly (e.g. treadmills) should apply the grade simulation parameter in this way. Other fitness equipment that is not capable of adjusting the incline (e.g. stationary bikes or trainers) shall use the grade field to calculate gravitational resistance to apply to the user.
 *
 *          The grade (slope) field is interpreted as a decimal value with units of 0.01% and an offset of -200.00%.
 */
static float track_grade_get(uint8_t* buffer)
{
    // Simulated Grade (%) = (Raw Grade Value x 0.01%) – 200.00%
    uint16_t raw_grade = uint16_decode(buffer);
    float grade = (raw_grade * 0.01f) - 200.0f;    
    
    // Internally we store as 0.06 for example, so move the decimal. 
    return grade / 100.0f;
}

/**@brief   Helper method to parse the coefficient of rolling resistance. 
 *
 */
static float track_crr_get(uint8_t buffer)
{
    float crr = 0.004f; // DEFAULT_CRR
    
    if (buffer != 0xFF) // Indicates invalid.
    {
        crr = (float)(buffer * 0.00005f);
    }

    return crr;
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

static uint32_t ManufacturerSpecificPage_Send(irt_context_t* context)
{
	uint8_t tx_buffer[8];
    uint16_t flywheel;
	flywheel = context->accum_flywheel_ticks;

	tx_buffer[PAGE_NUMBER_INDEX]			= ANT_IRT_PAGE_EXTRA_INFO;
	tx_buffer[EXTRA_INFO_SERVO_POS_LSB]		= LOW_BYTE(context->servo_position);
	tx_buffer[EXTRA_INFO_SERVO_POS_MSB]		= HIGH_BYTE(context->servo_position);
	tx_buffer[EXTRA_INFO_TARGET_LSB]		= LOW_BYTE(context->resistance_level);
	tx_buffer[EXTRA_INFO_TARGET_MSB]		= encode_resistance_level(context);
	tx_buffer[EXTRA_INFO_FLYWHEEL_REVS_LSB]	= LOW_BYTE(flywheel);
	tx_buffer[EXTRA_INFO_FLYWHEEL_REVS_MSB]	= HIGH_BYTE(flywheel);
	tx_buffer[EXTRA_INFO_TEMP]				= (uint8_t)(context->temp);

	return sd_ant_broadcast_message_tx(ANT_FEC_TX_CHANNEL, TX_BUFFER_SIZE, tx_buffer);
}

/**@brief   Parses user configuration and applies to the user profile.
 * 
 */
static uint32_t UserConfigurationPage_Send()
{
    // DEFAULT_TOTAL_WEIGHT_KG
    
    // Calculate weight bike weight as total weight - user weight.
    // Since we do not store bike weight directly.
    // Currently, total weight will never be invalid because the user profile init
    // routine will default the value.  
    // TODO: reconsider who is responsible for what here as the code is getting too complicated.
    uint16_t user_weight = mp_user_profile->user_weight_kg;
    uint16_t bike_weight = 0;
    
    FE_LOG("[FE] User Weight:%i\r\n", user_weight);
    
    // If user weight is invalid, set bike to default.
    if (user_weight == 0xFFFF)
    {
        bike_weight = DEFAULT_BIKE_WEIGHT_KG;
        user_weight = (mp_user_profile->total_weight_kg - bike_weight); 
    }
    else
    {
        // Otherwise calculate from total.
        bike_weight = (mp_user_profile->total_weight_kg - user_weight);
    }

    // Unit for bike weight is 0.05kg, 0x0FFF == 50kg, 
    // Calculate wire value by dividing by 5.
    bike_weight = (uint16_t)(bike_weight / 5u);
    
    // Bike weight == 1.5 bytes.
    m_page55.BikeWeightMSB = (uint8_t)((bike_weight >> 4) & 0x0FF);
    m_page55.BikeWeightLSN = (uint8_t)(bike_weight & 0x000F);
    
    // User weight.
    m_page55.UserWeightLSB = LOW_BYTE(user_weight);
    m_page55.UserWeightMSB = HIGH_BYTE(user_weight);

    // Wheel size is transmitted here in diameter, but we store and use in circumference.
    // This is stored as another 1.5 byte field.
    // Can't pass offset by ref because it's a bitfield, so this is very sloppy
    // The method sets the value of wheel diameter and 4 bits of the return value 
    // is used as the offset. 
    m_page55.WheelDiameterOffset = 0xF & calc_wheel_diameter(
            mp_user_profile->wheel_size_mm, &m_page55.WheelDiameter);
            
    m_page55.GearRatio = 0x00; // invalid.
        
	return sd_ant_broadcast_message_tx(ANT_FEC_TX_CHANNEL, TX_BUFFER_SIZE, 
		(uint8_t*)&m_page55);    
} 

static void CalibrationInProgress_Send(calibration_status_t * p_calibration_status)
{
    //
    static  FEC_Page2 page =  {
        .DataPageNumber = CALIBRATION_PROGRESS_PAGE
    };
    
	sd_ant_broadcast_message_tx(ANT_FEC_TX_CHANNEL, TX_BUFFER_SIZE, 
		(uint8_t*)&page);       
}

/**@brief	Sends manufacturer specific page containing settings.
 */
static uint32_t IRTSettingsPage_Send() {
    uint16_t drag = (uint16_t) (mp_user_profile->ca_drag * 1000000.0f);
    uint16_t rr = (uint16_t) (mp_user_profile->ca_rr * 1000.0f);

    FEC_IRTSettingsPage page = {
        .DataPageNumber = ANT_IRT_PAGE_SETTINGS,
        .DragLSB = LOW_BYTE(drag),
        .DragMSB = HIGH_BYTE(drag),
        .RRLSB = LOW_BYTE(rr),
        .RRMSB = HIGH_BYTE(rr),
        .ServoOffsetLSB = LOW_BYTE(mp_user_profile->servo_offset),
        .ServoOffsetMSB = HIGH_BYTE(mp_user_profile->servo_offset),
        .Settings = (uint8_t)mp_user_profile->settings // truncated to 1 byte.
    };
    
    return sd_ant_broadcast_message_tx(ANT_FEC_TX_CHANNEL, TX_BUFFER_SIZE, 
		(uint8_t*)&page);
}

/**@brief	Sends manufacturer specific page containing power adjustment settings.
 */
static uint32_t IRTSettingsPowerAdjustSend() {

    FEC_IRTSettingsPowerAdjustPage page = {
        .DataPageNumber = ANT_IRT_PAGE_POWER_ADJUST,
        .PowerMeterId = mp_user_profile->power_meter_ant_id,
        .PowerAdjustSeconds = mp_user_profile->power_adjust_seconds,
        .PowerAverageSeconds = mp_user_profile->power_average_seconds
    };
    
    return sd_ant_broadcast_message_tx(ANT_FEC_TX_CHANNEL, TX_BUFFER_SIZE, 
		(uint8_t*)&page);
}

/**@brief	Processes pages responsible for setting resistance modes and 
 *          raises the event to change resistance.
 */
static void HandleResistancePages(uint8_t* buffer)
{
	rc_evt_t resistance_evt; 
    memset(&resistance_evt, 0, sizeof(rc_evt_t));

    FE_LOG("[FE]:resistance message [%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\r\n",
            buffer[0],
            buffer[1],
            buffer[2],
            buffer[3],
            buffer[4],
            buffer[5],
            buffer[6],
            buffer[7],
            buffer[8]);

    // TODO: refactor and remove resistance_evt to just share page 71 (Last command).
    // Operation is the page number, which is the first byte.
    memset(m_last_command.Data, 0xFF, sizeof(m_last_command.Data));
    resistance_evt.operation = buffer[0]; 

    switch (resistance_evt.operation) 
    {
        case BASIC_RESISTANCE_PAGE:
            // Decode 0.5% increments, range 0 - 100%.
            resistance_evt.total_resistance = (float)(buffer[7] / 200.0f);
            // Store total resistance.
            m_last_command.Data[3] = buffer[7];
            FE_LOG("[FE] total_resistance: %.2f\r\n", 
                resistance_evt.total_resistance);
            break;
            
        case TARGET_POWER_PAGE:
            // Decode 0.25w increments, range 0 - 4000 watts.
            resistance_evt.target_power = uint16_decode(&buffer[6]) / 4;
            // Store target power.
            m_last_command.Data[2] = buffer[6];
            m_last_command.Data[3] = buffer[7];
            
            FE_LOG("[FE] target_power: %i\r\n",resistance_evt.target_power);            
            break;
        
        case WIND_RESISTANCE_PAGE:
            // Make a copy of the page.
            memcpy(&m_page50, buffer, sizeof(FEC_Page50));
            m_last_command.Data[1] = m_page50.WindResistanceCoeff;
            m_last_command.Data[2] = m_page50.WindSpeed;
            m_last_command.Data[3] = m_page50.DraftingFactor;    
            
            wind_resistance_get(&m_page50, &resistance_evt.wind);
            
            FE_LOG("[FE] wind_resistance \r\n");
            break;
        
        case TRACK_RESISTANCE_PAGE:
            // Make a copy of the page.
            memcpy(&m_page51, buffer, sizeof(FEC_Page51));
            m_last_command.Data[1] = m_page51.GradeLSB;
            m_last_command.Data[2] = m_page51.GradeMSB;
            m_last_command.Data[3] = m_page51.CoeffRollingResistance;    
            
            // Parse the grade.
            resistance_evt.track.grade = track_grade_get((uint8_t*)&m_page51.GradeLSB); 
            
            // Parse crr.
            resistance_evt.track.crr = track_crr_get(m_page51.CoeffRollingResistance);
            
            FE_LOG("[FE] track_resistance, grade:%.4f, crr:%.5f \r\n",
                resistance_evt.track.grade, resistance_evt.track.crr);
            break;
            
        default:
            break;   
    };

    m_last_command.LastReceivedCommandID = resistance_evt.operation;
    m_last_command.Sequence++;
    m_last_command.CommandStatus = FE_COMMAND_PENDING;
                
    mp_evt_handlers->on_set_resistance(resistance_evt);
    
    // TODO: There is an opportunity here to handle errors more gracefully.
    // If we didn't fail on previous call, assume a pass.
    m_last_command.CommandStatus = FE_COMMAND_PASS;
    
    // Queue a response to go out with last command.
    // This is not in the ANT+ FE-C spec, client is supposed to ask for it.
    // queue_request(COMMAND_STATUS_PAGE);
}

/**@brief   Parses the request calibration page.
 *
 */
static void HandleCalibrationRequestPage(uint8_t* buffer)
{
    // TODO: Indicate which TYPE of calibration requested from buffer[1] 
    // Raise event that calibratoin has been requested.
    mp_evt_handlers->on_request_calibration();
}

/**@brief   Parses user configuration and applies to the user profile.
 * 
 */
static void HandleUserConfigurationPage(uint8_t* buffer)
{
    bool dirty = false; 
    FEC_Page55 page55;
    uint16_t bike_weight;
    uint16_t user_weight;
    uint16_t total_weight;
    uint16_t wheel_size;
    
    FE_LOG("[FE] user configuration received: [%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\r\n",
        buffer[0],
        buffer[1],
        buffer[2],
        buffer[3],
        buffer[4],
        buffer[5],
        buffer[6],
        buffer[7]);
        
    // Copy into page 55 structure.
    memcpy(&page55, buffer, sizeof(FEC_Page55));
    
    // Parse user weight.
    user_weight = uint16_decode((const uint8_t*)&page55.UserWeightLSB);
    FE_LOG("[FE] User Weight Set to:%i\r\n", user_weight);

    if (user_weight != 0xFFFF /*invalid*/) 
    {
        // Wire value is 1.5 bytes.  Unit for bike weight is 0.05kg, 0x0FFF == 50kg, 
        // Calculate value from wire, multiply by * 5.
        // Store as 1/100kg, i.e. 6.7kg is stored as 670. 
        bike_weight = 5 * (page55.BikeWeightLSN | 
            (uint16_t)(page55.BikeWeightMSB << 4));
        FE_LOG("[FE] Bike Weight Set to:%i\r\n", bike_weight);
        
        total_weight = user_weight + bike_weight;
        
        if (mp_user_profile->total_weight_kg != total_weight) {
            mp_user_profile->total_weight_kg = total_weight;
            mp_user_profile->user_weight_kg = user_weight;  
            dirty = true;
        }
    }

    if (page55.WheelDiameter != 0xFF /*invalid*/) 
    {
        wheel_size = calc_wheel_circ(page55.WheelDiameter, page55.WheelDiameterOffset);    
        FE_LOG("[FE] Bike Wheel Size to:%i\r\n", wheel_size);
        
        // Determine if this represents a change to the user profile.
        if (mp_user_profile->wheel_size_mm != wheel_size) {
                mp_user_profile->wheel_size_mm = wheel_size;
            dirty = true;
        }
    }
    
    // If there were changes to the profile.
    if (dirty) 
    {
        // Signal to the user profile module to update persistent storage.
        user_profile_store();
    }
}


/**@brief   Parses IRT specific settings and applies to the user profile.
 * 
 */
static void HandleIRTSettingsPage(uint8_t* buffer) {
    FEC_IRTSettingsPage page;
    bool dirty = false;
    bool persist = true;  // Flag whether to persist to flash or not.
    float ca_drag = 0.0f;
    float ca_rr = 0.0f;
    int16_t servo_offset = 0;
    
    FE_LOG("[FE] IRT settings received: [%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\r\n",
        buffer[0],
        buffer[1],
        buffer[2],
        buffer[3],
        buffer[4],
        buffer[5],
        buffer[6],
        buffer[7]);
        
    // Copy into page structure.
    memcpy(&page, buffer, sizeof(FEC_IRTSettingsPage));
    
    if (page.DragMSB != 0xFF && page.DragLSB != 0xFF /*invalid*/)
    {
        ca_drag = (float)(page.DragMSB << 8 | page.DragLSB) / 1000000.0f;
        
        if (ca_drag != mp_user_profile->ca_drag) 
        {
            mp_user_profile->ca_drag = ca_drag;
            dirty = true;             
        }        
    } 

    if (page.RRMSB != 0xFF && page.RRLSB != 0xFF /*invalid*/)
    {
        ca_rr = (float)(page.RRMSB << 8 | page.RRLSB) / 1000.0f;

        if (ca_rr != mp_user_profile->ca_rr)
        {
            mp_user_profile->ca_rr = ca_rr;
            dirty = true;
        }
    }
    
    // Read flag persist changes to flash, stored in the MSB of byte 6 (ServoOffsetMSB).
    persist = (page.ServoOffsetMSB & 0x80);

    // Most significant bit is a persistance flag.
    servo_offset = ((page.ServoOffsetMSB & 0x7F) << 8) | page.ServoOffsetLSB;
    
    // Invalid servo offset can be 0x7FFF *or* 0xFFFF, so mask out MSbit.
    if ( (servo_offset & 0x7FFF) != 0x7FFF /*not invalid*/ && 
        mp_user_profile->servo_offset != servo_offset) 
    {
        mp_user_profile->servo_offset = servo_offset;
        dirty = true;
    }
    
    if ((mp_user_profile->settings & 0xFF) != page.Settings) 
    {
        // Only the 1st byte of settings comes over.
        mp_user_profile->settings |= page.Settings;
        dirty = true;
    }
    
    if (dirty && persist)
    {
        // Signal to the user profile module to update persistent storage.
        user_profile_store();
    }    
    else 
    {
        FE_LOG("[FE]:Flagged not to persist IRT Settings changes.\r\n");
    }
}

/**@brief   Parses IRT specific settings for power adjust and applies to the user profile.
 * 
 */
static void HandleIRTPowerAdjustPage(uint8_t* buffer) {
    FEC_IRTSettingsPowerAdjustPage page;
    bool dirty = false;

    // Copy into page structure.
    memcpy(&page, buffer, sizeof(FEC_IRTSettingsPowerAdjustPage));

    //
    // TODO: add validation and track dirty flag
    //
    if (  mp_user_profile->power_meter_ant_id != page.PowerMeterId && 
        mp_user_profile->power_meter_ant_id != 0xFFFF ) 
    {
        mp_user_profile->power_meter_ant_id = page.PowerMeterId;
        dirty = true;
    }

    if ( mp_user_profile->power_adjust_seconds != page.PowerAverageSeconds && 
        mp_user_profile->power_adjust_seconds != 0xFF ) 
    {
        mp_user_profile->power_adjust_seconds = page.PowerAverageSeconds;
        dirty = true;
    }

    if ( mp_user_profile->power_average_seconds != page.PowerAverageSeconds && 
        mp_user_profile->power_average_seconds != 0xFF )
    {
        mp_user_profile->power_average_seconds = page.PowerAverageSeconds;
        dirty = true;
    }
    
    if (dirty)
    {
        user_profile_store();
    }
}

/**@brief   Raises the set resistance event.
 */
static void HandleSetServo(ant_evt_t * p_ant_evt) {
    RC_EVT_SET_SERVO(p_ant_evt);
	mp_evt_handlers->on_set_resistance(evt);
}

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
        FE_LOG("[FE] queue_request : QUEUE FULL.\r\n"); 
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
            ant_common_page_transmit(ANT_FEC_TX_CHANNEL, ant_manufacturer_page);
            break;
            
        case ANT_COMMON_PAGE_81:
            ant_common_page_transmit(ANT_FEC_TX_CHANNEL, ant_product_page);
            break;
            
        case FE_CAPABILITIES_PAGE:
            FECapabilitiesDataPage_Send();
            break;
          
        case USER_CONFIGURATION_PAGE:
            UserConfigurationPage_Send();
            break;
            
        case WIND_RESISTANCE_PAGE:
            sd_ant_broadcast_message_tx(ANT_FEC_TX_CHANNEL, TX_BUFFER_SIZE,
                (uint8_t*)&m_page50);
            break;
        
        case TRACK_RESISTANCE_PAGE:
            sd_ant_broadcast_message_tx(ANT_FEC_TX_CHANNEL, TX_BUFFER_SIZE,
                (uint8_t*)&m_page51);
            break;

        case COMMAND_STATUS_PAGE:
            sd_ant_broadcast_message_tx(ANT_FEC_TX_CHANNEL, TX_BUFFER_SIZE, 
		      (uint8_t*)&m_last_command);       
            break;
            
        case ANT_IRT_PAGE_SETTINGS:
            IRTSettingsPage_Send();
            break;    

        case ANT_IRT_PAGE_POWER_ADJUST:
            IRTSettingsPowerAdjustSend();
            break;

        default:
            FE_LOG("[FE] dequeue_request, unrecognized page:%i.\r\n", 
                page_number); 
            return false;
    }
    
    return true;
}

/**@brief	Initialize the ANT+ FE-C profile and register callbacks.
 */
void ant_fec_tx_init(ant_ble_evt_handlers_t * evt_handlers)
{
    uint32_t err_code;

	// Assign callback for when resistance message is processed.	
    mp_evt_handlers = evt_handlers;

    // Initialize the queue.
    err_code = app_fifo_init(&request_queue, m_fifo_buffer, 
        (uint16_t)sizeof(m_fifo_buffer));
    APP_ERROR_CHECK(err_code);

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

/**@brief	Send appropriate message during calibration.
 */
void ant_fec_calibration_send(irt_context_t * p_power_meas,
    calibration_status_t * p_calibration_status)
{
    static uint8_t count = 0;
    // If ending calibration, send page 0x01 3 times.
    
    // If in calibration mode, alternate page 0x02 and 0x10, until complete.
    if (count % 2 == 0)
    {
        // send page 0x02
        CalibrationInProgress_Send(p_calibration_status);
    }
    else
    {
        // send page 0x10.
        GeneralFEDataPage_Send(p_power_meas);   
    }
}

/**@brief	Send appropriate message based on current event count.
 */
void ant_fec_tx_send(irt_context_t * p_power_meas)
{
    static uint8_t count = 0; 
    
    // TOOD: conslodate where we send the broadcast message, single call
    // the rest of this code should just get a handle to the page to send.
    // This way we can centralize error handling, etc...
    
    // Check to see if we have any pending data page requests.
    // If so that message is sent, so return for this cycle.
    if (dequeue_request())
        return;
    
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
        // Message sequence 7: Manufacturer specific page, IRT Extra Info.
        ManufacturerSpecificPage_Send(p_power_meas);
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
            case CALIBRATION_REQUEST_PAGE:
                //
                break;
                
            case BASIC_RESISTANCE_PAGE:
            case TARGET_POWER_PAGE:
            case WIND_RESISTANCE_PAGE:
            case TRACK_RESISTANCE_PAGE:           
                HandleResistancePages(&p_ant_evt->evt_buffer[3]);
                break;

            case USER_CONFIGURATION_PAGE:
                HandleUserConfigurationPage(&p_ant_evt->evt_buffer[3]);
                break;
            
			case ANT_PAGE_REQUEST_DATA:
				FE_LOG("[FE]:requesting data page: [%.2x]\r\n", p_ant_evt->evt_buffer[9]);
                
                // Add this to the request queue.
                queue_request(p_ant_evt->evt_buffer[9]);
				break;           
                
            case ANT_IRT_PAGE_SETTINGS:
                HandleIRTSettingsPage(&p_ant_evt->evt_buffer[3]);
                break; 

			case WF_ANT_RESPONSE_PAGE_ID:
                // Determine the "command".
                switch (p_ant_evt->evt_buffer[ANT_BP_COMMAND_OFFSET])
                {
                    case ANT_BP_ENABLE_DFU_COMMAND:	// Invoke device firmware update mode.
                        mp_evt_handlers->on_enable_dfu_mode();
                        break;

                    case ANT_BP_MOVE_SERVO_COMMAND: // Move the servo to a specific position.
                         HandleSetServo(p_ant_evt);
                        break;

                    default:
                        break;
                }            
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
