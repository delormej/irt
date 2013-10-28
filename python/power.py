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
		self.slope = {1: 2.6, 2: 3.8, 3: 5.0, 4: 6.1, 5: 7.1, 6: 8.2, 7: 9.2, 8: 10.1, 9: 11.0 }
		self.intercept = { 1: -9.60, 2: -18.75, 3: -25.00, 4: -28.94, 5: -29.99, 6: -29.23, 7: -26.87, 8: -20.90, 9: -13.34 }

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
