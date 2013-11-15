#!/usr/bin/env python
#################################################################
# Pololu Maestro USB Servo Controller
# By Jason De Lorme <jjdelorme@yahoo.com>
# 

import serial, time, struct, threading
from antprotocol.protocol import log

SET_TARGET_COMMAND=0x84
GET_POSITION_COMMAND=0x90
GET_MOVING_STATE_COMMAND=0x93

# ----------------------------------------------------------------------------
#
# Class responsible for interacting with the Pololu Maestro controller.
#
# ----------------------------------------------------------------------------
class Maestro(serial.Serial):

	def __init__(self, port="/dev/ttyACM0", baudrate=9600, channel=0x05, timeout=1, debug=False):
		super(Maestro, self).__init__(port=port, baudrate=baudrate, timeout=timeout)
		self.channel = channel
		
		self._lock = threading.Lock()

	def __del__(self):
		self.close()

	def isMoving(self):
		try:
			# make sure it's not moving before you check the position.
			self.write( \
				chr(GET_MOVING_STATE_COMMAND))

			r = self.read(1)
			if r is not None:
				val = struct.unpack('?', r)
				return val[0]
			else:
				raise "ERROR Could not determine if servo was moving."		
		except Exception as e:
			print str(e)
			return False

	def getFastPosition(self, channel):
		try:
			self.write( \
				chr(GET_POSITION_COMMAND) + \
				chr(channel))

			r = self.read(2)
			if r is not None:
				pos = struct.unpack('h', r)
			else:
				return None
		
			return pos[0]
		except Exception as e:
			print "ERROR - getFastPosition: " + str(e)
			return 0

	def getPosition(self):
		# let the servo finish moving if it is
		while self.isMoving():
			# wait just a 1/2 second and try again
			time.sleep(0.5)

		return self.getFastPosition(self.channel)

	def setTarget(self, position):
		low = position&0x7f
		high = position>>7
		
		self.write( \
			chr(SET_TARGET_COMMAND) + \
			chr(self.channel) + \
			chr(low) + \
			chr(high))

	#
	# Threadsafe read implementation
	#
	def read(self, count):
		with self._lock:
			return super(Maestro, self).read(count)

	#
	# Threadsafe write implementation
	#
	def write(self, message):
		with self._lock:
			return super(Maestro, self).write(message)

# simple test harness to walk through the various steps.
def _test(port="/dev/ttyACM0", channel=0x05):
	m = Maestro(port=port, channel=channel)
	m.setTarget(8428)	# 0 resistance
	time.sleep(1.0)
	m.setTarget(5090)	# 1 resistance
	time.sleep(1.0)
	m.setTarget(4600)	# 2 resistance
	time.sleep(1.0)
	m.setTarget(2796)	# 3 resistance
	time.sleep(2.0)
	m.setTarget(8428)
	m = None	
