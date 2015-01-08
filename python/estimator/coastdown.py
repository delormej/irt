import sys
import os
import numpy as np
from scipy import signal
import matplotlib.pyplot as plt
import scipy.optimize as spo, scipy.stats as stats 

# fit to a 2nd degree polynomial
def fit_poly2d(x_new, x, y):
	coefficients = np.polyfit(x, y, 2)
	polynomial = np.poly1d(coefficients)
	ys = polynomial(x_new)
	# y = ax^2 + bx + c
	f = ("y = %sx^2 + %sx + %s" % (coefficients[0], coefficients[1], coefficients[2]))
	print(f)
	plt.plot(x_new, ys, 'r--')
	#plt.xlim(min(x_new), max(x_new))
	
	# return the text
	return f

def power_func(x, a, b):
	#original: return a*(x**b) 
	#b1 = -2.51468709e-07  
	#a1 = -4.24881527e+07
	#return a*((x**2)+b)  # F = K(V^2) + b
	return a*(x**b)

# fit to a power curve
def fit_power(x_new, x, y):
	pars, covar = spo.curve_fit(power_func, x, y)
	plt.plot(x_new, power_func(x_new, *pars), 'y-')
	f = ("y = %sx^%s" % (pars[0], pars[1]))
	print(f)
	return f

def fit_linear(x_new, x, y):
	slope, intercept, r_value, p_value, std_err = stats.linregress(x,y)
	print("slope: %s intercept: %s" % (slope, intercept) )
	plt.plot(x_new, x_new * slope + intercept, 'g')


# returns the index of the last occurence of the maximum speed
# which is where the deceleraton begins.
def get_max_speed_idx(x):
	# get the last occurence of max speed
	occurences = np.where(x == x.max())
	
	#
	# BEGIN -- trying to find where deceleration starts, but this doesn't work.
	#
	# from the max speed, iterate through the array to ensure speed is always going down
	print("max: %s" % (max(occurences[0])))
	ix = max(occurences[0])

	# initial starting position of the maximum speed
	max_pos = ix

	for i in range(ix+1, len(x)):
		print(i, x[i])
		if (x[i] > x[i-1]):
			max_pos = i
	#return max_pos
	#
	# END --
	#

	return max(occurences[0])
	

def get_min_speed_idx(x):
	# get the index of the first occurence of 1 tick delta
	ix = np.where(x == 1)[0]
	if (ix > 0):
		return ix
	else:
		# return the last element
		return len(x)

def main(file_name):
	#
	# load the csv file
	#
	time, tick_delta = np.loadtxt(file_name, delimiter=',', skiprows=2,
							dtype=[('ms', int), ('tick_delta', int)], usecols=[0, 2], unpack=True, comments='"')

	# todo: add logic here to determine if you're using older than 1.4.3 that you use the old logic.

	mps = np.empty(len(tick_delta))
	seconds = np.empty(len(time))
	mps[0] = 0;
	seconds[0] = 0;

	# dt = delta ticks
	# ds = delta seconds
	for idx, val in enumerate(tick_delta):
		if (idx > 0):
			if (val < tick_delta[idx-1]):
				dt = val + (tick_delta[idx-1] ^ 0xFFFF)
			else:
				dt = val-tick_delta[idx-1]

			if (time[idx] < time[idx-1]):
				ds = time[idx] + (time[idx-1] ^ 0xFFFF)
			else:
				ds = time[idx]-time[idx-1]

			seconds[idx] = (ds/2048) + seconds[idx-1]
			mps[idx] = (dt * 0.1115/2) / (ds/2048)

	# get the max & min speeds
	ix_max = get_max_speed_idx(mps)
	ix_min = get_min_speed_idx(mps)

	# slice to build a new array between min & max
	seconds = seconds[ix_max:ix_min]
	mps = mps[ix_max:ix_min]

	print("Max speed was at ix: %s, min was at ix: %s" % ( time[ix_max-1], time[ix_min-1] ))

	# calculate new x/y to represent time in ms since 0 and speed in meters per second
	y = (seconds.max() - seconds)	# seconds until min
	x = mps #tick_delta * 20 * 0.1115/2		# meters per second

	# get core parameters
	speed_on_entry = x.max()
	speed_on_exit = x.min()
	duration = seconds.max() - seconds[0]
	results = ("entry_mps = %s, exit_mps = %s, duration = %ss" % (speed_on_entry, speed_on_exit, duration))
	print(results)

	# Set axis labels
	plt.xlabel('Speed (mps)')
	plt.ylabel('Coastdown Time (seconds)')

	# plot actual values
	plt.plot(x, y)
	plt.ylim(ymin=0)
	plt.xlim(xmin=0, xmax=x.max())

	# if I wanted to reverse the axis visualy, also need to adjust min/max for this.
	#plt.gca().invert_yaxis()

	# come up with even set of new x's - makes up for missing data points, etc...
	x_new = np.linspace(x[0], x[-1], len(x))
	fp = fit_power(x_new, x, y)
	f2d = fit_poly2d(x_new, x, y)
	fit_linear(x_new, x, y)

	# print the formula
	plt.text(x.max() * 0.05, y.max() * 0.95, fp, fontsize=8, color='y')
	plt.text(x.max() * 0.05, y.max() * 0.90, f2d, fontsize=8, color='r')
	plt.text(x.max() * 0.05, y.max() * 0.85, results, fontsize=8)

	# show and save the chart
	(fig_name, ext) = os.path.splitext(file_name)
	plt.savefig(fig_name + '.png')
	plt.show()
	
if __name__ == "__main__":
	if (len(sys.argv) > 2):
		speed_col = int(sys.argv[2])
	main(sys.argv[1])

