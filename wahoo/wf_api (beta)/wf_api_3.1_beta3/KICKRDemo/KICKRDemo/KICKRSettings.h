//
//  KICKRSettings.h
//  KICKRDemo
//
//  Created by Michael Moore on 11/13/12.
//
//

#import <Foundation/Foundation.h>
#import <WFConnector/hardware_connector_types.h>


@interface KICKRSettings : NSObject
{
    NSString* settingsDirectory;
	NSMutableDictionary* settingsRoot;
}


@property (nonatomic, assign) NSTimeInterval searchTimeout;

@end
