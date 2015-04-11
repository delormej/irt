import sys
import os
import math
import numpy as np
import matplotlib.pyplot as plt
import csv
import magnet as mag

"""
Calculates a moving average of an array of values.
"""
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

"""
Initializes the co-efficients for magnet power.
"""	
def init_mag():
	low_speed = 10 * 0.44704
	low_a = 1.30101069013e-06
	low_b = -0.00408959829622
	low_c = 3.89744863571
	low_d = -992.651847098

	high_speed = 25 * 0.44704
	high_a = 2.97088134733e-06
	high_b = -0.00984147720024
	high_c = 9.8702475382
	high_d = -2663.19364338

	mag.set_coeff(low_speed, 
		low_a, 
		low_b, 
		low_c, 
		low_d, 
		high_speed, 
		high_a, 
		high_b, 
		high_c,
		high_d)

"""
Calculates power based coast down fit (drag & rr), speed and magnet position.
"""
def get_power(speed_mps, servo_pos, drag, rr):
	# Calculates non-mag power based on speed in meters per second.
	no_mag_watts = drag*speed_mps**2+rr # Force = K(V^2) + (mgCrr)	
	
	# If the magnet is ON the position will be less than 1600
	if servo_pos < 1600:
		# Calculates the magnet-only power based on speed in meters per second.
		mag_watts = mag.watts(speed_mps, servo_pos)
		#print(servo_pos, mag_watts)
	else:
		mag_watts = 0	
	
	return no_mag_watts + mag_watts
	
"""
Reads an irt log file and returns the Drag and RR settings.
"""
def read_calibration(file_name):
	drag = 0
	rr = 0
	with open(file_name, 'r') as f:
		for row in reversed(list(csv.reader(f))):
			if row[0] == 'RR':
				rr = float(row[1])
			if row[0] == 'Drag':
				drag = float(row[1])
				return drag, rr

"""
Returns an array of the same dimension of mph & servo_pos with estimated power values.
"""
def estimate_power(mph, servo_pos, drag, rr):
    #assert mph.shape[0] == servo_pos.shape[0], "Speed and servo must be the same shape."

	est_watts = [] #mph.shape[0]
	
	for i, speed in enumerate(mph):
		#print(speed)
		est_watts.append(get_power(speed * 0.44704, servo_pos[i], drag, rr))
	
	return est_watts
	
def main(file_name):
	
	drag = None
	rr = None
	
	if (os.path.basename(file_name).startswith("irt_")):
		#
		# load the irt csv file
		#
		#print(read_calibration(file_name))
		power_watts, kmh, target_watts, emr_watts, servo_pos = \
			np.loadtxt(file_name, delimiter=',', skiprows=3,
							dtype=[('emr_watts', float), ('kmh', float), ('target_watts', float), ('power_watts', float), ('servo_pos', int)], 
							usecols=[5, 3, 6, 6, 7], unpack=True, comments='"')
		minutes = range(0, power_watts.shape[0])
		drag, rr = read_calibration(file_name)
		print(drag, rr)

	else:
		#
		# load a TrainerRoad log file
		#
		minutes, power_watts, kmh, target_watts, emr_watts, servo_pos = \
		np.loadtxt(file_name, delimiter=',', skiprows=3,
								dtype=[('minutes', float), ('emr_watts', float), ('kmh', float), ('target_watts', float), ('power_watts', float), ('servo_pos', int)], 
								usecols=[1, 2, 8, 9, 10, 11], unpack=True, comments='"')
		kmh = kmh * 0.621371

	labels = []

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
	ax3.set_ylim(50, 450)

	if drag is not None and rr is not None:
		init_mag()
		est_watts = estimate_power(kmh, servo_pos, drag, rr)
		ax3.plot(minutes, est_watts, linestyle=':', color='orange', linewidth='3', zorder=200)
	
	plt.show()

if __name__ == "__main__":
	if (len(sys.argv) > 2):
		speed_col = int(sys.argv[2])
	main(sys.argv[1])

