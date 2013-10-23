# This script plots the received speed value in real time
# It uses the Dynamic Data Display library (WPF) http://dynamicdatadisplay.codeplex.com/
# and assumes that DynamicDataDisplay.dll is in the same directory as the main simulator executable

# Add references to WPF assemblies
import clr
clr.AddReference('PresentationFramework')
clr.AddReference('PresentationCore')
clr.AddReference('WindowsBase')
clr.AddReference('System.Windows.Presentation')
clr.AddReference('DynamicDataDisplay') # or clr.AddReferenceToFile('DynamicDataDisplay.dll')

from System import Action, Func
from System.Windows.Threading import DispatcherExtensions, Dispatcher

dispatcher = Dispatcher.CurrentDispatcher

def LoadXaml(filename):
   from System.IO import File
   from System.Windows.Markup import XamlReader
   with File.OpenRead(filename) as f:
      return XamlReader.Load(f)

# Load window from Xaml
window = LoadXaml('Scripts/BikeSpeedDisplay/bspd_DisplayPlot.xaml') # path relative to main executable directory
plotter = window.FindName('plotter')

from Microsoft.Research.DynamicDataDisplay.DataSources import ObservableDataSource
from Microsoft.Research.DynamicDataDisplay import *
from System.Windows import Point

source = ObservableDataSource[Point]()

def PointMapping(point):
   return point

source.SetXYMapping(Func[Point,Point](PointMapping)) # create mapping to points
Plotter2DExtensions.AddLineGraph(plotter, source, 2, 'Bike Speed') # plotter.AddLineGraph is an extension method, and can't use it in Python!

simulator.TurnOn()

# This function gets called everytime the bike speed display simulator receives data
# The code is executed in the ANT Receive thread; we need the dispatcher to
# be able to modify the UI
def BikeSpeedDataReceived(data, key):   
   p1 = Point(data.TotalWheelEventTime/1024.0, data.Speed) #convert time to seconds
   source.AppendAsync(dispatcher, p1)

# Stop execution of the script, called when user presses the "Stop" button
def stopScript():
   window.Close()
   simulator.TurnOff()

window.Show()