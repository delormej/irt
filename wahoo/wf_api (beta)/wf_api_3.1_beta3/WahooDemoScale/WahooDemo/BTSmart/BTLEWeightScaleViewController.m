//{LEGAL_DISCLAIMER}
//
//  WeightScaleViewController.m
//  FisicaDemo
//
//  Created by Michael Moore on 4/5/10.
//  Copyright 2010 Wahoo Fitness. All rights reserved.
//

#import "BTLEWeightScaleViewController.h"



@interface BTLEWeightScaleViewController (_PRIVATE_)



@end


@implementation BTLEWeightScaleViewController
{
    
    BTLEWeightScaleUserViewController* userVC;
    int selectedRow;
    NSTimeInterval getSinceTimestamp;
}
@synthesize historyView;
@synthesize historyTableView;
@synthesize userView;
@synthesize usersTableView;
@synthesize miscView;

@synthesize weightScaleConnection;
@synthesize bodyWeightLabel;
@synthesize bmiLabel;
@synthesize bottomView;
@synthesize bottomViewSegControl;


#pragma mark -
#pragma mark UIViewController Implementation

//--------------------------------------------------------------------------------
- (void)dealloc
{
	[bodyWeightLabel release];
    [bottomView release];
    [bottomViewSegControl release];
    [miscView release];
    [userView release];
    [historyView release];
    [historyTableView release];
    [usersTableView release];
    [bmiLabel release];
    [super dealloc];
}

//--------------------------------------------------------------------------------
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

//--------------------------------------------------------------------------------
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    if ( (self=[super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) )
    {
    }
    
    return self;
}

//--------------------------------------------------------------------------------
- (void)viewDidLoad
{
    [super viewDidLoad];

    // set the sensor type for connection
    sensorType = WF_SENSORTYPE_WEIGHT_SCALE;
    
    selectedRow = -1;

    // configure the shared hardware connector
    hardwareConnector = [WFHardwareConnector sharedConnector];
    
	hardwareConnector.sampleRate = 0.25;  
    
    // determine support for BTLE.
    if ( hardwareConnector.hasBTLESupport )
    {
        // enable BTLE.
        [hardwareConnector enableBTLE:TRUE];
    }
    
    // set HW Connector to call hasData only when new data is available.
    [hardwareConnector setSampleTimerDataCheck:YES];

    
    // NOTE: an alternative to the implementation you see in this sample app is to implement the HardwareConnectorDelegate
    // in the main view controller of the app that will be interacting with the scale, such as this view controller.
    // In this case implement the HardwareConnectorDelegate methods directly instead of as notifications.
    
    // NOTE: To do an automatic connection without a button, the call can be placed in the delegate method
    // - (void)hardwareConnector:(WFHardwareConnector*)hwConnector stateChanged:(WFHardwareConnectorState_t)currentState
    // 
    // This method will get called as a response to the [hardwareConnector enableBTLE: TRUE] call above
    // with the state of WF_HWCONN_STATE_BT40_ENABLED.  When this state is received, the connector is ready for
    // a bluetooth connection to be established.

    // NOTE: For connecting, the wildcard switch and proximity switch for connection are optional.
    // The following call will create parameters for connection to any device of the specified sensor type that has already connected before,
    // or it will return one suitable for a wildcard search. It is recommended that you read the API documentation on
    // this method.
    //
    // WFConnectionParams* params = [hardwareConnector.settings connectionParamsForSensorType:sensorType];
    //
    // then call...
    //Create the new sensor connection
    //    WFSensorConnection* sensorConnection;
    //    sensorConnection = [hardwareConnector requestSensorConnection:params];
    //    self.sensorConnection = (WFWeightScaleConnection*)sensorConnection;
    //    self.sensorConnection.delegate = self;
    
    // NOTE: The callbacks from requestSensorConnection are
    // 1. - (void)connection:(WFSensorConnection*)connectionInfo stateChanged:(WFSensorConnectionStatus_t)connState
    // 2. - (void) hardwareConnector:(WFHardwareConnector *)hwConnector
    //               connectedSensor:(WFSensorConnection *)connectionInfo
    // It is currently recommended to process the connection in response to the second delegate callback above.
    // The hardwareConnector:connectedSensor: callback is later in the connection process.
    //
    // The hardwareConnector:connectedSensor: callback method, or a private method called in response to this callback or notification, should include
    // save the connection info
    //     [hardwareConnector.settings saveConnectionInfo:connectionInfo];
    // set the WFWeightScaleDelegate for callbacks in response to calls to the scale through the connection
    //      self.sensorConnection.wsDelegate = self;

    // NOTE: Once the connectionInfo has been saved in the hardware connector, it will return parameters for the
    // specific device just connected.  It will no longer return parameters that will perform a wildcard search.
    // To forget a device and return to a wildcard search for connection, remove the device from settings
    //    NSArray *deviceParams = [hardwareConnector.settings deviceParamsForSensorType:WF_SENSORTYPE_WEIGHT_SCALE];
    //    for ( WFDeviceParams *device in deviceParams)
    //    {
    //        // remove from settings
    //        BOOL removed = [hardwareConnector.settings removeDeviceParams:device
    //                                                        forSensorType:WF_SENSORTYPE_WEIGHT_SCALE];
    //    }
    
    self.navigationItem.title = @"Weight Scale";
    
    //init segment view
    self.bottomViewSegControl.selectedSegmentIndex = 0;
    self.historyView.hidden = YES;
    self.userView.hidden = YES;
}

//--------------------------------------------------------------------------------
- (void)viewDidUnload
{
    [self setBmiLabel:nil];
    [self setUsersTableView:nil];
    [self setHistoryTableView:nil];
    [self setHistoryView:nil];
    [self setUserView:nil];
    [self setMiscView:nil];
    [self setBottomViewSegControl:nil];
    [self setBottomView:nil];
}

#pragma mark -
#pragma mark UIAlertViewDelegate Implementation

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{ 
    
    //Handle dialog box for entering User Id
    if([alertView.title compare:@"User Id"] == NSOrderedSame && buttonIndex == 1)
    {
        if(selectedRow >= 0){
            
            //Assign entered user id to selected history record
            WFBTLEWeightScaleData* data = (WFBTLEWeightScaleData*)[weightHistory objectAtIndex:selectedRow];
            int userId = [[[alertView textFieldAtIndex:0] text] intValue];
            [self.weightScaleConnection assignUserId:userId toHistoryRecordWithTimestamp:data.timeStamp];
            //NSLog(@"Assigning id:%d to timestamp:%@", userId, data.timeStamp);
        }
    }
}

#pragma mark -
#pragma mark UITableViewDataSource Implementation
//--------------------------------------------------------------------------------
-(int)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    if(tableView == self.historyTableView)
    {
        //history table view
        return [weightHistory count];
    }
    else
    {
        //user records table view
        return [userRecords count];
    }
}

//--------------------------------------------------------------------------------
-(int)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

//--------------------------------------------------------------------------------
-(UITableViewCell*)tableView:(UITableView *)tv cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tv dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
    {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:CellIdentifier] autorelease];
    }
    
    //users
    if(tv == self.usersTableView)
    {
        
        WFBTLEWeightScaleUser* user = [userRecords objectAtIndex:indexPath.row];
        cell.textLabel.text = [NSString stringWithFormat:@"ID:%d In:%c%c%c Ht:%d Go:%1.1f", user.user_id, user.c_initial_left, user.c_initial_middle, user.c_initial_right, user.height_in_cm, user.user_goal_weight_kg];
        cell.detailTextLabel.text = [NSString stringWithFormat:@"%1.1fkg - %1.1fkg",user.user_range_min_weight_kg, user.user_range_max_weight_kg];
    
    //history
    }
    else
    {
        
        WFBTLEWeightScaleData* data = [weightHistory objectAtIndex:indexPath.row];
        NSDate* weightDate = [NSDate dateWithTimeIntervalSinceReferenceDate:data.timeStamp];
        cell.textLabel.text = [NSString stringWithFormat:@"ID:%d KG:%1.1f BMI:%1.1f",data.userProfileId, data.bodyWeight, data.bmi];
        cell.detailTextLabel.text = [NSString stringWithFormat:@"%@",weightDate];
    }
    
    
    
    
    return cell;
    
}


#pragma mark -
#pragma mark UITableViewDelegate Implementation
//--------------------------------------------------------------------------------
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    selectedRow = indexPath.row;
}

#pragma mark -
#pragma mark WFWeightScaleDelegate Implementation

//--------------------------------------------------------------------------------
- (void)wsConnection:(WFWeightScaleConnection*)wsConn didReceiveWeightHistory:(NSArray*)history
{
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"WeightScale" message:@"Received All Weight History" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];
    [alert release];
    alert = nil;
    
    
    for(WFBTLEWeightScaleData* data in history){
        NSDate* weightDate = [NSDate dateWithTimeIntervalSinceReferenceDate:data.timeStamp];
        
         NSLog(@"WFWeightScale Entry: w:%1.3f bmi:%1.3f t:%@ id:%d",data.bodyWeight, data.bmi, weightDate, data.userProfileId);
        
    }
    
    [weightHistory release];
    weightHistory = [history retain];
    [self.historyTableView reloadData];
   
}

//--------------------------------------------------------------------------------
- (void)wsConnection:(WFWeightScaleConnection*)wsConn didReceiveUserRecords:(NSArray *)users
{
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"WeightScale" message:@"Received All User Records" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];
    [alert release];
    alert = nil;
    
    [userRecords release];
    userRecords = [users retain];
    [self.usersTableView reloadData];
    
}

//--------------------------------------------------------------------------------
- (void)wsConnection:(WFWeightScaleConnection *)wsConn userProfileRejected:(WFBTLEWeightScaleUser *)rejectedProfile forOverlappingWith:(WFBTLEWeightScaleUser *)overlappedProfile
{
    NSString* alertMessage = [NSString stringWithFormat:@"User profile '%c%c%c' (%1.1fkg-%1.1fkg) was rejected for having an overlapping weight range with user #%d '%c%c%c' (%1.1fkg-%1.1fkg)", rejectedProfile.c_initial_left, rejectedProfile.c_initial_middle, rejectedProfile.c_initial_right, rejectedProfile.user_range_min_weight_kg, rejectedProfile.user_range_max_weight_kg, overlappedProfile.user_id, overlappedProfile.c_initial_left, overlappedProfile.c_initial_middle, overlappedProfile.c_initial_right, overlappedProfile.user_range_min_weight_kg, overlappedProfile.user_range_max_weight_kg];
    
    NSLog(@"%@",alertMessage);
                              
    UIAlertView * alert = [[UIAlertView alloc] initWithTitle:@"Profile Rejected" message:alertMessage delegate:self cancelButtonTitle:@"ok" otherButtonTitles:nil];
    [alert show];
    [alert release];
}


//--------------------------------------------------------------------------------
- (void)wsConnection:(WFWeightScaleConnection *)wsConn didAddNewUserRecord:(WFBTLEWeightScaleUser *)user
{
    NSString* alertMessage = [NSString stringWithFormat:@"User profile added: #%d '%c%c%c' (%1.1fkg-%1.1fkg)", user.user_id, user.c_initial_left, user.c_initial_middle, user.c_initial_right, user.user_range_min_weight_kg, user.user_range_max_weight_kg];
    
    UIAlertView * alert = [[UIAlertView alloc] initWithTitle:@"Profile Added" message:alertMessage delegate:self cancelButtonTitle:@"ok" otherButtonTitles:nil];
    [alert show];
    [alert release];
    
}


#pragma mark -
#pragma mark WFSensorCommonViewController Implementation

//--------------------------------------------------------------------------------
- (void)resetDisplay
{
	[super resetDisplay];
    
	bodyWeightLabel.text = @"n/a";
    bodyWeightLabel.textColor = [UIColor blackColor];
    bmiLabel.text = @"n/a";
    bmiLabel.textColor = [UIColor blackColor];
}

//--------------------------------------------------------------------------------
- (void)updateData
{
    [super updateData];
    
	WFBTLEWeightScaleData* wsData = (WFBTLEWeightScaleData*)[self.weightScaleConnection getWeightScaleData];
    
	if ( wsData != nil )
	{
        if ( wsData.bodyWeight == WF_WEIGHT_SCALE_INVALID )
        {
            bodyWeightLabel.text = @"n/a";
            bmiLabel.text = @"n/a";

        }
        else if ( wsData.bodyWeight == WF_WEIGHT_SCALE_COMPUTING )
        {
            bodyWeightLabel.text = @"--";
            bmiLabel.text = @"n/a";

        }
        else
        {
            bodyWeightLabel.text = [NSString stringWithFormat:@"%1.1f kg", wsData.bodyWeight];
            
            if (wsData.bmi > 0)
            {
                bmiLabel.text = [NSString stringWithFormat:@"%1.1f", wsData.bmi];
            }
            else
            {
                bmiLabel.text = @"n/a";
            }
            
            if (wsData.isWeightStabilized)
            {
                bodyWeightLabel.textColor = [UIColor blueColor];
                if (wsData.bmi > 0)
                {
                    bmiLabel.textColor = [UIColor blueColor];
                }
            }
            else 
            {
                bodyWeightLabel.textColor = [UIColor blackColor];
                bmiLabel.textColor = [UIColor blackColor];
            }
        }
        
	}
	else
	{
		[self resetDisplay];
	}
}


//--------------------------------------------------------------------------------
- (void)onSensorConnected:(WFSensorConnection*)connectionInfo
{
    [super onSensorConnected:connectionInfo];
    
    // set the CSC delegate parameters.
    WFWeightScaleConnection* connection = self.weightScaleConnection;
    if ( connection )
    {
        connection.wsDelegate = self;
    }
}


#pragma mark -
#pragma mark BTLEWeightScaleUserDelegate Implementation

//--------------------------------------------------------------------------------
-(BOOL)createUserRecordWithInitials:(NSString*)initials
                          minWeight:(float)minWeightKg
                          maxWeight:(float)maxWeightKg
                             height:(float)heightCm
                         goalWeight:(float)goalWeightKg
                  gainLossTolerance:(float)toleranceKg
{
    WFBTLEWeightScaleUser* user = [WFBTLEWeightScaleUser userRecordWithInitials:initials
                                                                      minWeight:minWeightKg
                                                                      maxWeight:maxWeightKg
                                                                         height:heightCm
                                                                     goalWeight:goalWeightKg
                                                              gainLossTolerance:toleranceKg];
    BOOL retVal;
    if (user == nil) {
        NSLog(@"User data error");
        retVal = NO;
    }
    else
    {
        NSLog(@"User created, cross checking with scale");
        retVal = [self.weightScaleConnection storeUserRecord:user];
    }
    return retVal;
}


#pragma mark Properties

//--------------------------------------------------------------------------------
- (WFWeightScaleConnection*)weightScaleConnection
{
	WFWeightScaleConnection* retVal = nil;
	if ( [self.sensorConnection isKindOfClass:[WFWeightScaleConnection class]] )
	{
		retVal = (WFWeightScaleConnection*)self.sensorConnection;
	}
	
	return retVal;
}


#pragma mark Event Handlers

//--------------------------------------------------------------------------------
- (IBAction)clearHistoryClicked:(id)sender {
    [self.weightScaleConnection clearAllHistory];
}

//--------------------------------------------------------------------------------
- (IBAction)clearSelectedClicked:(id)sender {
    
    if(selectedRow >= 0){
        WFBTLEWeightScaleData* data = (WFBTLEWeightScaleData*)[weightHistory objectAtIndex:selectedRow];
     
        [self.weightScaleConnection clearHistoryRecordWithTimestamp:data.timeStamp];
    }
}

//--------------------------------------------------------------------------------
- (IBAction)assignIdButtonClicked:(id)sender {
    if(selectedRow >= 0){
        
        //prompt user for id
        UIAlertView * alert = [[UIAlertView alloc] initWithTitle:@"User Id" message:@"Enter User Id to Assign to This Record" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Assign Id",nil];
        alert.alertViewStyle = UIAlertViewStylePlainTextInput;
        [alert show];
        [alert release];
    }
}

//--------------------------------------------------------------------------------
- (IBAction)getSinceButtonClicked:(id)sender {

    [self.weightScaleConnection requestHistorySinceLastSync];
    
}

//--------------------------------------------------------------------------------
- (IBAction)clearUsersClicked:(id)sender {
    [self.weightScaleConnection clearAllUserRecords];
}

//--------------------------------------------------------------------------------
- (IBAction)clearSelectedUserClicked:(id)sender {
    //[self.weightScaleConnection clearUserRecordWithId:2];
    
    if(selectedRow >= 0){
        WFBTLEWeightScaleUser* user = (WFBTLEWeightScaleUser*)[userRecords objectAtIndex:selectedRow];
        
        [self.weightScaleConnection clearUserRecordWithId:user.user_id];
    }
}

//--------------------------------------------------------------------------------
- (IBAction)getAllUsersClicked:(id)sender {
    [self.weightScaleConnection requestAllUserRecords];
}

//--------------------------------------------------------------------------------
- (IBAction)historyButtonClicked:(id)sender {
    [self.weightScaleConnection requestAllHistory];    
}

//--------------------------------------------------------------------------------
- (IBAction)newUserClicked:(id)sender {
    
    [userVC release];
    userVC = [[BTLEWeightScaleUserViewController alloc] initWithNibName:@"BTLEWeightScaleUserViewController" bundle:nil];
    userVC.delegate = self;
    [self presentModalViewController:userVC animated:YES];
    
}

//--------------------------------------------------------------------------------
- (IBAction)unitsControlValueChanged:(UISegmentedControl *)sender {
    WFBTLEWeightScaleUnitType_t units = (sender.selectedSegmentIndex == 0 ? WFBTLE_WEIGHT_SCALE_UNITS_METRIC : WFBTLE_WEIGHT_SCALE_UNITS_ENGLISH);
    NSLog(@"Setting to metric: %d", units == WFBTLE_WEIGHT_SCALE_UNITS_METRIC);

    [self.weightScaleConnection setDisplayUnitType:units];
}

//--------------------------------------------------------------------------------
- (IBAction)bottomViewSegControlChanged:(id)sender {
    
    switch (self.bottomViewSegControl.selectedSegmentIndex) {
        case 0:
            self.miscView.hidden = NO;
            self.historyView.hidden = YES;
            self.userView.hidden = YES;
            break;
            
        case 1:
            self.miscView.hidden = YES;
            self.historyView.hidden = NO;
            self.userView.hidden = YES;
            break;
            
        case 2:
            self.miscView.hidden = YES;
            self.historyView.hidden = YES;
            self.userView.hidden = NO;
            break;
            
        default:
            break;
    }
}

@end
