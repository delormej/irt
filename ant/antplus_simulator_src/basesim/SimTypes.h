/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/
 


#ifndef SIM_TYPES
#define SIM_TYPES

#include "config.h"

// Simulator specific classes
#ifdef SIM_AJE
	#define SIM_TX
	#include "HRMSensor.h"
	#include "SDMSensor.h"
	#include "BikePowerSensor.h"
	#include "BikeSpdCadSensor.h"
	#include "BikeCadenceSensor.h"
	#include "BikeSpeedSensor.h"
	#include "WeightScaleSensor.h"
	#include "MSMSensor.h"
	#include "LEVSensor.h"
	#include "GeocacheSensor.h"
	#include "TemperatureSensor.h"
   #include "AudioDevice.h"
	#include "CustomSensor.h"
#endif
#ifdef SIM_AJR
	#define SIM_RX
	#include "HRMDisplay.h"
	#include "SDMDisplay.h"
	#include "BikePowerDisplay.h"
	#include "BikeSpdCadDisplay.h"
	#include "BikeCadenceDisplay.h"
	#include "BikeSpeedDisplay.h"
	#include "WeightScaleDisplay.h"
	#include "MSMDisplay.h"
	#include "LEVDisplay.h"
	#include "GeocacheDisplay.h"
	#include "TemperatureDisplay.h"
   #include "AudioRemote.h"
	#include "CustomDisplay.h"
#endif

// Defines list of available simulators
#define INC_HRM
#define INC_SDM
#define INC_BIKE_POWER
#define INC_BIKE_CADENCE
#define INC_BIKE_SPEED
#define INC_BIKE_SPEED_CADENCE
#define INC_WEIGHT_SCALE
#define INC_MSM
#define INC_LEV
#define INC_GEOCACHE
#define INC_TEMPERATURE
#define INC_AUDIO
#define INC_CUSTOM

// List of available simulator types
enum simTypes{
	HEART_RATE_MONITOR,
	SPEED_DISTANCE_MONITOR,
	BIKE_POWER,
	BIKE_SPDCAD,
	BIKE_CADENCE,
	BIKE_SPEED,
	WEIGHT_SCALE,
	MULTISPORT_SPEED_DISTANCE,
	LIGHT_ELECTRIC_VEHICLE,
	GEOCACHE,
	TEMPERATURE,
   AUDIO,
   CUSTOM,
	UNSELECTED  //Leave 'unselected' last so the index numbers of the combo box match
};


#define SIM_BASE_MASK   ((UCHAR) 0x03)
#define SIM_SENSOR      ((UCHAR) 0x01)
#define SIM_DISPLAY     ((UCHAR) 0x02)


// List of simulators to display in combo box 
// Must be listed in the same order as the simTypes enum
#define SUPPORTED_DISPLAYS {L"Heart Rate (HRM) Display", L"Speed and Distance (SDM) Display", L"Bike Power Display", L"Bike Speed and Cadence Display", L"Bike Cadence Display", L"Bike Speed Display", L"Weight Display", L"Multi-Sport Speed and Distance (MSM) Display", L"Light Electric Vehicle (LEV) Display", L"Geocache Dispay", L"Environment Display",L"Audio Remote", L"Custom Display"}
#define SUPPORTED_SENSORS {L"Heart Rate Monitor (HRM)", L"Speed and Distance Monitor (SDM)", L"Bike Power Sensor", L"Bike Speed and Cadence Sensor", L"Bike Cadence Sensor", L"Bike Speed Sensor", L"Weight Scale", L"Multi-Sport Speed and Distance (MSM) Sensor", L"Light Electric Vehicle (LEV) Sensor", L"Geocache Sensor", L"Environment  Sensor", L"Audio Device", L"Custom Sensor"}
#endif // !SIM_TYPES