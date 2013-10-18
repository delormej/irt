/*
This software is subject to the license described in the License.txt file 
included with this software distribution. You may not use this file except in compliance 
with this license.

Copyright (c) Dynastream Innovations Inc. 2012
All rights reserved.
*/


// Main project file.
// compile in the common language runtime mixed mode (/clr)
// This program relies on the ANT dlls and the VS libraries to be run

#include "stdafx.h"
#include "MainForm.h"
#include <stdio.h>
#include <vcclr.h>
#include <windows.h>

using namespace ANTPlusSim;

class Thunk
// event forwarder
{
public:   
   Thunk(MainForm^ A_ANTMainPtr) 
   {
      ANTMainPtr = A_ANTMainPtr;   
   }

   static void __stdcall CallbackForwarder(void* param, UCHAR ucChannel_, UCHAR ucMessageCode_, UCHAR* pucBuffer_)
   {
      static_cast<Thunk*>(param)->ANTMainPtr->ProcessChannelEvent(ucChannel_, ucMessageCode_, pucBuffer_);
   }  
   static void __stdcall CallbackForwarder2(void* param, UCHAR ucChannel_, UCHAR ucMessageCode_, UCHAR* pucBuffer_)
   {
      static_cast<Thunk*>(param)->ANTMainPtr->ProcessProtocolEvent(ucChannel_, ucMessageCode_, pucBuffer_ );
   }   
  
private:
   gcroot<MainForm^> ANTMainPtr;
};

Thunk *pThunk;

//#pragma unmanaged
void UnmanagedSender(UCHAR ucChannel_, UCHAR ucMessageCode_, UCHAR* pucBuffer_)
{   
   pThunk->CallbackForwarder(pThunk, ucChannel_, ucMessageCode_, pucBuffer_);  
}

void UnmanagedEventSender(UCHAR ucChannel_, UCHAR ucMessageCode_, UCHAR* pucBuffer_)
{
   pThunk->CallbackForwarder2(pThunk, ucChannel_, ucMessageCode_, pucBuffer_);
}


#pragma managed
[STAThreadAttribute]
int main()
{
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;
	MainForm ^pMainForm;


#if defined(SIMULATOR_DISPLAY)
	pMainForm = gcnew MainForm(SIM_DISPLAY);
#elif defined(SIMULATOR_SENSOR)
   pMainForm = gcnew MainForm(SIM_SENSOR);
   
#else
   #error "SIMULATOR NOT DEFINED"
#endif                                                           

	pThunk = new Thunk(pMainForm);

	// Create the main window and run it
	Application::Run(pMainForm);
	
	delete pThunk;
	return 0;
}