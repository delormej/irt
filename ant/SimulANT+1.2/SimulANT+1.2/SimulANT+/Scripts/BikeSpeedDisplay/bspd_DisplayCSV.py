# It needs to access IronPython modules to use the CSV writer
import clr
clr.AddReferenceToFile('IronPython.Modules.dll')
from IronPython.Modules.PythonCsvModule import *

f = open('bikespd_log.csv', 'wb') # file stored in same directory as main simulator executable
logger = writer(f, delimiter=',', quotechar='|', quoting=QUOTE_MINIMAL)
logger.writerow(['Event Time', 'Event Count', 'Speed', 'Distance'])

simulator.TurnOn()

# This function gets called everytime data is received; will be logging calculated and raw data
def DataPageReceived(page, key):   
   logger.writerow([page.BikeSpeedEventTime, page.CumulativeSpeedRevolutionCount, simulator.Speed, simulator.Distance])

# Stop execution of the script, called when user presses the "Stop" button
def stopScript():
   simulator.TurnOff()
   f.close()