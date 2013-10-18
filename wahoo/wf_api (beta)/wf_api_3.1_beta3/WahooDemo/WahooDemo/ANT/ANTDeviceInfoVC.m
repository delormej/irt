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
//  ANTDeviceInfoVC.m
//  WahooDemo
//
//  Created by Michael Moore on 2/28/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import "ANTDeviceInfoVC.h"


@interface ANTDeviceInfoVC (_PRIVATE_)

- (NSString*)stringFromBattStatus:(WFBatteryStatus_t)battStatus;

@end



@implementation ANTDeviceInfoVC

@synthesize commonData;

@synthesize manufacturerIdLabel;
@synthesize modelNumberLabel;
@synthesize hardwareVerLabel;
@synthesize softwareVerLabel;
@synthesize serialUpperLabel;
@synthesize serialLowerLabel;
@synthesize serialNumberLabel;
@synthesize operatingTimeLabel;
@synthesize battVoltageLabel;
@synthesize battStatusLabel;


#pragma mark -
#pragma mark UIViewController Implementation

//--------------------------------------------------------------------------------
- (void)dealloc
{
    [commonData release];
    
    [manufacturerIdLabel release];
    [modelNumberLabel release];
    [hardwareVerLabel release];
    [softwareVerLabel release];
    [serialUpperLabel release];
    [serialLowerLabel release];
    [serialNumberLabel release];
    [operatingTimeLabel release];
    [battVoltageLabel release];
    [battStatusLabel release];
    
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
- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.navigationItem.title = @"Device Info";
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
    
    // update the display data.
    [self updateDisplay];
}

//--------------------------------------------------------------------------------
- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}

//--------------------------------------------------------------------------------
- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
}

//--------------------------------------------------------------------------------
- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
}

//--------------------------------------------------------------------------------
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


#pragma mark -
#pragma mark ANTDeviceInfoVC Implementation

#pragma mark Private Methods

//--------------------------------------------------------------------------------
- (NSString*)stringFromBattStatus:(WFBatteryStatus_t)battStatus
{
    NSString* retVal = @"n/a";
    switch ( battStatus )
    {
        case WF_BATTERY_STATUS_NOT_AVAILABLE:
            retVal = @"N/A";
            break;
        case WF_BATTERY_STATUS_NEW:
            retVal = @"New";
            break;
        case WF_BATTERY_STATUS_GOOD:
            retVal = @"Good";
            break;
        case WF_BATTERY_STATUS_OK:
            retVal = @"OK";
            break;
        case WF_BATTERY_STATUS_LOW:
            retVal = @"Low";
            break;
        case WF_BATTERY_STATUS_CRITICAL:
            retVal = @"Critical";
            break;
    }
    
    return retVal;
}

#pragma mark Public Methods

//--------------------------------------------------------------------------------
- (void)updateDisplay
{
    // update the display data.
    manufacturerIdLabel.text = [NSString stringWithFormat:@"%u", commonData.manufacturerId];
    modelNumberLabel.text = [NSString stringWithFormat:@"%u", commonData.modelNumber];
    hardwareVerLabel.text = [NSString stringWithFormat:@"%u", commonData.hardwareVersion];
    softwareVerLabel.text = [NSString stringWithFormat:@"%u", commonData.softwareVersion];
    serialUpperLabel.text = [NSString stringWithFormat:@"%u", commonData.serialNumberUpper];
    serialLowerLabel.text = [NSString stringWithFormat:@"%u", commonData.serialNumberLower];
    serialNumberLabel.text = [NSString stringWithFormat:@"%lu", commonData.serialNumber];
    operatingTimeLabel.text = [NSString stringWithFormat:@"%lu", commonData.operatingTime];
    battVoltageLabel.text = (commonData.batteryVoltage==WF_COMMON_BATTERY_INVALID) ? @"n/a" : [NSString stringWithFormat:@"%1.2f V", commonData.batteryVoltage];
    battStatusLabel.text = [self stringFromBattStatus:commonData.batteryStatus];
}

@end
