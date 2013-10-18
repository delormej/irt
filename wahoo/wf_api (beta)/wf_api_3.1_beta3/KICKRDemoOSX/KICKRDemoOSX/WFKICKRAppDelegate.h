//
//  WFKICKRAppDelegate.h
//  KICKRDemoOSX
//
//  Created by Barbara Wait on 1/23/13.
//  Copyright (c) 2013 Wahoo Fitness, LLC. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <IOBluetooth/IOBluetooth.h>
#import <WFConnector/WFConnector.h>

@interface WFKICKRAppDelegate : NSObject <NSApplicationDelegate, WFHardwareConnectorDelegate, WFSensorConnectionDelegate, WFBikeTrainerDelegate, WFBikePowerDelegate>
{
    NSWindow *window;
    
    WFBikePowerConnection *sensorConnection;
    
    // TODO: Debug
    WFBikeSpeedCadenceConnection *bikeSpeedCadenceConnection;
    WFHeartrateConnection *heartrateConnection;

    // for paired sensors saved in the Hardware Connector
    // array of WFDeviceParams
    NSArray *deviceParams; 

    // for sensors discovered by Hardware Connector discoverDevicesOfType: onNetwork:
    // array of WFDeviceParams pulled from the WFConnectionParams.device1 instance variable
    NSMutableArray *discoveredSensors;
    
    BOOL deviceInfoUpdated;
    BOOL isSearching;
    
    NSDate *requestDateKICKR;
    NSDate *requestDateHR;
    NSDate *requestDateSC;

    
    NSString *statusString;
    
    NSButton *wildcardCheckBox;
    NSButton *proximityCheckBox;
    NSButton *connectButton;
    NSTextField *deviceIdLabel;

    NSButton *enableBTLEButton;
    NSButton *enableANTButton;
    NSTextField *btConnectedLabel;
    NSTextField *dongleConnectedLabel;
    NSTextField *btleAvailableLabel;
    NSTextField *antAvailableLabel;
    
    NSTextField *signalLabel;
    NSTextField *connectionStatusLabel;
    NSTextField *powerLabel;
    NSTextField *speedLabel;
    NSTextField *currentModeLabel;
    NSTextField *standardLevelField;
    NSButton *standardModeSetButton;
    NSTextField *pwmModeLevelField;
    NSButton *pwmModeSetButton;
    NSTextField *hundredsField;
    NSTextField *tensField;
    NSTextField *onesField;
    NSButton *plusHundredsButton;
    NSButton *plusTensButton;
    NSButton *plusOnesButton;
    NSButton *minusHundredsButton;
    NSButton *minusTensButton;
    NSButton *minusOnesButton;
    NSTextField *simCrrInitField;
    NSTextField *simCInitField;
    NSTextField *weightField;
    NSTextField *simCField;
    NSTextField *wheelCircField;
    NSTextField *simGradeField;
    NSTextField *simWindSpeedField;
    NSButton *simModeSetButton;
    NSButton *simSetCButton;
    NSButton *setWheelCircButton;
    NSButton *simSetGradeButton;
    NSButton *simSetWindSpeedButton;
    NSButton *initSpindownButton;
    NSButton *manualZeroButton;
    NSTextField *simCrrField;
    NSButton *simSetCrrButton;
    NSTextField *ergModeLevelField;
    NSButton *ergModeSetButton;
    NSTextField *deviceNameLabel;
    NSTextField *manufacturerNameLabel;
    NSTextField *modelNumberLabel;
    NSTextField *serialNumberLabel;
    NSTextField *hardwareRevLabel;
    NSTextField *firmwareRevLabel;
    NSTextField *softwareRevLabel;
    NSTextField *systemIdLabel;
    NSButton *metricCheckBox;
    NSTextField *sampleRateText;
    NSTextField *wheelCircText;
    NSTextField *staleDataStringText;
    NSTextField *staleDataTimeText;
    NSTextField *coastingTimeText;
    NSTextField *searchTimeoutText;
    NSTextField *discoveryTimeoutText;
    NSButton *updateSettingsButton;
    NSTableView *discoveredSensorsTableView;
    NSButton *searchButton;
    NSTextField *previouslyPairedSensor1;
    NSTextField *previouslyPairedSensor2;
    NSTextField *previouslyPairedSensor3;
    NSTextField *previouslyPairedSensor4;
    NSButton *connectButton1;
    NSButton *connectButton2;
    NSButton *connectButton3;
    NSButton *connectButton4;
    NSButton *removeButton1;
    NSButton *removeButton2;
    NSButton *removeButton3;
    NSButton *removeButton4;
    NSButton *connectHRButton;
    NSButton *connectHRWildcardCheckBox;
    NSTextField *hearrateLabel;
    NSButton *connectSCButton;
    NSButton *connectSCWildcardCheckBox;
    NSTextField *scSpeedLabel;
    NSTextField *scCadenceLabel;
}

@property (assign) IBOutlet NSWindow *window;
@property (retain) WFBikePowerConnection *sensorConnection;

@property (retain) NSArray *deviceParams;
@property (retain) NSMutableArray *discoveredSensors;
@property (retain) NSString *statusString;

@property (retain) NSDate *requestDateKICKR;
@property (retain) NSDate *requestDateHR;
@property (retain) NSDate *requestDateSC;

@property (assign) IBOutlet NSButton *wildcardCheckBox;
@property (assign) IBOutlet NSButton *proximityCheckBox;
@property (assign) IBOutlet NSButton *connectButton;

@property (assign) IBOutlet NSTextField *btConnectedLabel;
@property (assign) IBOutlet NSTextField *dongleConnectedLabel;
@property (assign) IBOutlet NSTextField *btleAvailableLabel;
@property (assign) IBOutlet NSTextField *antAvailableLabel;
@property (assign) IBOutlet NSButton *enableBTLEButton;
@property (assign) IBOutlet NSButton *enableANTButton;

@property (assign) IBOutlet NSTextField *deviceIdLabel;
@property (assign) IBOutlet NSTextField *signalLabel;
@property (assign) IBOutlet NSTextField *connectionStatusLabel;

@property (assign) IBOutlet NSTextField *powerLabel;
@property (assign) IBOutlet NSTextField *speedLabel;
@property (assign) IBOutlet NSTextField *currentModeLabel;

// Standard Mode
@property (assign) IBOutlet NSTextField *standardLevelField;
@property (assign) IBOutlet NSButton *standardModeSetButton;

// PWM (Brake) Mode
@property (assign) IBOutlet NSTextField *pwmModeLevelField;
@property (assign) IBOutlet NSButton *pwmModeSetButton;

// Erg Mode
@property (assign) IBOutlet NSButton *ergModeSetButton;
@property (assign) IBOutlet NSTextField *ergModeLevelField;

// Erg mode alternate entry method 
@property (assign) IBOutlet NSTextField *hundredsField;
@property (assign) IBOutlet NSTextField *tensField;
@property (assign) IBOutlet NSTextField *onesField;
@property (assign) IBOutlet NSButton *plusHundredsButton;
@property (assign) IBOutlet NSButton *plusTensButton;
@property (assign) IBOutlet NSButton *plusOnesButton;
@property (assign) IBOutlet NSButton *minusHundredsButton;
@property (assign) IBOutlet NSButton *minusTensButton;
@property (assign) IBOutlet NSButton *minusOnesButton;

// Sim Mode
@property (assign) IBOutlet NSTextField *simCrrInitField;
@property (assign) IBOutlet NSTextField *simCInitField;
@property (assign) IBOutlet NSTextField *weightField;
@property (assign) IBOutlet NSTextField *simCrrField;
@property (assign) IBOutlet NSTextField *simCField;
@property (assign) IBOutlet NSTextField *wheelCircField;
@property (assign) IBOutlet NSTextField *simGradeField;
@property (assign) IBOutlet NSTextField *simWindSpeedField;
@property (assign) IBOutlet NSButton *simModeSetButton;
@property (assign) IBOutlet NSButton *simSetCrrButton;
@property (assign) IBOutlet NSButton *simSetCButton;
@property (assign) IBOutlet NSButton *setWheelCircButton;
@property (assign) IBOutlet NSButton *simSetGradeButton;
@property (assign) IBOutlet NSButton *simSetWindSpeedButton;

@property (assign) IBOutlet NSButton *initSpindownButton;
@property (assign) IBOutlet NSButton *manualZeroButton;

// Device Info
@property (assign) IBOutlet NSTextField *deviceNameLabel;
@property (assign) IBOutlet NSTextField *manufacturerNameLabel;
@property (assign) IBOutlet NSTextField *modelNumberLabel;
@property (assign) IBOutlet NSTextField *serialNumberLabel;
@property (assign) IBOutlet NSTextField *hardwareRevLabel;
@property (assign) IBOutlet NSTextField *firmwareRevLabel;
@property (assign) IBOutlet NSTextField *softwareRevLabel;
@property (assign) IBOutlet NSTextField *systemIdLabel;

// Settings
@property (assign) IBOutlet NSButton *metricCheckBox;
@property (assign) IBOutlet NSTextField *sampleRateText;
@property (assign) IBOutlet NSTextField *wheelCircText;
@property (assign) IBOutlet NSTextField *staleDataStringText;
@property (assign) IBOutlet NSTextField *staleDataTimeText;
@property (assign) IBOutlet NSTextField *coastingTimeText;
@property (assign) IBOutlet NSTextField *searchTimeoutText;
@property (assign) IBOutlet NSTextField *discoveryTimeoutText;
@property (assign) IBOutlet NSButton *updateSettingsButton;

// Manage Connections
@property (assign) IBOutlet NSTableView *discoveredSensorsTableView;
@property (assign) IBOutlet NSButton *searchButton;
@property (assign) IBOutlet NSTextField *previouslyPairedSensor1;
@property (assign) IBOutlet NSTextField *previouslyPairedSensor2;
@property (assign) IBOutlet NSTextField *previouslyPairedSensor3;
@property (assign) IBOutlet NSTextField *previouslyPairedSensor4;
@property (assign) IBOutlet NSButton *connectButton1;
@property (assign) IBOutlet NSButton *connectButton2;
@property (assign) IBOutlet NSButton *connectButton3;
@property (assign) IBOutlet NSButton *connectButton4;
@property (assign) IBOutlet NSButton *removeButton1;
@property (assign) IBOutlet NSButton *removeButton2;
@property (assign) IBOutlet NSButton *removeButton3;
@property (assign) IBOutlet NSButton *removeButton4;

// Connect Other Devices
@property (retain) WFHeartrateConnection *heartrateConnection;
@property (assign) IBOutlet NSButton *connectHRButton;
@property (assign) IBOutlet NSButton *connectHRWildcardCheckBox;
@property (assign) IBOutlet NSTextField *hearrateLabel;

@property (retain) WFBikeSpeedCadenceConnection *bikeSpeedCadenceConnection;
@property (assign) IBOutlet NSButton *connectSCButton;
@property (assign) IBOutlet NSButton *connectSCWildcardCheckBox;
@property (assign) IBOutlet NSTextField *scSpeedLabel;
@property (assign) IBOutlet NSTextField *scCadenceLabel;


// Connection
- (IBAction)enableBTLEClicked:(id)sender;
- (IBAction)enableANTClicked:(id)sender;

- (IBAction)connectButtonClicked:(id)sender;
- (IBAction)proximityCheckBoxClicked:(id)sender;
- (IBAction)wildcardCheckBoxClicked:(id)sender;

// Set Trainer Modes
- (IBAction)setStandardModeClicked:(id)sender;
- (IBAction)setPWMModeClicked:(id)sender;
- (IBAction)setSimModeClicked:(id)sender;
- (IBAction)setErgModeClicked:(id)sender;

// Additional Sim mode settings
- (IBAction)setCrrClicked:(id)sender;
- (IBAction)setCClicked:(id)sender;
- (IBAction)setWheelCircClicked:(id)sender;
- (IBAction)setGradeClicked:(id)sender;
- (IBAction)setWindSpeedClicked:(id)sender;

// Erg mode plus/minus buttons
- (IBAction)plusHundredsClicked:(id)sender;
- (IBAction)plusTensClicked:(id)sender;
- (IBAction)plusOnesClicked:(id)sender;
- (IBAction)minusHundredsClicked:(id)sender;
- (IBAction)minusTensClicked:(id)sender;
- (IBAction)minusOnesClicked:(id)sender;

// Spindown
- (IBAction)initSpindownClicked:(id)sender;

// Manual Zero
- (IBAction)manualZeroClicked:(id)sender;

// Settings
- (IBAction)setValuesClicked:(id)sender;

// Manage Connections
- (IBAction)searchClicked:(id)sender;
- (IBAction)connectToPreviouslyPairedSensor:(id)sender;
- (IBAction)removePreviouslyPairedSensor:(id)sender;

// Connect Other Devices
- (IBAction)connectHeartrateClicked:(id)sender;
- (IBAction)connectSpeedCadenceClicked:(id)sender;


@end
