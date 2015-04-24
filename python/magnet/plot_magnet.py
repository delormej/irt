import os
import sys
from log_parser import *
import matplotlib.pyplot as plt

#
# Calculates the magnet only portion of the power equation based only
# Drag and RR calibration values.
# 
def magonly_calc(data, drag, rr):
    power = 0
    
    # Only calculate power for magnet positions below 1600.
    if data.position < 1600:
        magoff = drag * data.speed_mps**2 + (data.speed_mps * rr)
        power = data.power - magoff

    if power < 0:
        power = 0
        
    return power
#
# Plots an array of PositionDataPoint.
#
def plot(data):
    keyfunc = lambda x: x.position
    data = sorted(data, key=keyfunc)
    for k, g in groupby(data, keyfunc):
        items = list(g)
        
        speed = [x.speed_mps * 2.23694 for x in items]
        power = [x.magonly_power for x in items]
        plt.scatter(speed, power)

    plt.show()        
#
# Main entry point to parse a file.
#        
def main(file_name):
    parser = PositionParser()
    data = []
    
    if os.path.isdir(file_name):
        data = parser.parse_multiple(file_name, magonly_calc)
    else:
        data = parser.parse(file_name, magonly_calc)
    
    plot(data)
    
        
if __name__ == "__main__":
    if (len(sys.argv) > 2):
        speed_col = int(sys.argv[2])
    main(sys.argv[1])
    #main(input_file_name)    