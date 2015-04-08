# let's see if this works.
import matplotlib.pyplot as plt
import numpy as np

# structure [position, slope, intercept]
class position:
	def __init__(self):
		self.servo = 0
		self.slope = 0.0
		self.intercept = 0.0

def fit_3rd_poly(positions):
	colors = iter(['r','g','b','y','c'])
	plt.subplot(2, 1, 1)
	plt.ylabel('Watts @ Position by Speed')
	# generate speed data 5-25 mph
	# calculate power for each position
	for mph in range(5,25,5):
		c = next(colors)
		
		mps = mph*0.44704
		power = []
		servo_pos = []

		for p in positions:
			power.append(mps * p.slope + p.intercept)
			servo_pos.append(p.servo)
			#print(p.servo, mph, watts)
			
		plt.plot(servo_pos, power, color=c)
		#print(p.intercept)
		
def fit_linear(positions):
	plt.subplot(2, 1, 2)
	plt.ylabel('Watts @ Speed by Servo Position')
	
	for p in positions:
		speed = []
		power = []
		
		for mph in range(5,25,5):
			speed.append(mph)
			power.append((mph * 0.44740) * p.slope + p.intercept)
			plt.plot(speed, power)

def get_position_data():
	# loads slope & intercept for each position.
	positions = []
	data = np.loadtxt('data2.csv', delimiter=',')
	
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

def main():

	positions = get_position_data()
	fit_3rd_poly(positions)
	fit_linear(positions)
	plt.show()
	
main()