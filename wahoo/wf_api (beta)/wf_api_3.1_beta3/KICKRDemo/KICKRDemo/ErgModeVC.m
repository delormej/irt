//
//  ErgModeVC.m
//  KICKRDemo
//
//  Created by Michael Moore on 8/18/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import "ErgModeVC.h"

@interface ErgModeVC ()

@end

@implementation ErgModeVC

@synthesize hundredsField;
@synthesize tensField;
@synthesize onesField;

@synthesize powerLabel;
@synthesize speedLabel;


#pragma mark -
#pragma mark UIViewController Implementation

//--------------------------------------------------------------------------------
- (void)dealloc
{
    [hundredsField release];
    [tensField release];
    [onesField release];
    
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
    self.navigationItem.title = @"Erg Mode";
    [self setWatts];


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
    // Show an error if the Wattage setpoint change fails
    if ( ucStatus != 0 )
    {
        NSString* msg = [NSString stringWithFormat:@"Mode: %d\nSuccess: %@", eMode, (ucStatus==0)?@"TRUE":@"FALSE"];
        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"SET TRAINER MODE" message:msg delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
        [alert show];
        [alert release];
    }
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
#pragma mark ErgModeVC Implementation

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
- (IBAction)plusHundredsClicked:(id)sender
{
    USHORT usHundreds = (USHORT)[hundredsField.text intValue] + 1;
    if(usHundreds > 9)
    {
        usHundreds = 0;
    }
    hundredsField.text = [NSString stringWithFormat:@"%d", usHundreds];
    [self setWatts];
}

//--------------------------------------------------------------------------------
- (IBAction)plusTensClicked:(id)sender
{
    USHORT usTens = (USHORT)[tensField.text intValue] + 1;
    if(usTens > 9)
    {
        usTens = 0;
    }
    tensField.text = [NSString stringWithFormat:@"%d", usTens];
    [self setWatts];
}

//--------------------------------------------------------------------------------
- (IBAction)plusOnesClicked:(id)sender
{
    USHORT usOnes = (USHORT)[onesField.text intValue] + 1;
    if(usOnes > 9)
    {
        usOnes = 0;
    }
    onesField.text = [NSString stringWithFormat:@"%d", usOnes];
    [self setWatts];
}
//--------------------------------------------------------------------------------
- (IBAction)minusHundredsClicked:(id)sender
{
    USHORT usHundreds = (USHORT)[hundredsField.text intValue];
    if(usHundreds == 0)
    {
        usHundreds = 9;
    }
    else
    {
        usHundreds = usHundreds - 1;
    }
    hundredsField.text = [NSString stringWithFormat:@"%d", usHundreds];
    [self setWatts];
}

//--------------------------------------------------------------------------------
- (IBAction)minusTensClicked:(id)sender
{
    USHORT usTens = (USHORT)[tensField.text intValue];
    if(usTens == 0)
    {
        usTens = 9;
    }
    else
    {
        usTens = usTens - 1;
    }
    tensField.text = [NSString stringWithFormat:@"%d", usTens];
    [self setWatts];
}

//--------------------------------------------------------------------------------
- (IBAction)minusOnesClicked:(id)sender
{
    USHORT usOnes = (USHORT)[onesField.text intValue];
    if(usOnes == 0)
    {
        usOnes = 9;
    }
    else
    {
        usOnes = usOnes - 1;
    }
    onesField.text = [NSString stringWithFormat:@"%d", usOnes];
    [self setWatts];
}

//--------------------------------------------------------------------------------
- (void)setWatts
{
    USHORT usHundreds = (USHORT)[hundredsField.text intValue];
    USHORT usTens = (USHORT)[tensField.text intValue];
    USHORT usOnes = (USHORT)[onesField.text intValue];
    USHORT usWatts = usHundreds * 100 + usTens * 10 + usOnes;
    [self.bikePowerConnection trainerSetErgMode:usWatts];
}
//--------------------------------------------------------------------------------
- (IBAction)textFieldDoneEditing:(id)sender
{
	[sender resignFirstResponder];
}

@end
