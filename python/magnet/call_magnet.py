# using pyMagnet
from magnet import *

speed_mps = 15 * 0.44704
servo_pos = 1300

watts = watts(speed_mps, servo_pos)
servo_pos = position(speed_mps, watts)

print("Watts:", watts)
print("Position:", servo_pos)