import time
from maestro import *

TICK_CHANNEL=1
TICK_ROLLOVER = 13120
TICKS_PER_01 = 144.0	# 144 ticks per 0.01 miles
MS_IN_HOUR = 3600.0

class speed(object):

	def __init__(self):
		self._last_time_ms = 0	# last reading in milliseconds
		self._last_count = 0	# last tick count, 144 ticks = 0.01 miles
		self._maestro = Maestro()
		self._maestro.open()

	def __del__(self):
		self._maestro.close()
	
	def get_speed(self):
		count_delta = 0
		time_delta = 0
		count = self._get_count()
		time_ms = self._get_time_ms()

		# for first second, just get the reading
		if self._last_time_ms == 0:
			self._last_time_ms = time_ms
			self._last_count = count
			return 0.0

		# if current count is less, a rollover occured
		if count < self._last_count:
			count_delta = (TICK_ROLLOVER - self._last_count) + count
		else:
			count_delta = count - self._last_count

		miles = (count_delta / TICKS_PER_01) / 100.0

		time_delta = time_ms - self._last_time_ms
		hours = time_delta / MS_IN_HOUR

		speed = miles / hours

		# store for next loop
		self._last_time_ms = time_ms
		self._last_count = count

		return speed

	def _get_count(self):
		return self._maestro.getFastPosition(TICK_CHANNEL)

	def _get_time_ms(self):
		return time.time()

def _test(self):
	s = speed()
	while True:
		time.sleep(1.0)
		print "Speed: " + str(s.get_speed())