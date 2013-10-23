# This scripts sets up a SDM sensor to transmit the specified
# range of sweeping values. Refer to the sesnsorSimulation module for additional
# details on the sweeping mechanism
from sensorSimulation import sweeper
from System.Timers import Timer

eventTimer = Timer(1000) # convert time to milliseconds
sweepCadence = sweeper()

# SWEEP CONFIGURATION - Modify the values in the following four lines to change the sweep characteristics
# Make sure that all of these are doubles (use a decimal point!) so that calculations are accurate
sweepCadence.minValue = 10.0 # Minimum value in sweeping range, in spm
sweepCadence.maxValue = 240.0 # Maximum value in sweeping range in spm
sweepCadence.sweepTime = 60.0 # Time to sweep between the minimum and maximum values, in seconds
sweepCadence.constantTime = 5.0 # Time to spend at a constant speed in the minimum and maximum values, in seconds

# Enable cadence, as this is an optional field
simulator.SupplementaryPagesEnabled = True
simulator.Capabilities.CadenceCapability = True
simulator.Cadence = sweepCadence.minValue

sweepSpeed = sweeper()

# SWEEP CONFIGURATION - Modify the values in the following four lines to change the sweep characteristics
# Make sure that all of these are doubles (use a decimal point!) so that calculations are accurate
sweepSpeed.minValue = 2.77 # Minimum value in sweeping range, in m/s (10kph)
sweepSpeed.maxValue = 27.77 # Maximum value in sweeping range in m/s (100kph)
sweepSpeed.sweepTime = 60.0 # Time to sweep between the minimum and maximum values, in seconds
sweepSpeed.constantTime = 5.0 # Time to spend at a constant speed in the minimum and maximum values, in seconds

simulator.Speed = sweepSpeed.minValue

def update(sender, args):
   simulator.Cadence = sweepCadence.getNextValue()
   simulator.Speed = sweepSpeed.getNextValue()

def stopScript():
   sweepCadence.stop()
   sweepSpeed.stop()
   eventTimer.Stop()
   simulator.TurnOff()

eventTimer.Elapsed += update
simulator.TurnOn()
eventTimer.Start()
sweepCadence.start()
sweepSpeed.start()