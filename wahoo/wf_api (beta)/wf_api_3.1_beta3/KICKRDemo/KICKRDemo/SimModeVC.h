//
//  SimModeVC.h
//  KICKRDemo
//
//  Created by Michael Moore on 8/18/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "WFSensorCommonViewController.h"


@interface SimModeVC : WFSensorCommonViewController <WFBikeTrainerDelegate>
{
    UITextField* weightField;
    UITextField* simCField;
    UITextField* simCInitField;
    UITextField* simCrrField;
    UITextField* simGradeField;
    UITextField* simWindSpeedField;
    UITextField* wheelCircField;
    
    UILabel* powerLabel;
    UILabel* speedLabel;
}


@property (readonly, nonatomic) WFBikePowerConnection* bikePowerConnection;

@property (nonatomic, retain) IBOutlet UITextField* weightField;
@property (nonatomic, retain) IBOutlet UITextField* simCField;
@property (nonatomic, retain) IBOutlet UITextField* simCInitField;
@property (nonatomic, retain) IBOutlet UITextField* simCrrField;
@property (nonatomic, retain) IBOutlet UITextField* simGradeField;
@property (nonatomic, retain) IBOutlet UITextField* simWindSpeedField;
@property (nonatomic, retain) IBOutlet UITextField* wheelCircField;

@property (nonatomic, retain) IBOutlet UILabel* powerLabel;
@property (nonatomic, retain) IBOutlet UILabel* speedLabel;


- (IBAction)setSimModeClicked:(id)sender;
- (IBAction)setCClicked:(id)sender;
- (IBAction)setGradeClicked:(id)sender;
- (IBAction)setWheelCircClicked:(id)sender;
- (IBAction)setWindSpeedClicked:(id)sender;
- (IBAction)textFieldDoneEditing:(id)sender;

@end
