//
//  WFBitFieldArray.h
//  WFConnector
//
//  Created by Michael Moore on 11/7/11.
//  Copyright (c) 2011 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/hardware_connector_types.h>

@interface WFBitFieldArray : NSObject
{
    ULONG values[32];
    UCHAR count;
    UCHAR ucIndex;
}


@property (nonatomic, readonly) UCHAR count;
@property (nonatomic, readonly) BOOL hasNext;


- (ULONG)first;
- (id)initWithBitField:(ULONG)bitField;
- (ULONG)next;
- (void)reset;

+ (WFBitFieldArray*)fromBitField:(ULONG)bitField;

@end
