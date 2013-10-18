//
//  WFBTLEFirmwareUpdateConnection.h
//  WFConnector
//
//  Created by Michael Moore on 5/30/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/WFSensorConnection.h>
#import <WFConnector/WFBTLEGlucoseDelegate.h>
#import <WFConnector/wf_btle_types.h>


@class WFBTLEGlucoseData;


/**
 * Represents a connection to a Wahoo BTLE Firmware Update service.
 */
@interface WFBTLEFirmwareUpdateConnection : WFSensorConnection
{
	/** \cond InterfaceDocs */
    
    id<WFBTLEGlucoseDelegate> glucoseDelegate;
    
	/** \endcond */
}


- (BOOL)updateFirmware;

@end
