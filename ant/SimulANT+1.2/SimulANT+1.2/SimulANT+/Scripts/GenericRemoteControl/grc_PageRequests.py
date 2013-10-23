# This script connects to and requests common data pages from the generic
# controllable device

import clr
clr.AddReference('SimulANT+')
from AntPlus.Profiles.Common import RequestDataPage

# Threading module is not in IronPython.Modules.dll
# Therefore, we need to use the threading module in a local Python installation
import sys
sys.path.append(r"c:\python27\lib")
from threading import Condition
import thread

foundDevice = False
gotBatteryPage = False
cv = Condition()

def requestAndResponse():
   global foundDevice, gotBatteryPage   
   simulator.TurnOn()
   # Wait until we acquire the device
   cv.acquire()
   while not foundDevice:
      cv.wait()
   cv.release()
   # Request a page, and wait until ge get it
   request = RequestDataPage()
   request.RequestedPageNumber = 82 #  Battery status
   simulator.SendDataPageRequest(request)
   cv.acquire()
   while not gotBatteryPage:
      cv.wait()
   gotBatteryPage = False
   cv.release()

def ControllableDeviceFound(devNumber, txType):
   global foundDevice
   cv.acquire()
   foundDevice = True
   cv.notify()
   cv.release()

def BatteryStatusPageReceived(page):
   global gotBatteryPage
   cv.acquire()
   gotBatteryPage = True
   cv.notify()
   cv.release()

def stopScript():
   simulator.TurnOff()

thread.start_new_thread(requestAndResponse, ())
