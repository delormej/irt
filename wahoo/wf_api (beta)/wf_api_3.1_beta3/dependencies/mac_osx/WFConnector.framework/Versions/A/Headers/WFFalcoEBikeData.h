//
//  WFFalcoEBikeData.h
//  WFConnector
//
//  Created by Murray Hughes on 30/08/11.
//  Copyright 2011 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/WFSensorData.h>


/**
 * Represents the data available from the Falco ANT+ LEV (Light
 * Electric Vehicle - or eBike).
 *
 * ANT+ sensors send data in multiple packets.  The WFEBikeSpecializedData combines the
 * more advanced (less commonly ed) of this data into a single entity.  The data
 * represents the latest of each data type sent from the sensor.
 */
@interface WFFalcoEBikeData : WFSensorData
{
    float currentSpeed;
    UCHAR current;
    UCHAR voltage;
    UCHAR power;
    UCHAR torque;
    UCHAR temperature;
    
    UCHAR powerLevel;
    UCHAR regenLevel;
    BOOL isTurboModeOn;
    BOOL isCruiseModeOn;
    BOOL isLocked;
}

@property (nonatomic, assign) float currentSpeed;
@property (nonatomic, assign) UCHAR current;
@property (nonatomic, assign) UCHAR voltage;
@property (nonatomic, assign) UCHAR power;
@property (nonatomic, assign) UCHAR torque;
@property (nonatomic, assign) UCHAR temperature;

@property (nonatomic, assign) UCHAR powerLevel;
@property (nonatomic, assign) UCHAR regenLevel;
@property (nonatomic, assign) BOOL isTurboModeOn;
@property (nonatomic, assign) BOOL isCruiseModeOn;
@property (nonatomic, assign) BOOL isLocked;


@end
