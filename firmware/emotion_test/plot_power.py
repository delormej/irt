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

