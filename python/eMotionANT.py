#!/usr/bin/env python
##############################################################################
# 
# By Jason De Lorme <jjdelorme@yahoo.com>
# http://www.roadacious.com
#
from ant_define import *
from speed import *
from antprotocol.bases import GarminANT
from antprotocol.protocol import log
import array, threading, time

# ----------------------------------------------------------------------------
#
# Class responsible for all ANT+ read/write interaction with eMotion device.
# On start() begins a loop that reads speed, sends power and responds to 
# messages adjusting mag resistance.
#
# ----------------------------------------------------------------------------
class eMotionANT(GarminANT):

	def __init__(self, maestro, speed, power, resistance, debug=False):
		super(eMotionANT, self).__init__(debug=debug)
		self._maestro = maestro
		self._speed = speed
		self._power = power
		self._resistance = resistance
		self._use_maestro_speed = (speed.__class__ == MaestroSpeed) # polymorphism hack
		self._init_ant()

	def __del__(self):
		pass

	#
	# Main loop, reads messages and sends power data.
	#
	def start(self):
		if self._use_maestro_speed:
			self._start_speed_thread()
			
		# state maintained for reading sequence burst messages.
		self._burst_resistance_state = False

		while True:
			msg = self._receive_message()
			# 0 length message normally indicates time out, no messages available.
			if len(msg) == 0:
				continue;

			self._process_message(msg)
		
	#
	# Starts a seperate thread to read speed and calculate power if using 
	# MaestroSpeed.  This isn't used if reading ANTSpeed.
	#
	def _start_speed_thread(self):
		threading.Thread(target=self._process_speed).start()

	#
	# Initializes ANT by opening, reseting, sending network key and opening the 
	# speed and power channels.
	#
	@log
	def _init_ant(self):
		self.open()
		self.reset()   
		self.send_network_key(0, ANT_NETWORK_KEY)
		self._openSpeedCadenceChannel()
		self._openPowerChannel()

	#
	# Handles parsing of ANT messages and dispatching accordingly.
	#
	def _process_message(self, msg):
		msgId = msg[INDEX_MESG_ID]
		if msgId == MESG_BROADCAST_DATA_ID:
			if msg[INDEX_CHANNEL_NUM] == ANT_SPEED_RX_CHANNEL \
					and not self._use_maestro_speed:
				self._process_speed_message(msg)

		elif msgId == MESG_BURST_DATA_ID:
			seq = msg[3]
			if seq == 0x01 and msg[4] == 0x48:
				self._burst_resistance_state = True
				self._resistance.setMode(msg[5])

			elif seq == 0x21:
				pass

			elif seq == 0xC1 and self._burst_resistance_state == True:
				# 3rd message and the one that contains the level.
				self._burst_resistance_state = False # reset flag
				# acknowledge the burst 
				self._send_message(0x4f, ANT_POWER_CHANNEL, [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
				self._resistance.setLevel(msg[4:5])

	# 
	# Handles speed message, calculates power and dispatches the ANT message.
	#
	def _process_speed_message(self, msg):
		mph = self._speed.get_mph(msg)
		self._calc_power(mph)

	#
	# Gets speed from Maestro and calculates power.
	#
	@log
	def _process_speed(self):
		while True:
			mph = self._speed.get_mph()
			self._calc_power(mph)
			time.sleep(0.02)

	#
	# Reads mag resistance level, calculates power from speed and sends ANT power message.
	#
	def _calc_power(self, mph):
		level = self._resistance.getLevel()
		watts = self._power.calcWatts(mph, level)
		self._transmitPower(watts)

	@log
	def _openSpeedChannel(self):
		return self._openChannel(ANT_SPEED_RX_CHANNEL, SPEED_DEVICE_TYPE, \
			SPEED_FREQUENCY, SPEED_MESSAGE_PERIOD, SPEED_TIME_OUT)

	@log
	def _openSpeedCadenceChannel(self):
		return self._openChannel(ANT_SPEED_RX_CHANNEL, SPEED_CAD_DEVICE_TYPE, \
			SPEED_FREQUENCY, SPEED_CAD_MESSAGE_PERIOD, SPEED_TIME_OUT)

	@log
	def _openPowerChannel(self):
		# State required for sending power events.
		self._powerEventCount = 0	# rolls over at 255
		self._powerEvents = 0		# doesn't roll over, cumulative
		self._accumPower = 0

		return self._openChannel(ANT_POWER_CHANNEL, POWER_DEVICE_TYPE, \
			POWER_FREQUENCY, POWER_MESSAGE_PERIOD, SPEED_TIME_OUT, \
			CHANNEL_TYPE_TX, POWER_KCKR_TRANSMISSION_TYPE, POWER_DEVICE_NUMBER)

	#
	# Opens an ANT channel for communication.
	#
	def _openChannel(self, channelId, deviceType, frequency, period, \
			timeout=0x0c, channelType=CHANNEL_TYPE_RX, transmissionType=0x00, \
			deviceNumber=[0x00,0x00]):
		# configure the channel
		
		self.assign_channel(channelId, channelType)
		self.set_channel_id(channelId=channelId, deviceType=deviceType, \
			deviceNumber=deviceNumber, transmissionType=transmissionType)
		self.set_channel_frequency(channelId, frequency)
		self.set_channel_period(channelId, period)
		self.set_search_timeout(channelId, timeout)
       
		self.open_channel(channelId)

		return channelId

	#
	# Send ANT+ Power
	#
	@log
	def _transmitPower(self, watts):
		accumPower = self._accumPower + watts

		# roll over 
		if (self._powerEventCount >= 255 or accumPower >= 65535):
			self._powerEventCount = 0
			accumPower = watts

		self._accumPower = accumPower
		page_number = 0x10	# power-only message
		self._powerEventCount+=1
		pedal = 0xFF
		cadence = 0xFF

		print "accumPower: " + str(accumPower) + " watts: " + str(watts)

		msg = struct.pack('@BBBBBBHH', \
			MESG_BROADCAST_DATA_ID, \
			ANT_POWER_CHANNEL, \
			page_number, \
			self._powerEventCount, \
			pedal, \
			cadence, \
			accumPower, \
			watts) 
		
		self._send_message(array.array('B', msg).tolist())
		self._powerEvents+=1

		self._transmitCommon()

	#
	# Sends required common messages for manufacturer and product.
	#
	def _transmitCommon(self):
		# Interleave manufacturer & product page page every 121 messages.
		if self._powerEvents % 242 == 0:
			self._send_message(POWER_PRODUCT_MESSAGE)
			self._powerEvents+=1
		elif self._powerEvents % 121 == 0:
			self._send_message(POWER_MANUFACTURER_MESSAGE)
			self._powerEvents+=1
