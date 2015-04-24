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
def plot_magonly_linear(file_name):
    fit = CalibrationFit()
    clr = ChartColor()
    
    util = Util()
    parser = PositionParser()
    records = util.open(file_name)

    valid = []
    
    for ix, avg in parser.power_ma_crossovers(records['power']):
        valid.append(ix)
    
    keyfunc = lambda x: x['position']
    data = sorted(records[valid], key=keyfunc)
    for k, g in groupby(data, keyfunc):
        items = list(g)
        if k < 1600:
            speed = [x['speed'] for x in items]
            power = [x['power'] for x in items]
            color = clr.get_color(k)
            plt.scatter( speed, power, color=color, label=(('Position: %i' % (k))), marker='o' )
            
            # try a linear fit of speed / magonly watts.
            slope, intercept, speed_new, power_new = fit.fit_lin_regress(speed, power)
            plt.plot(speed_new, power_new, color=color, linestyle='--')

    plt.grid(b=True, which='both', color='0.65', linestyle='-')
    plt.axhline(y=0, c='black', linewidth=2)
    plt.xlabel('Speed (mph)')
    plt.ylabel('Mag Only Power')
    plt.legend()
    plt.show()        
    
#
# Plots the entire ride and places markers at each of the places where long
# moving average crosses the short moving average.
#
def plot_crossover(file_name):
    util = Util()
    parser = PositionParser()
    records = util.open(file_name)
    
    labels = []

    ma_speed = parser.moving_average(records['speed'], 30)
    ma_power30 = parser.moving_average(records['power'], 15)
    ma_power10 = parser.moving_average(records['power'], 5)

    plt.rc('axes', grid=True)
    plt.rc('grid', color='0.75', linestyle='-', linewidth=0.5)

    ax1 = plt.subplot(311)
    ax2 = plt.subplot(312,sharex=ax1)
    ax3 = plt.subplot(313,sharex=ax1)

    time = range(0, len(records['speed']), 1)
    
    # ax1 = Speed & Servo
    ax1.plot(time, records['speed'])
    ax1.set_ylim(7, 30)

    ax1.plot(time, ma_speed)

    ax2.plot(time, records['position'], color='r')
    ax2.set_ylim(800, 1700)

    ax3.plot(time, records['power'], 'r')

    ax3.plot(time, ma_power30, color='b')
    ax3.plot(time, ma_power10, color='lightblue')
    #ax3.plot(time, ma_est_power, color='y')

    ax3.set_ylim(50, 600)
    
    for ix, avg_power in parser.power_ma_crossovers(records['power']):
        # Only plot if the servo position hasn't changed for a few seconds.
        # This eliminates the issue with averages appearing on the edge.
        if ix > 3 and records['position'][ix-3] == records['position'][ix]:
            plt.scatter(time[ix], avg_power)
            #if records['position'][ix] < 1600:
            #    plt.text(time[ix], avg_power, records['position'][ix])

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
    
    plot_magonly_linear(file_name)
    plot_crossover(file_name)
        
if __name__ == "__main__":
    if (len(sys.argv) > 2):
        speed_col = int(sys.argv[2])
    main(sys.argv[1])
    