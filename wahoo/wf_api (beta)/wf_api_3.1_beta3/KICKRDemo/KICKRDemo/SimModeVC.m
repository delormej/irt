//
//  SimModeVC.m
//  KICKRDemo
//
//  Created by Michael Moore on 8/18/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import "SimModeVC.h"

@interface SimModeVC ()

@end

@implementation SimModeVC

@synthesize weightField;
@synthesize simCField;
@synthesize simCInitField;
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
    [weightField release];
    [simCrrField release];
    [simCField release];
    [simCInitField release];
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
        applicableNetworks = WF_NETWORKTYPE_BTLE | WF_NETWORKTYPE_ANTPLUS;
    }
    return self;
}

//--------------------------------------------------------------------------------
- (void)viewDidLoad
{
    [super viewDidLoad];
    
    sensorType = WF_SENSORTYPE_BIKE_POWER;
    self.navigationItem.title = @"Sim Mode";
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
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerMode:(WFBikeTrainerMode_t)eMode status:(UCHAR)ucStatus
{
    NSString* msg = [NSString stringWithFormat:@"Mode: %d\nSuccess: %@", eMode, (ucStatus==0)?@"TRUE":@"FALSE"];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"SET TRAINER MODE" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];
    [alert release];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerGrade:(UCHAR)ucStatus
{
    NSString* msg = [NSString stringWithFormat:@"Success: %@", (ucStatus==0)?@"TRUE":@"FALSE"];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"SET GRADE" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];
    [alert release];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerWindResistance:(UCHAR)ucStatus
{
    NSString* msg = [NSString stringWithFormat:@"Success: %@", (ucStatus==0)?@"TRUE":@"FALSE"];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"SET Cw" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];
    [alert release];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerRollingResistance:(UCHAR)ucStatus
{
    NSString* msg = [NSString stringWithFormat:@"Success: %@", (ucStatus==0)?@"TRUE":@"FALSE"];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"SET Crr" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];
    [alert release];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerWindSpeed:(UCHAR)ucStatus
{
    NSString* msg = [NSString stringWithFormat:@"Success: %@", (ucStatus==0)?@"TRUE":@"FALSE"];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"SET Wind Speed" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];
    [alert release];
}

//--------------------------------------------------------------------------------
- (void)cpmConnection:(WFBikePowerConnection*)cpmConn didSetTrainerWheelCircumference:(UCHAR)ucStatus
{
    NSString* msg = [NSString stringWithFormat:@"Success: %@", (ucStatus==0)?@"TRUE":@"FALSE"];
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"SET Wheel Circumference" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
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
#pragma mark SimModeVC Implementation

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
- (IBAction)setSimModeClicked:(id)sender
{
    [self.simCrrField resignFirstResponder];
    [self.simCInitField resignFirstResponder];
    [self.weightField resignFirstResponder];
    float fWeight = [weightField.text floatValue];
    float fCrr = [simCrrField.text floatValue];
    float fC = [simCInitField.text floatValue];
    [self.bikePowerConnection trainerSetSimMode:fWeight rollingResistance:fCrr windResistance:fC];
}

//--------------------------------------------------------------------------------
- (IBAction)setCClicked:(id)sender
{
    [self.simCField resignFirstResponder];
    float fC = [simCField.text floatValue];
    [self.bikePowerConnection trainerSetWindResistance:fC];
}

//--------------------------------------------------------------------------------
- (IBAction)setGradeClicked:(id)sender
{
    [self.simGradeField resignFirstResponder];
    float fGrade = [simGradeField.text floatValue];
    [self.bikePowerConnection trainerSetGrade:fGrade];
}

//--------------------------------------------------------------------------------
- (IBAction)setWheelCircClicked:(id)sender
{
    [self.wheelCircField resignFirstResponder];
    float circ = [wheelCircField.text floatValue];
    [self.bikePowerConnection trainerSetWheelCircumference:circ];
}

//--------------------------------------------------------------------------------
- (IBAction)setWindSpeedClicked:(id)sender
{
    [self.simWindSpeedField resignFirstResponder];
    float windSpeed = [simWindSpeedField.text floatValue];
    [self.bikePowerConnection trainerSetWindSpeed:windSpeed];
}

//--------------------------------------------------------------------------------
- (IBAction)textFieldDoneEditing:(id)sender
{
	[sender resignFirstResponder];
    if (sender == self.simWindSpeedField)
    {
        [self setWindSpeedClicked:nil];
    }
    else if (sender == self.simCField)
    {
        [self setCClicked:nil];
    }
    else if (sender == self.simGradeField)
    {
        [self setGradeClicked:nil];
    }
    else if (sender == self.wheelCircField)
    {
        [self setWheelCircClicked:nil];
    }
    else if (sender == self.weightField || sender == self.simCrrField || sender == self.simCInitField)
    {
        [self setSimModeClicked:nil];
    }
}

@end
