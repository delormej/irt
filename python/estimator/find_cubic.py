# solve cubic
import sys
import os
import math
import numpy as np
from scipy import signal
import matplotlib.pyplot as plt
import scipy.optimize as spo, scipy.stats as stats 

def cubrt(x):
	if x > 0:
		return pow(x, float(1)/3)
	elif x < 0:
		return -pow(abs(x), float(1)/3)
	else:
		return 0

def cubic(a, b, c, d):
	delta = (18 * a * b * c * d) -		\
		(4 * pow(b, 3) * d) +			\
		(pow(b, 2) * pow(c, 2)) -		\
		(4 * a * pow(c, 3)) -			\
		(27 * pow(a, 2) * pow(d, 2))

	delta0 = (pow(b, 2) - (3 * a * c))
	
	delta1 = ( (2 * pow(b, 3)) - (9 * a * b * c) + (27 * pow(a, 2) * d) )

	delta21 = (-27 * pow(a, 2) * delta)
	
	if (delta21 < 0):
		print(delta21)
		delta21 = abs(delta21)
		
	print(delta21)

	C = cubrt( ( delta1 + sqrt(delta21) ) / 2 )

	x1 = -( 1 / (3 * a) ) * ( b + C + (delta0 / C) )

	u1 = 1
	u2 = (-1 + sqrt(3)) / 2
	u3 = (-1 - sqrt(3)) / 2

	x2 = -( 1 / (3 * a) ) * ( b + (u2*C) + (delta0 / (u2*C)) )
	
	x3 = -( 1 / (3 * a) ) * ( b + (u3*C) + (delta0 / (u3*C)) )

	print(x1, x2, x3)	

	# C = 

def fit_poly(x, y):
	
	x_new = np.linspace(x[0], x[-1])
	
	coeff = np.polyfit(x, y, 4)
	poly = np.poly1d(coeff)
	ys = poly(x_new)
	
	plt.plot(x, y, marker='o', linestyle='None', color='b')
	plt.plot(x_new, ys, linestyle='--', color='r')
	plt.axhline(y=0, color='grey')

def fit_rev_poly(pos, watts):
	
	new_watts = np.linspace(watts[0], watts[-1])
	
	coeff = np.polyfit(watts, pos, 5)
	poly = np.poly1d(coeff)
	new_pos = poly(new_watts)
	
	plt.plot(new_pos, new_watts, linestyle='--', color='g')


if __name__ == "__main__":
	a = 0.00000262
	b = -0.00892523
	c = 8.90742813
	d = -2139.95198000
	
	#print(cubic(a, b, c, d))

	x = [ 1700,
		1600,
		1500,
		1400,
		1300,
		1200,
		1100,
		1000,
		900,
		800 ]

	y = [ 2,
		4,
		15,
		48,
		116,
		218,
		345,
		484,
		561,
		619 ]

	fit_poly(x, y)
	fit_rev_poly(x, y)
	plt.show()
	
