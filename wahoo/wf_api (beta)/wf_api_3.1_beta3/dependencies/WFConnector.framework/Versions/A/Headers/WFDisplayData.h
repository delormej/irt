//
//  WFBTLEDisplayData.h
//  WFConnector
//
//  Created by Murray Hughes on 13/09/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "WFSensorData.h"

@class WFBTLECommonData;

@interface WFDisplayData : WFSensorData
{
    NSInteger _visablePageIndex;
    WFBTLECommonData* btleCommonData;
}

/** Gets the metadata for the BTLE device. */
@property (nonatomic, retain) WFBTLECommonData* btleCommonData;

@property (nonatomic) NSInteger visablePageIndex;

@end
