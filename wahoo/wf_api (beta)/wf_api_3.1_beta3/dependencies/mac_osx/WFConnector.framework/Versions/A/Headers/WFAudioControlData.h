//
//  WFAudioControlData.h
//  WFConnector
//
//  Created by Michael Moore on 3/18/11.
//  Copyright 2011 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/WFSensorData.h>


/**
 * Describes the capabilities of the audio control device.
 */
typedef struct
{
    /** <c>TRUE</c> if the device supports Custom Repeat Mode, otherwise <c>FALSE</c>. */
    BOOL supportsCustomRepeatMode;
    /** <c>TRUE</c> if the device supports Custom Shuffle Mode, otherwise <c>FALSE</c>. */
    BOOL supportsCustomShuffleMode;
    /** <c>TRUE</c> if the device supports Song Title, otherwise <c>FALSE</c>. */
    BOOL supportsSongTitle;
    
} WFAudioControlCapabilities_t;

/**
 * Describes the state of the Audio Control Device.
 */
typedef enum
{
    /** The audio device is off. */
    WF_AUDIO_CONTROL_DEVICE_STATE_OFF       = 0,
    /** The audio device is in play mode. */
    WF_AUDIO_CONTROL_DEVICE_STATE_PLAY      = 1,
    /** The audio device is paused. */
    WF_AUDIO_CONTROL_DEVICE_STATE_PAUSE     = 2,
    /** The audio device is stopped. */
    WF_AUDIO_CONTROL_DEVICE_STATE_STOP      = 3,
    /** The audio device is busy. */
    WF_AUDIO_CONTROL_DEVICE_STATE_BUSY      = 4,
    /** The state is unknown, repeat and shuffle states are ignored. */
    WF_AUDIO_CONTROL_DEVICE_STATE_UNKNOWN   = 15,
    
} WFAudioControlDeviceState_t;

/**
 * Describes the repeat state of the Audio Control Device.
 */
typedef enum
{
    /** Repeat is off, or repeat state unknown. */
    WF_AUDIO_CONTROL_REPEAT_STATE_OFF            = 0,
    /** Repeats the current track. */
    WF_AUDIO_CONTROL_REPEAT_STATE_CURRENT_TRACK  = 1,
    /** Repeats all songs. */
    WF_AUDIO_CONTROL_REPEAT_STATE_ALL_SONGS      = 2,
    
} WFAudioControlRepeatState_t;

/**
 * Describes the shuffle state of the Audio Control Device.
 */
typedef enum
{
    /** Shuffle is off, or shuffle state unknown. */
    WF_AUDIO_CONTROL_SHUFFLE_STATE_OFF           = 0,
    /** Shuffle at the track level. */
    WF_AUDIO_CONTROL_SHUFFLE_STATE_TRACK         = 1,
    /** Shuffle at the album level. */
    WF_AUDIO_CONTROL_SHUFFLE_STATE_ALBUM         = 2,
    
} WFAudioControlShuffleState_t;



/**
 * Represents the data available from the ANT+ Audio Control channel.
 *
 * ANT+ sensors send data in multiple packets.  The WFAudioControlData
 * combines this data into a single entity.  The data represents the latest of
 * each data type sent from the sensor.
 */
@interface WFAudioControlData : WFSensorData
{
    UCHAR volume;
    UCHAR totalTrackTime;
    UCHAR currentTrackTime;
    UCHAR audioDeviceState;
    UCHAR repeatState;
    UCHAR shuffleState;
    USHORT serialNumber;
    UCHAR commandNumber;
}


/** Gets the volume level (as a percentage 0 - 100). */
@property (nonatomic, assign) UCHAR volume;
/** Gets the total track time, in seconds. */
@property (nonatomic, assign) UCHAR totalTrackTime;
/** Gets the current track time, in seconds. */
@property (nonatomic, assign) UCHAR currentTrackTime;
/** Gets the Audio Device state. */
@property (nonatomic, assign) UCHAR audioDeviceState;
/** Gets the repeat state. */
@property (nonatomic, assign) UCHAR repeatState;
/** Gets the shuffle state. */
@property (nonatomic, assign) UCHAR shuffleState;
/** Gets the device serial number. */
@property (nonatomic, assign) USHORT serialNumber;
/** Gets the Audio Control command number. */
@property (nonatomic, assign) UCHAR commandNumber;

@end
