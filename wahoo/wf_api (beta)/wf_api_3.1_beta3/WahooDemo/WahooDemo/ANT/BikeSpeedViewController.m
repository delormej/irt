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
//  BikeSpeedViewController.m
//  FisicaDemo
//
//  Created by Michael Moore on 2/23/10.
//  Copyright 2010 Wahoo Fitness. All rights reserved.
//

#import "BikeSpeedViewController.h"


@implementation BikeSpeedViewController

@synthesize lastSpeedTimeLabel;
@synthesize totalSpeedRevolutionsLabel;
@synthesize computedSpeedLabel;
@synthesize averageSpeedLabel;
@synthesize distanceLabel;


#pragma mark -
#pragma mark UIViewController Implementation

//--------------------------------------------------------------------------------
- (void)dealloc
{
	[lastSpeedTimeLabel release];
	[totalSpeedRevolutionsLabel release];
	[computedSpeedLabel release];
	[averageSpeedLabel release];
    [distanceLabel release];
    
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
        sensorType = WF_SENSORTYPE_BIKE_SPEED;
    }
    
    return self;
}

//--------------------------------------------------------------------------------
- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.navigationItem.title = @"Bike Speed";
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
    
	lastSpeedTimeLabel.text = @"n/a";
	totalSpeedRevolutionsLabel.text = @"n/a";
	averageSpeedLabel.text = @"n/a";
	computedSpeedLabel.text = @"n/a";
    distanceLabel.text = @"n/a";
}

//--------------------------------------------------------------------------------
- (void)updateData
{
    [super updateData];
    
	WFBikeSpeedData* bsData = [self.bikeSpeedConnection getBikeSpeedData];
	if ( bsData != nil )
	{
        // update basic data.
		lastSpeedTimeLabel.text = [NSString stringWithFormat:@"%3.3f", bsData.accumSpeedTime];
		totalSpeedRevolutionsLabel.text = [NSString stringWithFormat:@"%ld", bsData.accumWheelRevolutions];
        
        computedSpeedLabel.text = [bsData formattedSpeed:TRUE];
        distanceLabel.text = [bsData formattedDistance:TRUE];
		
        // calculate the average speed.
		if (bsData.accumSpeedTime > 0)
		{
            // use wheel revs, time and wheel circumference to calculate speed.
			float averageSpeed = ((float) bsData.accumWheelRevolutions / bsData.accumSpeedTime) * 0.0771743 * 60; //miles per hour
			averageSpeedLabel.text = [NSString	stringWithFormat:@"%0.0f", (float) averageSpeed];
		}
        
        /*
         * this demonstrates calculating speed manually from unformated values.
         
        // calculate the speed.
        //
		// API provides wheel cadence in RPM's, need to multiply by circumference(6.79ft) or metric and 60 minutes
		// Be sure and add Wheel Size variable somewhere in App
		
		computedSpeedLabel.text = [NSString stringWithFormat:@"%0.0f", (float) bsData.instantWheelRPM * 0.0771743];
        */
	}
	else
	{
		[self resetDisplay];
	}
}



#pragma mark -
#pragma mark BikeSpeedViewController Implementation

#pragma mark Properties

//--------------------------------------------------------------------------------
- (WFBikeSpeedConnection*)bikeSpeedConnection
{
	WFBikeSpeedConnection* retVal = nil;
	if ( [self.sensorConnection isKindOfClass:[WFBikeSpeedConnection class]] )
	{
		retVal = (WFBikeSpeedConnection*)self.sensorConnection;
	}
	
	return retVal;
}


#pragma mark Event Handlers

//--------------------------------------------------------------------------------

@end
