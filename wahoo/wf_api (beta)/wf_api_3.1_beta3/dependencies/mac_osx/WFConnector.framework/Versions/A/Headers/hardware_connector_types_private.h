//
//  hardware_connector_types_private.h
//  WFConnector
//
//  Created by Michael Moore on 9/21/11.
//  Copyright 2011 Wahoo Fitness. All rights reserved.
//

#ifndef WFConnector_hardware_connector_types_private_h
#define WFConnector_hardware_connector_types_private_h

#include <WFConnector/common_types.h>


#ifndef _WF_SENSOR_TYPE_T_
#define _WF_SENSOR_TYPE_T_
/**
 * Describes a type of hardware sensor device.
 */
typedef enum
{
    ////////////////////////////////////////////////////////////////////////////
    //
    // FROM PUBLIC TYPES
    
	/** Specifies non-existent sensor. */
	WF_SENSORTYPE_NONE                           = 0,
	/** Specifies the bike power sensor. */
	WF_SENSORTYPE_BIKE_POWER                     = 0x00000001,
	/** Specifies the bike speed sensor. */
	WF_SENSORTYPE_BIKE_SPEED                     = 0x00000002,
	/** Specifies the bike cadence sensor. */
	WF_SENSORTYPE_BIKE_CADENCE                   = 0x00000004,
	/** Specifies the combined bike speed and cadence sensor. */
	WF_SENSORTYPE_BIKE_SPEED_CADENCE             = 0x00000008,
	/** Specifies the FootPod stride and distance sensor. */
	WF_SENSORTYPE_FOOTPOD                        = 0x00000010,
	/** Specifies the heart rate monitor sensor. */
	WF_SENSORTYPE_HEARTRATE                      = 0x00000020,
	/** Specifies the Weight Scale monitor sensor */
	WF_SENSORTYPE_WEIGHT_SCALE                   = 0x00000040,
	/** Specifies a generic ANT FS device. */
	WF_SENSORTYPE_ANT_FS                         = 0x00000080,
	/** Specifies the GPS location sensor. */
	WF_SENSORTYPE_LOCATION                       = 0x00000100,
	/** Specifies the calorimeter sensor. */
	WF_SENSORTYPE_CALORIMETER                    = 0x00000200,
	/** Specifies the GeoCache sensor. */
	WF_SENSORTYPE_GEO_CACHE                      = 0x00000400,
	/** Specifies the Fitness Equipment sensor. */
	WF_SENSORTYPE_FITNESS_EQUIPMENT              = 0x00000800,
    /** Specifies the Multi-Sport Speed and Distance sensor. */
    WF_SENSORTYPE_MULTISPORT_SPEED_DISTANCE      = 0x00001000,
    /** Specifies the BTLE Proximity sensor. */
    WF_SENSORTYPE_PROXIMITY                      = 0x00002000,
    /** Specifies the BTLE Health Thermometer sensor. */
    WF_SENSORTYPE_HEALTH_THERMOMETER             = 0x00004000,
    /** Specifies the BTLE Blood Pressure sensor. */
    WF_SENSORTYPE_BLOOD_PRESSURE                 = 0x00008000,
    /** Specifies the Blood Glucose Monitor sensor. */
    WF_SENSORTYPE_BTLE_GLUCOSE                   = 0x00010000,
    /** Specifies the ANT+ Blood Glucose Monitor sensor. */
    WF_SENSORTYPE_GLUCOSE                        = 0x00020000,

    ////////////////////////////////////////////////////////////////////////////
    //
    // PRIVATE ONLY
    /** Specifies the Accelormater sensor. */
    WF_SENSORTYPE_ACCELEROMETER                  = 0x00040000,
    /** Specifies the Audio Control device. */
    WF_SENSORTYPE_AUDIO_CONTROL                  = 0x00080000,
    /** Specifies the eBike (LEV, Light Electric Vehicle). */
    WF_SENSORTYPE_EBIKE                          = 0x00100000,
    /** Specifies the Falco eBike */
    WF_SENSORTYPE_FALCO_EBIKE                    = 0x00200000,
    /** Specifies the BTLE Display sensor. */
    WF_SENSORTYPE_DISPLAY                        = 0x00800000,

    WF_SENSORTYPE_BTLE_FIRMWARE_UPDATE           = 0x01000000,

} WFSensorType_t;
#endif  // _WF_SENSOR_TYPE_T_

#ifndef _WF_NETWORK_TYPE_T_
#define _WF_NETWORK_TYPE_T_
/**
 * Describes a type of hardware sensor device.
 */
typedef enum
{
    /**
     * The network type is not specified.
     *
     * This value may not be used for connection requests.  If used, the connection
     * request will fail.
     */
    WF_NETWORKTYPE_UNSPECIFIED                      = 0,
	/** Default, ANT+ standard network */
	WF_NETWORKTYPE_ANTPLUS                          = 0x01,
	/** Specifies BTLE devices */
	WF_NETWORKTYPE_BTLE                             = 0x02,
	/** Specifies Suunto devices */
	WF_NETWORKTYPE_SUUNTO                           = 0x04,
    
    ////////////////////////////////////////////////////////////////////////////
    //
    // PRIVATE ONLY
        
    //
    ////////////////////////////////////////////////////////////////////////////
	
    /**
     * Wildcard for any available network.
     *
     * This value may be specified in connection request.  When used, connection
     * attempt is made on any available network.  The first connection to be
     * established will be used.  Connection requests on other network types
     * will be cancelled.
     */
    WF_NETWORKTYPE_ANY                              = WF_NETWORKTYPE_ANTPLUS | WF_NETWORKTYPE_SUUNTO | WF_NETWORKTYPE_BTLE,

} WFNetworkType_t;
#endif  // _WF_NETWORK_TYPE_T_



#pragma mark -
#pragma mark LEV Sensor Types
/////////////////////////////////////////////////////////////////////////////
// LEV Sensor Types.
/////////////////////////////////////////////////////////////////////////////

/**
 * Describes the level of assistence provided by the LEV motor.
 */
typedef enum
{
    /** The LEV motor is not providiing assistence. */
    WF_LEV_ASSIST_LEVEL_OFF           = 0,
    /** The LEV motor is providing the minimum level of assistence. */
    WF_LEV_ASSIST_LEVEL_1             = 1,
    /** The LEV motor is providing assistence at level 2 of 7. */
    WF_LEV_ASSIST_LEVEL_2             = 2,
    /** The LEV motor is providing assistence at level 3 of 7. */
    WF_LEV_ASSIST_LEVEL_3             = 3,
    /** The LEV motor is providing assistence at level 4 of 7. */
    WF_LEV_ASSIST_LEVEL_4             = 4,
    /** The LEV motor is providing assistence at level 5 of 7. */
    WF_LEV_ASSIST_LEVEL_5             = 5,
    /** The LEV motor is providing assistence at level 6 of 7. */
    WF_LEV_ASSIST_LEVEL_6             = 6,
    /** The LEV motor is providing the maximum level of assistence. */
    WF_LEV_ASSIST_LEVEL_7             = 7,
    
} WFLEVAssistLevel_t;

/**
 * Describes the level at which the LEV battery is being recharged.
 */
typedef enum
{
    /** The LEV battery is not being recharged. */
    WF_LEV_REGEN_LEVEL_OFF            = 0,
    /** The LEV battery is being recharged at the minimum level. */
    WF_LEV_REGEN_LEVEL_1              = 1,
    /** The LEV battery is being recharged at level 2 of 7. */
    WF_LEV_REGEN_LEVEL_2              = 2,
    /** The LEV battery is being recharged at level 3 of 7. */
    WF_LEV_REGEN_LEVEL_3              = 3,
    /** The LEV battery is being recharged at level 4 of 7. */
    WF_LEV_REGEN_LEVEL_4              = 4,
    /** The LEV battery is being recharged at level 5 of 7. */
    WF_LEV_REGEN_LEVEL_5              = 5,
    /** The LEV battery is being recharged at level 6 of 7. */
    WF_LEV_REGEN_LEVEL_6              = 6,
    /** The LEV battery is being recharged at the maximum level. */
    WF_LEV_REGEN_LEVEL_7              = 7,
    
} WFLEVRegenLevel_t;

/**
 * Describes the set of configuration parameters reflecting the state, or
 * desired state of the LEV.
 *
 * This structure is used for both sending and receiving configuration data
 * to/from the LEV.
 */
typedef struct
{
    /** The LEV wheel circumference, in mm. */
    USHORT wheelCircumference;
    /** The current or desired assist level. */
    WFLEVAssistLevel_t assistLevel;
    /** The current or desired regen level. */
    WFLEVRegenLevel_t regenLevel;
    /** The current or desired rear gear. */
    UCHAR currentRearGear;
    /** The current or desired front gear. */
    UCHAR currentFrontGear;
    /** The current or desired head light state. */
    BOOL isLightOn;
    /** The current or desired high beam state. */
    BOOL isHighBeamOn;
    /** The current or desired left turn signal state. */
    BOOL isLeftSignalOn;
    /** The current or desired right turn signal state. */
    BOOL isRightSignalOn;
    
} WFLEVConfiguration_t;

/** Describes configuration data specific to the Specialized LEV. */
typedef struct
{
    /** The current or desired regen mode percentage. */
    UCHAR regenModePercent;
    /** The current or desired eco-mode percentage. */
    UCHAR ecoModePercent;
    
} WFLEVSpecializedConfiguration_t;

#endif
