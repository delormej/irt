#!/usr/bin/env python
#################################################################
# 
# By Jason De Lorme <jjdelorme@yahoo.com>
# http://www.roadacious.com
#

import RPIO, time, threading
from RPIO import PWM
from antprotocol.protocol import log

BCM_CHANNEL_OPTEK = 17			# Speed sensor
BCM_CHANNEL_BUTTON_III = 27		# RF button III
PWM_DMA_CHANNEL = 14

# ----------------------------------------------------------------------------
#
# Simple pulse width modulation wrapper responsible for moving the Servo.
#
# ----------------------------------------------------------------------------
class Servo(object):

	def __init__(self, gpio=4, debug=False):
		self._gpio = gpio
		self._position = 0
		self._lock = threading.Lock()

	def __del__(self):
		pass

	#
	# (Thread Safe) Moves the servo to the position.
	# Note that the bounds are from 2796 -> 8428 in 1/4 seconds
	#
	def setTarget(self, position):
		if (position < 2796 or position > 8428):
			raise "Position out of range."
		
		# because we step in 10us increments, round it to closest 10.
		rounded_pos = int(round(position/4/10.0)*10.0)

		with self._lock:
			servo = PWM.Servo()
			servo.set_servo(self._gpio, rounded_pos)
			# sleeps the thread while the servo moves which is 20ms
			time.sleep(0.5)
			servo.stop_servo(self._gpio)
			servo = None
			self._position = position

	#
	# (Thread Safe) Gets the last position the servo was set to.
	#
	def getPosition(self):
		with self._lock:	
			return self._position

	#
	# no-op - here for legacy compat, always returns false.
	#
	def isMoving(self):
		return false;

# ----------------------------------------------------------------------------
#
# Simple GPIO wrapper.
#
# ----------------------------------------------------------------------------
class Board(object):

	def __init__(self, debug=True):
		self._debug = debug
		RPIO.setup(BCM_CHANNEL_OPTEK, RPIO.IN) # , pull_up_down=RPIO.PUD_UP)
		RPIO.setup(BCM_CHANNEL_BUTTON_III, RPIO.IN)

		self._revs = 0

		RPIO.add_interrupt_callback(BCM_CHANNEL_OPTEK, edge='falling', \
			callback=self._onDrumRevolution, threaded_callback=True, debounce_timeout_ms=10)

		RPIO.add_interrupt_callback(BCM_CHANNEL_BUTTON_III, edge='rising', \
			callback=self._onButton, threaded_callback=True, debounce_timeout_ms=200)

		RPIO.wait_for_interrupts(threaded=True)

	def __del__(self):
		#GPIO.remove_event_detect(BCM_CHANNEL_OPTEK)
		#GPIO.remove_event_detect(BCM_CHANNEL_BUTTON_III)
		RPIO.del_interrupt_callback(BCM_CHANNEL_OPTEK)

	def get_revs(self):
		return self._revs / 2

	@log
	def _onDrumRevolution(self, gpio_id, value):
		self._revs += 1
		return

	def _onButton(self, gpio_id, value):
		print "Button III Clicked"
		return