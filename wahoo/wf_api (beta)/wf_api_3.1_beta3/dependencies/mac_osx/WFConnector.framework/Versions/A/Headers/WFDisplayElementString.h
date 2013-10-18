//
//  WFDisplayElementString.h
//  WFConnector
//
//  Created by Murray Hughes on 16/12/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "WFDisplayElement.h"
#import "WFDisplayTypes.h"

@interface WFDisplayElementString : WFDisplayElement
{
    wf_display_font_e font;
    wf_display_alignment_e align;
    NSString* _value;
    BOOL bold;
    wf_display_color_e color;
    BOOL constant;
}


// font type
@property (nonatomic, assign) wf_display_font_e font;

// text alignment in frame
@property (nonatomic, assign) wf_display_alignment_e align;

// default/inital value
@property (nonatomic, copy) NSString* value;

// bold text
@property (nonatomic, assign, getter = isBold) BOOL bold;

// text color
@property (nonatomic, assign) wf_display_color_e color;

// signals if the text is contant and cannot be changed
@property (nonatomic, assign, getter = isConstant) BOOL constant;


@end
