import sys
import os
import math
import numpy as np
from scipy import signal
import matplotlib.pyplot as plt
import scipy.optimize as spo, scipy.stats as stats 
import scipy.interpolate as spi

def interp():
	# z = f(x, y)
	# x = position, y = speed, z = watts

	x = [1600, 1500, 1400, 1300, 1200, 1100, 1000, 900, 800, 700]
	y = [15, 25] # speeds
	z = [
		[0, 6, 21, 60, 116, 180, 248, 309, 349, 355], 
		#[None, 11, 36, 92, None, None, None, None, None, None], 
		[4, 15, 48, 116, 218, 345, 484, 561, 619, 619]]
	

	#x22 = [1500, 1400, 1300]
	#y22 = [11, 36, 92]

	f = spi.interp2d(x, y, z, kind='linear')

	print(f)

	x_new = np.linspace(800, 1600, 100)

	plt.plot(x_new, f(x_new, 15))
	plt.plot(x_new, f(x_new, 20))
	plt.plot(x_new, f(x_new, 25))
	
	plt.show()



def main():

	interp()
		
if __name__ == "__main__":
	main()

