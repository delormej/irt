//
//  WFANTBikePowerData.h
//  WFConnector
//
//  Created by Michael Moore on 6/7/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/WFBikePowerData.h>


/**
 * Represents the most commonly used data available from the ANT+ Bike Power sensor.
 *
 * ANT+ sensors send data in multiple packets.  The WFBikePowerData combines the
 * most commonly used of this data into a single entity.  The data represents
 * the latest of each data type sent from the sensor.
 */
@interface WFANTBikePowerData : WFBikePowerData
{
	/** \cond InterfaceDocs */
	
	WFBikePowerType_t sensorType;
	ULONG ulAccumulatedTicks;
	float fpCalculatedCrankTicks;
    float fpAccumulatedPower;
	UCHAR ucEventCount;
	NSTimeInterval timestamp;
	BOOL timestampOverflow;
	
	/** \endcond */
}


/**
 * Indicates the type of the power meter.
 *
 * @note After the sensor connects, or is reset, it will take several
 * messages from the device before the sensor type can be determined.
 */
@property (nonatomic, assign) WFBikePowerType_t sensorType;
/**
 * The total accumulated power in Watts
 *
 * Accumlated power is the sum of the average torgue for each update period.
 * This is ONLY used for Power Only sensors, all other power meter types
 * use fpAccumulatedTorque
 *
 */
@property (nonatomic, assign) float fpAccumulatedPower;

/**
 * The total accumulated revolutions measured by the sensor.
 *
 * This source of this value depends on the sensor type.  The value will be
 * wheel revolutions for wheel-based sensors, and crank revolutions for
 * crank based sensors.
 */
@property (nonatomic, assign) ULONG ulAccumulatedTicks;
/**
 * The total calculated crank revolutions.
 *
 * This value is available only for certain types of wheel-based sensors.
 */
@property (nonatomic, assign) float fpCalculatedCrankTicks;
/**
 * The event counter value.
 *
 * See the event counter value of the specific power meter type for more
 * about this value (WFBikePowerCTFData, WFBikePowerCrankTorqueData,
 * and WFBikePowerWheelTorqueData).
 */
@property (nonatomic, assign) UCHAR ucEventCount;
/**
 * The real-time timestamp for data from the sensor (as an offset from
 * the Cocoa reference date).
 *
 * Time values from the sensors are implemented as an offset in seconds
 * between the time when the sensor is turned on and the time when a
 * data sample is taken.  This value is typically a 16-bit unsigned
 * integer in units of 1/1024 second.  The rollover is then 64 seconds.
 *
 * A base real-time value is stored when the first sample from the
 * sensor is received.  The timestamp of each subsequent sample is the
 * base time value offset by the time value offset from the sensor.
 * If the time between samples is greater than the rollover time, the
 * base time value is set to the time the first sample after the delay
 * is received.
 */
@property (nonatomic, assign) NSTimeInterval timestamp;
/**
 * Indicates that the time between data samples from the sensor has
 * exceeded the rollover time (64 seconds).
 */
@property (nonatomic, assign) BOOL timestampOverflow;

@end
