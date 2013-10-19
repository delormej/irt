#!/usr/bin/env python
#################################################################
# 
# By Jason De Lorme <jjdelorme@yahoo.com>
# http://www.roadacious.com
#
from antprotocol.bases import GarminANT

ANT_NETWORK_KEY = [0xB9,0xA5,0x21,0xFB,0xBD,0x72,0xC3,0x45]

class eMotionANT(GarminANT):
	# 
	# Main interface to the eMotion wireless device.  Responsible for sending / recieving ANT+ messages.
	#
	def __init__(self, debug=False):
		super(eMotionANT, self).__init__(debug=debug)
		self.open()

	def __del__(self):
		self.close()

	# Here's what the program should do:
	#
	# 1. initialize ANT+ via USB
	# 2. get the user profile data (rider+bike weight)
	# 3. listen for ANT+ messages
	#		dispatch accordingly (speed, resistance, power/calibration/etc)
	def start(self):
		# let's rock and roll!
		print "Here's where it all begins."

		# ANT device initialization

		self.reset()   
		self.send_network_key(0, ANT_NETWORK_KEY)
		
		self._send_message(0x54)
		x = self._receive_message()
		print x[3]
