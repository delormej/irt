# This script logs raw data received from a heart rate sensor to a CSV file
# It needs to access IronPython modules to use the CSV writer
import clr
clr.AddReferenceToFile('IronPython.Modules.dll')
from IronPython.Modules.PythonCsvModule import *

f = open('sdm_log.csv', 'wb') # file stored in same directory as main simulator executable
logger = writer(f, delimiter=',', quotechar='|', quoting=QUOTE_MINIMAL)
logger.writerow(['Time', 'Stride Count', 'Speed', 'Distance'])

simulator.TurnOn()

# This function gets called everytime the default data page is received; will be logging raw data
def DefaultDataPageReceived(page, key):   
   logger.writerow([page.Time, page.StrideCount, page.InstantaneousSpeed, page.Distance])

# Stop execution of the script, called when user presses the "Stop" button
def stopScript():
   simulator.TurnOff()
   f.close()