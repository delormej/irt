/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/



#pragma once
#include "StdAfx.h"

typedef enum {
	SIM_FIXED,				// Fixed simulation type
	SIM_SWEEP,				// Sweep values
	SIM_STEP				// Alternate between min and max
} SimDataTypes;

//This delegate definition allows implementation of sim classes sending acknowledged messages
public delegate void dRequestAckMsg(UCHAR* pucTxBuffer_);
// This delegate definition allows implementation of sim classes sending broadcast data
public delegate void dRequestBcastMsg(UCHAR* pucTxBuffer_);
//This delegate definition allows implementation of sim classes updating message period
public delegate void dRequestUpdateMesgPeriod(USHORT usMesgPeriod_);

public interface class ISimBase{
	//Constructor: all simulators can define their own constructors, providing they are called properly in the channel class

	//Get Functions: simply return the requested values
	UCHAR getDeviceType();
	UCHAR getTransmissionType();
	USHORT getTransmitPeriod();
	DOUBLE getTimerInterval();

	//onTimerTock: called every channel timer event occurence
	void onTimerTock(USHORT eventTime);

	//ANT_eventNotification: handles incoming ANT events
	void ANT_eventNotification(UCHAR eventCode, UCHAR* pcBuffer);

	//get panel functions: Return the panel with sim defined controls pre-attached and the size and position values
	System::Windows::Forms::Panel^ getSimSettingsPanel();
	System::Windows::Forms::Panel^ getSimTranslatedDisplay();

};