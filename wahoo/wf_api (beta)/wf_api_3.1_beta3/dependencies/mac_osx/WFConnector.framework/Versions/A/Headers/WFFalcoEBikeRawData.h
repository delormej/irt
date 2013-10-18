//
//  WFFalcoEBikeRawData.h
//  WFConnector
//
//  Created by Murray Hughes on 30/08/11.
//  Copyright 2011 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/WFSensorData.h>

@class WFCommonData;



/**
 * Represents the raw data available from the Falco ANT+ LEV (Light
 * Electric Vehicle - or eBike).
 *
 * ANT+ sensors send data in multiple packets.  The WFFootpodRawData combines the
 * more advanced (less commonly used) of this data into a single entity.  The data
 * represents the latest of each data type sent from the sensor.
 */
@interface WFFalcoEBikeRawData :  WFSensorData
{
    WFCommonData* commonData;
}


/**
 * Common sensor data, provides information about the sensor.
 */
@property (nonatomic, readonly) WFCommonData* commonData;


@end
