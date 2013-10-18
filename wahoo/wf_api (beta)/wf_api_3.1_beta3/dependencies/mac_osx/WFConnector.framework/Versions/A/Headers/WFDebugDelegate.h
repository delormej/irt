//
//  WFDebugDelegate.h
//  WFConnector
//
//  Created by Michael Moore on 7/13/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>

@class _WFSensorConnection;


@protocol WFDebugDelegate <NSObject>

- (void)connection:(_WFSensorConnection*)conn didReceiveDebugData:(NSData*)data;
- (void)connection:(_WFSensorConnection*)conn didReceiveDebugModeResponse:(BOOL)bSuccess;

@end
