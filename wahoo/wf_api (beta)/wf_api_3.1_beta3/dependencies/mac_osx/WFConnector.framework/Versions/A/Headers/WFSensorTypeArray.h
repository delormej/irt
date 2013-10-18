//
//  WFSensorTypeArray.h
//  WFConnector
//
//  Created by Michael Moore on 10/26/11.
//  Copyright (c) 2011 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/WFBitFieldArray.h>


@interface WFSensorTypeArray : WFBitFieldArray
{
}


- (WFSensorType_t)firstSensorType;
- (id)initWithSensorTypeEnum:(WFSensorType_t)sensorType;
- (WFSensorType_t)nextSensorType;

+ (WFSensorTypeArray*)fromSensorTypeEnum:(WFSensorType_t)sensorType;

@end
