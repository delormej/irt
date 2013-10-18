//
//  WFEBikeSpecializedData.h
//  WFConnector
//
//  Created by Murray Hughes on 25/08/11.
//  Copyright 2011 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/WFSensorData.h>

@class WFCommonData;


/**
 * Represents the data available from the Specialzied ANT+ LEV (Light
 * Electric Vehicle - or eBike).
 *
 * ANT+ sensors send data in multiple packets.  The WFEBikeSpecializedData combines the
 * more advanced (less commonly ed) of this data into a single entity.  The data
 * represents the latest of each data type sent from the sensor.
 */
@interface WFEBikeSpecializedData : WFSensorData
{
    /** \cond InterfaceDocs */

    WFCommonData* commonData;

    //Page 240
    USHORT hardwareVersionBattery;
    USHORT hardwareVersionRemote;
    USHORT hardwareVersionLight;
    USHORT hardwareVersionMotor;
    
    //Page 241
    USHORT firmwareVersionBattery;
    USHORT firmwareVersionRemote;
    USHORT firmwareVersionLight;
    USHORT firmwareVersionMotor;
    
    //Page 242
    UCHAR regenModePercent;
    UCHAR ECOModePercent;
    UCHAR eCOModePercent;
    SHORT motorTemperature;
    SHORT batteryTempature;
    float batteryVoltage;
    
    //Page 243
    UCHAR maxSpeedPedelecFast;
    UCHAR maxSpeedPedelecSlow;
    UCHAR maxSpeedThrottleFast;
    UCHAR maxSpeedThrottleSlow;
    UCHAR maxPedelecSpeed;
    UCHAR maxThrottleSpeed;
    
    //Page 244
    float accelEco;
    float accelPerformance;
    USHORT motorDriveCommend;
    UCHAR  configNibble;
    
    //Page 245
    float powerOutput;
    float  batteryCurrent;
    USHORT remainingRange;
    UCHAR  configWord;
    float currentSpeed;
    
    /** \endcond */
}

/**
 * Common sensor data, provides information about the sensor.
 */
@property (nonatomic, readonly) WFCommonData* commonData;


//Page 240
@property (nonatomic, assign) USHORT hardwareVersionBattery;
@property (nonatomic, assign) USHORT hardwareVersionRemote;
@property (nonatomic, assign) USHORT hardwareVersionLight;
@property (nonatomic, assign) USHORT hardwareVersionMotor;

//Page 241
@property (nonatomic, assign) USHORT firmwareVersionBattery;
@property (nonatomic, assign) USHORT firmwareVersionRemote;
@property (nonatomic, assign) USHORT firmwareVersionLight;
@property (nonatomic, assign) USHORT firmwareVersionMotor;

//Page 242
@property (nonatomic, assign) UCHAR regenModePercent;
@property (nonatomic, assign) UCHAR ECOModePercent;
@property (nonatomic, assign) SHORT motorTemperature;
@property (nonatomic, assign) SHORT batteryTempature;
@property (nonatomic, assign) float batteryVoltage;

//Page 243
@property (nonatomic, assign) UCHAR maxSpeedPedelecFast;
@property (nonatomic, assign) UCHAR maxSpeedPedelecSlow;
@property (nonatomic, assign) UCHAR maxSpeedThrottleFast;
@property (nonatomic, assign) UCHAR maxSpeedThrottleSlow;
@property (nonatomic, assign) UCHAR maxPedelecSpeed;
@property (nonatomic, assign) UCHAR maxThrottleSpeed;

//Page 244
@property (nonatomic, assign) float accelEco;
@property (nonatomic, assign) float accelPerformance;
@property (nonatomic, assign) USHORT motorDriveCommend;
@property (nonatomic, assign) UCHAR  configWord;

//Page 245
@property (nonatomic, assign) float powerOutput;
@property (nonatomic, assign) float  batteryCurrent;
@property (nonatomic, assign) USHORT remainingRange;
@property (nonatomic, assign) float currentSpeed;

@end




