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
from maestro import *

#
# Test / Main method
#
def main():

	try:
		profile = Profile()
		profile.weight = 175 # sample data
		maestro = Maestro()
		speed = MaestroSpeed(maestro, profile.wheel_size, debug=True)		# could be ANTSpeed or BTSpeed in future
		#speed = ANTSpeed(profile.wheel_size)
		power = Power(profile.weight)		# could be a BT power in future
		resistance = Resistance(maestro)	# could be a BTResistance in future

		emotion = eMotionANT(maestro, speed, power, resistance, debug=False)	
		emotion.start()
	
	except KeyboardInterrupt:
		emotion._closing = True # hack for now to see if we can shutdown the thread
		print "Thank you for playing."
		return 0

if __name__ == '__main__':
    sys.exit(main())
