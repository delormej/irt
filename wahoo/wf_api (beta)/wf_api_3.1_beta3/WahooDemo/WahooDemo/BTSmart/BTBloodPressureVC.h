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
//  BTBloodPressureVC.h
//  WahooDemo
//
//  Created by Michael Moore on 2/17/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "WFSensorCommonViewController.h"



@interface BTBloodPressureVC : WFSensorCommonViewController
{
    UILabel* inProgressLabel;
    UILabel* pressureLabel;
    UILabel* systolicLabel;
    UILabel* diastolicLabel;
    UILabel* meanPressureLabel;
    UILabel* heartRateLabel;
    UILabel* userIdLabel;
    UILabel* timestampLabel;
}


@property (readonly, nonatomic) WFBloodPressureConnection* bloodPressureConnection;
@property (nonatomic, retain) IBOutlet UILabel* inProgressLabel;
@property (nonatomic, retain) IBOutlet UILabel* pressureLabel;
@property (nonatomic, retain) IBOutlet UILabel* systolicLabel;
@property (nonatomic, retain) IBOutlet UILabel* diastolicLabel;
@property (nonatomic, retain) IBOutlet UILabel* meanPressureLabel;
@property (nonatomic, retain) IBOutlet UILabel* heartRateLabel;
@property (nonatomic, retain) IBOutlet UILabel* userIdLabel;
@property (nonatomic, retain) IBOutlet UILabel* timestampLabel;

@end
