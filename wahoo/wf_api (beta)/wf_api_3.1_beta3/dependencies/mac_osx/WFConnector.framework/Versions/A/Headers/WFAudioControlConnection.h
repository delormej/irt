//
//  WFAudioControlConnection.h
//  WFConnector
//
//  Created by Michael Moore on 3/18/11.
//  Copyright 2011 Wahoo Fitness. All rights reserved.
//

#import <WFConnector/_WFAudioControlConnection.h>
#import <WFConnector/WFAudioControlData.h>


@class WFConnectionParams;


/**
 * Represents an ANT+ Audio Control connection.
 *
 * The WFAudioControlConnection and WFMultiSportConnection are different from the
 * standard connection types, in that they establish the "Master" side of the
 * connection, to which other devices connect.  The WFAudioControlConnection will
 * open a channel (if a free channel is available), and begin broadcasting based
 * on the ANT+ Audio Control profile.  Methods are provided in the
 * WFAudioControlConnection class to configure the values which are broadcast
 * (see WFAudioControlConnection::setCurrentTrackTime:,
 * WFAudioControlConnection::setVolume:, etc).
 *
 * The Audio Control profile provides for two-way communication.  Connected
 * devices (watches, etc.) may send commands to do things such as start or stop
 * playback, skip to the next track, etc.  These commands are forwarded to the
 * application via the WFAudioControlDelegate.  To receive incoming commands, the
 * application must assign the WFAudioControlDelegate instance which will handle
 * incoming commands to the <c>WFAudioControlConnection.acDelegate</c> property.
 */
@interface WFAudioControlConnection : _WFAudioControlConnection
{
}


/**
 * Gets a WFAudioControlCapabilities_t structure describing the current Audio
 * Control Device capabilities.
 *
 * This reflects the current state of the application-defined capabilities.
 * Values in this stucture will be broadcast to any connected devices. To modify
 * the values, use the WFAudioControlConnection::setCapabilities: method.
 *
 * @return The current application-defined Audio Control Device capabilities.
 */
- (WFAudioControlCapabilities_t)getCurrentCapabilities;

/**
 * Gets the current device state.
 *
 * This reflects the current application-defined device state. To modify
 * the value, use the WFAudioControlConnection::setDeviceState: method.
 *
 * @return The current application-defined device state.
 */
- (WFAudioControlDeviceState_t)getCurrentDeviceState;

/**
 * Gets the current repeat state.
 *
 * This reflects the current application-defined repeat state. To modify
 * the value, use the WFAudioControlConnection::setRepeatState: method.
 *
 * @return The current application-defined repeat state.
 */
- (WFAudioControlRepeatState_t)getCurrentRepeatState;

/**
 * Gets the current shuffle state.
 *
 * This reflects the current application-defined shuffle state. To modify
 * the value, use the WFAudioControlConnection::setShuffleState: method.
 *
 * @return The current application-defined shuffle state.
 */
- (WFAudioControlShuffleState_t)getCurrentShuffleState;

/**
 * Sets the Audio Control Device capabilities.
 *
 * @param pCaps A pointer to a WFAudioControlCapabilities structure describing
 * the Audio Control Device capabilities.
 *
 * @return <c>TRUE</c> if the new value was set successfully, otherwise <c>FALSE</c>.
 */
- (BOOL)setCapabilities:(WFAudioControlCapabilities_t*)pCaps;

/**
 * Sets the Device State.
 *
 * @param deviceState The new Device State.
 *
 * @return <c>TRUE</c> if the new value was set successfully, otherwise <c>FALSE</c>.
 */
- (BOOL)setDeviceState:(WFAudioControlDeviceState_t)deviceState;

/**
 * Sets the Repeat State.
 *
 * @param repeatState The new Repeat State.
 *
 * @return <c>TRUE</c> if the new value was set successfully, otherwise <c>FALSE</c>.
 */
- (BOOL)setRepeatState:(WFAudioControlRepeatState_t)repeatState;

/**
 * Sets the Shuffle State.
 *
 * @param shuffleState The new Shuffle State.
 *
 * @return <c>TRUE</c> if the new value was set successfully, otherwise <c>FALSE</c>.
 */
- (BOOL)setShuffleState:(WFAudioControlShuffleState_t)shuffleState;

/**
 * Sets the current track time to be broadcast.
 *
 * This is the time which will be broadcasted to any connected devices.  This
 * value is continually broadcasted until it is updated with a new value.
 *
 * @param currentTrackTime The current track time, in seconds.
 *
 * @return <c>TRUE</c> if the new value was set successfully, otherwise <c>FALSE</c>.
 */
- (BOOL)setCurrentTrackTime:(USHORT)currentTrackTime;

/**
 * Sets the total track time to be broadcast.
 *
 * This is the time which will be broadcasted to any connected devices.  This
 * value is continually broadcasted until it is updated with a new value.
 *
 * @param totalTrackTime The total track time, in seconds.
 *
 * @return <c>TRUE</c> if the new value was set successfully, otherwise <c>FALSE</c>.
 */
- (BOOL)setTotalTrackTime:(USHORT)totalTrackTime;

/**
 * Sets the volume to be broadcast.
 *
 * This is the volume which will be broadcasted to any connected devices.  This
 * value is continually broadcasted until it is updated with a new value.
 *
 * @param volume The volume (as a percentage, 1 to 100).
 *
 * @return <c>TRUE</c> if the new value was set successfully, otherwise <c>FALSE</c>.
 */
- (BOOL)setVolume:(UCHAR)volume;


/**
 * Returns the default connection params for the WFAudioControlConnection.
 *
 * @return A WFConnectionParams instance representing the default connection
 * parameters for the WFAudioControlConnection.
 */
+ (WFConnectionParams*)defaultConnectionParams;

@end
