import sys
import os
import math
import numpy as np
from scipy import signal
import matplotlib.pyplot as plt
import scipy.optimize as spo, scipy.stats as stats 

def main(file_name):
	#
	# load the csv file
	#
	speed, position, watts, watts_verify = np.loadtxt(file_name, delimiter=',', skiprows=1,
							dtype=[('speed', float), ('position', int), ('watts', float), ('watts_verify', float)], 
							usecols=[0, 1, 2, 3], unpack=True, comments='"')

	last_idx = 0
	last_speed = speed[last_idx]
	labels = []

	for idx, val in enumerate(speed):
		if (val != last_speed):
			line, = plt.plot(position[last_idx:idx-1], watts[last_idx:idx-1])
			labels.append(r'y = %i' % (last_speed))
			
			clr=line.get_color()
			plt.plot(position[last_idx:idx-1], watts_verify[last_idx:idx-1], color=clr, linestyle='None', marker='x')
			labels.append(r'y = %i' % (last_speed))

			last_speed = val
			last_idx = idx
	
	# Plot against actual data.
	x = [1600, 1500, 1400, 1300, 1200, 1100, 1000, 900, 800, 700]
	y15 = [0, 6, 21, 60, 116, 180, 248, 309, 349, 355]
	y25 = [4, 15, 48, 116, 218, 345, 484, 561, 619, 619]
	
	x22 = [1500, 1400, 1300]
	y22 = [11, 36, 92]

	y16 = [None, 5, 20, 60, None, None, None, None, None, None]

	plt.plot(x, y15, linestyle='None', marker='o')
	labels.append(r'y = %i' % (15))
	
	plt.plot(x, y16, linestyle='None', marker='o')
	labels.append(r'y = %i' % (16))
	
	plt.plot(x22, y22, linestyle='None', marker='o')
	labels.append(r'y = %i' % (22))

	plt.plot(x, y25, linestyle='None', marker='o')
	labels.append(r'y = %i' % (25))


	plt.xlabel('Position')
	plt.ylabel('Watts')
	plt.legend(labels, loc='upper right')
	plt.axhline(y=0, color='grey')
	plt.axvline(x=1400, color='grey')
	plt.axvline(x=800, color='grey')
	plt.xlim(700, 1800)
	plt.ylim(-10, 1000)




	plt.show()

if __name__ == "__main__":
	if (len(sys.argv) > 2):
		speed_col = int(sys.argv[2])
	main(sys.argv[1])

