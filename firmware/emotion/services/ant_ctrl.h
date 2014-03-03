/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
*
*	Encapsulates the interaction with the ANT+ Generic Control Profile v2.0
*	Specifically this is designed to work with the 3 button o-synce bike
*	remote control.  
*
*	6 functions are transmitted from the remote based on short & long button
*	presses of the remote's 3 buttons.
*
*	Required ANT common background pages (0x50, 0x51) must be sent as well.
*
*/

#ifndef ANT_CTRL_H__
#define ANT_CTRL_H__

#define NOTIFICATIONS_NO_LIMIT		0x0
#define NOTIFICATIONS_LIMITED		0x1

#include "ant_stack_handler_types.h"


// Enum of the accepted commands, more could be sent that will be ignored.
typedef enum 
{
	// Button press.
	Menu_Up = 0u,
	Menu_Down = 1u,
	Menu_Select = 2u,
	// Long button press.
	Menu_Back = 3u,
	Start = 32u,
	Lap = 36u
} ctrl_command_e;

// Payload of a command.
typedef struct
{
	uint8_t			sequence;
	ctrl_command_e	command;
} ctrl_evt_t;

// Event Handler type.
typedef void (*ctrl_evt_handler_t)(ctrl_evt_t);

// Initializes the module.
void ant_ctrl_tx_init(uint8_t channel_id, ctrl_evt_handler_t on_ctrl_command);

// Opens the channel and starts sending master broadcasts.
void ant_ctrl_tx_start(void);

// Stops sending the master broadcast and closes the channel.
void ant_ctrl_tx_stop(void);

// Sends device availability message.
// Notifications = 0 if no limit specificed or limit not reached.
// Notifications = 1 if device cannot connect to additional remotes.
void ant_ctrl_device_avail_tx(uint8_t notifications);

// Handles events destined for this channel.
void ant_ctrl_rx_handle(ant_evt_t * p_ant_evt);

#endif	// ANT_CTRL_H__
