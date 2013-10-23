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
		#speed = MaestroSpeed(maestro)		# could be ANTSpeed or BTSpeed in future
		speed = ANTSpeed(profile.wheel_size)
		power = Power(profile.weight)		# could be a BT power in future
		resistance = Resistance(maestro)	# could be a BTResistance in future

		emotion = eMotionANT(maestro, speed, power, resistance, debug=True)	
		emotion.start()
	
	except KeyboardInterrupt:
		print "Thank you for playing."
		return 0

if __name__ == '__main__':
    sys.exit(main())
