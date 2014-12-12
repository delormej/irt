import numpy as np
from scipy import signal
import matplotlib.pyplot as plt
import scipy.optimize as spo, scipy.stats as stats 

#input_file_name = "coastdown.csv"

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

def main(file_name):
	#
	# load the csv file
	#
	time, tick_delta = np.loadtxt(file_name, delimiter=',', skiprows=1,
							dtype=[('ms', int), ('tick_delta', int)], usecols=[0, 2], unpack=True, comments='"')

	# calculate new x/y to represent time in ms since 0 and speed in meters per second
	y = (time.max() - time) / 1000.0	# seconds until min
	x = tick_delta * 20 * 0.1115/2		# meters per second

	# Set axis labels
	plt.xlabel('Speed (mps)')
	plt.ylabel('Coastdown Time (seconds)')

	# plot actual values
	plt.plot(x, y)
	plt.ylim(ymin=0)
	plt.xlim(xmax=x.max())

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

	# show the chart
	plt.show()

main("C:\\Users\\Jason\\SkyDrive\\InsideRide\\Tech\\Ride Logs\\Tariq\\20141206_Calibration\\coastdown.csv")
