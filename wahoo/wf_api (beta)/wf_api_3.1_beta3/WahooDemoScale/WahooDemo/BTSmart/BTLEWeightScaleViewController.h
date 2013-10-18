//{LEGAL_DISCLAIMER}
//
//  WeightScaleViewController.h
//  FisicaDemo
//
//  Created by Michael Moore on 4/5/10.
//  Copyright 2010 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "WFSensorCommonViewController.h"
#import "BTLEWeightScaleUserViewController.h"


@interface BTLEWeightScaleViewController : WFSensorCommonViewController<WFWeightScaleDelegate, UITableViewDelegate, UITableViewDataSource, BTLEWeightScaleUserDelegate, UIAlertViewDelegate>
{
    UILabel* bodyWeightLabel;
    NSArray* weightHistory;
    NSArray* userRecords;
    WFBTLEWeightScaleUser* userRecordToStore;
    BOOL userRecordReadyToStore;
}


//common
@property (readonly, nonatomic) WFWeightScaleConnection* weightScaleConnection;

@property (retain, nonatomic) IBOutlet UIView *bottomView;
@property (retain, nonatomic) IBOutlet UISegmentedControl *bottomViewSegControl;

- (IBAction)bottomViewSegControlChanged:(id)sender;


//history view
@property (retain, nonatomic) IBOutlet UIView *historyView;
@property (retain, nonatomic) IBOutlet UITableView *historyTableView;

- (IBAction)historyButtonClicked:(id)sender;
- (IBAction)clearHistoryClicked:(id)sender;
- (IBAction)clearSelectedClicked:(id)sender;
- (IBAction)assignIdButtonClicked:(id)sender;
- (IBAction)getSinceButtonClicked:(id)sender;



//users view
@property (retain, nonatomic) IBOutlet UIView *userView;
@property (retain, nonatomic) IBOutlet UITableView *usersTableView;

- (IBAction)newUserClicked:(id)sender;
- (IBAction)clearUsersClicked:(id)sender;
- (IBAction)clearSelectedUserClicked:(id)sender;
- (IBAction)getAllUsersClicked:(id)sender;



//misc view
@property (retain, nonatomic) IBOutlet UIView *miscView;
@property (nonatomic, retain) IBOutlet UILabel* bodyWeightLabel;
@property (retain, nonatomic) IBOutlet UILabel *bmiLabel;

- (IBAction)unitsControlValueChanged:(UISegmentedControl *)sender;

@end
