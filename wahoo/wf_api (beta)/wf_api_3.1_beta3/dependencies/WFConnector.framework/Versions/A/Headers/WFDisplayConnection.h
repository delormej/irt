//
//  WFDisplayConnection.h
//  WFConnector
//
//  Created by Murray Hughes on 23/05/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import <Foundation/Foundation.h>
#import <WFConnector/WFSensorConnection.h>
#import "WFDisplayConnectionDelegate.h"

#import "WFDisplayData.h"

@class WFDisplaySensorConfiguration;

/**
 * Represents a connection to an detatched display (bike computer / watch)
 */
@interface WFDisplayConnection : WFSensorConnection 
{
    id<WFDisplayConnectionDelegate> displayConnectionDelegate;
    
    
    int currentPage;
    NSMutableArray* updateQueue;
    WFDisplaySensorConfiguration* configuration;

    float _lastLoadingProgress;
}

@property (nonatomic, assign) id<WFDisplayConnectionDelegate> displayConnectionDelegate;

- (NSString*) currentPageKey;

- (WFDisplayData*) getDisplayData;

// Load a new configuation
- (void) loadConfiguration:(WFDisplayConfiguration*) configuation;

// Begin a group of updates
- (void) beginUpdates;

// Begin a group of updates that are sent regardless of the current page.
// Used mostly for hidden pages
- (void) beginForcedUpdates;

// End group updates
- (void) endUpdates;

// Updates a element with a new value. Currently only applies to strings
- (BOOL) setValue:(id)value forElementWithKey:(NSString *)key;

// Updates the element's frame
- (BOOL) setFrame:(CGRect)frame forElementWithKey:(NSString *)key;

// Hide/Show the element with key
- (BOOL) setHidden:(BOOL)hidden forElementWithKey:(NSString *)key;



/**
 * Sets the current Page being displayed
 *
 * @param pageKey As defined in the configuration file.
 *
 * @param timeout Optional timeout value to automatically go pack to the previous page
 *                  Set to 0 if you do not want it to go back.
 *
 * @return <c>TRUE</c> if the value is sent, otherwise <c>FALSE</c>.
 */
- (BOOL) setPageVisableWithKey:(NSString*) pageKey timeout:(NSTimeInterval) timeout;



- (BOOL) setBacklightOn:(BOOL) on;


@end