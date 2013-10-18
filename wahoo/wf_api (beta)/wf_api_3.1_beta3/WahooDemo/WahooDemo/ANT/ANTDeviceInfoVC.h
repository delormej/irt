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
//  ANTDeviceInfoVC.h
//  WahooDemo
//
//  Created by Michael Moore on 2/28/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <WFConnector/WFConnector.h>


@interface ANTDeviceInfoVC : UIViewController
{
    WFCommonData* commonData;
    
    UILabel* manufacturerIdLabel;
    UILabel* modelNumberLabel;
    UILabel* hardwareVerLabel;
    UILabel* softwareVerLabel;
    UILabel* serialUpperLabel;
    UILabel* serialLowerLabel;
    UILabel* serialNumberLabel;
    UILabel* operatingTimeLabel;
    UILabel* battVoltageLabel;
    UILabel* battStatusLabel;
}


@property (nonatomic, retain) WFCommonData* commonData;
@property (nonatomic, retain) IBOutlet UILabel* manufacturerIdLabel;
@property (nonatomic, retain) IBOutlet UILabel* modelNumberLabel;
@property (nonatomic, retain) IBOutlet UILabel* hardwareVerLabel;
@property (nonatomic, retain) IBOutlet UILabel* softwareVerLabel;
@property (nonatomic, retain) IBOutlet UILabel* serialUpperLabel;
@property (nonatomic, retain) IBOutlet UILabel* serialLowerLabel;
@property (nonatomic, retain) IBOutlet UILabel* serialNumberLabel;
@property (nonatomic, retain) IBOutlet UILabel* operatingTimeLabel;
@property (nonatomic, retain) IBOutlet UILabel* battVoltageLabel;
@property (nonatomic, retain) IBOutlet UILabel* battStatusLabel;


- (void)updateDisplay;

@end
