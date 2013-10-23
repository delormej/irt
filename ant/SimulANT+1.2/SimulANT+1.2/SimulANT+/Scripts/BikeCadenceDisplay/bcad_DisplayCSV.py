# This script logs raw data received from a bike cadence sensor to a CSV file
# It needs to access IronPython modules to use the CSV writer
import clr
clr.AddReferenceToFile('IronPython.Modules.dll')
from IronPython.Modules.PythonCsvModule import *

f = open('bikecad_log.csv', 'wb') # file stored in same directory as main simulator executable
logger = writer(f, delimiter=',', quotechar='|', quoting=QUOTE_MINIMAL)
logger.writerow(['Event Time', 'Event Count', 'Cadence'])

simulator.TurnOn()

# This function gets called everytime data is received; will be logging calculated and raw data
def DataPageReceived(page, key):   
   logger.writerow([page.BikeCadenceEventTime, page.CumulativeCadenceRevolutionCount, simulator.Cadence])

# Stop execution of the script, called when user presses the "Stop" button
def stopScript():
   simulator.TurnOff()
   f.close()