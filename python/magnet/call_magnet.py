import magnet as mag

speed_mps = 15 * 0.44704
servo_pos = 1300

low_speed = 10 * 0.44704
low_a = 1.30101069013e-06
low_b = -0.00408959829622
low_c = 3.89744863571
low_d = -992.651847098

high_speed = 25 * 0.44704
high_a = 2.97088134733e-06
high_b = -0.00984147720024
high_c = 9.8702475382
high_d = -2663.19364338

mag.set_coeff(low_speed, 
	low_a, 
	low_b, 
	low_c, 
	low_d, 
	high_speed, 
	high_a, 
	high_b, 
	high_c,
	high_d)

watts = mag.watts(speed_mps, servo_pos)
servo_pos = mag.position(speed_mps, watts)

print("Watts: %i" % round(watts, 0))
print("Position:", servo_pos)