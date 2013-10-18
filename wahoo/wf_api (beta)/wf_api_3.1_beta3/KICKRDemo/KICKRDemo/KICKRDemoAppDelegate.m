//{LEGAL_DISCLAIMER}
//
//  KICKRDemoAppDelegate.m
//  KICKRDemo
//
//  Created by Michael Moore on 9/1/11.
//  Copyright 2011 Wahoo Fitness. All rights reserved.
//

#import "KICKRDemoAppDelegate.h"
#import "KICKRVC.h"


@interface KICKRDemoAppDelegate (_PRIVATE_)

@end



@implementation KICKRDemoAppDelegate

@synthesize window = _window;
@synthesize navigationController = _navigationController;

#pragma mark -
#pragma mark NSObject Implementation

//--------------------------------------------------------------------------------
- (void)dealloc
{
    [_window release];
    [_navigationController release];
    
    [super dealloc];
}


#pragma mark -
#pragma mark UIApplicationDelegate Implementation

//--------------------------------------------------------------------------------
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    NSLog(@"application:didFinishLaunchingWithOptions");

    // Override point for customization after application launch.
    // Add the navigation controller's view to the window and display.
    KICKRVC *kickrViewController = [[KICKRVC alloc] initWithNibName:@"KICKRVC" bundle:nil];
    
    UINavigationController *navController = [[UINavigationController alloc]initWithRootViewController:kickrViewController];
    [kickrViewController release];
    
    self.navigationController = navController;
    
    self.window.rootViewController = self.navigationController;
    [self.window makeKeyAndVisible];
    
    [navController release];
    
	// disable sleep mode.
	application.idleTimerDisabled = TRUE;
	
    // configure the hardware connector.
    hardwareConnector = [WFHardwareConnector sharedConnector];
    hardwareConnector.delegate = self;

	hardwareConnector.sampleRate = 0.1;  // sample rate 100 ms, or 10 Hz.
    //
    // determine support for BTLE.
    if ( hardwareConnector.hasBTLESupport )
    {
        // enable BTLE.
        [hardwareConnector enableBTLE:TRUE];
    }
    NSLog(@"%@", hardwareConnector.hasBTLESupport?@"DEVICE HAS BTLE SUPPORT":@"DEVICE DOES NOT HAVE BTLE SUPPORT");
    
    // set HW Connector to call hasData only when new data is available.
    [hardwareConnector setSampleTimerDataCheck:YES];
    
    return YES;
}

//--------------------------------------------------------------------------------
- (void)applicationWillResignActive:(UIApplication *)application
{
    NSLog(@"applicationWillResignActive");
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
}

//--------------------------------------------------------------------------------
- (void)applicationDidEnterBackground:(UIApplication *)application
{
    NSLog(@"applicationDidEnterBackground");
    
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
     */
}

//--------------------------------------------------------------------------------
- (void)applicationWillEnterForeground:(UIApplication *)application
{
    NSLog(@"applicationWillEnterForeground");
    /*
     Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
     */
}

//--------------------------------------------------------------------------------
- (void)applicationDidBecomeActive:(UIApplication *)application
{
     NSLog(@"applicationDidBecomeActive");
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
}

//--------------------------------------------------------------------------------
- (void)applicationWillTerminate:(UIApplication *)application
{
     NSLog(@"applicationWillTerminate");
    /*
     Called when the application is about to terminate.
     Save data if appropriate.
     See also applicationDidEnterBackground:.
     */
}


#pragma mark -
#pragma mark HardwareConnectorDelegate Implementation

//--------------------------------------------------------------------------------
- (void)hardwareConnector:(WFHardwareConnector*)hwConnector connectedSensor:(WFSensorConnection*)connectionInfo
{
    NSDictionary* userInfo = [NSDictionary dictionaryWithObjectsAndKeys:
                              connectionInfo, @"connectionInfo",
                              nil];
    [[NSNotificationCenter defaultCenter] postNotificationName:WF_NOTIFICATION_SENSOR_CONNECTED object:nil userInfo:userInfo];
}

//--------------------------------------------------------------------------------
- (void)hardwareConnector:(WFHardwareConnector*)hwConnector didDiscoverDevices:(NSSet*)connectionParams searchCompleted:(BOOL)bCompleted
{
    // post the sensor type and device params to the notification.
    NSDictionary* userInfo = [NSDictionary dictionaryWithObjectsAndKeys:
                         connectionParams, @"connectionParams",
                         [NSNumber numberWithBool:bCompleted], @"searchCompleted",
                         nil];
    [[NSNotificationCenter defaultCenter] postNotificationName:WF_NOTIFICATION_DISCOVERED_SENSOR object:nil userInfo:userInfo];
}

//--------------------------------------------------------------------------------
- (void)hardwareConnector:(WFHardwareConnector*)hwConnector disconnectedSensor:(WFSensorConnection*)connectionInfo
{
    [[NSNotificationCenter defaultCenter] postNotificationName:WF_NOTIFICATION_SENSOR_DISCONNECTED object:nil];
}

//--------------------------------------------------------------------------------
- (void)hardwareConnector:(WFHardwareConnector*)hwConnector stateChanged:(WFHardwareConnectorState_t)currentState
{
	BOOL connected = ((currentState & WF_HWCONN_STATE_ACTIVE) || (currentState & WF_HWCONN_STATE_BT40_ENABLED)) ? TRUE : FALSE;
	if (connected)
	{
        [[NSNotificationCenter defaultCenter] postNotificationName:WF_NOTIFICATION_HW_CONNECTED object:nil];
	}
	else
	{
        [[NSNotificationCenter defaultCenter] postNotificationName:WF_NOTIFICATION_HW_DISCONNECTED object:nil];
	}
}

//--------------------------------------------------------------------------------
- (void)hardwareConnectorHasData
{
    [[NSNotificationCenter defaultCenter] postNotificationName:WF_NOTIFICATION_SENSOR_HAS_DATA object:nil];
}

//--------------------------------------------------------------------------------
- (void) hardwareConnector:(WFHardwareConnector*)hwConnector hasFirmwareUpdateAvailableForConnection:(WFSensorConnection*)connectionInfo
                  required:(BOOL)required
    withWahooUtilityAppURL:(NSURL *)wahooUtilityAppURL
{
    NSDictionary* userInfo = [NSDictionary dictionaryWithObjectsAndKeys:
                              wahooUtilityAppURL, @"wahooUtilityAppURL",
                              [NSNumber numberWithBool:required], @"firmwareUpdateRequired",
                              nil];
    [[NSNotificationCenter defaultCenter] postNotificationName:WF_NOTIFICATION_FIRMWARE_UPDATE_AVAILABLE object:nil userInfo:userInfo];
}


#pragma mark -
#pragma mark KICKRDemoAppDelegate Implementation

#pragma mark Private Methods

//--------------------------------------------------------------------------------

@end




