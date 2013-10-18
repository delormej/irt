//
//  WFEBikeRawData.h
//  WFConnector
//
//  Created by Michael Moore on 8/12/11.
//  Copyright 2011 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/WFSensorData.h>

@class WFCommonData;


/**
 * Represents the raw data available from the ANT+ LEV (Light
 * Electric Vehicle - or eBike).
 *
 * ANT+ sensors send data in multiple packets.  The WFFootpodRawData combines the
 * more advanced (less commonly used) of this data into a single entity.  The data
 * represents the latest of each data type sent from the sensor.
 */
@interface WFEBikeRawData : WFSensorData
{
	/** \cond InterfaceDocs */
	
	WFCommonData* commonData;
	
    
    // page 5 data.
    UCHAR assistModesSupported;
    UCHAR regenModesSupported;
    USHORT wheelCircumference;

	/** \endcond */
}


/**
 * Common sensor data, provides information about the sensor.
 */
@property (nonatomic, readonly) WFCommonData* commonData;

/** Gets the number of assist modes supported by the LEV. */
@property (nonatomic, assign) UCHAR assistModesSupported;
/** Gets the number of regen modes supported by the LEV. */
@property (nonatomic, assign) UCHAR regenModesSupported;
/** Gets the wheel circumference, in mm. */
@property (nonatomic, assign) USHORT wheelCircumference;

@end
