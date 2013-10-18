//{LEGAL_DISCLAIMER}
//
//  KICKRVC.m
//  KICKRDemo
//
//  Created by Michael Moore on 3/25/10.
//  Copyright 2010 Wahoo Fitness. All rights reserved.
//

#import "KICKRVC.h"
#import "SettingsViewController.h"
#import "StandardModeVC.h"
#import "BrakeModeVC.h"
#import "ErgModeVC.h"
#import "SimModeVC.h"
#import "KICKRDemoAppDelegate.h"

typedef enum
{
    WF_CPM_CAL_STATUS_SUCCESS           = 0x00,
    WF_CPM_CAL_STATUS_FAILED            = 0x01,
    
} wf_cpm_cal_status_t;

/////////////////////////////////////////////////////////////////////////////
// KICKRVC Implementation.
/////////////////////////////////////////////////////////////////////////////
@interface KICKRVC ()

// private methods and properties
@property (nonatomic, retain)NSURL *wahooUtilityAppURL;

@end

@implementation KICKRVC

@synthesize instantPowerLabel;
@synthesize speedLabel;

#pragma mark -
#pragma mark UIViewController Implementation

//--------------------------------------------------------------------------------
- (void)dealloc
{
	[instantPowerLabel release];
    [speedLabel release];
    [_wahooUtilityAppURL release];
    [super dealloc];
}

//--------------------------------------------------------------------------------
- (void)didReceiveMemoryWarning
{
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

//--------------------------------------------------------------------------------
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self)
    {
        sensorType = WF_SENSORTYPE_BIKE_POWER;
        applicableNetworks = WF_NETWORKTYPE_BTLE | WF_NETWORKTYPE_ANTPLUS;
    }
    return self;
}

//--------------------------------------------------------------------------------
- (void)viewDidLoad
{
    [super viewDidLoad];
    
    sensorType = WF_SENSORTYPE_BIKE_POWER;
}

//--------------------------------------------------------------------------------
- (void)viewDidUnload
{
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}

//--------------------------------------------------------------------------------
-  (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    self.navigationItem.title = @"KICKR";
    WFBikePowerConnection* bpc = self.bikePowerConnection;
    if ( bpc )
    {
        bpc.btDelegate = self;
        bpc.cpmDelegate = self;
    }

}

//--------------------------------------------------------------------------------
- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    
    WFBikePowerConnection* bpc = self.bikePowerConnection;
    if ( bpc )
    {
        bpc.btDelegate = nil;
        bpc.cpmDelegate = nil;
    }
}

#pragma mark -
#pragma mark WFSensorCommonViewController Implementation

//--------------------------------------------------------------------------------
- (void)onSensorConnected:(WFSensorConnection*)connectionInfo
{
    [super onSensorConnected:connectionInfo];
    WFBikePowerConnection* bpc = self.bikePowerConnection;
    if ( bpc )
    {
        bpc.btDelegate = self;
        bpc.cpmDelegate = self;
    }
}

//--------------------------------------------------------------------------------
- (void)resetDisplay
{
    [super resetDisplay];
    
	instantPowerLabel.text = @"n/a";
	speedLabel.text = @"n/a";
}

//--------------------------------------------------------------------------------
- (void)updateData
{
    [super updateData];
    
    WFBikePowerData* btleData = [self.bikePowerConnection getBikePowerData];
	if (btleData)
    {
        speedLabel.text = [btleData formattedSpeed:TRUE];
        if ( [hardwareConnector.settings.staleDataString isEqualToString:speedLabel.text] )
        {
            instantPowerLabel.text = hardwareConnector.settings.staleDataString;
        }
        else
        {
            instantPowerLabel.text = [NSString stringWithFormat:@"%d W", btleData.instantPower];
        }
    }
	else
	{
		[self resetDisplay];
	}
}

//--------------------------------------------------------------------------------
- (void)firmwareUpdateAvailable:(NSNotification*)notification
{
    [super firmwareUpdateAvailable:notification];

    NSLog(@"in firmwareUpdateAvailable notification received");
    NSDictionary* info = notification.userInfo;
    if ( info != nil )
    {
        // parse the user info.
        self.wahooUtilityAppURL = (NSURL *)[info objectForKey:@"wahooUtilityAppURL"];
        NSNumber *requiredNumber = (NSNumber *)[info objectForKey:@"firmwareUpdateRequired"];
        BOOL firmwareUdateRequired = [requiredNumber boolValue];
        
        // create an alert
        NSMutableString *message = [[[NSMutableString alloc] init] autorelease];
        
        if (firmwareUdateRequired)
        {
            [message appendString:@"A newer version of the KICKR software is required before you can connect. "];
        }
        else
        {
            [message appendString:@"A newer version of the KICKR software is available. "];
        }
        
        [message appendString:@"Installing the upgrade requires the Wahoo Utility app. The Wahoo Utility app will open or you will be transferred to iTunes where you can download the Wahoo Utility app.\n\nWould you like to upgrade now?"];
        
        UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"KICKR Software Update"
                                                            message:message
                                                           delegate:self
                                                  cancelButtonTitle:@"No"
                                                  otherButtonTitles:@"Yes", nil];
        [alertView show];
        [alertView autorelease];
    }
    
}



#pragma mark - UIAlertViewDelegate implementation
//--------------------------------------------------------------------------------
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    NSLog(@"button index = %d", buttonIndex);
    
	// check if the "Yes" button was selected.
	if (buttonIndex == 0)
	{
        // button index == 0 is typically the cancel button
        // allow the cancel        
	}
    else if (buttonIndex == 1)
    {
        if ([alertView.title isEqualToString:@"KICKR Software Update"])
        {
            // YES button selected to update firmware, open the Wahoo Utility App URL
            if (self.wahooUtilityAppURL != nil)
            {
                [[UIApplication sharedApplication] openURL:self.wahooUtilityAppURL];
            }
        }
    }
}



#pragma mark -
#pragma mark KICKRVC Implementation

#pragma mark Properties

//--------------------------------------------------------------------------------
- (WFBikePowerConnection*)bikePowerConnection
{
	WFBikePowerConnection* retVal = nil;
	if ( [self.sensorConnection isKindOfClass:[WFBikePowerConnection class]] )
	{
		retVal = (WFBikePowerConnection*)self.sensorConnection;
	}
	
	return retVal;
}

#pragma mark -
#pragma mark WFBikeTrainerDelegate Implementation


//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didReceiveCalibrationResponse:(UCHAR)ucStatus offset:(USHORT)usOffset temperature:(SCHAR)scTemp
{
    wf_cpm_cal_status_t e_status = (wf_cpm_cal_status_t)ucStatus;
    if ( e_status == WF_CPM_CAL_STATUS_SUCCESS )
    {
        NSString* cal = [NSString stringWithFormat:@"Zero Offset: %d\nTemperature: %d C",usOffset,scTemp];
        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Calibration Response" message:cal delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
        [alert release];
        alert = nil;
    }
    else // error
    {
        NSString* err = @"Make Sure you are not moving and try again!";
        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Calibration Error" message:err delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
        [alert release];
        alert = nil;
    }
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didReceiveTrainerInitSpindownResponse:(UCHAR)ucStatus
{
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
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:title message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];
    [alert release];
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
    
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:title message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];
    [alert release];
    
    // NOTE: At the end of spindown the device resets to Standard Mode level 2.
    // The app or the user may want to set a different mode or level.
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerMode:(WFBikeTrainerMode_t)eMode status:(UCHAR)ucStatus
{
    NSString* msg = [NSString stringWithFormat:@"Mode: %d\nSuccess: %@", eMode, (ucStatus==0)?@"TRUE":@"FALSE"];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"SET TRAINER MODE" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];
    [alert release];
}

#pragma mark Event Handlers

//--------------------------------------------------------------------------------
- (IBAction)initSpindownClicked:(id)sender
{
    [self.bikePowerConnection trainerInitSpindown];
}

//--------------------------------------------------------------------------------
- (IBAction)manualZeroClicked:(id)sender
{
    [self.bikePowerConnection setManualZeroCalibration];
}

//--------------------------------------------------------------------------------
- (IBAction)settingsClicked:(id)sender
{
    // configure and display a resistance view controller.
	SettingsViewController *vc = [[SettingsViewController alloc] initWithNibName:@"SettingsViewController" bundle:nil];
	[self.navigationController pushViewController:vc animated:TRUE];
    
	[vc release];
}

//--------------------------------------------------------------------------------
- (IBAction)standardModeClicked:(id)sender
{
    // configure and display a sim mode view controller.
	StandardModeVC *vc = [[StandardModeVC alloc] initWithNibName:@"StandardModeVC" bundle:nil];
	[self.navigationController pushViewController:vc animated:TRUE];
    
	[vc release];
}

//--------------------------------------------------------------------------------
- (IBAction)brakeModeClicked:(id)sender
{
    // configure and display a trainer view controller.
	BrakeModeVC *vc = [[BrakeModeVC alloc] initWithNibName:@"BrakeModeVC" bundle:nil];
	[self.navigationController pushViewController:vc animated:TRUE];
    
	[vc release];
}
//--------------------------------------------------------------------------------
- (IBAction)ergModeClicked:(id)sender
{
    // configure and display a trainer view controller.
	ErgModeVC *vc = [[ErgModeVC alloc] initWithNibName:@"ErgModeVC" bundle:nil];
	[self.navigationController pushViewController:vc animated:TRUE];
    
	[vc release];
}
//--------------------------------------------------------------------------------
- (IBAction)simModeClicked:(id)sender
{
    // configure and display a trainer view controller.
	SimModeVC *vc = [[SimModeVC alloc] initWithNibName:@"SimModeVC" bundle:nil];
	[self.navigationController pushViewController:vc animated:TRUE];
    
	[vc release];
}


@end
