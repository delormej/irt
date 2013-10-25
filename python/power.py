#!/usr/bin/env python
#################################################################
# 
# By Jason De Lorme <jjdelorme@yahoo.com>
# http://www.roadacious.com
#

from antprotocol.protocol import log

# ----------------------------------------------------------------------------
#
# Class responsible for calculating power.
#
# ----------------------------------------------------------------------------
class Power(object):
	#
	# Class that is responsible for reading & calculating power, storing state about last power.
	#
	def __init__(self, weight, debug=False):
		self._debug = debug
		self.weight = weight
		self.slope = { 1: 2.6, 2: 7.1, 3: 11.0 }
		self.intercept = { 1: -9.60, 2: -29.99, 3: -13.34 }

	#
	# Calculates wattage based on eMotion curve.
	#
	@log
	def calcWatts(self, speed, level):
		watts = 0

		if speed == 0:
			return watts

		# calculates current power output based on speed and level
		level0 = (speed*14.04-33.6)-(((speed*14.04-33.06)-(speed*8.75-16.21))/90)*(220-self.weight)
		
		if level == 0:
			watts = level0
		else:
			watts = level0 + speed * self.slope[level] + self.intercept[level]

		# never return less than 0
		if watts < 0:
			watts = 0

		return watts
