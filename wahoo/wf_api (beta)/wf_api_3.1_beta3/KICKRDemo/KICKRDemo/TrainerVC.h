//
//  TrainerVC.h
//  KICKRDemo
//
//  Created by Michael Moore on 8/18/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "WFSensorCommonViewController.h"


@interface TrainerVC : WFSensorCommonViewController <WFBikeTrainerDelegate>
{
    UITextField* pwmModeLevelField;
    UITextField* standardLevelField;
    UITextField* ergWattsField;
    UITextField* weightField;
    UITextField* simCwField;
    UITextField* simCrrField;
    UITextField* simGradeField;
    
    UITextField* simWindSpeedField;
    UITextField* wheelCircField;
    
    UILabel* powerLabel;
    UILabel* speedLabel;
}


@property (readonly, nonatomic) WFBikePowerConnection* bikePowerConnection;

@property (nonatomic, retain) IBOutlet UITextField* pwmModeLevelField;
@property (nonatomic, retain) IBOutlet UITextField* standardLevelField;
@property (nonatomic, retain) IBOutlet UITextField* ergWattsField;
@property (nonatomic, retain) IBOutlet UITextField* weightField;
@property (nonatomic, retain) IBOutlet UITextField* simCwField;
@property (nonatomic, retain) IBOutlet UITextField* simCrrField;
@property (nonatomic, retain) IBOutlet UITextField* simGradeField;

@property (nonatomic, retain) IBOutlet UITextField* simWindSpeedField;
@property (nonatomic, retain) IBOutlet UITextField* wheelCircField;

@property (nonatomic, retain) IBOutlet UILabel* powerLabel;
@property (nonatomic, retain) IBOutlet UILabel* speedLabel;


- (IBAction)readModeClicked:(id)sender;
- (IBAction)initSpindownClicked:(id)sender;
- (IBAction)setPWMModeClicked:(id)sender;
- (IBAction)setStandardModeClicked:(id)sender;
- (IBAction)setErgModeClicked:(id)sender;
- (IBAction)setSimModeClicked:(id)sender;
- (IBAction)setCwClicked:(id)sender;
- (IBAction)setGradeClicked:(id)sender;
- (IBAction)setWheelCircClicked:(id)sender;
- (IBAction)setWindSpeedClicked:(id)sender;
- (IBAction)textFieldDoneEditing:(id)sender;

@end
