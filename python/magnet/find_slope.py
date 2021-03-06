# let's see if this works.
import matplotlib.pyplot as plt
import numpy as np

# structure [position, slope, intercept]
class position:
	def __init__(self):
		self.servo = 0
		self.slope = 0.0
		self.intercept = 0.0

def fit_polynomial(x, y, color):
	x_new = np.arange(800, 1600, 100)
	coefficients = np.polyfit(x, y, 3)
	polynomial = np.poly1d(coefficients)
	ys = polynomial(x_new)
	# y = ax^2 + bx + c
	f = ("y = %sx^3 + %sx^2 + %sx + %s" % (coefficients[0], coefficients[1], coefficients[2], coefficients[3]))
	#print(r)
	#plt.subplot(2, 1, 1)
	#plt.plot(x_new, ys, linestyle='--', color=color)

	# return the text
	return f, coefficients

def interop_poly(coeffs, mps):
	# takes 2 polynomials and linearly interopolates by speed.
	return 0
	
def get_power(coeff, mps, servo_pos):
	f = np.poly1d(coeff)
	y = f(servo_pos)
		
def fit_3rd_poly(positions):
	values = []
	polys = []	
	colors = iter(['r','g','b','y','c'])
	labels = []
	#plt.subplot(2, 1, 1)
	#plt.grid(b=True, which='major', color='gray', linestyle='--')
	#plt.ylabel('Watts @ Position by Speed')
	# generate speed data 5-25 mph
	# calculate power for each position
	for mph in range(10,26,5):
		c = next(colors)
		
		mps = mph*0.44704
		power = []
		servo_pos = []

		for p in positions:
			power.append(mps * p.slope + p.intercept)
			servo_pos.append(p.servo)
			#print(p.servo, mph, watts)
			
		#plt.plot(servo_pos, power, color=c)
		labels.append(r'%1.1f' % (mph))
		
		f, coeff = fit_polynomial(servo_pos, power, c)
		values.append((mph, coeff))

		#print(mph, f)
		
		#print(p.intercept)
		
	#plt.legend(labels, loc='upper right')
	return values
		
def fit_linear(positions):
	#plt.subplot(2, 1, 2)
	#plt.ylabel('Watts @ Speed by Servo Position')
	#plt.grid(b=True, which='major', color='gray', linestyle='--')
	
	for p in positions:
		speed = []
		power = []
		
		for mph in range(5,25,5):
			speed.append(mph)
			power.append((mph * 0.44740) * p.slope + p.intercept)
			#plt.plot(speed, power)

def get_position_data():
	# loads slope & intercept for each position.
	positions = []
	data = np.loadtxt('position_slope_intercept.csv', delimiter=',', comments='#')
	
	for r in data:
		p = position()
		p.servo = r[0]
		p.slope = r[1]
		p.intercept = r[2]
		positions.append(p)
	
	#print(positions[0].slope)
	
	""""
	# 1500
	p = position()
	p.servo = 1500
	p.slope = 0.914183709
	p.intercept = -1.596169359
	positions.append(p)

	# 1400
	p = position()
	p.servo = 1400
	p.slope = 2.327719728
	p.intercept = -3.058747672
	positions.append(p)

	# 1300
	p = position()
	p.servo = 1300
	p.slope = 5.967740399
	p.intercept = -7.866779028

	positions.append(p)
	"""
	return positions

def plot():

	positions = get_position_data()
	fit_3rd_poly(positions)
	fit_linear(positions)
	plt.show()
	
