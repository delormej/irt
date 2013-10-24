#!/usr/bin/env python
##############################################################################
# 
# By Jason De Lorme <jjdelorme@yahoo.com>
# http://www.roadacious.com
#
##############################################################################

# ----------------------------------------------------------------------------
#
# Class responsible for getting / setting profile information.
#
# ----------------------------------------------------------------------------
class Profile(object):
	# 
	# Class that is responsible for requesting, reading and storing state of user profile.
	#
	def __init__(self):
		# combined rider and bicycle weight in lbs.
		self.weight = 0
		# wheel circumference in meters
		self.wheel_size=2.096