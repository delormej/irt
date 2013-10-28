#!/usr/bin/env python
##############################################################################
# 
# By Jason De Lorme <jjdelorme@yahoo.com>
# http://www.roadacious.com
#
##############################################################################


# ----------------------------------------------------------------------------
# Local defines
# ----------------------------------------------------------------------------
RESISTANCE_MODE_PERCENT = 0x40
RESISTANCE_MODE_STANDARD = 0x41
RESISTANCE_MODE_ERG = 0x42

from antprotocol.protocol import log

# ----------------------------------------------------------------------------
#
# Class responsible for getting / setting mag resistance.
#
# ----------------------------------------------------------------------------
class Resistance(object):
	#
	# Class that is responsible for calculating resistance settings, sending/reading messages to servo control and storing state.
	# Use the servo control class Maestro.
	#
	def __init__(self, servo, debug=False):
		self._debug = debug
		self.servo = servo
		self._mode = RESISTANCE_MODE_STANDARD
		self._positionToLevel = \
			{ 8428:0,
                5090:1,
                5091:2,
                5009:3,
                4846:4,
                4600:5,
                4272:6,
                3862:7,
                3370:8,
                2796:9 }

		# inverse level to position mapping
		self._levelToPosition = { v:k for k, v in self._positionToLevel.items() } 

	def __del__(self):
		pass
	
	@log
	def setLevel(self, level):
		if self._mode == RESISTANCE_MODE_STANDARD:
			level = level[0] # first byte
		elif self._mode == RESISTANCE_MODE_PERCENT:
			return # unsupported 

		# ideally this should be a value between 0 and 9, but it's only 3 right now.
		if level <0 or level >9:
			raise "Value must be between 0 and 9"
			
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

	def setMode(self, mode):
		self._mode = mode
