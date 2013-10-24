#!/usr/bin/env python
#################################################################
# 
# By Jason De Lorme <jjdelorme@yahoo.com>
# http://www.roadacious.com
#

import time, sys
from ant_define import *
from maestro import *
from antprotocol.protocol import log

TICK_CHANNEL=1
TICK_ROLLOVER = 13120
TICKS_PER_01 = 144.0	# 144 ticks per 0.01 miles
MS_IN_HOUR = 3600.0

# ----------------------------------------------------------------------------
#
# Class responsible for reading and calculating speed based on ticks recorded 
# on the maestro device.
#
# ----------------------------------------------------------------------------
class MaestroSpeed(object):

	def __init__(self, maestro, wheel_size=0.0, debug=False):
		self._wheel_size = wheel_size
		self._debug = debug
		self._last_time_ms = 0	# last reading in milliseconds
		self._last_count = 0	# last tick count, 144 ticks = 0.01 miles
		self._maestro = maestro

	def __del__(self):
		pass
	
	@log
	def get_mph(self, count=0):
		try:
			count_delta = 0
			time_delta = 0
			time_ms = self._get_time_ms()
			
			if count == 0:
				count = self.get_revs()

			# if for some reason we still can't read the count, just return 0.0 speed.
			if count == 0:
				return 0.0

			# for first second, just get the reading
			if self._last_time_ms == 0:
				self._last_time_ms = time_ms
				self._last_count = count
				return 0.0

			# if current count is less, a rollover occured
			if count < self._last_count:
				count_delta = (TICK_ROLLOVER - self._last_count) + count
			else:
				count_delta = count - self._last_count

			miles = (count_delta / TICKS_PER_01) / 100.0

			time_delta = time_ms - self._last_time_ms
			hours = time_delta / MS_IN_HOUR

			speed = miles / hours

			# store for next loop
			self._last_time_ms = time_ms
			self._last_count = count

			return speed
		except Exception as e:
			print "ERROR: could not get speed: " + str(e)
			return 0.0

	# 
	# Calculates the number of wheel revolutions based on servo revolutions.
	#
	def servo_to_wheel_revs(self, servo_revs):
		"""
		 TODO: this needs to be validated, but insideride says 144:0.01 is the 
		 ratio of servo revs to miles, we'll use this ratio to calculate

		 0.01 miles : 144 s_revs 
		 0.01 miles = 16.09344 meters
		 1 servo_rev = 0.11176 distance_meters

		 This assumes a 2.07m wheel circumference. If different, we need to change.
		"""
		REV_TO_DISTANCE_M = 0.11176
		distance_m = servo_revs * REV_TO_DISTANCE_M
		wheel_revs = distance_m / self._wheel_size

		return wheel_revs

	def get_revs(self):
		return self._maestro.getFastPosition(TICK_CHANNEL)

	def _get_time_ms(self):
		return time.time()

# ----------------------------------------------------------------------------
#
# Class responsible for calculating speed from ANT messages.
#
# ----------------------------------------------------------------------------
class ANTSpeed(object):
	#
	# Reads speed messages and stores state about last speed.
	#
	def __init__(self, wheel_size=0.0, debug=False):
		self._wheel_size = wheel_size
		self._lastTime = 0
		self._cumulativeRevCount = 0
		self._last_mph = 0

	def get_mph(self, msg):
		page = Speed_Page0(msg)

		# QUESTION/TODO: rev count rolls over at 65536, are we respecting that?
		if page.time == self._lastTime or page.revs < self._cumulativeRevCount:
			return self._last_mph

		mph = 0.0

		# for the very first call, just store, don't calc.
		if self._lastTime > 0:
			# caclulate speed
			mph = self._calc_speed(page.time, page.revs)
			self._last_mph = mph

		# store for next call
		self._lastTime = page.time 
		self._cumulativeRevCount = page.revs

		if mph < 0.0:
			mph = 0.0

		return mph

	#
	# Core speed calculation
	#
	def _calc_speed(self, time, revs):
		meters_per_sec = (self._wheel_size * (revs - self._cumulativeRevCount) * 1024)/ \
			(time - self._lastTime)
		
		# convert to mp/h
		mph = meters_per_sec * 2.23693629 
		return mph

# ----------------------------------------------------------------------------
#
# Helper object for working with the ANT speed page0 data.
#
# ----------------------------------------------------------------------------
class Speed_Page0(object):
	
	def __init__(self, msg):
		page = msg[INDEX_MESG_DATA+1:len(msg)-1]
		self.time = int(page[4])
		self.time |= int(page[5]) << 8
		self.revs = int(page[6])
		self.revs |= int(page[7]) << 8

# test harness method
def _test(self):
	s = speed()
	while True:
		time.sleep(1.0)
		print "Speed: " + str(s.get_speed())