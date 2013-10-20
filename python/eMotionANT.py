#!/usr/bin/env python
#################################################################
# 
# By Jason De Lorme <jjdelorme@yahoo.com>
# http://www.roadacious.com
#
from antprotocol.bases import GarminANT
from antprotocol.protocol import log
from eMotion import Power,Resistance

#
# global defines
#
ANT_NETWORK_KEY = [0xB9,0xA5,0x21,0xFB,0xBD,0x72,0xC3,0x45]

INDEX_MESG_ID=2
INDEX_MESG_DATA=3
INDEX_CHANNEL_NUM=INDEX_MESG_DATA
MESG_BROADCAST_DATA_ID=0x4E

RESPONSE_NO_ERROR=0x00
		
MESG_RESPONSE_EVENT_ID=0x40
MESG_CHANNEL_ID_ID=0x51
MESG_BURST_DATA_ID=0x50

class eMotionANT(GarminANT):
	# 
	# Main interface to the eMotion wireless device.  Responsible for sending / recieving ANT+ messages.
	#
	def __init__(self, profile, debug=False):
		super(eMotionANT, self).__init__(debug=debug)
		self._profile = profile
		self.device_number = 0
		self.open()
		self.reset()   
		self.send_network_key(0, ANT_NETWORK_KEY)

	def __del__(self):
		self.close()

	def _get_deviceNumber(self, message):				
		#num = struct.unpack('H', msg[INDEX_MESG_DATA:INDEX_MESG_DATA+1])
		#return num
		pass

	def set_channel_id(self, channelId=0, deviceNumber=0, pairing=0, deviceType=0, transmissionType=0):
		self._send_message(0x51, channelId, deviceNumber, pairing, deviceType, transmissionType)
		self._check_ok_response()

	@log
	def _openSpeedChannel(self):
		SPEED_DEVICE_TYPE=0x7B
		SPEED_MESSAGE_PERIOD=[0xB6, 0x1F]
		SPEED_FREQUENCY=0x39 #57
		SPEED_TIME_OUT=0x0C

		#################################################
		# TODO: support checking for both speed (type 123)
		#		AND speed and cadence (type 121)
		#################################################

		# configure the channel
		self._chan = 0
		self.assign_channel()
		self.set_channel_id(channelId=self._chan, deviceType=SPEED_DEVICE_TYPE)
		self.set_channel_frequency(SPEED_FREQUENCY)
		self.set_channel_period(SPEED_MESSAGE_PERIOD)
		self.set_search_timeout(SPEED_TIME_OUT)
       
		self.open_channel()

		return self._chan

	# Here's what the program should do:
	#
	# 1. initialize ANT+ via USB
	# 2. get the user profile data (rider+bike weight)
	# 3. listen for ANT+ messages
	#		dispatch accordingly (speed, resistance, power/calibration/etc)
	def start(self):

		# let's rock and roll!
		speedChannel = self._openSpeedChannel()
		speed = Speed(self._profile.wheel_size)

		power = Power(self._profile.weight)
		resistance = Resistance()

		while True:
			msg = self._receive_message()
			# 0 length message normally indicates time out, no messages available.
			if len(msg) == 0:
				continue;
			#
			# Raw ANT message structure is:
			#	Sync|Msg length|Msg Id|{Content(Bytes 0-(N-1))}|Checksum 
			#
			# Speed sensor message content is:
			# {channel num|PageChangeToggle-msb,PageNumber:0-7 bits|reserved*3bytes(FF)|time*2bytes|revs*2bytes}
			#

			# is this a broadcast message?
			msgId = msg[INDEX_MESG_ID]
			if msgId == MESG_BROADCAST_DATA_ID:
				if msg[INDEX_CHANNEL_NUM] == speedChannel:
					speed_page = Speed_Page0(msg)
					
					mph = speed.get_mph(speed_page)
					level = resistance.getLevel()
					watts = power.calcWatts(mph, level)
					
					print("Speed|Watts|Level:: " + \
						str(mph) + " | " + \
						str(watts) + " | " + \
						str(level) )
			"""
			#TODO: implement this in the future
			if self.device_number == 0:
				# request message to get below

			elif msgId == MESG_CHANNEL_ID_ID:
				self.deviceNumber = self._get_deviceNumber(msg)
				print "Device number" + str(deviceNumber)
			"""

class Speed_Page0(object):
	def __init__(self, msg):
		try:
			page = msg[INDEX_MESG_DATA+1:len(msg)-1]
			self.time = int(page[4])
			self.time |= int(page[5]) << 8
			self.revs = int(page[6])
			self.revs |= int(page[7]) << 8
		except:
			raise "Unable to read speed page 0 data."

class Speed(object):
	#
	# Reads speed messages and stores state about last speed.
	#
	def __init__(self, wheel_size):
		self._wheel_size = wheel_size
		self._lastTime = 0
		self._cumulativeRevCount = 0
		self._last_mph = 0

	def get_mph(self, page):
		if page.time == self._lastTime or page.revs < self._cumulativeRevCount:
			return self._last_mph

		mph = 0

		# for the very first call, just store, don't calc.
		if self._lastTime > 0:
			# caclulate speed
			mph = self._calc_speed(page.time, page.revs)
			self._last_mph = mph

		# store for next call
		self._lastTime = page.time 
		self._cumulativeRevCount = page.revs

		return mph

	def _calc_speed(self, time, revs):
		meters_per_sec = (self._wheel_size * (revs - self._cumulativeRevCount) * 1024)/ \
			(time - self._lastTime)
		
		# convert to mp/h
		mph = meters_per_sec * 2.23693629 
		return mph