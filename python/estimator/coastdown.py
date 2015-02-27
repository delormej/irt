import sys
import os
import math
import numpy as np
from scipy import signal
import matplotlib.pyplot as plt
import scipy.optimize as spo, scipy.stats as stats 


def contact_patch():
	# 'Using this method: http://bikeblather.blogspot.com/2013/02/tire-crr-testing-on-rollers-math.html
	wheel_diameter_cm = 210.7 / math.pi
	drum_diameter_cm = 26 / math.pi
	patch = ((1/(1+ (wheel_diameter_cm/drum_diameter_cm)))**0.7)
	
	return patch

# used to fit curve by drag (K) an rolling resistance (rr) variables
# to return power in watts given speed (v) in mps
def drag_rr_func(v, K, rr):
	p = K * v**2 + ((v * rr) / contact_patch())

	return p

# F = ma, returns the inertia mass
def get_inertia_mass(entry_speed, exit_speed, time, avg_power):
	a = (entry_speed - exit_speed) / time
	f = avg_power / entry_speed
	
	# If you need to hard code, uncomment and fill in these values:
	#a = ((18.3 * 0.44704) - exit_speed) / 4.28356 # this is the time to decelerate from that speed
	#a = 1.516073732
	#f = 134 / (15 * 0.44704) # watts / mps
	
	m = f / a

	return m

# fit speed:power by drag & rolling resistance
# returns K (drag), rr (rolling resistance)
def fit_bike_power_by_drag_rr(mps, power):
	pars, covar = spo.curve_fit(drag_rr_func, mps, power)
	return pars[0], pars[1]

def fit_bike_power_by_decel(mass, coeff):
	#
	# Fits the speed to bike power curve based on the coastdown.
	# mass: intertia mass, exit_speed: last speed recorded at time 0
	# coeff: coefficients a,b,c used for polynomial of speed to coastdown duration
	#			where y = ax^2 + bx + c
	#
	# returns: x: speed (mps), y: power (watts)

	# speed in mps
	x = np.linspace(5 * 0.44704, 35 * 0.44704)

	# duration in seconds to coastdown from the speed
	y = (coeff[0] * (x**2)) + (coeff[1] * x) + coeff[2]

	min_x = min(x)
	min_y = min(y)

	# first element contains 0 value, so skip
	x = x[1:]
	y = y[1:]

	# deceleration rate for a given speed
	decel = (x - min_x) / (y - min_y)

	# power for a given speed
	pwr = mass * decel * x
	
	return x, pwr
	
# fit to a 2nd degree polynomial
def fit_poly2d(x_new, x, y):
	coefficients = np.polyfit(x, y, 2)
	polynomial = np.poly1d(coefficients)
	ys = polynomial(x_new)
	# y = ax^2 + bx + c
	f = ("y = %sx^2 + %sx + %s" % (coefficients[0], coefficients[1], coefficients[2]))
	print(f)
#	plt.plot(x_new, ys, 'r--')
	#plt.xlim(min(x_new), max(x_new))
	
	# return the text
	return f, coefficients, ys

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

# fits linear slope/intercept and returns new x, y
def fit_linear(x_new, x, y):
	slope, intercept, r_value, p_value, std_err = stats.linregress(x,y)
	print("slope: %s intercept: %s" % (slope, intercept) )
#	plt.plot(x_new, x_new * slope + intercept, 'g')
	#return x_new, x_new * slope + intercept
	return slope, intercept


# returns the index of the last occurence of the maximum speed
# which is where the deceleraton begins.
def get_max_speed_idx(x):
    # Speed is in reverse order, start at the end and iterate backwards until it stops accelerating.
    for ix in range(len(x)-1, 0, -1):
        if (x[ix] >= (x[ix-1])):
            print("max speed", x[ix])
            # return the item before this index to ensure clean data. 
            return ix + 1

    # if we made it here we're in trouble
    print("Didn't find a deceleration point.")
    return 0

def get_min_speed_idx(x):
	# get the index of the first occurence of 1 tick delta
	ix = np.where(x == 1)[0]
	if (ix > 0):
		return ix
	else:
		# return the last element
		return len(x)

def get_avg_watts(power, decel_idx):
	#
	# Looks back 3 seconds and gets the average power from the deceleration point.
	# power:	 nparray of (time, events, accumumulated power)
	# decel_idx: index of where data starts to decelerate  
	#
	
	# get the power event and accum_power of deceleration point
	end_pwr_time = power[decel_idx, 0]
	end_pwr_event = power[decel_idx, 1]
	end_pwr = power[decel_idx, 2]

	# go back 3 seconds
	start_pwr_time = end_pwr_time - (2048 * 3) # 1/2048's of a second
	start_idx = 0

	for idx in range(0, decel_idx):
		if (power[decel_idx-idx, 0] <= start_pwr_time):
			start_idx = decel_idx-idx
			break
	
	# return 0 if we didn't find a time 3 seconds back, likely not enough data
	if (start_idx == 0):
		return 0

	# determine the count of power events
	events = end_pwr_event - power[start_idx, 1]

	# get the total accum power between the events
	accum_power = end_pwr - power[start_idx, 2]

	# calculate the average
	avg_power = accum_power / events
	
	return avg_power

def main(file_name):
	#
	# load the csv file
	#
	time, tick_delta, events_pwr, accum_pwr = np.loadtxt(file_name, delimiter=',', skiprows=2,
							dtype=[('ms', int), ('tick_delta', int), ('events', int), ('accum_pwr', int)], 
							usecols=[0, 2, 4, 5], unpack=True, comments='"')

	# todo: add logic here to determine if you're using older than 1.4.3 that you use the old logic.

	mps = np.empty(len(tick_delta)/4)
	seconds = np.empty(len(time)/4)
	mps[0] = 0;
	seconds[0] = 0;

	# dt = delta ticks
	# ds = delta seconds
	ix = 0
	for idx, val in enumerate(tick_delta):
		if (idx > 0 and idx % 4 == 0):
			if (val < tick_delta[idx-4]):
				dt = val + (tick_delta[idx-4] ^ 0xFFFF)
			else:
				dt = val-tick_delta[idx-4]

			if (time[idx] < time[idx-4]):
				ds = time[idx] + (time[idx-4] ^ 0xFFFF)
			else:
				ds = time[idx]-time[idx-4]

			if (ix > 0):
			    seconds[ix] = (ds/2048) + seconds[ix-1]
			else:
			    seconds[ix] = (ds/2048)
			mps[ix] = (dt * 0.1115/2) / (ds/2048)
			ix = ix +1

	# get the max & min speeds
	ix_max = get_max_speed_idx(mps)
	ix_min = get_min_speed_idx(mps)

	# calculate the average power at point of deceleration
	power = np.column_stack((time, events_pwr, accum_pwr))
	avg_power = get_avg_watts(power, ix_max)
	avg_power_text = "Average watts on entry: %s" % (avg_power)
	print(avg_power_text)

	# slice to build a new array between min & max
	seconds = seconds[ix_max:ix_min]
	mps = mps[ix_max:ix_min]
	
	print("Max speed was at ix: %s, min was at ix: %s" % ( time[ix_max*4-1], time[ix_min*4-1] ))

	# calculate new x/y to represent time in ms since 0 and speed in meters per second
	y = (seconds.max() - seconds)	# seconds until min
	x = mps #tick_delta * 20 * 0.1115/2		# meters per second

	# get core parameters
	speed_on_entry = x.max()
	speed_on_exit = x.min()
	duration = seconds.max() - seconds[0]
	results = ("entry_mps = %s, exit_mps = %s, duration = %ss" % (speed_on_entry, speed_on_exit, duration))
	print(results)

	# first subplot
	plt.subplot(2, 1, 1)

	# Set axis labels
	plt.xlabel('Speed (mps)')
	plt.ylabel('Coastdown Time (seconds)')

	# plot actual values
	plt.plot(x, y, 'bo')
	plt.ylim(ymin=0)
	plt.xlim(xmin=0, xmax=x.max())

	# if I wanted to reverse the axis visualy, also need to adjust min/max for this.
	#plt.gca().invert_yaxis()

	# come up with even set of new x's - makes up for missing data points, etc...
	x_new = np.linspace(x[0], x[-1], len(x))
	#fp = fit_power(x_new, x, y)
	f2d, coeff, y_new = fit_poly2d(x_new, x, y)
	plt.plot(x_new, y_new, 'r--')

	slope, intercept = fit_linear(x_new, x, y) 
	x_lin = x_new
	y_lin = x_new * slope + intercept
	plt.plot(x_lin, y_lin, 'g')

	# print the formula
	#plt.text(x.max() * 0.05, y.max() * 0.95, fp, fontsize=8, color='y')
	plt.text(x.max() * 0.05, y.max() * 0.90, f2d, fontsize=8, color='r')
	plt.text(x.max() * 0.05, y.max() * 0.85, results, fontsize=8)
	plt.text(x.max() * 0.05, y.max() * 0.80, avg_power_text, fontsize=8)
	
	# get the mass in F=ma
	mass = get_inertia_mass(speed_on_entry, speed_on_exit, duration, avg_power)
	print("mass", mass)

	# smooth out deceleration and get back a curve of speed (mps) to power (watts)
	x_pwr, y_pwr = fit_bike_power_by_decel(mass, coeff)

	# 2nd subplot
	plt.subplot(2, 1, 2)

	# plot speed to power based on deceleration
	plt.plot(x_pwr * 2.23694, y_pwr, 'bo')
	
	try:
	    # plot speed to power based on drag and rolling resistance
	    K, rr = fit_bike_power_by_drag_rr(x_pwr, y_pwr)
	    plt.plot(x_pwr * 2.23694, drag_rr_func(x_pwr, K, rr), 'r-')	
	    plt.ylabel('Power (watts)')
	    plt.xlabel('Speed (mph)')
	    #plt.xlim(xmax=x.max())
	except:
	    print("Error calculating rr & drag.")
	    K = 0
	    rr = 0

	# fit a linear equation to the power curve
	pwr_slope, pwr_intercept = fit_linear(x_pwr, x_pwr, y_pwr)
	x_pwr_lin = x_pwr
	y_pwr_lin = x_pwr * pwr_slope + pwr_intercept
	plt.plot(x_pwr_lin * 2.23694, y_pwr_lin, 'g--')

	# uncomment to compare another linear equation:
	#plt.plot(x_pwr_lin * 2.23694, x_pwr_lin * 20.707 - 19.976 , 'm--')

	flin = "Slope: %i, Offset: %i" % (round(pwr_slope * 1000, 0), round(pwr_intercept * -1000,0))
	fdrag = "K: %s rr: %s" % (K, (rr/contact_patch()) )
	print(fdrag)

	plt.text(10, y_pwr.max() * 0.90, fdrag, fontsize=8, color='r')
	plt.text(10, y_pwr.max() * 0.85, flin, fontsize=8, color='g')

	# turns grid lines on.
	#plt.grid(b=True, which='major', color='b', linestyle='-')

	# show and save the chart
	(fig_name, ext) = os.path.splitext(file_name)
	plt.savefig(fig_name + '.png')
	plt.show()
	
if __name__ == "__main__":
	if (len(sys.argv) > 2):
		speed_col = int(sys.argv[2])
	main(sys.argv[1])

