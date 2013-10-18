//
//  BrakeModeVC.h
//  KICKRDemo
//
//  Created by Michael Moore on 8/18/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "WFSensorCommonViewController.h"


@interface BrakeModeVC : WFSensorCommonViewController <WFBikeTrainerDelegate>
{
    UITextField* pwmModeLevelField;
    
    UILabel* powerLabel;
    UILabel* speedLabel;
}


@property (readonly, nonatomic) WFBikePowerConnection* bikePowerConnection;

@property (nonatomic, retain) IBOutlet UITextField* pwmModeLevelField;

@property (nonatomic, retain) IBOutlet UILabel* powerLabel;
@property (nonatomic, retain) IBOutlet UILabel* speedLabel;

- (IBAction)setPWMModeClicked:(id)sender;
- (IBAction)textFieldDoneEditing:(id)sender;

@end
