# This scripts sets up a heart rate sensor to transmit the specified
# range of sweeping values. Refer to the sesnsorSimulation module for additional
# details on the sweeping mechanism
from sensorSimulation import sweeper
from System.Timers import Timer

eventTimer = Timer(1000) # convert time to milliseconds
sweepBpm = sweeper()

# SWEEP CONFIGURATION - Modify the values in the following four lines to change the sweep characteristics
# Make sure that all of these are doubles (use a decimal point!) so that calculations are accurate
sweepBpm.minValue = 45.0 # Minimum value in sweeping range, in bpm
sweepBpm.maxValue = 200.0 # Maximum value in sweeping range in bpm
sweepBpm.sweepTime = 60.0 # Time to sweep between the minimum and maximum values, in seconds
sweepBpm.constantTime = 5.0 # Time to spend at a constant speed in the minimum and maximum values, in seconds

simulator.HeartRate = sweepBpm.minValue

def update(sender, args):
   simulator.HeartRate = sweepBpm.getNextValue()

def stopScript():
   sweepBpm.stop()
   eventTimer.Stop()
   simulator.TurnOff()

eventTimer.Elapsed += update
simulator.TurnOn()
eventTimer.Start()
sweepBpm.start()