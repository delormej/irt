//{LEGAL_DISCLAIMER}
//
//  WFSensorCommonViewController.h
//  KICKRDemo
//
//  Created by Michael Moore on 2/23/10.
//  Copyright 2010 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <WFConnector/WFConnector.h>
#import "SensorManagerViewController.h"


@class BTDeviceInfoVC;


@interface WFSensorCommonViewController : UIViewController <WFSensorConnectionDelegate, SensorManagerDelegate>
{
	WFHardwareConnector* hardwareConnector;
	WFSensorConnection* sensorConnection;
	WFSensorType_t sensorType;
	UILabel* deviceIdLabel;
	UILabel* signalEfficiencyLabel;
	
	UIButton* connectButton;
	UISwitch* wildcardSwitch;
    UISwitch* proximitySwitch;
    
    BTDeviceInfoVC* btDeviceInfo;
    
    USHORT applicableNetworks;
}


@property (assign, nonatomic) USHORT applicableNetworks;
@property (retain, nonatomic) WFSensorConnection* sensorConnection;
@property (retain, nonatomic) IBOutlet UILabel* signalEfficiencyLabel;
@property (retain, nonatomic) IBOutlet UILabel* deviceIdLabel;

@property (retain, nonatomic) IBOutlet UIButton* connectButton;
@property (retain, nonatomic) IBOutlet UISwitch* wildcardSwitch;
@property (retain, nonatomic) IBOutlet UISwitch* proximitySwitch;


- (IBAction)connectSensorClicked:(id)sender;
- (IBAction)deviceInfoClicked:(id)sender;
- (IBAction)manageClicked:(id)sender;
- (IBAction)proximityToggled:(id)sender;
- (IBAction)textFieldDoneEditing:(id)sender;
- (IBAction)wildcardToggled:(id)sender;

- (void)onSensorConnected:(WFSensorConnection*)connectionInfo;
- (void)updateData;
- (void)firmwareUpdateAvailable:(NSNotification*)notification;

- (void)resetDisplay;


+ (NSString*)formatUUIDString:(NSString*)uuid;
+ (NSString*)signalStrengthFromConnection:(WFSensorConnection*)conn;
+ (NSString*)stringFromSensorType:(WFSensorType_t)sensorType;

@end
