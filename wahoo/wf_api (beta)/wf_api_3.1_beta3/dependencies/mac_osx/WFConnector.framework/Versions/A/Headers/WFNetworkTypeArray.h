//
//  WFNetworkTypeArray.h
//  WFConnector
//
//  Created by Michael Moore on 11/7/11.
//  Copyright (c) 2011 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/WFBitFieldArray.h>


@interface WFNetworkTypeArray : WFBitFieldArray

- (WFNetworkType_t)firstNetworkType;
- (id)initWithNetworkTypeEnum:(WFNetworkType_t)networkType;
- (WFNetworkType_t)nextNetworkType;

+ (WFNetworkTypeArray*)fromNetworkTypeEnum:(WFNetworkType_t)networkType;

@end
