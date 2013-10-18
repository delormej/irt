//
//  ErgModeVC.h
//  KICKRDemo
//
//  Created by Michael Moore on 8/18/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "WFSensorCommonViewController.h"


@interface ErgModeVC : WFSensorCommonViewController <WFBikeTrainerDelegate>
{
    UITextField* hundredsField;
    UITextField* tensField;
    UITextField* onesField;
    
    UILabel* powerLabel;
    UILabel* speedLabel;
}


@property (readonly, nonatomic) WFBikePowerConnection* bikePowerConnection;

@property (nonatomic, retain) IBOutlet UITextField* hundredsField;
@property (nonatomic, retain) IBOutlet UITextField* tensField;
@property (nonatomic, retain) IBOutlet UITextField* onesField;

@property (nonatomic, retain) IBOutlet UILabel* powerLabel;
@property (nonatomic, retain) IBOutlet UILabel* speedLabel;

- (IBAction)plusHundredsClicked:(id)sender;
- (IBAction)plusTensClicked:(id)sender;
- (IBAction)plusOnesClicked:(id)sender;
- (IBAction)minusHundredsClicked:(id)sender;
- (IBAction)minusTensClicked:(id)sender;
- (IBAction)minusOnesClicked:(id)sender;
- (IBAction)textFieldDoneEditing:(id)sender;

- (void)setWatts;

@end
