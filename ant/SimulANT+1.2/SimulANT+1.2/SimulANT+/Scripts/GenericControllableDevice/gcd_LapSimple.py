# This script handles the Lap generic command, and increases
# a lap counter every time a new command is received.
# Additionally, it requests the battery status of the slave after 
# receiving a command for the first time

import clr
# Add a reference to the ANT+ Simulator
clr.AddReference('SimulANT+')

# Import Command Enums
from AntPlus.Profiles.Controls.Controls import GenericCommand
from AntPlus.Profiles.Common.Common import CommandStatus
from AntPlus.Profiles.Common import RequestDataPage, Common

laps = 0
seenSlave = False

simulator.TurnOn()

# The first time we receive a command, request the slave battery status
def GenericCommandPageReceived(command):
   global seenSlave
   if not seenSlave:
      seenSlave = True
      request = RequestDataPage()
      request.SlaveSerialNumber = command.SlaveSerialNumber
      request.CommandType = Common.CommandType.RequestDataPageFromSlave
      request.RequestedPageNumber = 82 #  Battery status
      simulator.SendDataPageRequest(request)


# This function gets called everytime a generic command is received
def GenericCommandReceived(command):   
   global laps
   if command == GenericCommand.Lap:  # handle lap command
      laps = laps + 1
      string = 'LAP COUNT: %d' %(laps)
      logScriptEvent(string)
      return CommandStatus.Pass
   else: # anything else is not supported
      return CommandStatus.NotSupported

# Stop execution of the script, called when user presses the "Stop" button
def stopScript():
   simulator.TurnOff()




