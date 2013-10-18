//
//  WFFalcoEBikeConnection.h
//  WFConnector
//
//  Created by Murray Hughes on 30/08/11.
//  Copyright 2011 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/WFSensorConnection.h>

@class WFFalcoEBikeData;
@class WFFalcoEBikeRawData;

/**
 * Represents a connection to an Falco ANT eBike.
 */
@interface WFFalcoEBikeConnection : WFSensorConnection


/**
 * Returns the latest data available from the sensor.
 *
 * @see WFSensorConnection::getData
 *
 * @return A WFFalcoEBikeData instance containing data if available,
 * otherwise <c>nil</c>.
 */
- (WFFalcoEBikeData*)getFalcoEBikeData;

/**
 * Returns the latest raw (unformatted) data available from the sensor.
 *
 * @see WFSensorConnection::getRawData
 *
 * @return A WFFalcoEBikeRawData instance containing data if available,
 * otherwise <c>nil</c>.
 */
- (WFFalcoEBikeRawData*)getFalcoEBikeRawData;

/**
 * Sets the Power Level for the motor
 *
 * @param powerLevel Power assistance level, range 0 to 9
 *
 * @return <c>TRUE</c> if the request was successful, otherwise <c>FALSE</c>.
 */
- (BOOL) setPowerLevel:(UCHAR) powerLevel;

/**
 * Sets the Regenation Level for the motor
 *
 * @param regenLevel Regenation level, range 0 to 9
 *
 * @return <c>TRUE</c> if the request was successful, otherwise <c>FALSE</c>.
 */
- (BOOL) setRegenLevel:(UCHAR) regenLevel;

/**
 * Sets the Turbo mode for the motor
 *
 * @param turboOn <c>TRUE</c> to turn on, otherwise <c>FALSE</c>.
 *
 * @return <c>TRUE</c> if the request was successful, otherwise <c>FALSE</c>.
 */
- (BOOL) setTurboOn:(BOOL) turboOn;

/**
 * Sets the Cruise mode for the motor
 *
 * @param cruiseOn <c>TRUE</c> to turn on, otherwise <c>FALSE</c>.
 *
 * @return <c>TRUE</c> if the request was successful, otherwise <c>FALSE</c>.
 */
- (BOOL) setCruiseOn:(BOOL) cruiseOn;

/**
 * Sets the Lock mode for the motor
 *
 * @param lockOn <c>TRUE</c> to turn on, otherwise <c>FALSE</c>.
 *
 * @return <c>TRUE</c> if the request was successful, otherwise <c>FALSE</c>.
 */
- (BOOL) setLockOn:(BOOL) lockOn;


/**
 * Sests limits on current, speed and power
 *
 * @return <c>TRUE</c> if the request was successful, otherwise <c>FALSE</c>.
 */
- (BOOL) setCurrentLimit:(UCHAR) currentLimit speedLimit:(UCHAR) speedLimit powerLimit:(UCHAR) powerLimit;

@end
