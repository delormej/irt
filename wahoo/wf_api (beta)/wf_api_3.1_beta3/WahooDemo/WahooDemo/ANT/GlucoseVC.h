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
//  GlucoseVC.h
//  WahooDemo
//
//  Created by Michael Moore on 2/23/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "WFSensorCommonViewController.h"


@interface GlucoseVC : WFSensorCommonViewController <WFGlucoseDelegate>
{
    uint8_t auc_tx_id[5];
    uint8_t auc_perm_key[4];
    
    UITextField* permissionKeyField;
    UITextField* txIdField;
    
	UILabel* concentrationLabel;
	UILabel* changeRateLabel;
	UILabel* timestampLabel;
	UILabel* alertHighLabel;
    UILabel* alertLowLabel;
    UILabel* alertRisingLabel;
    UILabel* alertFallingLabel;
    UILabel* alertBelow55Label;
    
    UILabel* deviceTimeLabel;
    UILabel* elapsedTimeLabel;
}


@property (readonly, nonatomic) WFGlucoseConnection* glucoseConnection;

@property (nonatomic, retain) IBOutlet UITextField* permissionKeyField;
@property (nonatomic, retain) IBOutlet UITextField* txIdField;

@property (nonatomic, retain) IBOutlet UILabel* concentrationLabel;
@property (nonatomic, retain) IBOutlet UILabel* changeRateLabel;
@property (nonatomic, retain) IBOutlet UILabel* timestampLabel;
@property (nonatomic, retain) IBOutlet UILabel* alertHighLabel;
@property (nonatomic, retain) IBOutlet UILabel* alertLowLabel;
@property (nonatomic, retain) IBOutlet UILabel* alertRisingLabel;
@property (nonatomic, retain) IBOutlet UILabel* alertFallingLabel;
@property (nonatomic, retain) IBOutlet UILabel* alertBelow55Label;

@property (nonatomic, retain) IBOutlet UILabel* deviceTimeLabel;
@property (nonatomic, retain) IBOutlet UILabel* elapsedTimeLabel;


@end
