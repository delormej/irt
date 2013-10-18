#!/usr/bin/env python
#################################################################
# Pololu Maestro USB Servo Controller
# By Jason De Lorme <jjdelorme@yahoo.com>
# 

import serial, time, struct

SET_TARGET_COMMAND=0x84
GET_POSITION_COMMAND=0x90

RESISTANCE_0=8428
RESISTANCE_1=5090
RESISTANCE_2=4600
RESISTANCE_3=2796

class Maestro(object):

	def __init__(self, port="/dev/ttyACM0", baudrate=9600, channel=0x05, timeout=1, debug=False):
		self.s = serial.Serial()
		self.s.port = port
		self.s.baudrate = baudrate
		self.s.timeout = timeout
		self.channel = channel

	def open(self):
		self.s.open()

	def close(self):
		self.s.close()

	def getPosition(self):
		self.s.write( \
			chr(GET_POSITION_COMMAND) + \
			chr(self.channel))

		r = self.s.read(2)
		if r is not None:
			pos = struct.unpack('h', r)
		else:
			return None
		
		return pos[0]


	def setTarget(self, position):
		low = position&0x7f
		high = position>>7
		
		self.s.write( \
			chr(SET_TARGET_COMMAND) + \
			chr(self.channel) + \
			chr(low) + \
			chr(high))

	def setResistance0(self):
		self.setTarget(RESISTANCE_0)

	def setResistance1(self):
		self.setTarget(RESISTANCE_1)

	def setResistance2(self):
		self.setTarget(RESISTANCE_2)

	def setResistance3(self):
		self.setTarget(RESISTANCE_3)

# simple test harness to walk through the various steps.
def _test(port="/dev/ttyACM0", channel=0x05):
	m = Maestro(port=port, channel=channel)
	m.open()
	m.setTarget(8428)	# 0 resistance
	time.sleep(1.0)
	m.setTarget(5090)	# 1 resistance
	time.sleep(1.0)
	m.setTarget(4600)	# 2 resistance
	time.sleep(1.0)
	m.setTarget(2796)	# 3 resistance
	time.sleep(2.0)
	m.setTarget(8428)
	m.close()
