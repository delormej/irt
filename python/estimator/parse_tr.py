import sys
import os
import math
import numpy as np
import matplotlib.pyplot as plt

def moving_average(x, n, type='simple'):
    """
    compute an n period moving average.

    type is 'simple' | 'exponential'

    """
    x = np.asarray(x)
    if type=='simple':
        weights = np.ones(n)
    else:
        weights = np.exp(np.linspace(-1., 0., n))

    weights /= weights.sum()


    a =  np.convolve(x, weights, mode='full')[:len(x)]
    a[:n] = a[n]
    return a

def main(file_name):
	#
	# load the csv file
	#
	if (os.path.basename(file_name).startswith("irt_")):
		power_watts, kmh, target_watts, emr_watts, servo_pos = \
		np.loadtxt(file_name, delimiter=',', skiprows=3,
							dtype=[('emr_watts', float), ('kmh', float), ('target_watts', float), ('power_watts', float), ('servo_pos', int)], 
							usecols=[5, 3, 6, 6, 7], unpack=True, comments='"')
		minutes = range(0, power_watts.shape[0])

	else:
		minutes, power_watts, kmh, target_watts, emr_watts, servo_pos = \
		np.loadtxt(file_name, delimiter=',', skiprows=3,
								dtype=[('minutes', float), ('emr_watts', float), ('kmh', float), ('target_watts', float), ('power_watts', float), ('servo_pos', int)], 
								usecols=[1, 2, 8, 9, 10, 11], unpack=True, comments='"')

	labels = []

	kmh = kmh * 0.621371

	ma_speed = moving_average(kmh, 30)
	ma_power30 = moving_average(power_watts, 30)
	ma_power10 = moving_average(power_watts, 10)
	ma_est_power = moving_average(emr_watts, 30)

	plt.rc('axes', grid=True)
	plt.rc('grid', color='0.75', linestyle='-', linewidth=0.5)

	#left, width = 1, 0.8
	#rect1 = [left, 1, width, 1]
	#fig = plt.figure()
	#ax1 = fig.add_axes(rect1)
	#ax2 = fig.add_axes(rect1, sharex=ax1)
	ax1 = plt.subplot(311)
	ax2 = plt.subplot(312,sharex=ax1)
	#ax2 = ax1.twinx()
	ax3 = plt.subplot(313,sharex=ax1)

	# ax1 = Speed & Servo
	ax1.plot(minutes, kmh)
	#labels.append(r'y = %s' % ('kmh'))
	ax1.set_ylim(10, 30)

	ax1.plot(minutes, ma_speed)

	ax2.plot(minutes, servo_pos, color='r')
	ax2.set_ylim(800, 1700)

	ax3.plot(minutes, power_watts, 'r')
	#labels.append(r'y = %s' % ('Actual'))
	
	ax3.plot(minutes, emr_watts, color='c')
	#labels.append(r'y = %s' % ('Estimated'))

	#plt.legend(labels, loc='upper right')

	ax3.plot(minutes, ma_power30, color='b')
	ax3.plot(minutes, ma_power10, color='lightblue')
	ax3.plot(minutes, ma_est_power, color='y')

	#ax2.stackplot(minutes, target_watts)
	ax3.plot(minutes, target_watts, linestyle='--', linewidth='2', color='g', zorder=100)
	#labels.append(r'y = %s' % ('Target'))
	ax3.set_ylim(100, 425)

	plt.show()

if __name__ == "__main__":
	if (len(sys.argv) > 2):
		speed_col = int(sys.argv[2])
	main(sys.argv[1])

