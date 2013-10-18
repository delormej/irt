//
//  FirmwareVersionDescriptor.h
//  FisicaUtility
//
//  Created by Barb Wait on 12/24/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "common_types.h"


@interface WFWBFirmwareVersionDescriptor : NSObject

@property (nonatomic, copy) NSString *firmwareName;
@property (nonatomic, copy) NSString *firmwareVendor;
@property (nonatomic, copy) NSString *firmwareVersion;
@property (nonatomic, copy) NSString *firmwareVersionComment;
@property (nonatomic) BOOL currentVersion;
@property (nonatomic) ULONG firmwareFileCRC32;

@end
