# This script creates a window and displays the instantaneous
# speed received, converting it to mi/h
# The UI is generated using WPF, loading a XAML file

# Add references to WPF assemblies
import clr
clr.AddReference('PresentationFramework')
clr.AddReference('PresentationCore')
clr.AddReference('WindowsBase')
clr.AddReference('System.Windows.Presentation')

from System import Action
from System.Windows.Threading import DispatcherExtensions, Dispatcher

dispatcher = Dispatcher.CurrentDispatcher

def LoadXaml(filename):
   from System.IO import File
   from System.Windows.Markup import XamlReader
   with File.OpenRead(filename) as f:
      return XamlReader.Load(f)

# Load window from Xaml
window = LoadXaml('Scripts/BikeSpeedDisplay/bspd_DisplaySimple.xaml') # path relative to main executable directory
text = window.FindName('text')

simulator.TurnOn()

def setSpeed():
   text.Text = '%.3f mph' %(simulator.Speed * 0.621371) # convert km/h to mi/h

# This function gets called everytime the bike speed display simulator receives data
# The code is executed in the ANT Receive thread; we need the dispatcher since
# setSpeed touches the UI
def BikeSpeedDataReceived(data, key):   
   DispatcherExtensions.BeginInvoke(dispatcher, Action(setSpeed))

# Stop execution of the script, called when user presses the "Stop" button
def stopScript():
   window.Close()
   simulator.TurnOff()

window.Show()



