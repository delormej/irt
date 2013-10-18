//
//  WFDisplayTypes.h
//  WFConnector
//
//  Created by Murray Hughes on 16/12/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#ifndef WFConnector_WFDisplayTypes_h
#define WFConnector_WFDisplayTypes_h


// Support hardware button gestures
#define WF_DISPLAY_BUTTON_SOUTH_WEST_TOUCHED_NAME    @"buttonSouthWestTouched"
#define WF_DISPLAY_BUTTON_NORTH_WEST_TOUCHED_NAME    @"buttonNorthWestTouched"
#define WF_DISPLAY_BUTTON_SOUTH_EAST_TOUCHED_NAME    @"buttonSouthEastTouched"
#define WF_DISPLAY_BUTTON_NORTH_EAST_TOUCHED_NAME    @"buttonNorthEastTouched"

// Supported hardware button functions
#define WF_DISPLAY_BTNFN_PAGELEFT_KEY                @"hardwarePageLeft"
#define WF_DISPLAY_BTNFN_PAGERIGHT_KEY               @"hardwarePageRight"


// ====================================================
// ==== ENUMS
// ====================================================

// Colors, 2-bit color values. If only 1-bit is supported then
// Light-Gray can be treated as WHITE
// Dark-Gray can be treated as BLACK
typedef enum
{
    WF_DISPLAY_COLOR_BLACK          = 0x00,
    WF_DISPLAY_COLOR_DARK_GREY      = 0x01,
    WF_DISPLAY_COLOR_LIGHT_GREY     = 0x02,
    WF_DISPLAY_COLOR_WHITE          = 0x03,

    WF_DISPLAY_COLOR_NONE           = 0x04,

    WF_DISPLAY_COLOR_INVALID        = 0xFF,

} wf_display_color_e;

// -----------------------------------------------------------------------------
// Alignment (Text)

typedef enum
{
    WF_DISPLAY_ALIGNMENT_LEFT       = 0x00,
    WF_DISPLAY_ALIGNMENT_CENTER     = 0x01,
    WF_DISPLAY_ALIGNMENT_RIGHT      = 0x02,
    
    WF_DISPLAY_ALIGNMENT_INVALID    = 0xFF,

} wf_display_alignment_e;

// -----------------------------------------------------------------------------
// Font (Text)

typedef enum
{
    WF_DISPLAY_FONT_SYSTEM10     = 0x00,
    WF_DISPLAY_FONT_SYSTEM19     = 0x01,
    WF_DISPLAY_FONT_SYSTEM26     = 0x02,
    WF_DISPLAY_FONT_SYSTEM33     = 0x03,
    WF_DISPLAY_FONT_SYSTEM48     = 0x04,
    
    WF_DISPLAY_FONT_INVALID      = 0xFF,

} wf_display_font_e;


// -----------------------------------------------------------------------------
// Element Property ID's

typedef enum
{
    WF_DISPLAY_PROPERTY_VALUE   = 0x00,
    WF_DISPLAY_PROPERTY_FRAME   = 0x01,
    WF_DISPLAY_PROPERTY_ALIGN   = 0x02,
    WF_DISPLAY_PROPERTY_FONT    = 0x03,
    WF_DISPLAY_PROPERTY_FORMAT  = 0x04,
    WF_DISPLAY_PROPERTY_MASK    = 0x05,
    WF_DISPLAY_PROPERTY_SCROLL  = 0x06,
    WF_DISPLAY_PROPERTY_DELAY   = 0x07,
    WF_DISPLAY_PROPERTY_ENABLED = 0x08,
    WF_DISPLAY_PROPERTY_HIDDEN  = 0x09,
    
    WF_DISPLAY_PROPERTY_VALUE_UINT8 = 0x0A,
    WF_DISPLAY_PROPERTY_VALUE_DECIMAL8 = 0x0B,
    
} wf_display_property_e;


// -----------------------------------------------------------------------------

// Elements Types
typedef enum
{
    WF_DISPLAY_ELEMENT_PAGE = 0x01,
    WF_DISPLAY_ELEMENT_RECT = 0x03,
    WF_DISPLAY_ELEMENT_STRING = 0x04,
    WF_DISPLAY_ELEMENT_TIMER = 0x05,
    WF_DISPLAY_ELEMENT_BITMAP = 0x06,
    
    WF_DISPLAY_ELEMENT_INVALID = 0xFF,
    
} wf_display_element_e;

#endif
