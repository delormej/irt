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
//  BikePowerViewController.h
//  FisicaDemo
//
//  Created by Michael Moore on 3/25/10.
//  Copyright 2010 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "WFSensorCommonViewController.h"

@class WFBikePowerConnection;


@interface BikePowerViewController : WFSensorCommonViewController
{
	UILabel* eventCountLabel;
	UILabel* instantCadenceLabel;
	UILabel* accumulatedTorqueLabel;
	UILabel* instantPowerLabel;
	UILabel* speedLabel;
}


@property (readonly, nonatomic) WFBikePowerConnection* bikePowerConnection;
@property (retain, nonatomic) IBOutlet UILabel* eventCountLabel;
@property (retain, nonatomic) IBOutlet UILabel* instantCadenceLabel;
@property (retain, nonatomic) IBOutlet UILabel* accumulatedTorqueLabel;
@property (retain, nonatomic) IBOutlet UILabel* instantPowerLabel;
@property (retain, nonatomic) IBOutlet UILabel* speedLabel;
@property (retain, nonatomic) IBOutlet UILabel *speedUnitsLabel;

@property (retain, nonatomic) IBOutlet UILabel *pedalRightLabel;
@property (retain, nonatomic) IBOutlet UILabel *pedalLeftLabel;
@property (retain, nonatomic) IBOutlet UIProgressView *pedalPower;

- (IBAction)calibrateClicked:(id)sender;

@end
