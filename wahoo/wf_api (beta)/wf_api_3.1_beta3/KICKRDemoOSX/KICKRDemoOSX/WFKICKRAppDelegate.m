//
//  WFKICKRAppDelegate.m
//  KICKRDemoOSX
//
//  Created by Barbara Wait on 1/23/13.
//  Copyright (c) 2013 Wahoo Fitness, LLC. All rights reserved.
//

#import "WFKICKRAppDelegate.h"

typedef enum
{
    WF_CPM_CAL_STATUS_SUCCESS           = 0x00,
    WF_CPM_CAL_STATUS_FAILED            = 0x01,
    
} wf_cpm_cal_status_t;


@implementation WFKICKRAppDelegate

@synthesize window;
@synthesize sensorConnection;

@synthesize heartrateConnection;
@synthesize bikeSpeedCadenceConnection;

@synthesize requestDateKICKR;
@synthesize requestDateSC;
@synthesize requestDateHR;

@synthesize statusString;
@synthesize deviceParams;
@synthesize discoveredSensors;

@synthesize discoveredSensorsTableView;
@synthesize searchButton;
@synthesize previouslyPairedSensor1;
@synthesize previouslyPairedSensor2;
@synthesize previouslyPairedSensor3;
@synthesize previouslyPairedSensor4;
@synthesize connectButton1;
@synthesize connectButton2;
@synthesize connectButton3;
@synthesize connectButton4;
@synthesize removeButton1;
@synthesize removeButton2;
@synthesize removeButton3;
@synthesize removeButton4;
@synthesize metricCheckBox;
@synthesize sampleRateText;
@synthesize wheelCircText;
@synthesize staleDataStringText;
@synthesize staleDataTimeText;
@synthesize coastingTimeText;
@synthesize searchTimeoutText;
@synthesize discoveryTimeoutText;
@synthesize updateSettingsButton;
@synthesize deviceNameLabel;
@synthesize manufacturerNameLabel;
@synthesize modelNumberLabel;
@synthesize serialNumberLabel;
@synthesize hardwareRevLabel;
@synthesize firmwareRevLabel;
@synthesize softwareRevLabel;
@synthesize systemIdLabel;

@synthesize simSetCrrButton;
@synthesize simCrrInitField;
@synthesize simCInitField;
@synthesize weightField;
@synthesize simCField;
@synthesize wheelCircField;
@synthesize simGradeField;
@synthesize simWindSpeedField;
@synthesize simModeSetButton;
@synthesize simSetCButton;
@synthesize setWheelCircButton;
@synthesize simSetGradeButton;
@synthesize simSetWindSpeedButton;
@synthesize initSpindownButton;
@synthesize manualZeroButton;
@synthesize simCrrField;
@synthesize ergModeSetButton;
@synthesize ergModeLevelField;
@synthesize hundredsField;
@synthesize tensField;
@synthesize onesField;
@synthesize plusHundredsButton;
@synthesize plusTensButton;
@synthesize plusOnesButton;
@synthesize minusHundredsButton;
@synthesize minusTensButton;
@synthesize minusOnesButton;
@synthesize pwmModeLevelField;
@synthesize pwmModeSetButton;
@synthesize standardLevelField;
@synthesize standardModeSetButton;
@synthesize connectionStatusLabel;
@synthesize powerLabel;
@synthesize speedLabel;
@synthesize currentModeLabel;

@synthesize wildcardCheckBox;
@synthesize proximityCheckBox;
@synthesize connectButton;

@synthesize btConnectedLabel;
@synthesize dongleConnectedLabel;
@synthesize btleAvailableLabel;
@synthesize antAvailableLabel;
@synthesize enableANTButton;
@synthesize enableBTLEButton;

@synthesize deviceIdLabel;
@synthesize signalLabel;

@synthesize connectHRButton;
@synthesize connectHRWildcardCheckBox;
@synthesize hearrateLabel;
@synthesize connectSCButton;
@synthesize connectSCWildcardCheckBox;
@synthesize scSpeedLabel;
@synthesize scCadenceLabel;

#pragma mark - Application Delegate Implementation
//--------------------------------------------------------------------------------
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // NOTE: To use the Wahoo Fitness API, you will need to set up the following:
    //
    // Add the following two standard frameworks:
    // - IOKit.framework
    // - IOBluetooth.framework
    //
    // Add the Wahoo Fitness WFConnector.framework
    //
    // Ensure the the following are set in your Build Settings:
    // Other Linker Flags: -all_load -lstdc++
    //
    //

    // CONNECTING:
    // NOTE: To do an automatic connection without a button, the call can be placed in the delegate method
    // - (void)hardwareConnector:(WFHardwareConnector*)hwConnector stateChanged:(WFHardwareConnectorState_t)currentState
    //
    // This method will get called as a response to the [hardwareConnector enableBTLE: TRUE] call above
    // with the state of WF_HWCONN_STATE_BT40_ENABLED.  When this state is received, the connector is ready for
    // a bluetooth connection to be established.
    
    // NOTE: For connecting, the wildcard switch and proximity switch for connection are optional.
    // The following call will create parameters for connection to any device of the specified sensor type that has already connected before,
    // or it will return parameters suitable for a wildcard search. It is recommended that you read the API documentation on
    // this method.
    //
    // WFConnectionParams* params = [hardwareConnector.settings connectionParamsForSensorType:sensorType];
    //
    //Create the new sensor connection
    //    WFSensorConnection* sensorConnection;
    //    sensorConnection = [hardwareConnector requestSensorConnection:params];
    // This sensorConnection will need to be cast to the specific subclass for the sensorType requested, e.g.
    //    self.bikePowerConnection = (WFBikePowerConnection *)sensorConnection;
    
    // NOTE: The callback from requestSensorConnection is
    //  - (void)connection:(WFSensorConnection*)connectionInfo stateChanged:(WFSensorConnectionStatus_t)connState
    // save the connection info and set any additional delegates required by the sensor type when this callback is received
    
    // NOTE: Once the connectionInfo has been saved in the hardware connector, it will return parameters for the
    // specific device just connected.  It will no longer return parameters that will perform a wildcard search.
    // To forget a device and return to a wildcard search for connection, remove the device from settings
    //    NSArray *deviceParams = [hardwareConnector.settings deviceParamsForSensorType:sensorType];
    //    for ( WFDeviceParams *device in deviceParams)
    //    {
    //        // remove from settings
    //        BOOL removed = [hardwareConnector.settings removeDeviceParams:device forSensorType:sensorType];
    //    }

    // set up the hardware connector
    WFHardwareConnector* hardwareConnector = [WFHardwareConnector sharedConnector];
    hardwareConnector.delegate = self;
	hardwareConnector.sampleRate = 0.1;  // in seconds, 100 ms or 10Hz
    
    // determine support for BTLE and ANT
    // NOTE: Currently the OSX implementation will always return YES for hasBTLESupport
    [self.btleAvailableLabel setStringValue: hardwareConnector.hasBTLESupport ? @"Yes*" : @"No"];
    [self.btConnectedLabel setStringValue: hardwareConnector.isBTLEEnabled ? @"Yes" : @"No"];
    if (hardwareConnector.hasBTLESupport == NO)
    {
        [self.enableBTLEButton setEnabled: NO];
    }
    
	[self.dongleConnectedLabel setStringValue: @"No"];

    // the ANT stick is not a "Fisica" 
//    if (hardwareConnector.isFisicaConnected == NO)
//    {
//        [self.enableANTButton setEnabled:NO];
//    }
    
    // set HW Connector to call hasData only when new data is available.
    [hardwareConnector setSampleTimerDataCheck:YES];
        
    // set the wildcard and proximity check boxes based on whether the hardware connector
    // has device params or not. The hardware connector has params if it has connected to a device before.
    // If you want to connect to a different device than you have connected to before, set the wildcard checkbox to on.
    // If you have wildcard on, and you want to connect to only the closest device, set proximity checkbox to on.
    WFConnectionParams* params = [[WFHardwareConnector sharedConnector].settings connectionParamsForSensorType:WF_SENSORTYPE_BIKE_POWER];
    
    if (!params | [params isWildcard])
    {
        self.wildcardCheckBox.state = NSOnState;
        self.proximityCheckBox.state = NSOffState;
    }
    else
    {
        self.wildcardCheckBox.state = NSOffState;
        self.proximityCheckBox.state = NSOffState;
    }
    
    // initialize the device list for manage connections
    [self initializeDeviceParams];
    [self resetSearchState];
    self.discoveredSensors = [[NSMutableArray alloc] initWithCapacity:3];
    [self.discoveredSensorsTableView setTarget:self];
    [self.discoveredSensorsTableView setDoubleAction:@selector(doNothingOnDoubleClick:)];
    
    // initialize the display
    deviceInfoUpdated = NO;
    [self resetDisplay];
    [self initializeSettings];
    
    // Turn off the buttons that require a connection
    [self disableTrainerButtons];
    
    // Connect Other Devices
    [self resetHeartrateDisplay];
    [self resetSpeedCadenceDisplay];
    WFConnectionParams *hrParams = [[WFHardwareConnector sharedConnector].settings connectionParamsForSensorType:WF_SENSORTYPE_HEARTRATE];
    if (!hrParams | [hrParams isWildcard])
    {
        self.connectHRWildcardCheckBox.state = NSOnState;
    }
    else
    {
        self.connectHRWildcardCheckBox.state = NSOffState;
    }
    
    WFConnectionParams *scParams = [[WFHardwareConnector sharedConnector].settings connectionParamsForSensorType:WF_SENSORTYPE_BIKE_SPEED_CADENCE];
    if (!scParams | [scParams isWildcard])
    {
        self.connectSCWildcardCheckBox.state = NSOnState;
    }
    else
    {
        self.connectSCWildcardCheckBox.state = NSOffState;
    }
    
}

#pragma mark - Connect KICKR
//--------------------------------------------------------------------------------
- (IBAction)enableBTLEClicked:(id)sender
{
    WFHardwareConnector* hardwareConnector = [WFHardwareConnector sharedConnector];
    if (hardwareConnector.isBTLEEnabled == NO)
    {
        // enable BTLE.
        [hardwareConnector enableBTLE:TRUE];
    }
    else
    {
        // disable BTLE
        [hardwareConnector enableBTLE:FALSE];
    }
}

//--------------------------------------------------------------------------------
- (IBAction)enableANTClicked:(id)sender
{
    // initiate an ANT USB session.
    WFHardwareConnector* hardwareConnector = [WFHardwareConnector sharedConnector];
//    [hardwareConnector openUSBSession:57600];
    BOOL retVal = [hardwareConnector openUSBSession:57600];
    if (retVal == YES)
    {
        [self.dongleConnectedLabel setStringValue:@"Yes"];
        [self.enableANTButton setEnabled:NO];
    }
    else
    {
        [self showAlertPanelWithTitle:@"No ANT connection" message:@"Unable to establish an ANT connection."];
    }
}

//--------------------------------------------------------------------------------
- (IBAction)connectButtonClicked:(id)sender
{
    NSLog(@"connectButtonClicked:");
    
    // toggles between connecting and disconnecting
    
    // get the current connection status.
    WFSensorConnectionStatus_t connState = WF_SENSOR_CONNECTION_STATUS_IDLE;
    
    if (self.sensorConnection != nil )
    {
        connState = self.sensorConnection.connectionStatus;
    }
    
    
    //Debug: simulate all three connecting/disconnecting at the same time
//    [self connectHeartrateClicked:nil];
//    [self connectSpeedCadenceClicked:nil];
    
    
    switch (connState)
    {
        case WF_SENSOR_CONNECTION_STATUS_IDLE:
            // connect the sensor.
            [self connectSensor];
            break;
            
        case WF_SENSOR_CONNECTION_STATUS_CONNECTING:
        case WF_SENSOR_CONNECTION_STATUS_CONNECTED:
            // disconnect the sensor.
            [self disconnectSensor];
            break;
            
        case WF_SENSOR_CONNECTION_STATUS_DISCONNECTING:
        case WF_SENSOR_CONNECTION_STATUS_INTERRUPTED:
            // do nothing.
            break;
    }
}

//--------------------------------------------------------------------------------
- (IBAction)proximityCheckBoxClicked:(id)sender
{
    // Proximity requires wildcard to be on, so if proximity is on, make sure wildcard is turned on
    
    BOOL proximityState = self.proximityCheckBox.state == NSOnState ? YES : NO;
    BOOL wildcardState = self.wildcardCheckBox.state == NSOnState ? YES : NO;
    NSString *proximityStateString = self.proximityCheckBox.state == NSOnState ? @"YES" : @"NO";
    NSString *wildcardStateString = self.wildcardCheckBox.state == NSOnState ? @"YES" : @"NO";
    
    NSLog(@"proximity = %@, wildcard = %@", proximityStateString, wildcardStateString);
    
    if ( proximityState == YES && wildcardState == NO)
    {
        self.wildcardCheckBox.state = NSOnState;
    }
}

//--------------------------------------------------------------------------------
- (IBAction)wildcardCheckBoxClicked:(id)sender
{
    // Proximity requires wildcard to be on, so if wildcard is turned off, make sure proximity is turned off

    BOOL proximityState = self.proximityCheckBox.state == NSOnState ? YES : NO;
    BOOL wildcardState = self.wildcardCheckBox.state == NSOnState ? YES : NO;
    NSString *proximityStateString = self.proximityCheckBox.state == NSOnState ? @"YES" : @"NO";
    NSString *wildcardStateString = self.wildcardCheckBox.state == NSOnState ? @"YES" : @"NO";
    
    NSLog(@"proximity = %@, wildcard = %@", proximityStateString, wildcardStateString);
    
    if ( proximityState == YES && wildcardState == NO)
    {
        self.proximityCheckBox.state = NSOffState;
    }
}

#pragma mark - Sensor Connection Helpers / Events

//--------------------------------------------------------------------------------
- (void) connectSensor
{
    NSLog(@"connectSensor");
    
    // call to create the connection params.
    WFConnectionParams* params = [self connectionParams];
    WFSensorConnection* connection;
    
    //Create the new sensor connection
    if ( params != nil)
    {
        if([params isWildcard])
        {
            if(self.proximityCheckBox.state == NSOnState)
            {
                
                connection = [[WFHardwareConnector sharedConnector] requestSensorConnection:params withProximity:WF_PROXIMITY_RANGE_6 ];
                self.requestDateKICKR = [NSDate date];
            }
            else
            {
                connection = [[WFHardwareConnector sharedConnector] requestSensorConnection:params];
                self.requestDateKICKR = [NSDate date];
            }
        }
        else
        {
            connection = [[WFHardwareConnector sharedConnector] requestSensorConnection:params];
            self.requestDateKICKR = [NSDate date];
        }
        
        if ([connection isKindOfClass: [WFBikePowerConnection class]])
        {
            self.sensorConnection = (WFBikePowerConnection *)connection;
        }
        
        self.sensorConnection.delegate = self;
    }
    
    // update the display.
    [self updateConnectState];
    
}

//--------------------------------------------------------------------------------
- (WFConnectionParams*) connectionParams
{
    // create the connection params.
    WFConnectionParams* params = nil;
    
    // if wildcard search is specified, create empty connection params.
    if ( self.wildcardCheckBox.state == NSOnState )
    {
        params = [[[WFConnectionParams alloc] init] autorelease];
        params.sensorType = WF_SENSORTYPE_BIKE_POWER;
        params.sensorSubType = WF_SENSOR_SUBTYPE_BIKE_POWER_KICKR;
        params.networkType = WF_NETWORKTYPE_BTLE | WF_NETWORKTYPE_ANTPLUS;
    }
    // otherwise, get the params from the stored settings.
    else
    {
        params = [[WFHardwareConnector sharedConnector].settings connectionParamsForSensorType:WF_SENSORTYPE_BIKE_POWER];
    }
    
    if ( params != nil)
    {
        // set the search timeout.
        params.searchTimeout = [WFHardwareConnector sharedConnector].settings.searchTimeout;
    }
    
    return params;
}

//--------------------------------------------------------------------------------
- (void)requestConnectionToDevice:(WFDeviceParams*)devParams
{
    // this connection method is called when the user selects a specific device from Manage Connections
    // don't allow a connection if a device is already connected
    
    // get the current connection status.
    WFSensorConnectionStatus_t connState = WF_SENSOR_CONNECTION_STATUS_IDLE;
    
    if (self.sensorConnection != nil )
    {
        connState = self.sensorConnection.connectionStatus;
    }
    
    if (connState == WF_SENSOR_CONNECTION_STATUS_IDLE)
    {
        // configure the connection params for the specified device
        WFConnectionParams* params = [[[WFConnectionParams alloc] init] autorelease];
        params.sensorType = WF_SENSORTYPE_BIKE_POWER;
        params.sensorSubType = WF_SENSOR_SUBTYPE_BIKE_POWER_KICKR;
        params.device1 = devParams;
        
        // request the sensor connection.
        WFSensorConnection* connection = [[WFHardwareConnector sharedConnector] requestSensorConnection:params];
        self.requestDateKICKR = [NSDate date];
        
        if ([connection isKindOfClass: [WFBikePowerConnection class]])
        {
            self.sensorConnection = (WFBikePowerConnection *)connection;
        }
        
        self.sensorConnection.delegate = self;
        
        // update the display.
        [self updateConnectState];
    }
}



//--------------------------------------------------------------------------------
- (void) disconnectSensor
{
    NSLog(@"disconnectSensor:");
    
    [self.sensorConnection disconnect];
    
    // update the display.
    [self updateConnectState];
}

//--------------------------------------------------------------------------------
- (void) onSensorConnected:(WFSensorConnection*) connectionInfo
{
    // called from connection: stateChanged:
    
    NSLog(@"onSensorConnected:");
    
    //Save the connection settings
    [[WFHardwareConnector sharedConnector].settings saveConnectionInfo:connectionInfo];
    
    if ([connectionInfo isKindOfClass: [WFBikePowerConnection class]])
    {
        NSLog(@"================ time to KICKR connection %f", [self.requestDateKICKR timeIntervalSinceNow]);
        // set the delegates
        sensorConnection.btDelegate = self;
        sensorConnection.cpmDelegate = self;

        // reset the device params in case we just connected to a different device than what we already know about
        [self initializeDeviceParams];

        // update the display.
        [self updateConnectState];
        [self enableTrainerButtons];
        
        // request current trainer mode
        [self requestTrainerMode];
    }
    else if ([connectionInfo isKindOfClass:[WFHeartrateConnection class]])
    {
        NSLog(@"================ time to HR connection %f", [self.requestDateHR timeIntervalSinceNow]);
        [self updateHeartrateConnectState];
    }
    else if ([connectionInfo isKindOfClass:[WFBikeSpeedCadenceConnection class]])
    {
        NSLog(@"================ time to SC connection %f", [self.requestDateSC timeIntervalSinceNow]);
        [self updateSpeedCadenceConnectState];
    }

}

//--------------------------------------------------------------------------------
- (void) onSensorDisconnected:(WFSensorConnection*) connectionInfo
{
    NSLog(@"onSensorDisconnected:");

    // called from connection: stateChanged:

    // check for a connection error.
    if ( connectionInfo.hasError )
    {
        NSString* msg = nil;
        switch ( connectionInfo.error )
        {
            case WF_SENSOR_CONN_ERROR_PAIRED_DEVICE_NOT_AVAILABLE:
                msg = @"Paired device error.\n\nA device specified in the connection parameters was not found in the Bluetooth Cache.  Please use the paring manager to remove the device, and then re-pair.";
                break;
                
            case WF_SENSOR_CONN_ERROR_PROXIMITY_SEARCH_WHILE_CONNECTED:
                msg = @"Proximity search is not allowed while a device of the specified type is connected to the iPhone.";
                break;
                
            default:
                break;
        }
        
        if ( msg )
        {
            NSString *title = @"Connection Error";
            [self showAlertPanelWithTitle:title message:msg];
            NSLog(@"%@",msg);
        }
    }
    
    if ([connectionInfo isKindOfClass: [WFBikePowerConnection class]])
    {
        // update the display.
        [self resetDisplay];
        [self disableTrainerButtons];
        [self updateConnectState];
    }
    else if ([connectionInfo isKindOfClass:[WFHeartrateConnection class]])
    {
        [self updateHeartrateConnectState];
        [self resetHeartrateDisplay];
    }
    else if ([connectionInfo isKindOfClass:[WFBikeSpeedCadenceConnection class]])
    {
        [self updateSpeedCadenceConnectState];
        [self resetSpeedCadenceDisplay];
    }
    
}

#pragma mark - Methods to Update UI
//--------------------------------------------------------------------------------
- (void) updateConnectState
{
    NSLog(@"updateConnectState");

    // update the connnection state on the display
    
	// get the current connection status.
	WFSensorConnectionStatus_t connState = WF_SENSOR_CONNECTION_STATUS_IDLE;

	if ( self.sensorConnection != nil )
	{
		connState = self.sensorConnection.connectionStatus;
	}
	
	// set the button state and status string based on the connection state.
	switch (connState)
	{
		case WF_SENSOR_CONNECTION_STATUS_IDLE:
			[self.connectButton setTitle:@"Connect"];
            self.statusString = @"...";
			break;
		case WF_SENSOR_CONNECTION_STATUS_CONNECTING:
			[self.connectButton setTitle:@"Cancel"];
            self.statusString = @"Searching...";
			break;
		case WF_SENSOR_CONNECTION_STATUS_CONNECTED:
			[self.connectButton setTitle:@"Disconnect"];
            self.statusString = @"...";
			break;
		case WF_SENSOR_CONNECTION_STATUS_DISCONNECTING:
			[self.connectButton setTitle:@"Disconnecting..."];
            self.statusString = @"...";
			break;
        case WF_SENSOR_CONNECTION_STATUS_INTERRUPTED:
            self.statusString = @"Interrupted, Now Searching...";
            break;
        default:
            self.statusString = @"...";
            break;
	}
    
    [self.connectionStatusLabel setStringValue:self.statusString];
    [self.deviceIdLabel setStringValue:[self deviceIdString]];
    [self.signalLabel setStringValue:[self signalStrength]];
    
}

//--------------------------------------------------------------------------------
- (void)updateTrainerMode:(WFBikeTrainerMode_t)eMode
{
    [self.currentModeLabel setStringValue:[self trainerModeString:eMode]];
}

//--------------------------------------------------------------------------------
- (NSString*)signalStrength
{
    NSString* retVal = @"--";
    
    if ( self.sensorConnection )
    {
        // format the signal efficiency value.
		float signal = [self.sensorConnection signalEfficiency];
        //
        // signal efficency is % for ANT connections, dBm for BTLE.
        if ( self.sensorConnection.isANTConnection && signal != -1 )
        {
            retVal = [NSString stringWithFormat:@"%1.0f%%", (signal*100)];
        }
        else if ( self.sensorConnection.isBTLEConnection && signal != 0 )
        {
            retVal = [NSString stringWithFormat:@"%1.0f dBm", signal];
        }
    }
    
    return retVal;
}

//--------------------------------------------------------------------------------
- (NSString*)deviceIdString
{
    NSString* retVal = @"--";
    
    if ( sensorConnection )
    {
        if ( sensorConnection.isBTLEConnection )
        {
            retVal = self.sensorConnection.deviceIDString ? self.sensorConnection.deviceIDString : @"--";
        }
        // format ANT+ device ID string.
        else if ( sensorConnection.isANTConnection )
        {
            retVal = [NSString stringWithFormat:@"%u", sensorConnection.deviceNumber];
        }
    }
    
    return retVal;
}

//--------------------------------------------------------------------------------
- (NSString *)trainerModeString:(WFBikeTrainerMode_t)eMode
{
    NSString *retVal;
    
	switch (eMode)
	{
		case WF_BIKE_TRAINER_MODE_NONE:
            retVal = @"None";
			break;
		case WF_BIKE_TRAINER_MODE_STANDARD:
            retVal = @"Standard Mode";
			break;
		case WF_BIKE_TRAINER_MODE_ERG:
            retVal = @"Ergometer Mode";
			break;
		case WF_BIKE_TRAINER_MODE_SIM:
            retVal = @"Sim Mode";
			break;
        case WF_BIKE_TRAINER_MODE_RESISTANCE:
            retVal = @"PWM (Brake) Mode";
            break;
        default:
            retVal = @"unknown";
            break;
	}
    
    return retVal;
}

//--------------------------------------------------------------------------------
-(void)resetDisplay
{
    [self.speedLabel setStringValue:@"n/a"];
    [self.powerLabel setStringValue:@"n/a"];
    [self.currentModeLabel setStringValue:@"n/a"];
    [self updateDeviceInfo];
    deviceInfoUpdated = NO;
    [self hwConnectChanged];
}

//--------------------------------------------------------------------------------
-(void)showAlertPanelWithTitle:(NSString *)title message: (NSString *)message
{
    // call showAlertPanel method with default button set to OK...
    [self showAlertPanelWithTitle:title message:message defaultButton:@"OK"];
}

//--------------------------------------------------------------------------------
-(void)showAlertPanelWithTitle:(NSString *)title message: (NSString *)message defaultButton:(NSString *)defaultButtonTitle
{
    // can add other methods that take the other button titles if needed...
    NSRunAlertPanel(title, message, defaultButtonTitle, nil, nil);
}

//--------------------------------------------------------------------------------
- (void)updateDeviceInfo
{
    WFBTLECommonData *commonData = nil;
    WFSensorData* data = [sensorConnection getData];
    if ( data )
    {
        // check the radio type of the sensor connection.
        if ( sensorConnection.isBTLEConnection )
        {
            // check that the BTLE common data is present.
            if ( ![data respondsToSelector:@selector(btleCommonData)] )
            {
                // check for BTLE common data in raw data instance.
                data = [sensorConnection getRawData];
            }
            
            // check that the BTLE common data is present.
            if ( [data respondsToSelector:@selector(btleCommonData)] )
            {
                // get the BTLE common data and display the detail
                commonData = (WFBTLECommonData*)[data performSelector:@selector(btleCommonData)];
                
                // set the flag to stop attempting to set the device info once it has been updated once
                deviceInfoUpdated = YES;
            }
        }
        
    }
    // update the display data.
    [self.deviceNameLabel setStringValue: commonData.deviceName ? commonData.deviceName : @"n/a"];
    [self.manufacturerNameLabel setStringValue: (commonData.manufacturerName) ? commonData.manufacturerName : @"n/a"];
    [self.modelNumberLabel setStringValue: (commonData.modelNumber) ? commonData.modelNumber : @"n/a"];
    [self.serialNumberLabel setStringValue: (commonData.serialNumber) ? commonData.serialNumber : @"n/a"];
    [self.hardwareRevLabel setStringValue: (commonData.hardwareRevision) ? commonData.hardwareRevision : @"n/a"];
    [self.firmwareRevLabel setStringValue: (commonData.firmwareRevision) ? commonData.firmwareRevision : @"n/a"];
    [self.softwareRevLabel setStringValue: (commonData.softwareRevision) ? commonData.softwareRevision : @"n/a"];
    
    [self.systemIdLabel setStringValue: [NSString stringWithFormat:@"%llu", commonData.systemId]];
}

//--------------------------------------------------------------------------------
- (void)hwConnectChanged
{
    WFHardwareConnector* hardwareConnector = [WFHardwareConnector sharedConnector];

    // BTLE
    if ( hardwareConnector.hasBTLESupport )
    {
        [self.btleAvailableLabel setStringValue:@"Yes*"];

        if (hardwareConnector.isBTLEEnabled == YES)
        {
            [self.btConnectedLabel setStringValue: @"Yes"];
            [self.enableBTLEButton setTitle:@"Disable"];
        }
        else
        {
            [self.btConnectedLabel setStringValue: @"No"];
            [self.enableBTLEButton setTitle:@"Enable"];
        }
        
    }
    else
    {
        [self.btleAvailableLabel setStringValue:@"No"];
        [self.btConnectedLabel setStringValue: hardwareConnector.isBTLEEnabled ? @"Yes" : @"No"];
    }
}

//--------------------------------------------------------------------------------
- (void)initializeSettings
{
    // initialize the settings fields.
    WFConnectorSettings* settings = [WFHardwareConnector sharedConnector].settings;

    if (settings.useMetricUnits == YES)
    {
        self.metricCheckBox.state = NSOnState;
    }
    else
    {
        self.metricCheckBox.state = NSOffState;
    }

    [self.searchTimeoutText setStringValue:[NSString stringWithFormat:@"%1.1f", settings.searchTimeout]];
    [self.discoveryTimeoutText setStringValue:[NSString stringWithFormat:@"%1.1f", settings.discoveryTimeout]];
    [self.staleDataTimeText setStringValue:[NSString stringWithFormat:@"%1.1f", settings.staleDataTimeout]];
    [self.staleDataStringText setStringValue:settings.staleDataString];
    [self.coastingTimeText setStringValue:[NSString stringWithFormat:@"%1.1f", settings.bikeCoastingTimeout]];

    // convert wheel circumference from meters to centimeters for display
    [self.wheelCircText setStringValue:[NSString stringWithFormat:@"%1.0f", settings.bikeWheelCircumference * 100]];

    // sample rate is on the hardware connector itself, convert from seconds to milliseconds for display
    NSTimeInterval timeInterval = [WFHardwareConnector sharedConnector].sampleRate;
    [self.sampleRateText setDoubleValue:timeInterval * 1000];
}

//--------------------------------------------------------------------------------
- (void)initializeDeviceParams
{
    // first hide everything
    [self.previouslyPairedSensor1 setHidden:YES];
    [self.previouslyPairedSensor2 setHidden:YES];
    [self.previouslyPairedSensor3 setHidden:YES];
    [self.previouslyPairedSensor4 setHidden:YES];
    [self.connectButton1 setHidden:YES];
    [self.connectButton2 setHidden:YES];
    [self.connectButton3 setHidden:YES];
    [self.connectButton4 setHidden:YES];
    [self.removeButton1 setHidden:YES];
    [self.removeButton2 setHidden:YES];
    [self.removeButton3 setHidden:YES];
    [self.removeButton4 setHidden:YES];

    self.deviceParams = [[WFHardwareConnector sharedConnector].settings deviceParamsForSensorType:WF_SENSORTYPE_BIKE_POWER];
    
    for (int i = 0; i < [self.deviceParams count]; i++)
    {
        WFDeviceParams *device = [self.deviceParams objectAtIndex:i];
        if (i == 0)
        {
            [self.previouslyPairedSensor1 setHidden:NO];
            [self.connectButton1 setHidden:NO];
            [self.removeButton1 setHidden:NO];
            [self.previouslyPairedSensor1 setStringValue:device.deviceIDString];
        }
        else if (i == 1)
        {
            [self.previouslyPairedSensor2 setHidden:NO];
            [self.connectButton2 setHidden:NO];
            [self.removeButton2 setHidden:NO];
            [self.previouslyPairedSensor2 setStringValue:device.deviceIDString];
        }
        else if (i == 2)
        {
            [self.previouslyPairedSensor3 setHidden:NO];
            [self.connectButton3 setHidden:NO];
            [self.removeButton3 setHidden:NO];
            [self.previouslyPairedSensor3 setStringValue:device.deviceIDString];
        }
        else
        {
            [self.previouslyPairedSensor4 setHidden:NO];
            [self.connectButton4 setHidden:NO];
            [self.removeButton4 setHidden:NO];
            [self.previouslyPairedSensor4 setStringValue:device.deviceIDString];
        }
    }
}

//--------------------------------------------------------------------------------
- (void) disableTrainerButtons
{
    [self.standardModeSetButton setEnabled:NO];
    [self.pwmModeSetButton setEnabled:NO];
    [self.ergModeSetButton setEnabled:NO];
    [self.plusHundredsButton setEnabled:NO];
    [self.plusTensButton setEnabled:NO];
    [self.plusOnesButton setEnabled:NO];
    [self.minusHundredsButton setEnabled:NO];
    [self.minusTensButton setEnabled:NO];
    [self.minusOnesButton setEnabled:NO];
    [self.simModeSetButton setEnabled:NO];
    [self.simSetCrrButton setEnabled:NO];
    [self.simSetCButton setEnabled:NO];
    [self.setWheelCircButton setEnabled:NO];
    [self.simSetGradeButton setEnabled:NO];
    [self.simSetWindSpeedButton setEnabled:NO];
    [self.initSpindownButton setEnabled:NO];
    [self.manualZeroButton setEnabled:NO];

}

//--------------------------------------------------------------------------------
- (void) enableTrainerButtons
{
    [self.standardModeSetButton setEnabled:YES];
    [self.pwmModeSetButton setEnabled:YES];
    [self.ergModeSetButton setEnabled:YES];
    [self.plusHundredsButton setEnabled:YES];
    [self.plusTensButton setEnabled:YES];
    [self.plusOnesButton setEnabled:YES];
    [self.minusHundredsButton setEnabled:YES];
    [self.minusTensButton setEnabled:YES];
    [self.minusOnesButton setEnabled:YES];
    [self.simModeSetButton setEnabled:YES];
    [self.simSetCrrButton setEnabled:YES];
    [self.simSetCButton setEnabled:YES];
    [self.setWheelCircButton setEnabled:YES];
    [self.simSetGradeButton setEnabled:YES];
    [self.simSetWindSpeedButton setEnabled:YES];
    [self.initSpindownButton setEnabled:YES];
    [self.manualZeroButton setEnabled:YES];
    
}
#pragma mark -
#pragma mark WFBikePowerDelegate Implementation
//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didReceiveCalibrationResponse:(UCHAR)ucStatus offset:(USHORT)usOffset temperature:(SCHAR)scTemp
{
    // status = 0 is success, see enum at top of this file...
    wf_cpm_cal_status_t e_status = (wf_cpm_cal_status_t)ucStatus;
    if ( e_status == WF_CPM_CAL_STATUS_SUCCESS )
    {
        NSString *title = @"Calibration Response";
        NSString* msg = [NSString stringWithFormat:@"Zero Offset: %d\nTemperature: %d C",usOffset,scTemp];
        
        [self showAlertPanelWithTitle:title message:msg];
    }
    else // error
    {
        NSString *title = @"Calibration Error";
        NSString* msg = @"Make Sure you are not moving and try again!";
        
        [self showAlertPanelWithTitle:title message:msg];
    }
}

#pragma mark -
#pragma mark WFBikeTrainerDelegate Implementation
//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerMode:(WFBikeTrainerMode_t)eMode status:(UCHAR)ucStatus
{
    if ( ucStatus == 0 )
    {
        [self updateTrainerMode:eMode];
        if (eMode != WF_BIKE_TRAINER_MODE_ERG) // Alert gets annoying when you click Erg mode +/- buttons.
        {
            NSString *title = @"SET TRAINER MODE";
            NSString* msg = [NSString stringWithFormat:@"Mode: %@\nSuccess: %@", [self trainerModeString:eMode], (ucStatus==0)?@"TRUE":@"FALSE"];
            
            [self showAlertPanelWithTitle:title message:msg];
        }
    }
    else // not successful, so always show the alert
    {
        NSString *title = @"SET TRAINER MODE";
        NSString* msg = [NSString stringWithFormat:@"Mode: %@\nSuccess: %@", [self trainerModeString:eMode], (ucStatus==0)?@"TRUE":@"FALSE"];
    
        [self showAlertPanelWithTitle:title message:msg];
    }
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerGrade:(UCHAR)ucStatus
{
    NSString *title = @"SET GRADE";
    NSString* msg = [NSString stringWithFormat:@"Success: %@", (ucStatus==0)?@"TRUE":@"FALSE"];
    
    [self showAlertPanelWithTitle:title message:msg];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerRollingResistance:(UCHAR)ucStatus
{
    NSString *title = @"SET Crr";
    NSString* msg = [NSString stringWithFormat:@"Success: %@", (ucStatus==0)?@"TRUE":@"FALSE"];
    
    [self showAlertPanelWithTitle:title message:msg];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerWindResistance:(UCHAR)ucStatus
{
    NSString *title = @"SET C";
    NSString* msg = [NSString stringWithFormat:@"Success: %@", (ucStatus==0)?@"TRUE":@"FALSE"];
    
    [self showAlertPanelWithTitle:title message:msg];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerWindSpeed:(UCHAR)ucStatus
{
    NSString *title = @"SET Wind Speed";
    NSString* msg = [NSString stringWithFormat:@"Success: %@", (ucStatus==0)?@"TRUE":@"FALSE"];
    
    [self showAlertPanelWithTitle:title message:msg];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerWheelCircumference:(UCHAR)ucStatus
{
    NSString *title = @"SET Wheel Circumference";
    NSString* msg = [NSString stringWithFormat:@"Success: %@", (ucStatus==0)?@"TRUE":@"FALSE"];
    
    [self showAlertPanelWithTitle:title message:msg];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didReceiveTrainerReadModeResponse:(UCHAR)ucStatus mode:(WFBikeTrainerMode_t)eMode;
{
    [self updateTrainerMode:eMode];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didReceiveTrainerInitSpindownResponse:(UCHAR)ucStatus
{
    // check to make sure we have the right mode set on the ui
    [self requestTrainerMode];
    
    NSString *title = @"Init Spindown Response";
    NSString *msg;
    if ( ucStatus == 0 )
    {
        NSString *spindownMessage;
        if ([[[WFHardwareConnector sharedConnector] settings] useMetricUnits] == YES)
        {
            spindownMessage = @"Pedal up to 35 kph, stop pedaling, and coast down to 15 kph.";
        }
        else
        {
            spindownMessage = @"Pedal up to 22 mph, stop pedaling, and coast down to 10 mph.";
        }
        
        msg = [NSString stringWithFormat:@"Init Spindown successful! Ready for you to spindown.\n\n%@", spindownMessage];
    }
    else
    {
        msg = @"Init spindown failed. Please try again.";
    }
    
    [self showAlertPanelWithTitle:title message:msg];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didReceiveTrainerSpindownResult:(float)spindownTime temperature:(float)spindownTemperature offset:(USHORT)spindownOffset
{
    NSString *title; 
    NSString* msg; 
    
    if (spindownTime == 0.0 && spindownTemperature == 0.0 && spindownOffset == 0)
    {
        title = @"Spindown Failed";
        msg = @"Spindown failed. Do not start pedaling during a spindown. Please initiate another spindown, otherwise the power calculations will continue to use the last known spindown results.";
    }
    else
    {
        title = @"Spindown Result";
        msg = [NSString stringWithFormat:@"Received spindown result:\n\nSpindown time: %0.3f\nTemperature: %0.2f\nOffset: %d", spindownTime, spindownTemperature, spindownOffset];
    }
    
    [self showAlertPanelWithTitle:title message:msg];
    
    [self requestTrainerMode];
    
    // NOTE: at the end of the spindown test, the device resets to Standard mode level 2.
    // The app or the user may want to set a different mode at this point.
}

#pragma mark - WFSensorConnectionDelegate Implementation

//--------------------------------------------------------------------------------
- (void)connectionDidTimeout:(WFSensorConnection*)connectionInfo
{
    NSLog(@"connectionDidTimeout:");
    
    // update the button state.
    [self updateConnectState];
    
    NSString *title = @"Search Timeout";
    NSString *msg = @"A connection was not established before the maximum search time expired.";

    
    [self showAlertPanelWithTitle:title message:msg];
}

//--------------------------------------------------------------------------------
- (void)connection:(WFSensorConnection*)connectionInfo stateChanged:(WFSensorConnectionStatus_t)connState
{    
    NSString *connStateString;
    
    switch (connState) {
        case WF_SENSOR_CONNECTION_STATUS_IDLE:
            connStateString = @"WF_SENSOR_CONNECTION_STATUS_IDLE";
            break;
        case WF_SENSOR_CONNECTION_STATUS_CONNECTING:
            connStateString = @"WF_SENSOR_CONNECTION_STATUS_CONNECTING";
            break;
        case WF_SENSOR_CONNECTION_STATUS_CONNECTED:
            connStateString = @"WF_SENSOR_CONNECTION_STATUS_CONNECTED";
            break;
        case WF_SENSOR_CONNECTION_STATUS_INTERRUPTED:
            connStateString = @"WF_SENSOR_CONNECTION_STATUS_INTERRUPTED";
            break;
        case WF_SENSOR_CONNECTION_STATUS_DISCONNECTING:
            connStateString = @"WF_SENSOR_CONNECTION_STATUS_DISCONNECTING";
            break;
        default:
            connStateString = @"Unknown connection status";
            break;
    }
    
    NSLog(@"SENSOR CONNECTION STATE CHANGED: %@",connStateString);
    
    // check for a valid connection.
    if ( connectionInfo.isValid && connectionInfo.isConnected )
    {
        // process connection
        [self onSensorConnected:connectionInfo];
    }
    
    // check for disconnected sensor.
    else if ( connState == WF_SENSOR_CONNECTION_STATUS_IDLE )
    {
        // process the disconnect
        [self onSensorDisconnected:connectionInfo];
    }
}

#pragma mark - WFHardwareConnectorDelegate implementation
//--------------------------------------------------------------------------------
- (void) hardwareConnector:(WFHardwareConnector *)hwConnector
           connectedSensor:(WFSensorConnection *)connectionInfo
{
    NSLog(@"hardwareConnector: connectedSensor delegate method");
}

//--------------------------------------------------------------------------------
- (void) hardwareConnector:(WFHardwareConnector *)hwConnector
        didDiscoverDevices:(NSSet *)connectionParams
           searchCompleted:(BOOL)bCompleted
{
    NSLog(@"hardwareConnector: didDiscoverDevices delegate method");
            
    // if the search is completed, reload all devices to the table.
    if ( bCompleted == YES)
    {
        // clear the array, it will be reloaded below.
        [self.discoveredSensors removeAllObjects];
    }
    
    // Add discovered device or devices. If bCompleted is NO, one device will be in the set.
    // If bCompleted is YES, all discovered devices will be in the set.
    for ( WFConnectionParams* connParams in connectionParams )
    {
        // ensure the correct sensor type and sensor subtype
        //TODO: add sensor subtype back when the API sets it...
        if ( connParams.sensorType == WF_SENSORTYPE_BIKE_POWER) // && connParams.sensorSubType == WF_SENSOR_SUBTYPE_BIKE_POWER_KICKR)
        {
            // the discovered device will be in the device1 position in the WFConnectionParams
            [self.discoveredSensors addObject:connParams.device1];
            
            // Debug: what other information is available in the connParams returned?
            
        }
    }
    
    // reload the table.
    [self.discoveredSensorsTableView reloadData];
    
    // if the search is done, alert the user.
    if ( bCompleted )
    {
        // reset the search state.
        [self resetSearchState];
        
        // alert the user.
        NSString *title = @"Device Discovery";
        NSString *msg = [NSString stringWithFormat:@"Device discovery finished.  %lu devices found", (unsigned long)[self.discoveredSensors count]];
        [self showAlertPanelWithTitle:title message:msg];
        
    }
}

//--------------------------------------------------------------------------------
- (void) hardwareConnector:(WFHardwareConnector *)hwConnector disconnectedSensor:(WFSensorConnection *)connectionInfo
{
    NSLog(@"hardwareConnector: disconnectedSensor delegate method");
}

//--------------------------------------------------------------------------------
- (void) hardwareConnector:(WFHardwareConnector *)hwConnector stateChanged:(WFHardwareConnectorState_t)currentState
{
    NSString *connStateString;
        
    switch (currentState) {
        case WF_HWCONN_STATE_NOT_CONNECTED:
            connStateString = @"WF_HWCONN_STATE_NOT_CONNECTED";
            break;
        case WF_HWCONN_STATE_CONNECTED:
            connStateString = @"WF_HWCONN_STATE_CONNECTED";
            break;
        case WF_HWCONN_STATE_ACTIVE:
            connStateString = @"WF_HWCONN_STATE_ACTIVE";
            break;
        case WF_HWCONN_STATE_RESET:
            connStateString = @"WF_HWCONN_STATE_RESET";
            break;
        case WF_HWCONN_STATE_BT40_ENABLED:
            connStateString = @"WF_HWCONN_STATE_BT40_ENABLED";
            break;
        case WF_HWCONN_STATE_BT_BONDING_MODE:
            connStateString = @"WF_HWCONN_STATE_BT_BONDING_MODE";
            break;
        default:
            connStateString = @"Unknown hardware state status";
            break;
    }
    
    NSLog(@"HARDWARE CONNECTOR STATE CHANGED: %@", connStateString);
    [self hwConnectChanged];
    
}

//--------------------------------------------------------------------------------
- (void) hardwareConnectorHasData
{
    NSLog(@"in hardwareConnectorHasData delegate method");
    WFHardwareConnector *hardwareConnector = [WFHardwareConnector sharedConnector];
    
    // update the signal strength display
    [self.signalLabel setStringValue:[self signalStrength]];
    
    //-----------------------------------
    // update bike power data
    WFBikePowerData* bikePowerData = [self.sensorConnection getBikePowerData];
	if ( bikePowerData != nil )
    {
        // get the speed from the data, formatted speed checks for coasting
        // if coasting, it sets the string to the stale data string
        NSString *speedString = [bikePowerData formattedSpeed:YES];
        [self.speedLabel setStringValue:speedString];
        
        if ( [hardwareConnector.settings.staleDataString isEqualToString:speedString] )
        {
            // if the speed indicates coasting, set the power to coasting also
            [self.powerLabel setStringValue:hardwareConnector.settings.staleDataString];
        }
        else
        {
            [self.powerLabel setStringValue:[bikePowerData formattedPower:YES]];
        }

        // update device info
        if (deviceInfoUpdated == NO)
        {
            [self updateDeviceInfo];
        }
    }
    else
    {
        [self resetDisplay];
    }
    
    //-----------------------------------
    // update heartrate data
    WFHeartrateData *hrData = [self.heartrateConnection getHeartrateData];
    if (hrData != nil)
    {
        [self.hearrateLabel setStringValue:[hrData formattedHeartrate:YES]];
    }
    else
    {
        [self resetHeartrateDisplay];
    }
    
    //-----------------------------------
    // update speed and cadence data
    WFBikeSpeedCadenceData *scData = [self.bikeSpeedCadenceConnection getBikeSpeedCadenceData];
    if (scData != nil)
    {
        [self.scSpeedLabel setStringValue:[scData formattedSpeed:YES]];
        [self.scCadenceLabel setStringValue:[scData formattedCadence:YES]];
    }
    else
    {
        [self resetSpeedCadenceDisplay];
    }
    
}

//--------------------------------------------------------------------------------
- (void)hardwareConnector:(WFHardwareConnector *)hwConnector hasFirmwareUpdateAvailableForConnection:(WFSensorConnection *)connectionInfo required:(BOOL)required withWahooUtilityAppURL:(NSURL *)wahooUtilityAppURL
{
    NSLog(@"in hardwareConnector: hasFirmwareUpdateAvailableForConnection");
    
    NSString *title = @"KICKR Software Update";
    NSMutableString *message = [[[NSMutableString alloc] init] autorelease];
    
    if (required == YES)
    {
        [message appendString:@"A newer version of the KICKR software is required before you can connect. "];
    }
    else
    {
        [message appendString:@"A newer version of the KICKR software is available. "];
    }
    
    [message appendString:@"Installing the upgrade requires the Wahoo Utility app on a BTLE enabled iPhone or iPad. You will be transferred to iTunes on the web where you can download the Wahoo Utility app.\n\nWould you like to go to iTunes now?"];
        
    
    NSInteger buttonPressed = NSRunAlertPanel(title, message, @"Yes", @"No", nil);
    
    NSLog(@"button pressed = %ld", (long)buttonPressed);
    
    // if the user clicked the default button (Yes), then show the wahoo utility in iTunes on the web
    if (buttonPressed == NSAlertDefaultReturn)
    {
        NSLog(@"user clicked default button");
        [[NSWorkspace sharedWorkspace] openURL:wahooUtilityAppURL];
    }


}

#pragma mark - Trainer Modes
//--------------------------------------------------------------------------------
- (void)requestTrainerMode
{
    [self.sensorConnection trainerReadMode];
}

#pragma mark Standard Mode
//--------------------------------------------------------------------------------
- (IBAction)setStandardModeClicked:(id)sender
{
    WFBikeTrainerLevel_t eLevel = (WFBikeTrainerLevel_t)[self.standardLevelField intValue];
    [self.sensorConnection trainerSetStandardMode:eLevel];
}

#pragma mark PWM (Brake) Mode
//--------------------------------------------------------------------------------
- (IBAction)setPWMModeClicked:(id)sender
{
    // the trainerSetResistanceMode takes a floating point scale value between
    // zero and one.  convert the entered percentage to zero to one scale.
    [self.pwmModeLevelField resignFirstResponder];
    float fpScale = [self.pwmModeLevelField floatValue] / 100.0;
    [self.sensorConnection trainerSetResistanceMode:fpScale];
}

#pragma mark Erg mode
//--------------------------------------------------------------------------------
- (IBAction)setErgModeClicked:(id)sender
{
    int watts = [self.ergModeLevelField intValue];
    
    // set the hundreds, tens and ones fields to match
    int hundreds = watts / 100;
    int tens = (watts - (hundreds * 100)) / 10;
    
    [self.hundredsField setIntValue:hundreds];
    [self.tensField setIntValue:tens];
    [self.onesField setIntValue:(watts % 10)];
    
    [self.sensorConnection trainerSetErgMode:watts];
//    [self setWatts];
}

//--------------------------------------------------------------------------------
- (IBAction)plusHundredsClicked:(id)sender
{
    USHORT usHundreds = (USHORT)[self.hundredsField intValue] + 1;
    if(usHundreds > 9)
    {
        usHundreds = 0;
    }
    [self.hundredsField setIntValue:usHundreds];
    [self setWatts];
}

//--------------------------------------------------------------------------------
- (IBAction)plusTensClicked:(id)sender
{
    USHORT usTens = (USHORT)[self.tensField intValue] + 1;
    if(usTens > 9)
    {
        usTens = 0;
    }
    [self.tensField setIntValue:usTens];
    [self setWatts];
}

//--------------------------------------------------------------------------------
- (IBAction)plusOnesClicked:(id)sender
{
    USHORT usOnes = (USHORT)[self.onesField intValue] + 1;
    if(usOnes > 9)
    {
        usOnes = 0;
    }
    [self.onesField setIntValue:usOnes];
    [self setWatts];
}
//--------------------------------------------------------------------------------
- (IBAction)minusHundredsClicked:(id)sender
{
    USHORT usHundreds = (USHORT)[self.hundredsField intValue];
    if(usHundreds == 0)
    {
        usHundreds = 9;
    }
    else
    {
        usHundreds = usHundreds - 1;
    }
    [self.hundredsField setIntValue:usHundreds];
    [self setWatts];
}

//--------------------------------------------------------------------------------
- (IBAction)minusTensClicked:(id)sender
{
    USHORT usTens = (USHORT)[self.tensField intValue];
    if(usTens == 0)
    {
        usTens = 9;
    }
    else
    {
        usTens = usTens - 1;
    }
    [self.tensField setIntValue:usTens];
    [self setWatts];
}

//--------------------------------------------------------------------------------
- (IBAction)minusOnesClicked:(id)sender
{
    USHORT usOnes = (USHORT)[self.onesField intValue];
    if(usOnes == 0)
    {
        usOnes = 9;
    }
    else
    {
        usOnes = usOnes - 1;
    }
    [self.onesField setIntValue:usOnes];
    [self setWatts];
}

//--------------------------------------------------------------------------------
- (void)setWatts
{
    // gather the watts from the individual fields and set the erg mode
    // (could add a thousands place if desired)
    USHORT usHundreds = (USHORT)[self.hundredsField intValue];
    USHORT usTens = (USHORT)[self.tensField intValue];
    USHORT usOnes = (USHORT)[self.onesField intValue];
    USHORT usWatts = usHundreds * 100 + usTens * 10 + usOnes;
    
    // set the ergModeLevelField to match the hundreds, tens and ones
    [self.ergModeLevelField setIntValue:usWatts];
    
    [self.sensorConnection trainerSetErgMode:usWatts];
}

#pragma mark Sim Mode
- (IBAction)setSimModeClicked:(id)sender
{
    [self.simCrrInitField resignFirstResponder];
    [self.simCInitField resignFirstResponder];
    [self.weightField resignFirstResponder];
    float fWeight = [self.weightField floatValue];
    float fCrr = [self.simCrrInitField floatValue];
    float fC = [self.simCInitField floatValue];
    [self.sensorConnection trainerSetSimMode:fWeight rollingResistance:fCrr windResistance:fC];
}

//--------------------------------------------------------------------------------
- (IBAction)setCrrClicked:(id)sender
{
    [self.simCrrField resignFirstResponder];
    float fCrr = [self.simCrrField floatValue];
    [self.sensorConnection trainerSetRollingResistance:fCrr];
}

//--------------------------------------------------------------------------------
- (IBAction)setCClicked:(id)sender
{
    [self.simCField resignFirstResponder];
    float fC = [self.simCField floatValue];
    [self.sensorConnection trainerSetWindResistance:fC];
}

//--------------------------------------------------------------------------------
- (IBAction)setGradeClicked:(id)sender
{
    [self.simGradeField resignFirstResponder];
    float fGrade = [self.simGradeField floatValue];
    [self.sensorConnection trainerSetGrade:fGrade];
}

//--------------------------------------------------------------------------------
- (IBAction)setWheelCircClicked:(id)sender
{
    [self.wheelCircField resignFirstResponder];
    float circ = [self.wheelCircField floatValue];
    [self.sensorConnection trainerSetWheelCircumference:circ];
}

//--------------------------------------------------------------------------------
- (IBAction)setWindSpeedClicked:(id)sender
{
    [self.simWindSpeedField resignFirstResponder];
    float windSpeed = [self.simWindSpeedField floatValue];
    [self.sensorConnection trainerSetWindSpeed:windSpeed];
}

#pragma mark - Init Spindown
//--------------------------------------------------------------------------------
- (IBAction)initSpindownClicked:(id)sender
{
    [self.sensorConnection trainerInitSpindown];
}

#pragma mark - Manual Zero
//--------------------------------------------------------------------------------
- (IBAction)manualZeroClicked:(id)sender
{
    [self.sensorConnection setManualZeroCalibration];
}

#pragma mark - Update Hardware Connector Settings
//--------------------------------------------------------------------------------
- (IBAction)setValuesClicked:(id)sender
{
    // update the sample rate.
    WFHardwareConnector* hwConn = [WFHardwareConnector sharedConnector];
    // convert sample rate from milliseconds back to seconds for saving
	NSTimeInterval sampleRate = [self.sampleRateText doubleValue] / 1000;
	hwConn.sampleRate = sampleRate;
    
    // update the connector settings.
    WFConnectorSettings* settings = hwConn.settings;
    settings.searchTimeout = [self.searchTimeoutText doubleValue];
    settings.discoveryTimeout = [self.discoveryTimeoutText doubleValue];
    settings.staleDataTimeout = [self.staleDataTimeText doubleValue];
    settings.staleDataString = [self.staleDataStringText stringValue];
    settings.bikeCoastingTimeout = [self.coastingTimeText doubleValue];
    // convert wheel circumference from centimeters back to meters for saving
    settings.bikeWheelCircumference = [self.wheelCircText floatValue] / 100;

    if (self.metricCheckBox.state == NSOnState)
    {
        settings.useMetricUnits = YES;
    }
    else
    {
        settings.useMetricUnits = NO;
    }
}

#pragma mark - Previously Paired Sensors
//--------------------------------------------------------------------------------
- (IBAction)connectToPreviouslyPairedSensor:(id)sender
{
    if (sender == self.connectButton1)
    {
        WFDeviceParams* devParams = (WFDeviceParams*)[deviceParams objectAtIndex:0];
        NSLog(@"user selected device ID %@", devParams.deviceIDString);
                
        [self requestConnectionToDevice:devParams];
    }
    else if (sender == self.connectButton2)
    {
        WFDeviceParams* devParams = (WFDeviceParams*)[deviceParams objectAtIndex:1];
        NSLog(@"user selected device ID %@", devParams.deviceIDString);
        
        [self requestConnectionToDevice:devParams];
    }
    else if (sender == self.connectButton3)
    {
        WFDeviceParams* devParams = (WFDeviceParams*)[deviceParams objectAtIndex:2];
        NSLog(@"user selected device ID %@", devParams.deviceIDString);
        
        [self requestConnectionToDevice:devParams];
    }
    else if (sender == self.connectButton4)
    {
        WFDeviceParams* devParams = (WFDeviceParams*)[deviceParams objectAtIndex:3];
        NSLog(@"user selected device ID %@", devParams.deviceIDString);
        
        [self requestConnectionToDevice:devParams];
    }
}

//--------------------------------------------------------------------------------
- (IBAction)removePreviouslyPairedSensor:(id)sender
{
    if (sender == self.removeButton1)
    {
        WFDeviceParams* devParams = (WFDeviceParams*)[deviceParams objectAtIndex:0];
        NSLog(@"user selected to remove device ID %@", devParams.deviceIDString);
        
        [[WFHardwareConnector sharedConnector].settings removeDeviceParams:devParams forSensorType:WF_SENSORTYPE_BIKE_POWER];
    }
    else if (sender == self.removeButton2)
    {
        WFDeviceParams* devParams = (WFDeviceParams*)[deviceParams objectAtIndex:1];
        NSLog(@"user selected to remove device ID %@", devParams.deviceIDString);
        
        [[WFHardwareConnector sharedConnector].settings removeDeviceParams:devParams forSensorType:WF_SENSORTYPE_BIKE_POWER];
    }
    else if (sender == self.removeButton3)
    {
        WFDeviceParams* devParams = (WFDeviceParams*)[deviceParams objectAtIndex:2];
        NSLog(@"user selected to remove device ID %@", devParams.deviceIDString);
        
        [[WFHardwareConnector sharedConnector].settings removeDeviceParams:devParams forSensorType:WF_SENSORTYPE_BIKE_POWER];
    }
    else if (sender == self.removeButton4)
    {
        WFDeviceParams* devParams = (WFDeviceParams*)[deviceParams objectAtIndex:3];
        NSLog(@"user selected to remove device ID %@", devParams.deviceIDString);
        
        [[WFHardwareConnector sharedConnector].settings removeDeviceParams:devParams forSensorType:WF_SENSORTYPE_BIKE_POWER];
    }
    
    [self initializeDeviceParams];
}



#pragma mark - Discover Sensors
//--------------------------------------------------------------------------------
- (IBAction)searchClicked:(id)sender
{
    // check whether discovery is currently in progress.
    if ( isSearching )
    {
        // cancel the discovery in progress.
        [self stopDiscovery:TRUE];
    }
    
    else
    {
        // initiate device discovery.
        [self startDiscovery];
    }
}

//--------------------------------------------------------------------------------
- (void)startDiscovery
{
    // initiate device discovery.
    //
    
    isSearching = TRUE;
    
    // set the search button title.
    [self.searchButton setTitle:@"Cancel"];
    
    // remove existing search results.
    [self.discoveredSensors removeAllObjects];
    [self.discoveredSensorsTableView reloadData];
    
    // determine the search timeout period.
    NSTimeInterval timeout = [WFHardwareConnector sharedConnector].settings.discoveryTimeout;
    
    // initiate the search.
    if ( [[WFHardwareConnector sharedConnector] discoverDevicesOfType:WF_SENSORTYPE_BIKE_POWER onNetwork:WF_NETWORKTYPE_BTLE searchTimeout:timeout] )
    {
        // the request to discover devices was successful
    }
    else
    {
        // DISCOVERY FAILED TO INITIALIZE.
        //
        // this typically happens because there are existing connections on the network type.
        //
        // reset search state.
        [self resetSearchState];
        
        // alert the user.
        NSString *title = @"Device Discovery";
        // BTLE only network type for now.
        NSString* netType = @"BTLE";
        NSString* msg = [NSString stringWithFormat:@"Failed to initiate device discovery.  Please close any existing %@ connections and try again.", netType];
        [self showAlertPanelWithTitle:title message:msg];
    }
}

//--------------------------------------------------------------------------------
- (void)stopDiscovery:(BOOL)bReloadTable
{
    // cancel the discovery in progress.
    //
    
    // reset the search state.
    [self resetSearchState];
    
    // cancel the discovery.
    NSSet* sensors = [[WFHardwareConnector sharedConnector] cancelDiscoveryOnNetwork:WF_NETWORKTYPE_BTLE];
    
    // reload the array with the completed search.
    if ( bReloadTable )
    {
        [self.discoveredSensors removeAllObjects];
        [self.discoveredSensors addObjectsFromArray:[sensors allObjects]];
        [self.discoveredSensorsTableView reloadData];
    }
}

//--------------------------------------------------------------------------------
- (void)resetSearchState
{
    // reset the search UI.
    [searchButton setTitle:@"Search for Sensors"];
    isSearching = NO;
}
    
#pragma mark - Table View Datasource methods
//--------------------------------------------------------------------------------
- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
    if (aTableView == self.discoveredSensorsTableView)
    {
        if (self.discoveredSensors != nil)
        {
            return [self.discoveredSensors count];
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

//--------------------------------------------------------------------------------
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
    NSString *retVal = @"";
    
    if (aTableView == self.discoveredSensorsTableView)
    {
        WFDeviceParams *devParams = (WFDeviceParams*)[self.discoveredSensors objectAtIndex:rowIndex];
        retVal = devParams.deviceIDString;
    }
    return retVal;
}

#pragma mark - Table View Delegate Methods
//--------------------------------------------------------------------------------
- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
    NSTableView *aTableView = (NSTableView *)aNotification.object;
    NSInteger rowIndex = [aTableView selectedRow];

    if (aTableView == self.discoveredSensorsTableView)
    {
        if (rowIndex >= 0 && rowIndex < [discoveredSensors count])
        {
            WFDeviceParams* devParams = (WFDeviceParams*)[discoveredSensors objectAtIndex:rowIndex];
            NSLog(@"user selected device ID %@", devParams.deviceIDString);
            
            [aTableView deselectRow:rowIndex];
            
            [self requestConnectionToDevice:devParams];
        }
    }
}

//--------------------------------------------------------------------------------
- (void)doNothingOnDoubleClick:(id)sender
{
    NSLog(@"in doNothingOnDoubleClick");
}

#pragma mark - Connect Other Sensors
//--------------------------------------------------------------------------------
- (IBAction)connectHeartrateClicked:(id)sender
{
    NSLog(@"connectHeartRateClicked:");
    
    // toggles between connecting and disconnecting
    
    // get the current connection status.
    WFSensorConnectionStatus_t connState = WF_SENSOR_CONNECTION_STATUS_IDLE;
    
    if (self.heartrateConnection != nil )
    {
        connState = self.heartrateConnection.connectionStatus;
    }
    
    switch (connState)
    {
        case WF_SENSOR_CONNECTION_STATUS_IDLE:
            // connect the sensor.
            [self connectHeartrateSensor];
            break;
            
        case WF_SENSOR_CONNECTION_STATUS_CONNECTING:
        case WF_SENSOR_CONNECTION_STATUS_CONNECTED:
            // disconnect the sensor.
            NSLog(@"disconnectHeartrateSensor:");
            
            [self.heartrateConnection disconnect];
            
            // update the display.
            [self updateHeartrateConnectState]; 
            break;
            
        case WF_SENSOR_CONNECTION_STATUS_DISCONNECTING:
        case WF_SENSOR_CONNECTION_STATUS_INTERRUPTED:
            // do nothing.
            break;
    }
}

//--------------------------------------------------------------------------------
- (void) connectHeartrateSensor
{
    // create the connection params.
    WFConnectionParams* params = nil;
    
    // if wildcard search is specified, create empty connection params.
    if ( self.connectHRWildcardCheckBox.state == NSOnState )
    {
        params = [[[WFConnectionParams alloc] init] autorelease];
        params.sensorType = WF_SENSORTYPE_HEARTRATE;
        params.networkType = WF_NETWORKTYPE_BTLE;
    }
    // otherwise, get the params from the stored settings.
    else
    {
        params = [[WFHardwareConnector sharedConnector].settings connectionParamsForSensorType:WF_SENSORTYPE_HEARTRATE];
    }
    
    if ( params != nil)
    {
        // set the search timeout.
        params.searchTimeout = [WFHardwareConnector sharedConnector].settings.searchTimeout;
    }
    
    WFSensorConnection *heartConn = [[WFHardwareConnector sharedConnector] requestSensorConnection:params];
    self.requestDateHR = [NSDate date];
    
    if ([heartConn isKindOfClass:[WFHeartrateConnection class]])
    {
        self.heartrateConnection = (WFHeartrateConnection *)heartConn;
    }
    
    self.heartrateConnection.delegate = self;
    
    // update the display.
    [self updateHeartrateConnectState];
    
}

//--------------------------------------------------------------------------------
- (void) updateHeartrateConnectState
{
    NSLog(@"updateHeartrateConnectState");
    
    // update the connnection state on the display
    
	// get the current connection status.
	WFSensorConnectionStatus_t connState = WF_SENSOR_CONNECTION_STATUS_IDLE;
    
	if ( self.heartrateConnection != nil )
	{
		connState = self.heartrateConnection.connectionStatus;
	}
	
	// set the button state and status string based on the connection state.
	switch (connState)
	{
		case WF_SENSOR_CONNECTION_STATUS_IDLE:
			[self.connectHRButton setTitle:@"Connect HR"];
			break;
		case WF_SENSOR_CONNECTION_STATUS_CONNECTING:
			[self.connectHRButton setTitle:@"Cancel"];
			break;
		case WF_SENSOR_CONNECTION_STATUS_CONNECTED:
			[self.connectHRButton setTitle:@"Disconnect"];
			break;
		case WF_SENSOR_CONNECTION_STATUS_DISCONNECTING:
			[self.connectHRButton setTitle:@"Disconnecting..."];
			break;
        case WF_SENSOR_CONNECTION_STATUS_INTERRUPTED:
            // do nothing
            break;
        default:
            // do nothing
            break;
	}
}

//--------------------------------------------------------------------------------
-(void)resetHeartrateDisplay
{
    [self.hearrateLabel setStringValue:@"n/a"];
}

//--------------------------------------------------------------------------------
- (IBAction)connectSpeedCadenceClicked:(id)sender
{
    NSLog(@"connectSpeedCadenceClicked:");
    
    // toggles between connecting and disconnecting
    
    // get the current connection status.
    WFSensorConnectionStatus_t connState = WF_SENSOR_CONNECTION_STATUS_IDLE;
    
    if (self.bikeSpeedCadenceConnection != nil )
    {
        connState = self.bikeSpeedCadenceConnection.connectionStatus;
    }
    
    switch (connState)
    {
        case WF_SENSOR_CONNECTION_STATUS_IDLE:
            // connect the sensor.
            [self connectSpeedCadenceSensor];
            break;
            
        case WF_SENSOR_CONNECTION_STATUS_CONNECTING:
        case WF_SENSOR_CONNECTION_STATUS_CONNECTED:
            // disconnect the sensor.
            NSLog(@"disconnectSpeedCadenceSensor:");
            
            [self.bikeSpeedCadenceConnection disconnect];
            
            // update the display.
            [self updateSpeedCadenceConnectState]; 
            break;
            
        case WF_SENSOR_CONNECTION_STATUS_DISCONNECTING:
        case WF_SENSOR_CONNECTION_STATUS_INTERRUPTED:
            // do nothing.
            break;
    }
}

//--------------------------------------------------------------------------------
-(void)connectSpeedCadenceSensor
{
    // create the connection params.
    WFConnectionParams* params = nil;
    
    // if wildcard search is specified, create empty connection params.
    if ( self.connectSCWildcardCheckBox.state == NSOnState )
    {
        params = [[[WFConnectionParams alloc] init] autorelease];
        params.sensorType = WF_SENSORTYPE_BIKE_SPEED_CADENCE;
        params.networkType = WF_NETWORKTYPE_BTLE;
    }
    // otherwise, get the params from the stored settings.
    else
    {
        params = [[WFHardwareConnector sharedConnector].settings connectionParamsForSensorType:WF_SENSORTYPE_BIKE_SPEED_CADENCE];
    }
    
    if ( params != nil)
    {
        // set the search timeout.
        params.searchTimeout = [WFHardwareConnector sharedConnector].settings.searchTimeout;
    }
        
    WFSensorConnection *speedConn = [[WFHardwareConnector sharedConnector] requestSensorConnection:params];
    self.requestDateSC = [NSDate date];
    
    if ([speedConn isKindOfClass:[WFBikeSpeedCadenceConnection class]])
    {
        self.bikeSpeedCadenceConnection = (WFBikeSpeedCadenceConnection *)speedConn;
    }
    self.bikeSpeedCadenceConnection.delegate = self;
    
    // update the display.
    [self updateSpeedCadenceConnectState];
    
}

//--------------------------------------------------------------------------------
- (void) updateSpeedCadenceConnectState
{
    NSLog(@"updateSpeedCadenceConnectState");
    
    // update the connnection state on the display
    
	// get the current connection status.
	WFSensorConnectionStatus_t connState = WF_SENSOR_CONNECTION_STATUS_IDLE;
    
	if ( self.bikeSpeedCadenceConnection != nil )
	{
		connState = self.bikeSpeedCadenceConnection.connectionStatus;
	}
	
	// set the button state and status string based on the connection state.
	switch (connState)
	{
		case WF_SENSOR_CONNECTION_STATUS_IDLE:
			[self.connectSCButton setTitle:@"Connect SC"];
			break;
		case WF_SENSOR_CONNECTION_STATUS_CONNECTING:
			[self.connectSCButton setTitle:@"Cancel"];
			break;
		case WF_SENSOR_CONNECTION_STATUS_CONNECTED:
			[self.connectSCButton setTitle:@"Disconnect"];
			break;
		case WF_SENSOR_CONNECTION_STATUS_DISCONNECTING:
			[self.connectSCButton setTitle:@"Disconnecting..."];
			break;
        case WF_SENSOR_CONNECTION_STATUS_INTERRUPTED:
            // do nothing
            break;
        default:
            // do nothing
            break;
	}
}

//--------------------------------------------------------------------------------
-(void)resetSpeedCadenceDisplay
{
    [self.scSpeedLabel setStringValue:@"n/a"];
    [self.scCadenceLabel setStringValue:@"n/a"];
}


@end
