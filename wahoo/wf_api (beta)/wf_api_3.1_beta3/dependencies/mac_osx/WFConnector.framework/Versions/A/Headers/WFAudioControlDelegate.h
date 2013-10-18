//
//  WFAudioControlDelegate.h
//  WFConnector
//
//  Created by Michael Moore on 3/18/11.
//  Copyright 2011 Wahoo Fitness. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WFConnector/hardware_connector_types.h>


/**
 * Provides the interface for callback methods used by the WFAudioControlConnection.
 *
 * Delegate methods in this protocol represent commands sent from a remote device
 * (watch, etc).  When these commands are received via ANT, they are forwarded
 * to the application via the delegate methods.
 */
@protocol WFAudioControlDelegate <NSObject>

@optional

/**
 * A custom command.
 *
 * @param ucCommand The command number.
 */
- (void)acCustomCommand:(UCHAR)ucCommand;
/** Request to fast forward. */
- (void)acFastForward;
/** Request to rewind. */
- (void)acFastRewind;
/** Request to mute. */
- (void)acMute;
/** Request to pause. */
- (void)acPause;
/** Request to play. */
- (void)acPlay;
/** Request to repeat all tracks. */
- (void)acRepeatAll;
/** Request to repeat the current track. */
- (void)acRepeatCurrentTrack;
/** Request to disable repeat. */
- (void)acRepeatOff;
/** Request to shuffle albums. */
- (void)acShuffleAlbums;
/** Request to shuffle songs. */
- (void)acShuffleSongs;
/** Request to disable shuffle. */
- (void)acShuffleOff;
/** Request to skip to the next track. */
- (void)acSkipToNext;
/** Request to skip to the previous track. */
- (void)acSkipToPrevious;
/** Requst to stop playback. */
- (void)acStop;
/** Request to decrease the volume. */
- (void)acVolumeDown;
/** Request to increase the volume. */
- (void)acVolumeUp;

@end
