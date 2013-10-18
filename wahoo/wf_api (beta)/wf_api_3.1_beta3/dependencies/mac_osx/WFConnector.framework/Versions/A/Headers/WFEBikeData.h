//
//  WFEBikeData.h
//  WFConnector
//
//  Created by Michael Moore on 8/12/11.
//  Copyright 2011 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/WFSensorData.h>


/**
 * Describes the LEV motor or battery temperature alert status.
 */
typedef enum
{
    /** No alert, or unknown alert condition. */
    WF_LEV_TEMPERATURE_ALERT_NONE     = 0,
    /** Temperature in overheating condition. */
    WF_LEV_TEMPERATURE_ALERT_OVERHEAT = 1,
    
} WFLEVTemperatureAlert_t;

/**
 * Describes the LEV motor or battery temperature status.
 */
typedef enum
{
    /** Temperature is unknown. */
    WF_LEV_TEMPERATURE_UNKNOWN        = 0,
    /** Temperature is cold. */
    WF_LEV_TEMPERATURE_COLD           = 1,
    /** Temperature is between cold and warm. */
    WF_LEV_TEMPERATURE_MID_WARM       = 2,
    /** Temperature is warm. */
    WF_LEV_TEMPERATURE_WARM           = 3,
    /** Temperature is between warm and hot. */
    WF_LEV_TEMPERATURE_MID_HOT        = 4,
    /** Temperature is hot. */
    WF_LEV_TEMPERATURE_HOT            = 5,
    
} WFLEVTemperature_t;

/**
 * Describes an error condition in the LEV.
 */
typedef enum
{
    /** No error condition. */
    WF_LEV_ERROR_NONE                 = 0,
    /** Error in the LEV battery. */
    WF_LEV_ERROR_BATTERY              = 1,
    /** Error in the LEV drive train. */
    WF_LEV_ERROR_DRIVE_TRAIN          = 2,
    /** The LEV battery has reached End of Life. */
    WF_LEV_ERROR_BATTERY_EOL          = 3,
    /** The LEV is in overheating condition. */
    WF_LEV_ERROR_OVERHEATING          = 4,
    
} WFLEVError_t;



/**
 * Represents the most commonly used data available from the ANT+ LEV (Light
 * Electric Vehicle - or eBike).
 *
 * ANT+ sensors send data in multiple packets.  The WFEBikeData combines the
 * most commonly used of this data into a single entity.  The data represents
 * the latest of each data type sent from the sensor.
 */
@interface WFEBikeData : WFSensorData
{
    // page 1 data.
    WFLEVTemperature_t motorTemperature;
    WFLEVTemperatureAlert_t motorTemperatureAlert;
    WFLEVTemperature_t batteryTemperature;
    WFLEVTemperatureAlert_t batteryTemperatureAlert;
    WFLEVAssistLevel_t assistLevel;
    WFLEVRegenLevel_t regenLevel;
    BOOL isThrottleOn;
    BOOL isLightOn;
    BOOL isHighBeamOn;
    BOOL isLeftSignalOn;
    BOOL isRightSignalOn;
    BOOL doesGearExist;
    BOOL isGearManual;
    UCHAR currentFrontGear;
    UCHAR currentRearGear;
    WFLEVError_t errorState;
    
    // page 2 data.
    float currentSpeed;
    float accumulatedDistance;
    USHORT remainingRange;
    
    // page 3 data.
    UCHAR batteryPercentCharged;
    BOOL batteryEmptyWarning;
    UCHAR assistPercent;
    
    // page 4 data.
    USHORT chargeCycleCount;
    float fuelConsumption;
    float batteryVoltage;
    float distanceOnCharge;
}


/** Gets the current motor temperature. */
@property (nonatomic, assign) WFLEVTemperature_t motorTemperature;
/** Gets the current motor temperature alert status. */
@property (nonatomic, assign) WFLEVTemperatureAlert_t motorTemperatureAlert;
/** Gets the current battery temperature. */
@property (nonatomic, assign) WFLEVTemperature_t batteryTemperature;
/** Gets the current battery temperature alert status. */
@property (nonatomic, assign) WFLEVTemperatureAlert_t batteryTemperatureAlert;
/** Gets the current assist level. */
@property (nonatomic, assign) WFLEVAssistLevel_t assistLevel;
/** Gets the current regen level. */
@property (nonatomic, assign) WFLEVRegenLevel_t regenLevel;
/** <c>TRUE</c> if the throttle is on, otherwise <c>FALSE</c>. */
@property (nonatomic, assign) BOOL isThrottleOn;
/** <c>TRUE</c> if the light is on, otherwise <c>FALSE</c>. */
@property (nonatomic, assign) BOOL isLightOn;
/** <c>TRUE</c> if the high beam is on, otherwise <c>FALSE</c>. */
@property (nonatomic, assign) BOOL isHighBeamOn;
/** <c>TRUE</c> if the left turn signal is on, otherwise <c>FALSE</c>. */
@property (nonatomic, assign) BOOL isLeftSignalOn;
/** <c>TRUE</c> if the right turn signal is on, otherwise <c>FALSE</c>. */
@property (nonatomic, assign) BOOL isRightSignalOn;
/** <c>TRUE</c> if the gear is available, otherwise <c>FALSE</c>. */
@property (nonatomic, assign) BOOL doesGearExist;
/** <c>TRUE</c> if the gear is manual, otherwise <c>FALSE</c>. */
@property (nonatomic, assign) BOOL isGearManual;
/** Gets the current front gear setting. */
@property (nonatomic, assign) UCHAR currentFrontGear;
/** Gets the current rear gear setting. */
@property (nonatomic, assign) UCHAR currentRearGear;
/** Gets the current error state. */
@property (nonatomic, assign) WFLEVError_t errorState;

/** Gets the current speed, in km/h. */
@property (nonatomic, assign) float currentSpeed;
/** Gets the accumulated distance, in km. */
@property (nonatomic, assign) float accumulatedDistance;
/** Gets the remaining range based on current conditions, in km. */
@property (nonatomic, assign) USHORT remainingRange;

/** Gets the current battery charge percentage. */
@property (nonatomic, assign) UCHAR batteryPercentCharged;
/** <c>TRUE</c> if the battery empty warning is on, otherwise <c>FALSE</c>. */
@property (nonatomic, assign) BOOL batteryEmptyWarning;
/** Gets the current assist percentage. */
@property (nonatomic, assign) UCHAR assistPercent;

/** Gets the current charge cycle count. */
@property (nonatomic, assign) USHORT chargeCycleCount;
/** Gets the current fuel consumption, in Wh/km. */
@property (nonatomic, assign) float fuelConsumption;
/** Gets the current battery voltage. */
@property (nonatomic, assign) float batteryVoltage;
/** Gets the distance in km that the LEV has traveled on the current charge. */
@property (nonatomic, assign) float distanceOnCharge;


@end
