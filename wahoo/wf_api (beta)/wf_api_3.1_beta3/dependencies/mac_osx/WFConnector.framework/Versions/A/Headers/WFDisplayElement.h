//
//  WFDisplayElement.h
//  WFConnector
//
//  Created by Murray Hughes on 16/12/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import "WFDisplayTypes.h"

@class WFDisplayPage;
@class WFDisplayElementGroup;

@interface WFDisplayElement : NSObject
{
    NSString* _key;
    CGRect frame;
    BOOL hidden;
    NSString* comment;
    WFDisplayPage* _page;

    @private
    uint8_t binaryKey;
    NSMutableArray* _errors;
    NSString* _type;
    WFDisplayElementGroup* group;
}

// Key used to identify the element. Only needs to be set if you wish to modify any properties at runtime.
// Must be unique to the page but can be re-used on other pages.
// eg, you might have HR on all pages
@property (nonatomic, copy) NSString* key;


@property (nonatomic, assign) CGRect frame;

// Allows you to show and hide elements at runtime.
@property (nonatomic, assign, getter = isHidden) BOOL hidden;


@property (nonatomic, copy) NSString* comment;

@property (nonatomic, assign) WFDisplayPage* page;



@end
