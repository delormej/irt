#!/usr/bin/env python
#################################################################
# 
# By Jason De Lorme <jjdelorme@yahoo.com>
# http://www.roadacious.com
#

import time, math
from antprotocol.protocol import log

# ----------------------------------------------------------------------------
#
# Class responsible for calculating torque.
#
# ----------------------------------------------------------------------------
class Torque(object):

	def __init__(self, debug=True):
		self._debug = debug
		self._last_time = 0
		self._events = 0
		self._accum_torque = 0
		self._accum_period = 0

	#
	# torque is stored in 1/32nm
	#
	def get_accum_torque(self):
		return self._accum_torque

	def get_event_count(self):
		return self._events

	#
	# accumulated period is stored in 1/2048 seconds
	#
	def get_accum_period(self):
		return self._accum_period

	#
	# period is 1/2048 seconds.
	#
	@log
	def process_torque(self, watts, period2048):
		torque = self._torque(watts, period2048)
		self._events += 1
		self._accum_torque += (int)(torque)
		self._accum_period += period2048
		
		return self._accum_torque

	# 
	# The average angular velocity (rad/s) between two received messages is computed from the number of rotation events
	# divided by the rotation period.
	# Returns radians / second
	#
	def _angular_velocity(self, wheel_ticks, period):
		return (2 * math.pi * wheel_ticks) / (period)

	#
	# The average torque between two received messages is computed from the difference in accumulated torque, divided by the
	# number of rotation events. Accumulated torque is broadcast in 1/32Nm, which must be factored back out.
	# Returns nanometers 
	#
	def _avg_torque(self, wheel_ticks, new_accum_torque):
		return (new_accum_torque - self._accum_torque) / (32 * wheel_ticks)

	#
	# Retuns watts
	#
	#def _power(self, torque, ang_velocity):
	#	return torque * ang_velocity

	#
	# power in watts period in 1/2048 seconds
	# torque is returned in 1/32nm
	#
	def _torque(self, power, period):
		return (power * period) / (128 * math.pi)

	#
	# torque in 1/32nm, period in 1/2048 seconds
	# power returned in watts
	#
	def _power(self, torque, period):
		return (128 * math.pi) * (torque  / period)

	# cycle is 2hz (2 per second) -- period = 0.5*2048
	#period = 44946-44265
	#torque = 31680-31200

