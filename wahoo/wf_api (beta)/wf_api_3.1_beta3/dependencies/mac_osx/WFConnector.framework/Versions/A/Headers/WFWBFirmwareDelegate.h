//
//  WFWBFirmwareDelegate.h
//  WFConnector
//
//  Created by Michael Moore on 7/10/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>


/**
 * Provides the interface for callback methods used by the WFSensorConnection
 * during a firmware update on supported Wahoo BTLE devices.
 */
@protocol WFWBFirmwareDelegate <NSObject>

/**
 * Invoked when a firmware update packet has been sent ot the device.
 *
 * @param totalBytesSent The total number of bytes sent to the device thus far.
 */
- (void)firmwareUpdatePacketSent:(int)totalBytesSent;
/**
 * Invoked when the firmware update transfer has finished.
 *
 * @param bSuccess <c>TRUE</c> if the firmware update was successful, otherwise <c>FALSE</c>.
 */
- (void)firmwareUpdateTransferFinished:(BOOL)bSuccess;
/**
 * Invoked when the firmware update transfer has started.
 *
 * @param totalBytesToSend The total number of bytes to be sent to the device.
 */
- (void)firmwareUpdateTransferStarted:(int)totalBytesToSend;

@optional
// TODO:  move to private header.
- (void)firmwareUpdateDebugModeResponse:(BOOL)bSucceeded;

// private, this method is implemented in WFSensorConnection. It checks the web site to see
// if a newer version of the firmware is available from the firmware update web site for the device name
-(void)checkAvailableFirmwareVersionForDeviceName:(NSString *)deviceName withCurrentVersion:(NSString *)currentFirmwareVersion;

- (void) firmwareBootloaderVersionResponse:(uint8_t*) version success:(BOOL) bSuccess;


@end
