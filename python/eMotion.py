#!/usr/bin/env python
#################################################################
# python eMotion object
# By Jason De Lorme <jjdelorme@yahoo.com>
# http://www.roadacious.com
#

import sys, time, usb
from antprotocol.bases import GarminANT
from antprotocol.protocol import ANTReceiveException
from profile import *
from eMotionANT import *
from speed import *
from power import *
from resistance import *
from mygpio import *

#
# Test / Main method
#
def main():

	try:
		profile = Profile()
		profile.weight = 175 # sample data
		
		board = Board(debug=True)
		speed = GPIOSpeed(board, profile.wheel_size, debug=True)
		power = Power(profile.weight)								# could be a BT power in future
		servo = Servo()
		resistance = Resistance(servo=servo, debug=True)			# could be a BTResistance in future

		emotion = eMotionANT(speed, power, resistance)	
		emotion.start()
	
	except KeyboardInterrupt:
		if emotion is not None:
			emotion._closing = True # hack for now to see if we can shutdown the thread
		print "Thank you for playing."
		return 0

if __name__ == '__main__':
    sys.exit(main())
