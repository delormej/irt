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
//  FootpodViewController.m
//  FisicaDemo
//
//  Created by Michael Moore on 2/23/10.
//  Copyright 2010 Wahoo Fitness. All rights reserved.
//

#import "FootpodViewController.h"


@implementation FootpodViewController

@synthesize lastTimeLabel;
@synthesize distanceLabel;
@synthesize instantaneousSpeedLabel;
@synthesize strideCountLabel;
@synthesize latencyLabel;
@synthesize cadenceLabel;
@synthesize moduleLocationLabel;
@synthesize unitHealthLabel;
@synthesize useStateLabel;
@synthesize accumulatedDistanceLabel;
@synthesize accumulatedStrideLabel;



#pragma mark -
#pragma mark UIViewController Implementation

//--------------------------------------------------------------------------------
- (void)dealloc
{
	[lastTimeLabel release];
	[distanceLabel release];
	[instantaneousSpeedLabel release];
	[strideCountLabel release];
	[latencyLabel release];
	[cadenceLabel release];
	[moduleLocationLabel release];
	[unitHealthLabel release];
	[useStateLabel release];
	[accumulatedDistanceLabel release];
	[accumulatedStrideLabel release];
	
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
    if ( (self=[super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) )
    {
        sensorType = WF_SENSORTYPE_FOOTPOD;
    }
    
    return self;
}

//--------------------------------------------------------------------------------
- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.navigationItem.title = @"Stride Sensor";
}

//--------------------------------------------------------------------------------
- (void)viewDidUnload
{
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


#pragma mark -
#pragma mark WFSensorCommonViewController Implementation

//--------------------------------------------------------------------------------
- (void)resetDisplay
{
    [super resetDisplay];
    
	accumulatedDistanceLabel.text = @"n/a";
	instantaneousSpeedLabel.text = @"n/a";
	lastTimeLabel.text = @"n/a";
	distanceLabel.text = @"n/a";
	strideCountLabel.text = @"n/a";
	latencyLabel.text = @"n/a";
	cadenceLabel.text = @"n/a";
	moduleLocationLabel.text = @"n/a";
	unitHealthLabel.text = @"n/a";
	useStateLabel.text = @"n/a";
	accumulatedStrideLabel.text = @"n/a";
}

//--------------------------------------------------------------------------------
- (void)updateData
{
    [super updateData];
    
	WFFootpodData* fpData = [self.footpodConnection getFootpodData];
	WFFootpodRawData* fpRawData = [self.footpodConnection getFootpodRawData];
	if ( fpData != nil )
	{
        // basic data.
        accumulatedDistanceLabel.text = [fpData formattedDistance:TRUE];
        instantaneousSpeedLabel.text = [fpData formattedPace:TRUE];
        cadenceLabel.text = [fpData formattedCadence:TRUE];
        
        /*
         * this demonstrates computing speed, pace and distance manually, using unformatted values.
         *
         
		// pretty up distance data, walk around and test it... (Nike+ accuracy is plus or minus 20%, ANT+ is plus or minus 2%, really...)
		if (fpData.accumulatedDistance < 304)  // less than 1000ft, use ft, otherwise convert to miles
		{
			accumulatedDistanceLabel.text = [NSString stringWithFormat:@"%1.0f ft",fpData.accumulatedDistance / .3048];
		}
		else accumulatedDistanceLabel.text = [NSString stringWithFormat:@"%1.2f mi",fpData.accumulatedDistance / 1609.344];
		
		// pretty up pace data, walk around and test it...(sorry no metric units) 
		if (fpData.instantaneousSpeed < .01) // you are stopped min/mi should be infinity
		{
			instantaneousSpeedLabel.text = [NSString stringWithFormat:@"n/a"];
		}
		else instantaneousSpeedLabel.text = [NSString stringWithFormat:@"%1.1f", 26.8224 / fpData.instantaneousSpeed];

        cadenceLabel.text = [NSString stringWithFormat:@"%1.1f", fpRawData.cadence];
        */

		// other data
		lastTimeLabel.text = [NSString stringWithFormat:@"%1.1f", fpRawData.lastTime];
		distanceLabel.text = [NSString stringWithFormat:@"%1.1f", fpRawData.distance];
		strideCountLabel.text = [NSString stringWithFormat:@"%d", fpRawData.strideCount];
		latencyLabel.text = [NSString stringWithFormat:@"%d", fpRawData.latency];
		moduleLocationLabel.text = [NSString stringWithFormat:@"%d", fpRawData.moduleLocation];
		unitHealthLabel.text = [NSString stringWithFormat:@"%d", fpRawData.unitHealth];
		useStateLabel.text = [NSString stringWithFormat:@"%d", fpRawData.useState];
		accumulatedStrideLabel.text = [NSString stringWithFormat:@"%ld", fpData.accumulatedStride];
	}
	else
	{
		[self resetDisplay];
	}
}



#pragma mark -
#pragma mark FootpodViewController Implementation

#pragma mark Properties

//--------------------------------------------------------------------------------
- (WFFootpodConnection*)footpodConnection
{
	WFFootpodConnection* retVal = nil;
	if ( [self.sensorConnection isKindOfClass:[WFFootpodConnection class]] )
	{
		retVal = (WFFootpodConnection*)self.sensorConnection;
	}
	
	return retVal;
}


#pragma mark Event Handlers

//--------------------------------------------------------------------------------

@end
