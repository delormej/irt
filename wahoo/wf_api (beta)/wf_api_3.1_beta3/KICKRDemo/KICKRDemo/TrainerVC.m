//
//  TrainerVC.m
//  KICKRDemo
//
//  Created by Michael Moore on 8/18/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import "TrainerVC.h"

@interface TrainerVC ()

@end

@implementation TrainerVC

@synthesize pwmModeLevelField;
@synthesize standardLevelField;
@synthesize ergWattsField;
@synthesize weightField;
@synthesize simCwField;
@synthesize simCrrField;
@synthesize simGradeField;

@synthesize simWindSpeedField;
@synthesize wheelCircField;

@synthesize powerLabel;
@synthesize speedLabel;


#pragma mark -
#pragma mark UIViewController Implementation

//--------------------------------------------------------------------------------
- (void)dealloc
{
    [pwmModeLevelField release];
    [standardLevelField release];
    [ergWattsField release];
    [weightField release];
    [simCrrField release];
    [simCwField release];
    [simGradeField release];
    
    [simWindSpeedField release];
    [wheelCircField release];
    
    [powerLabel release];
    [speedLabel release];
    
    [super dealloc];
}

//--------------------------------------------------------------------------------
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self)
    {
        applicableNetworks = WF_NETWORKTYPE_BTLE;
    }
    return self;
}

//--------------------------------------------------------------------------------
- (void)viewDidLoad
{
    [super viewDidLoad];
    
    sensorType = WF_SENSORTYPE_BIKE_POWER;
    self.navigationItem.title = @"Trainer";
}

//--------------------------------------------------------------------------------
- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

//--------------------------------------------------------------------------------
- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    WFBikePowerConnection* bpc = self.bikePowerConnection;
    if ( bpc )
    {
        bpc.btDelegate = self;
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
    }
}

//--------------------------------------------------------------------------------
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


#pragma mark -
#pragma mark WFBikeTrainerDelegate Implementation

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerMode:(WFBikeTrainerMode_t)eMode status:(BOOL)bSuccess
{
    NSString* msg = [NSString stringWithFormat:@"Mode: %d\nSuccess: %@", eMode, bSuccess?@"TRUE":@"FALSE"];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"SET TRAINER MODE" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];
    [alert release];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerGrade:(BOOL)bSuccess
{
    NSString* msg = [NSString stringWithFormat:@"Success: %@", bSuccess?@"TRUE":@"FALSE"];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"SET GRADE" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];
    [alert release];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerWindResistance:(BOOL)bSuccess
{
    NSString* msg = [NSString stringWithFormat:@"Success: %@", bSuccess?@"TRUE":@"FALSE"];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"SET Cw" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];
    [alert release];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerRollingResistance:(BOOL)bSuccess
{
    NSString* msg = [NSString stringWithFormat:@"Success: %@", bSuccess?@"TRUE":@"FALSE"];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"SET Crr" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];
    [alert release];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerWindSpeed:(BOOL)bSuccess
{
    NSString* msg = [NSString stringWithFormat:@"Success: %@", bSuccess?@"TRUE":@"FALSE"];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"SET Wind Speed" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];
    [alert release];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerWheelCircumference:(BOOL)bSuccess
{
    NSString* msg = [NSString stringWithFormat:@"Success: %@", bSuccess?@"TRUE":@"FALSE"];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"SET Wheel Circumference" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];
    [alert release];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didReceiveKurtSpindownResult:(ULONG)ulSpindownPeriod
{
    NSString* msg = [NSString stringWithFormat:@"Received Spindown Result:\n\nSpindown Period: %lu", ulSpindownPeriod];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Spindown Result" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];
    [alert release];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didReceiveTrainerReadModeResponse:(UCHAR)ucStatus mode:(WFBikeTrainerMode_t)eMode;
{
    NSString* msg = [NSString stringWithFormat:@"Received Read Mode response:\n\nMode: %u", eMode];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Received Response" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];
    [alert release];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didReceiveTrainerInitSpindownResponse:(BOOL)bSuccess
{
    NSString* msg = [NSString stringWithFormat:@"Received Init Spindown response:\n\nSuccess: %@", bSuccess?@"TRUE":@"FALSE"];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Received Response" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];
    [alert release];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didReceiveTrainerSpindownResult:(float)slope intercept:(float)yIntercept
{
    NSString* msg = [NSString stringWithFormat:@"Received Spindown Result:\n\nSlope: %f\nY-Intercept: %f", slope, yIntercept];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Spindown Result" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];
    [alert release];
}


#pragma mark -
#pragma mark WFSensorCommonViewController Implementation

//--------------------------------------------------------------------------------
- (void)onSensorConnected:(WFSensorConnection*)connectionInfo
{
    WFBikePowerConnection* bpc = self.bikePowerConnection;
    if ( bpc )
    {
        bpc.btDelegate = self;
    }
}

//--------------------------------------------------------------------------------
- (void)resetDisplay
{
    [super resetDisplay];
    
	powerLabel.text = @"n/a";
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
            powerLabel.text = hardwareConnector.settings.staleDataString;
        }
        else
        {
            powerLabel.text = [NSString stringWithFormat:@"%d W", btleData.instantPower];
        }
    }
	else
	{
		[self resetDisplay];
	}
}



#pragma mark -
#pragma mark TrainerVC Implementation

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

#pragma mark Event Handlers

//--------------------------------------------------------------------------------
- (IBAction)readModeClicked:(id)sender
{
    [self.bikePowerConnection trainerReadMode];
}

//--------------------------------------------------------------------------------
- (IBAction)initSpindownClicked:(id)sender
{
    [self.bikePowerConnection trainerInitSpindown];
}

//--------------------------------------------------------------------------------
- (IBAction)setPWMModeClicked:(id)sender
{
    // the trainerSetResistanceMode takes a floating point scale value between
    // zero and one.  convert the entered percentage to zero to one scale.
    // the integer PWM scale value to floating point.
    float fpScale = [pwmModeLevelField.text floatValue] / 100.0;
    [self.bikePowerConnection trainerSetResistanceMode:fpScale];
}

//--------------------------------------------------------------------------------
- (IBAction)setStandardModeClicked:(id)sender
{
    WFBikeTrainerLevel_t eLevel = (WFBikeTrainerLevel_t)[standardLevelField.text intValue];
    [self.bikePowerConnection trainerSetStandardMode:eLevel];
}

//--------------------------------------------------------------------------------
- (IBAction)setErgModeClicked:(id)sender
{
    USHORT usWatts = (USHORT)[ergWattsField.text intValue];
    [self.bikePowerConnection trainerSetErgMode:usWatts];
}

//--------------------------------------------------------------------------------
- (IBAction)setSimModeClicked:(id)sender
{
    float fWeight = [weightField.text floatValue];
    float fCrr = [simCrrField.text floatValue];
    float fCw = [simCwField.text floatValue];
    [self.bikePowerConnection trainerSetSimMode:fWeight rollingResistance:fCrr windResistance:fCw];
}

//--------------------------------------------------------------------------------
- (IBAction)setCwClicked:(id)sender
{
    float fCw = [simCwField.text floatValue];
    [self.bikePowerConnection trainerSetWindResistance:fCw];
}

//--------------------------------------------------------------------------------
- (IBAction)setGradeClicked:(id)sender
{
    float fGrade = [simGradeField.text floatValue];
    [self.bikePowerConnection trainerSetGrade:fGrade];
}

//--------------------------------------------------------------------------------
- (IBAction)setWheelCircClicked:(id)sender
{
    float circ = [wheelCircField.text floatValue];
    [self.bikePowerConnection trainerSetWheelCircumference:circ];
}

//--------------------------------------------------------------------------------
- (IBAction)setWindSpeedClicked:(id)sender
{
    float windSpeed = [simWindSpeedField.text floatValue];
    [self.bikePowerConnection trainerSetWindSpeed:windSpeed];
}

//--------------------------------------------------------------------------------
- (IBAction)textFieldDoneEditing:(id)sender
{
	[sender resignFirstResponder];
}

@end
