//
//  WFEBikeConnection.h
//  WFConnector
//
//  Created by Michael Moore on 8/12/11.
//  Copyright 2011 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/WFSensorConnection.h>


@class WFEBikeData;
@class WFEBikeRawData;
@class WFEBikeSpecializedData;


/**
 * Represents a connection to an ANT+ eBike (LEV - Light Electric Vehicle).
 */
@interface WFEBikeConnection : WFSensorConnection
{
}


/**
 * Returns the latest data available from the sensor.
 *
 * @see WFSensorConnection::getData
 *
 * @return A WFEBikeData instance containing data if available,
 * otherwise <c>nil</c>.
 */
- (WFEBikeData*)getEBikeData;

/**
 * Returns the latest raw (unformatted) data available from the sensor.
 *
 * @see WFSensorConnection::getRawData
 *
 * @return A WFEBikeRawData instance containing data if available,
 * otherwise <c>nil</c>.
 */
- (WFEBikeRawData*)getEBikeRawData;


/**
 * Returns the latest Specialized data available from the sensor.
 *
 * @return A WFEBikeSpecializedData instance containing data if available,
 * otherwise <c>nil</c>.
 */
- (WFEBikeSpecializedData*)getEBikeSpecializedData;

/**
 * Request a data page from a Specialized LEV (only valid datapages accepted)
 *
 * @return YES if page request was sent, NO otherwise 
 */
- (BOOL) requestEBikeSpecializedDataPage:(UCHAR) pageNumber;

/**
 * Sets the LEV configuration.
 *
 * @param pstData A pointer to the configuration structure.
 *
 * @return <c>TRUE</c> if the configuration is set successfully, otherwise <c>FALSE</c>.
 */
- (BOOL)setConfiguration:(WFLEVConfiguration_t*)pstData;

/**
 * Sets the configuration specific to the Specialized LEV.
 *
 * @param pstData A pointer to the configuration structure.
 *
 * @return <c>TRUE</c> if the configuration is set successfully, otherwise <c>FALSE</c>.
 */
- (BOOL)setSpecializedConfiguration:(WFLEVSpecializedConfiguration_t*)pstData;

/**
 * Returns the latest LEV configuration available from the sensor.
 * can be used to fill in the current states before sending
 *
 * @see WFSensorConnection::setConfiguration
 *
 * @return A WFLEVConfiguration_t structure containing data if available
 */
- (WFLEVConfiguration_t)getCurrentConfiguration;

/**
 * Increases the travel mode based on the recomended settigns
 * It will also move from regen mode to assit mode
 *
 * @return <c>TRUE</c> if the travel mode was changed successfully, otherwise <c>FALSE</c>
 */
- (BOOL) increaseTravelMode;

/**
 * Decreases the travel mode based on the recomended settigns
 * It will also move from assit mode to regen mode
 *
 * @return <c>TRUE</c> if the travel mode was changed successfully, otherwise <c>FALSE</c>
 */
- (BOOL) decreaseTravelMode;


/**
 * Turns travel modes off, both assist and regen
 *
 * @return <c>TRUE</c> if the travel mode was changed successfully, otherwise <c>FALSE</c>
 */
- (BOOL) travelModesOff;

@end
