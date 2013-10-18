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
//  FootpodViewController.h
//  FisicaDemo
//
//  Created by Michael Moore on 2/23/10.
//  Copyright 2010 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "WFSensorCommonViewController.h"

@class WFFootpodConnection;


@interface FootpodViewController : WFSensorCommonViewController
{
	UILabel* lastTimeLabel;
	UILabel* distanceLabel;
	UILabel* instantaneousSpeedLabel;
	UILabel* strideCountLabel;
	UILabel* latencyLabel;
	UILabel* cadenceLabel;
	UILabel* moduleLocationLabel;
	UILabel* unitHealthLabel;
	UILabel* useStateLabel;
	UILabel* accumulatedDistanceLabel;
	UILabel* accumulatedStrideLabel;
}

@property (readonly, nonatomic) WFFootpodConnection* footpodConnection;
@property (retain, nonatomic) IBOutlet UILabel* lastTimeLabel;
@property (retain, nonatomic) IBOutlet UILabel* distanceLabel;
@property (retain, nonatomic) IBOutlet UILabel* instantaneousSpeedLabel;
@property (retain, nonatomic) IBOutlet UILabel* strideCountLabel;
@property (retain, nonatomic) IBOutlet UILabel* latencyLabel;
@property (retain, nonatomic) IBOutlet UILabel* cadenceLabel;
@property (retain, nonatomic) IBOutlet UILabel* moduleLocationLabel;
@property (retain, nonatomic) IBOutlet UILabel* unitHealthLabel;
@property (retain, nonatomic) IBOutlet UILabel* useStateLabel;
@property (retain, nonatomic) IBOutlet UILabel* accumulatedDistanceLabel;
@property (retain, nonatomic) IBOutlet UILabel* accumulatedStrideLabel;

@end
