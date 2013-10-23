# This script connects to a generic controllable device, sends generic commands,
# and requests the command status

import clr
clr.AddReference('SimulANT+')
from AntPlus.Profiles.Common import RequestDataPage
from AntPlus.Profiles.Controls.Controls import GenericCommand

# Threading module is not in IronPython.Modules.dll
# Therefore, we need to use the threading module in a local Python installation
import sys
sys.path.append(r"c:\python27\lib")
from threading import Condition
import thread

foundDevice = False
gotStatus = False

cv = Condition()

def requestAndResponse():
   global foundDevice, gotStatus   
   simulator.TurnOn()
   # Wait until we acquire the device
   cv.acquire()
   while not foundDevice:
      cv.wait()
   cv.release()
   # Send command sequence
   commandSequence = [GenericCommand.Start, GenericCommand.Lap, GenericCommand.Lap, GenericCommand.Stop]
   for command in commandSequence:
      simulator.SendGenericCommand(int(command))
      # Request the command status page, and wait until ge get the response
      # To account for multiple slaves, request as broadcast.
      request = RequestDataPage()
      request.RequestedNumberTx = 1
      request.UseAck = False
      request.RequestedPageNumber = 71 #  Command status      
      cv.acquire()
      simulator.SendDataPageRequest(request)
      while not gotStatus:
         cv.wait(0.5) # wait up to 0.5 seconds
      gotStatus = False
      cv.release()
   endScript()

def ControllableDeviceFound(devNumber, txType):
   global foundDevice
   cv.acquire()
   foundDevice = True
   cv.notify()
   cv.release()

def CommandStatusPageReceived(page):
   global gotStatus
   cv.acquire()
   gotStatus = True
   cv.notify()
   cv.release()

def stopScript():
   simulator.TurnOff()

thread.start_new_thread(requestAndResponse, ())