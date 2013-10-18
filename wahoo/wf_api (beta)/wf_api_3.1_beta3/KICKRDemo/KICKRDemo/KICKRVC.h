//{LEGAL_DISCLAIMER}
//
//  KICKRVC.h
//  KICKRDemo
//
//  Created by Michael Moore on 3/25/10.
//  Copyright 2010 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "WFSensorCommonViewController.h"

@class WFBikePowerConnection;


@interface KICKRVC : WFSensorCommonViewController <WFBikeTrainerDelegate, WFBikePowerDelegate>
{
	UILabel* instantPowerLabel;
    UILabel* speedLabel;
}


@property (readonly, nonatomic) WFBikePowerConnection* bikePowerConnection;
@property (retain, nonatomic) IBOutlet UILabel* instantPowerLabel;
@property (retain, nonatomic) IBOutlet UILabel* speedLabel;

- (IBAction)settingsClicked:(id)sender;
- (IBAction)initSpindownClicked:(id)sender;
- (IBAction)manualZeroClicked:(id)sender;
- (IBAction)standardModeClicked:(id)sender;
- (IBAction)brakeModeClicked:(id)sender;
- (IBAction)ergModeClicked:(id)sender;
- (IBAction)simModeClicked:(id)sender;

@end
