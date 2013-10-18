//{LEGAL_DISCLAIMER}
//
//  SettingsViewController.h
//  WahooDemo
//
//  Created by Michael Moore on 11/11/11.
//  Copyright (c) 2011 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <MessageUI/MFMailComposeViewController.h>


@interface SettingsViewController : UIViewController <MFMailComposeViewControllerDelegate>
{
    UILabel* dongleConnectedLabel;
    UILabel* btConnectedLabel;
	UITextField* sampleRateText;
    UITextField* staleDataStringText;
    UITextField* staleDataTimeText;
    UITextField* coastingTimeText;
    UITextField* wheelCircText;
    UITextField* searchTimeout;
    UITextField* discoveryTimeout;
    UISwitch* metricSwitch;
}


@property (retain, nonatomic) IBOutlet UILabel* dongleConnectedLabel;
@property (retain, nonatomic) IBOutlet UILabel* btConnectedLabel;
@property (retain, nonatomic) IBOutlet UITextField* sampleRateText;
@property (retain, nonatomic) IBOutlet UITextField* staleDataStringText;
@property (retain, nonatomic) IBOutlet UITextField* staleDataTimeText;
@property (retain, nonatomic) IBOutlet UITextField* coastingTimeText;
@property (retain, nonatomic) IBOutlet UITextField* wheelCircText;
@property (retain, nonatomic) IBOutlet UITextField* searchTimeout;
@property (retain, nonatomic) IBOutlet UITextField* discoveryTimeout;
@property (retain, nonatomic) IBOutlet UISwitch* metricSwitch;
   

- (IBAction)emailLogClicked:(id)sender;
- (IBAction)setValuesClicked:(id)sender;
- (IBAction)textFieldDoneEditing:(id)sender;

@end
