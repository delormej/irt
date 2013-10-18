//
//  WFFirmwareUpdater.h
//  WFConnector
//
//  Created by Michael Moore on 5/19/10.
//  Copyright 2010 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/WFFirmwareUpdaterDelegate.h>


@class WFHardwareController;
@class WFHexParser;


@interface WFFirmwareUpdater : NSObject
{
	id <WFFirmwareUpdaterDelegate> delegate;
	
	WFHardwareController* hardwareController;
	WFFirmwareVersionInfo_t currentVersionInfo;
	WFHexParser* hexParser;
	int currentIndex;
}


@property (nonatomic, readonly) WFFirmwareVersionInfo_t currentVersionInfo;
@property (nonatomic, assign) id <WFFirmwareUpdaterDelegate> delegate;


- (BOOL)getFirmwareInfo;
- (id)initWithHardwareController:(WFHardwareController*)hwController;
- (void)nextTransfer;
- (void)processAck:(UCHAR)status forCommand:(UCHAR)ackCommand;
- (void)processDeviceInfo:(UCHAR*)msgBuffer;
- (BOOL)startUpdate:(NSString*)hexFilePath;
- (WFFirmwareVersionInfo_t)getHexInfo:(NSString*)hexFilePath;

@end
