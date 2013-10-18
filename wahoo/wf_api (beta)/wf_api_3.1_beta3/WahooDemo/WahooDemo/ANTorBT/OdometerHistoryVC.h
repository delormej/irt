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
//  OdometerHistory.h
//  WahooDemo
//
//  Created by Michael Moore on 3/29/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <WFConnector/WFConnector.h>


@interface OdometerHistoryVC : UIViewController <UITableViewDelegate, UITableViewDataSource, WFBikeSpeedCadenceDelegate>
{
    WFOdometerHistory* odometerHistory;
    UITableView* odometerTable;
    
    NSMutableArray* odometerReadings;
    WFBikeSpeedCadenceConnection* bscConnection;
}


@property (nonatomic, retain) WFBikeSpeedCadenceConnection* bscConnection;
@property (nonatomic, retain) WFOdometerHistory* odometerHistory;
@property (nonatomic, retain) IBOutlet UITableView* odometerTable;


- (IBAction)resetOdometerClicked:(id)sender;
- (IBAction)requestHistoryClicked:(id)sender;

@end
