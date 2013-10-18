//
//  _WFAudioControlConnection.h
//  WFConnector
//
//  Created by Michael Moore on 3/18/11.
//  Copyright 2011 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/WFSensorConnection.h>
#import <WFConnector/WFAudioControlDelegate.h>


@interface _WFAudioControlConnection : WFSensorConnection <WFAudioControlDelegate>
{
}


@property (nonatomic, retain) id <WFAudioControlDelegate> acDelegate;

@end
