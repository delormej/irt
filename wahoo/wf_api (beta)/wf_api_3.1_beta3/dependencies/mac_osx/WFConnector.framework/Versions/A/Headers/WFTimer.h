//
//  WFTimer.h
//  WFConnector
//
//  Created by Michael Moore on 6/29/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>


@class WFTimer;
@protocol WFTimerDelegate

- (void)timerDidExpire:(WFTimer*)wfTimer;

@end


@interface WFTimer : NSObject
{
    id <WFTimerDelegate> delegate;
    NSTimer* timer;
    NSTimeInterval timerInterval;
    BOOL doesRepeat;
}


// NOTE:  this is intentionall a non-retain reference.
@property (nonatomic, assign) id <WFTimerDelegate> delegate;


- (BOOL)cancel;
- (id)initWithDelegate:(id<WFTimerDelegate>)theDelegate interval:(NSTimeInterval)interval repeats:(BOOL)bRepeats;
- (BOOL)reset;
- (BOOL)start;

@end
