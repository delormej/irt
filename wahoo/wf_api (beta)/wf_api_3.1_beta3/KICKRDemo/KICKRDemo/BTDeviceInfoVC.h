//{LEGAL_DISCLAIMER}
//
//  BTDeviceInfoVC.h
//  KICKRDemo
//
//  Created by Michael Moore on 2/28/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <WFConnector/WFConnector.h>


@interface BTDeviceInfoVC : UIViewController
{
    WFBTLECommonData* commonData;
    WFSensorConnection* sensorConnection;
    
    UILabel* deviceNameLabel;
    UILabel* manufacturerNameLabel;
    UILabel* modelNumberLabel;
    UILabel* serialNumberLabel;
    UILabel* hardwareRevLabel;
    UILabel* firmwareRevLabel;
    UILabel* softwareRevLabel;
    UILabel* systemIdLabel;
    UILabel* battLevelLabel;
    UILabel* battPresentLabel;
    UILabel* battDischargeLabel;
    UILabel* battChargeLabel;
    UILabel* battCriticalLabel;
}


@property (nonatomic, retain) WFBTLECommonData* commonData;
@property (nonatomic, retain) WFSensorConnection* sensorConnection;
@property (nonatomic, retain) IBOutlet UILabel* deviceNameLabel;
@property (nonatomic, retain) IBOutlet UILabel* manufacturerNameLabel;
@property (nonatomic, retain) IBOutlet UILabel* modelNumberLabel;
@property (nonatomic, retain) IBOutlet UILabel* serialNumberLabel;
@property (nonatomic, retain) IBOutlet UILabel* hardwareRevLabel;
@property (nonatomic, retain) IBOutlet UILabel* firmwareRevLabel;
@property (nonatomic, retain) IBOutlet UILabel* softwareRevLabel;
@property (nonatomic, retain) IBOutlet UILabel* systemIdLabel;
@property (nonatomic, retain) IBOutlet UILabel* battLevelLabel;
@property (nonatomic, retain) IBOutlet UILabel* battPresentLabel;
@property (nonatomic, retain) IBOutlet UILabel* battDischargeLabel;
@property (nonatomic, retain) IBOutlet UILabel* battChargeLabel;
@property (nonatomic, retain) IBOutlet UILabel* battCriticalLabel;

- (void)updateDisplay;

@end
