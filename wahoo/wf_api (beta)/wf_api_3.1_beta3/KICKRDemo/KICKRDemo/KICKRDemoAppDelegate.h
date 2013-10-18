//{LEGAL_DISCLAIMER}
//
//  KICKRDemoAppDelegate.h
//  KICKRDemo
//
//  Created by Michael Moore on 9/1/11.
//  Copyright 2011 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <WFConnector/WFConnector.h>




@interface KICKRDemoAppDelegate : NSObject <UIApplicationDelegate, WFHardwareConnectorDelegate>
{
    WFHardwareConnector* hardwareConnector;
}


@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet UINavigationController *navigationController;

@end
