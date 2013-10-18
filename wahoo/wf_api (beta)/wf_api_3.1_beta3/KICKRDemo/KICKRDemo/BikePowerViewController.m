//{LEGAL_DISCLAIMER}
//
//  BikePowerViewController.m
//  KICKRDemo
//
//  Created by Michael Moore on 3/25/10.
//  Copyright 2010 Wahoo Fitness. All rights reserved.
//

#import "BikePowerViewController.h"
#import "SettingsViewController.h"
#import "TrainerVC.h"
#import "ErgModeVC.h"
#import "KICKRDemoAppDelegate.h"



/////////////////////////////////////////////////////////////////////////////
// BikePowerViewController Implementation.
/////////////////////////////////////////////////////////////////////////////

@implementation BikePowerViewController

@synthesize instantPowerLabel;
@synthesize speedLabel;

#pragma mark -
#pragma mark UIViewController Implementation

//--------------------------------------------------------------------------------
- (void)dealloc
{
	[instantPowerLabel release];
    [speedLabel release];
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
        applicableNetworks = WF_NETWORKTYPE_BTLE;
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
            self.navigationItem.title = @"KICKR";
}

#pragma mark -
#pragma mark WFSensorCommonViewController Implementation

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
        if ( TRUE )
        {
            static double prevTorque = 0;
            static NSTimeInterval prevTime = 0;
            
            if ( prevTime )
            {
                const double_t twoPi = 2*M_PI;
                double torqueOffset = btleData.accumulatedTorque - prevTorque;
                double timeOffset = btleData.accumulatedTime - prevTime;
                double pwr = (timeOffset == 0) ? 0 : (torqueOffset / timeOffset) * twoPi;
                instantPowerLabel.text = [NSString stringWithFormat:@"%1.0f", pwr];
            }
            
            prevTorque = btleData.accumulatedTorque;
            prevTime = btleData.accumulatedTime;
        }
        else if ([hardwareConnector.settings.staleDataString isEqualToString:speedLabel.text])
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



#pragma mark -
#pragma mark BikePowerViewController Implementation

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
- (IBAction)settingsClicked:(id)sender
{
    // configure and display a resistance view controller.
	SettingsViewController *vc = [[SettingsViewController alloc] initWithNibName:@"SettingsViewController" bundle:nil];
	[self.navigationController pushViewController:vc animated:TRUE];
    
	[vc release];
}

//--------------------------------------------------------------------------------
- (IBAction)trainerClicked:(id)sender
{
    // configure and display a trainer view controller.
	TrainerVC *vc = [[TrainerVC alloc] initWithNibName:@"TrainerVC" bundle:nil];
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

@end
