#!/usr/bin/env python
#################################################################
# python eMotion object
# By Jason De Lorme <jjdelorme@yahoo.com>
# http://www.roadacious.com
#

import sys, time, usb
from maestro import *
from antprotocol.bases import GarminANT
from antprotocol.protocol import ANTReceiveException

class Speed(object):
	#
	# Reads speed messages and stores state about last speed.
	#
	def __init__(self):
		#sample data
		self.last_speed = 0

class Power(object):
	#
	# Class that is responsible for reading & calculating power, storing state about last power.
	#
	def __init__(self, weight):
		self.weight = weight
		self.last_power = 0
		self.slope = { 1: 2.6, 2: 7.1, 3: 11.0 }
		self.intercept = { 1: -9.60, 2: -29.99, 3: -13.34 }

	def calcWatts(self, speed, level):
		watts = 0
		# calculates current power output based on speed and level
		level0 = (speed*14.04-33.6)-(((speed*14.04-33.06)-(speed*8.75-16.21))/90)*(220-self.weight)
		
		if level == 0:
			watts = level0
		else:
			watts = level0 + speed * self.slope[level] + self.intercept[level]

		return watts

class Resistance(object):
	#
	# Class that is responsible for calculating resistance settings, sending/reading messages to servo control and storing state.
	# Use the servo control class Maestro.
	#
	def __init__(self):
		self._positionToLevel = \
			{ 8428:0,
			5090:1,
			4600:2,
			2796:3 }

		# inverse level to position mapping
		self._levelToPosition = { v:k for k, v in self._positionToLevel.items() } 
		self.servo = Maestro()
		self.servo.open()
		
	def setLevel(self, level):
		# ideally this should be a value between 0 and 9, but it's only 3 right now.
		if level <0 or level >3:
			raise "Value must be between 0 and 3"
			
		try:
			position = self._levelToPosition[level]
		except:
			raise "Failed to find a valid servo position for level " + level + "."

		self.servo.setTarget(position)

	def getLevel(self):
		# returns the current level
		position = self.servo.getPosition()
		level = self._positionToLevel[position]
		return level

	def __del__(self):
		self.servo.close()

class Profile(object):
	# 
	# Class that is responsible for requesting, reading and storing state of user profile.
	#
	def __init__(self):
		# combined rider and bicycle weight in lbs.
		self.weight = 0

class eMotion(GarminANT):
	# 
	# Main interface to the eMotion wireless device.  Responsible for sending / recieving ANT+ messages.
	#
	def __init__(self):
		# nothing to see here yet	
		print "I was started."

	# Here's what the pgm should do:
	#
	# 1. initialize ANT+ via USB
	# 2. get the user profile data (rider+bike weight)
	# 3. listen for ANT+ messages
	#		dispatch accordingly (speed, resistance, power/calibration/etc)
	def start(self):
		# let's rock and roll!
		print "Here's where it all begins."

def main():

	# test power calculations
	# 
	profile = Profile()
	profile.weight = 175 # sample data

	speed = Speed()
	speed.last_speed = 16.9 # sample data

	p = Power(profile.weight)
	r = Resistance()

	while True:
		try:
			level = r.getLevel()
			print("Watts for level: " + str(level) + ":" + \
				str(p.calcWatts(speed.last_speed, level)) )
			time.sleep(1.0) # sleep for a second
	
		except KeyboardInterrupt:
			print "Thank you for playing."
			return 0

if __name__ == '__main__':
    sys.exit(main())
