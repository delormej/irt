//
//  StandardModeVC.h
//  KICKRDemo
//
//  Created by Michael Moore on 8/18/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "WFSensorCommonViewController.h"


@interface StandardModeVC : WFSensorCommonViewController <WFBikeTrainerDelegate>
{
    UITextField* standardLevelField;
    
    UILabel* powerLabel;
    UILabel* speedLabel;
}


@property (readonly, nonatomic) WFBikePowerConnection* bikePowerConnection;

@property (nonatomic, retain) IBOutlet UITextField* standardLevelField;

@property (nonatomic, retain) IBOutlet UILabel* powerLabel;
@property (nonatomic, retain) IBOutlet UILabel* speedLabel;


- (IBAction)setStandardModeClicked:(id)sender;
- (IBAction)textFieldDoneEditing:(id)sender;

@end
