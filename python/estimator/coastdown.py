import numpy as np
from scipy import signal
import matplotlib.pyplot as plt
import scipy.optimize as spo, scipy.stats as stats 

input_file_name = "coastdown.csv"

# fit to a 2nd degree polynomial
def fit_poly2d(x_new, x, y):
	coefficients = np.polyfit(x, y, 2)
	polynomial = np.poly1d(coefficients)
	ys = polynomial(x)
	# y = ax^2 + bx + c
	f = ("y = %sx^2 + %sx + %s" % (coefficients[0], coefficients[1], coefficients[2]))
	print(f)
	plt.plot(x_new, ys, 'r--')

def power_func(x, a, b):
	#original: return a*(x**b) 
	#b1 = -2.51468709e-07  
	#a1 = -4.24881527e+07
	#return a*((x**2)+b)  # F = K(V^2) + b
	return a*(x**b)

# fit to a power curve
def fit_power(x_new, x, y):
	pars, covar = spo.curve_fit(power_func, x, y)
	plt.plot(x_new, power_func(x_new, *pars), 'r--')
	#print(pars)

def fit_linear(x_new, x, y):
	slope, intercept, r_value, p_value, std_err = stats.linregress(x,y)
	print("slope: %s intercept: %s" % (slope, intercept) )
	plt.plot(x_new, x_new * slope + intercept, 'g')


# load the csv file
time, tick_delta = np.loadtxt(input_file_name, delimiter=',', skiprows=1,
						dtype=[('ms', int), ('tick_delta', int)], usecols=[0, 2], unpack=True, comments='"')

# calculate new x/y to represent time in ms since 0 and speed in meters per second
x = (time - time[0]) / 1000.0
y = tick_delta * 20 * 0.1115/2

# plot actual values
plt.plot(x, y)

# come up with even set of new x's - makes up for missing data points, etc...
x_new = np.linspace(x[0], x[-1], len(x))
#fit_power(x_new, x, y)
fit_poly2d(x_new, x, y)
fit_linear(x_new, x, y)

# show the chart
plt.show()
