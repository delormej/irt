/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
*
*	Encapsulates the interaction with the ANT+ FE-C Profile. 
*
*/

#ifndef ANT_FE_C_H__
#define ANT_FE_C_H__

#include <stdbool.h>
#include <stdint.h>
#include "ble_ant.h"
#include "irt_common.h"
#include "ant_stack_handler_types.h"

#define EQUIPMENT_TRAINER			25

#define CALIBRATION_REQUEST_PAGE	0x01
#define CALIBRATION_PROGRESS_PAGE   0x02
#define GENERAL_FE_DATA_PAGE		16
#define GENERAL_SETTINGS_PAGE		17
#define SPECIFIC_TRAINER_PAGE		25
#define BASIC_RESISTANCE_PAGE		48
#define TARGET_POWER_PAGE			49
#define WIND_RESISTANCE_PAGE		50
#define TRACK_RESISTANCE_PAGE		51
#define FE_CAPABILITIES_PAGE        54
#define USER_CONFIGURATION_PAGE     55
#define COMMAND_STATUS_PAGE			71

#define FE_CAPABILITIES_BIT_FIELD   0x7     // (Basic = 1 | Target Power = 2 | Simulation = 4) == 7  

typedef enum {
    FE_COMMAND_PASS = 0,
    FE_COMMAND_FAIL,
    FE_COMMAND_NOT_SUPPORTED,
    FE_COMMAND_REJECTED,
    FE_COMMAND_PENDING,
    FE_COMMAND_UNINITIALIZED = 0xFF
} fec_command_status_e;

typedef struct {
	uint8_t 	DataPageNumber;
	uint8_t 	CalibrationStatus;
    uint8_t     CalibrationConditions;
    uint8_t     CurrentTemp;
    uint8_t     TargetSpeedLSB;
    uint8_t     TargetSpeedMSB;
    uint8_t     TargetSpinDownTimeLSB;
    uint8_t     TargetSpinDownTimeMSB;        
} FEC_Page2;

typedef struct {
	uint8_t 	DataPageNumber;
	uint8_t 	EquipmentType;
	uint8_t 	ElapsedTime;
	uint8_t 	Distance;
	uint8_t 	SpeedLSB;
	uint8_t 	SpeedMSB;
	uint8_t 	HeartRate;
	/* On the wire FEState bits appear in most significant nibble, before capabilities
	   nibble. bit 0 of EACH nibble (4 bits) is the right most bit (least significant). */
	uint8_t 	Capabilities:4;
	uint8_t 	FEState:4;
} FEC_Page16; // General FE Data Page

typedef struct {
	uint8_t 	DataPageNumber;
	uint8_t 	Reserved[2];
	uint8_t 	CycleLength;				// Wheel Circumference on a Trainer in meters. 0.01 - 2.54m
	uint16_t 	Incline;                    // Not used, send 0x7FFF.
	uint8_t		ResistanceLevelFEC; 		// Percentage of maximum applicable resitsance (0-100%)
	uint8_t		Capabilities:4; 			// Reserved for future, set to: 0x0
	uint8_t		FEState:4;					//  
} FEC_Page17; // General Settings Page

typedef struct {
	uint8_t 	DataPageNumber;
	uint8_t		UpdateEventCount;
	uint8_t		InstantCadence;
	uint8_t		AccumulatedPowerLSB;
	uint8_t		AccumulatedPowerMSB;
	uint8_t		InstantPowerLSB;
	uint8_t		InstantPowerMSB:4;			// Uses 1.5 bytes
	uint8_t		TrainerStatusBit:4;			
	uint8_t		Flags:4;                    // Holds the target power limits flag.
	uint8_t		FEState:4;
} FEC_Page25; // Specific Trainer Data Page

typedef struct {
	uint8_t		DataPageNumber;
	uint8_t		Reserved[6];
	uint8_t		TotalResistance;			// 0-100%.
} FEC_Page48; // Basic Resistance

typedef struct {
	uint8_t		DataPageNumber;
	uint8_t		Reserved[5];
	uint8_t		TargetPowerLSB;
	uint8_t		TargetPowerMSB;				// 0 - 4000W in 0.25W units.				
} FEC_Page49; // Target Power

typedef struct {
	uint8_t		DataPageNumber;
	uint8_t		Reserved[4];
	uint8_t		WindResistanceCoeff;
	uint8_t		WindSpeed;					// -127 - +127 km/h
    uint8_t		DraftingFactor;				// 0 - 1.00				
} FEC_Page50; // Wind Resistance

typedef struct {
	uint8_t		DataPageNumber;
	uint8_t		Reserved[4];
	uint8_t		GradeLSB;
	uint8_t		GradeMSB;
	uint8_t		CoeffRollingResistance;				
} FEC_Page51; // Track Resistance

typedef struct {
	uint8_t		DataPageNumber;
	uint8_t		Reserved[4];
	uint8_t		MaxResistanceLSB;
	uint8_t		MaxResistanceMSB;				// 0 - 65534 newtons.
    uint8_t     FECapabilities;        
} FEC_Page54; // FE Capabilities

typedef struct {
	uint8_t		DataPageNumber;
	uint8_t		UserWeightLSB;
	uint8_t		UserWeightMSB;
	uint8_t		Reserved;
	uint8_t		BikeWeightLSN : 4;				// units: 0.05kg, range: 0 - 50 kg Least significant nibble.
	uint8_t		WheelDiameterOffset : 4;		// units: 1mm, range: 0 - 10 mm.  No offset = 0xF.
    uint8_t     BikeWeightMSB;					// Most significant byte.
	uint8_t		WheelDiameter;					// units: 0.01m, range: 0 - 2.54m
	uint8_t		GearRatio;						// Invalid 0.00, units: 0.03, range 0.03 - 7.65    
} FEC_Page55; // FE User Configuration

typedef struct {
	uint8_t		DataPageNumber;
	uint8_t		LastReceivedCommandID;
	uint8_t		Sequence; 					   // Default to 255 if no control page yet been receieved.
	uint8_t		CommandStatus;				
	uint8_t	    Data[4];						// Response data specific to command ID.
} FEC_Page71; // Command Status Page

// Contains device settings for get/set parameters.
typedef struct {
    uint8_t		DataPageNumber;
    uint8_t     DragLSB;
    uint8_t     DragMSB;
    uint8_t     RRLSB;
    uint8_t     RRMSB;
    uint8_t     ServoOffsetLSB;
    uint8_t     ServoOffsetMSB;
    uint8_t     Settings;           // truncated to 1 byte for now.        
} FEC_IRTSettingsPage;

void ant_fec_tx_init(ant_ble_evt_handlers_t * evt_handlers);

void ant_fec_tx_start(void);

void ant_fec_tx_send(irt_context_t* p_power_meas);

void ant_fec_rx_handle(ant_evt_t* p_ant_evt);

void ant_fec_calibration_send(irt_context_t * p_power_meas,
    calibration_status_t * p_calibration_status);

#endif	// ANT_FE_C_H__
