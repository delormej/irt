# It needs to access IronPython modules to use the CSV writer
import clr
clr.AddReferenceToFile('IronPython.Modules.dll')
from IronPython.Modules.PythonCsvModule import *

f = open('bikepwr_log.csv', 'wb') # file stored in same directory as main simulator executable
logger = writer(f, delimiter=',', quotechar='|', quoting=QUOTE_MINIMAL)
logger.writerow(['Event Count', 'Accumulated Power', 'Power', 'Cadence'])

simulator.TurnOn()

# This function gets called everytime the power only page is received; will be logging raw data
def StandardPowerOnlyPageReceived(page, key):   
   logger.writerow([page.EventCount, page.AccumulatedPower, page.InstantaneousPower, page.InstantaneousCadence])

# Stop execution of the script, called when user presses the "Stop" button
def stopScript():
   simulator.TurnOff()
   f.close()