/*
 *  WFFirmwareUpdaterDelegate.h
 *  WFConnector
 *
 *  Created by Michael Moore on 5/19/10.
 *  Copyright 2010 Wahoo Fitness. All rights reserved.
 *
 */

#import <WFConnector/common_types.h>


typedef struct
{
	USHORT hardwareVersion;
	USHORT bootLoaderVersion;
	UCHAR firmwareVersionMajor;
	UCHAR firmwareVersionMinor;
	UCHAR firmwareVersionRevision;
	BOOL runningHighMem;
	
} WFFirmwareVersionInfo_t;


@class WFFirmwareUpdater;


@protocol WFFirmwareUpdaterDelegate

- (void)firmwareUpdater:(WFFirmwareUpdater*)fwUpdater receivedDeviceInfo:(WFFirmwareVersionInfo_t)info;
- (void)firmwareUpdater:(WFFirmwareUpdater*)fwUpdater transferProgress:(float)progress;
- (void)firmwareUpdater:(WFFirmwareUpdater*)fwUpdater transferComplete:(UCHAR)transferStatus;
- (void)firmwareUpdater:(WFFirmwareUpdater*)fwUpdater transferError:(UCHAR)errorCode;

@end