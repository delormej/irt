///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Wahoo Fitness. All Rights Reserved.
//
// The information contained herein is property of Wahoo Fitness LLC.
// Terms and conditions of usage are described in detail in the
// WAHOO FITNESS API LICENSE AGREEMENT.
//
// Licensees are granted free, non-transferable use of the information.
// NO WARRANTY of ANY KIND is provided.
// This heading must NOT be removed from the file.
///////////////////////////////////////////////////////////////////////////////
//
//  AntViewController.h
//  FisicaDemo
//
//  Created by Michael Moore on 4/7/10.
//  Copyright 2010 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <WFConnector/WFAdvancedANT.h>


@interface AntViewController : UIViewController <WFAntReceiverDelegate>
{
	WFHardwareConnector* hardwareConnector;

	UILabel* keyConnectedLabel;
	UITextField* txMessageField;
	UITextView* rxMessageView;
}


@property (nonatomic, retain) IBOutlet UILabel* keyConnectedLabel;
@property (nonatomic, retain) IBOutlet UITextField* txMessageField;
@property (nonatomic, retain) IBOutlet UITextView* rxMessageView;


- (IBAction)chooseClicked:(id)sender;
- (IBAction)clearClicked:(id)sender;
- (IBAction)sendClicked:(id)sender;
- (IBAction)textFieldDoneEditing:(id)sender;

@end
