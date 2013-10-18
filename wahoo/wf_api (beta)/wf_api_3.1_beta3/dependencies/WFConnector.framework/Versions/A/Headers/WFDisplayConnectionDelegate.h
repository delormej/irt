//
//  WFDisplayConnectionDelegate.h
//  WFConnector
//
//  Created by Murray Hughes on 23/05/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>

@class WFDisplayConnection, WFDisplayConfiguration;

@protocol WFDisplayConnectionDelegate <NSObject>

/**
 * Delegate method called when the conneciton is ready to receive a configuation file.
 * If nil, the display will display "No Config"
 " If the configation matches the one loaded on the device, it doesn't bother reloading it.
 */
- (WFDisplayConfiguration*) configurationForDisplayConnection:(WFDisplayConnection*) connection;

/**
 * Delegate method when a button is pressed down. Always called regardless of configuration
 */
- (void) displayConnection:(WFDisplayConnection*) connection didButtonDown:(int) buttonIndex;

/**
 * Delegate method when a button is release. Always called regardless of configuration
 */
- (void) displayConnection:(WFDisplayConnection*) connection didButtonUp:(int) buttonIndex;

/**
 * Delegate method when the visable page is changed by the user or the iOS device
 */
- (void) displayConnection:(WFDisplayConnection*) connection visablePageChanged:(NSString*) visablePageKey;

/**
 * Delegate methods for config loading progress and errors
 */

- (void) displayConnectionDidStartConfigurationLoading:(WFDisplayConnection*) connection;

- (void) displayConnection:(WFDisplayConnection*) connection didProgressConfigurationLoading:(float) progress;

- (void) displayConnectionDidFinishConfigurationLoading:(WFDisplayConnection*) connection;

- (void) displayConnection:(WFDisplayConnection*) connection didFailConfigurationLoadingWithError:(NSError*) error;




@end
