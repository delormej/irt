//{LEGAL_DISCLAIMER}
//
//  BikePowerViewController.h
//  KICKRDemo
//
//  Created by Michael Moore on 3/25/10.
//  Copyright 2010 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "WFSensorCommonViewController.h"

@class WFBikePowerConnection;


@interface BikePowerViewController : WFSensorCommonViewController
{
	UILabel* instantPowerLabel;
    UILabel* speedLabel;
}


@property (readonly, nonatomic) WFBikePowerConnection* bikePowerConnection;
@property (retain, nonatomic) IBOutlet UILabel* instantPowerLabel;
@property (retain, nonatomic) IBOutlet UILabel* speedLabel;

- (IBAction)settingsClicked:(id)sender;
- (IBAction)trainerClicked:(id)sender;
- (IBAction)ergModeClicked:(id)sender;

@end
