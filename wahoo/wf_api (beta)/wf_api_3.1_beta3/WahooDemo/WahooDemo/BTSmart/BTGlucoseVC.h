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
//  BTGlucoseVC.h
//  WahooDemo
//
//  Created by Michael Moore on 2/22/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "WFSensorCommonViewController.h"



@interface BTGlucoseVC : WFSensorCommonViewController <WFBTLEGlucoseDelegate, UITableViewDelegate, UITableViewDataSource>
{
    NSMutableArray* records;
    UITableView* recordTable;
}


@property (readonly, nonatomic) WFBTLEGlucoseConnection* glucoseConnection;
@property (nonatomic, retain) IBOutlet UITableView* recordTable;


- (IBAction)retrieveFirstClicked:(id)sender;
- (IBAction)retrieveLastClicked:(id)sender;
- (IBAction)retrieveAllClicked:(id)sender;

@end
