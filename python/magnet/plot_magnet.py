import os
import sys
from log_parser import *
import matplotlib.pyplot as plt
from calibration_fit import *
import itertools

# ----------------------------------------------------------------------------
#
# Encapsulates function for getting a unique color for plotting.
#
# ----------------------------------------------------------------------------
class ChartColor:
    def __init__(self):
        self.index = 0
        self.positions = []
        self.colors = ['g', 'c', 'y', 'b', 'r', 'm', 'k', 'orange', 'navy', 'darkolivegreen', 'mediumturquoise']        
        self.marker = itertools.cycle((',', '+', '.', 'o', '*')) 
    
    #
    # Returns a unique color for each servo position.
    #
    def get_color(self, position):
        # if the position has been seen before, return it's color, otherwise grab a new color.
        for c, p in enumerate(self.positions):
            if p == position:
                return self.positions[c,1]
        
        color = self.colors[self.index]
        if (self.index == 10):
            self.index = 0
        else:
            self.index = self.index + 1
        
        self.positions.append((position, color))
        
        return color
        
#
# Calculates the magnet only portion of the power equation based only
# Drag and RR calibration values when the magnet is not off.
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
    fit = CalibrationFit()
    clr = ChartColor()
    
    keyfunc = lambda x: x.position
    data = sorted(data, key=keyfunc)
    for k, g in groupby(data, keyfunc):
        items = list(g)
        if k < 1600:
            speed = [x.speed_mps * 2.23694 for x in items]
            power = [x.magonly_power for x in items]
            color = clr.get_color(k)
            plt.scatter( speed, power, color=color, label=(('Position: %i' % (k))), marker='o' )
            
            # try a linear fit of speed / magonly watts.
            slope, intercept = fit.fit_lin_regress(np.asarray(speed), np.asarray(power))
            
            speed_new = np.arange(5,30)
            power_new = lambda x: x * slope + intercept
            
            plt.plot(speed_new, power_new(speed_new), color=color, linestyle='--')

    plt.grid(b=True, which='both', color='0.65', linestyle='-')
    plt.axhline(y=0, c='black', linewidth=2)
    plt.xlabel('Speed (mph)')
    plt.ylabel('Mag Only Power')
    plt.legend()
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
    