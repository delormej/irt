import magnet as mag

low_speed = 15 * 0.44704
low_a = 1.41260098255e-11
low_b = -8.57621626418e-08
low_c = 0.000206564406769
low_d = -0.245741070668
low_e = 143.288022217
low_f = -32262.2922275

high_speed = 25 * 0.44704
high_a = 1.02869966634e-11
high_b = -6.49259256372e-08
high_c = 0.00016329768474
high_d = -0.203016701381
high_e = 123.005780913
high_f = -28270.8112326

mag.set_coeff(
    low_speed, 
    low_a, 
    low_b, 
    low_c, 
    low_d, 
    low_e,
    low_f,
    high_speed, 
    high_a, 
    high_b, 
    high_c,
    high_d,
    high_e,
    high_f)

speed_mps = 25 * 0.44704
servo_pos = 1200
    
watts = mag.watts(speed_mps, servo_pos)
#servo_pos = mag.position(speed_mps, watts)

print("Watts: %i" % round(watts, 0))
#print("Position:", servo_pos)