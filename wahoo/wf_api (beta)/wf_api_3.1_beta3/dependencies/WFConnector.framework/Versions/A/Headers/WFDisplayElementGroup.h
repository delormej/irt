//
//  WFDisplayElementGroup.h
//  WFConnector
//
//  Created by Murray Hughes on 17/12/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import "WFDisplayElement.h"

@interface WFDisplayElementGroup : WFDisplayElement
{
    NSMutableArray* mutableElements;
}


// Array of dictionaries representing elements.
@property (nonatomic, readonly) NSArray* elements;
@property (nonatomic, retain) NSString* groupId;

- (void) addElement:(WFDisplayElement*) element;
- (void) removeElement:(WFDisplayElement*) element;
- (void) removeAllElements;


@end
