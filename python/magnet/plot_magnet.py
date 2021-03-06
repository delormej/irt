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
# Plots an array of PositionDataPoint.
#
def plot_magonly_linear(records):
    parser = PositionParser()
    fit = CalibrationFit()
    clr = ChartColor()
    
    plt.subplot(121)
    plt.title('Linear Magnet Power')
   
   # Group data by position.
    keyfunc = lambda x: x['position']
    data = sorted(records, key=keyfunc)
    for k, g in groupby(data, keyfunc):
        items = list(g)
        if k < 1600:
            speed_mps = [x['speed_mps'] for x in items]
            power = [x['magonly_power'] for x in items]
            color = clr.get_color(k)
            plt.scatter( speed_mps, power, color=color, label=(('Position: %i' % (k))), marker='o' )
            
            # try a linear fit of speed / magonly watts.
            try:
                slope, intercept, speed_new, power_new = fit.fit_lin_regress(speed_mps, power)
                plt.plot(speed_new, power_new, color=color, linestyle='--')
                print("position, slope, intercept", k, slope, intercept)
            except:
                print("couldn't solve for position:", k)

    plt.grid(b=True, which='both', color='0.65', linestyle='-')
    plt.axhline(y=0, c='black', linewidth=2)
    plt.xlabel('Speed (mps)')
    plt.ylabel('Mag Only Power')
    plt.legend()
    
#
# Plots the entire ride and places markers at each of the places where long
# moving average crosses the short moving average.
#
def plot_ride(records):
    parser = PositionParser()

    ma_speed = parser.speed_moving_average(records['speed'], 90)
    ma_power30 = parser.moving_average(records['power'], 30)
    ma_power10 = parser.moving_average(records['power'], 5)
    ma_power_est30 = parser.moving_average(records['power_est'], 30)
    
    plt.rc('axes', grid=True)
    plt.rc('grid', color='0.75', linestyle='-', linewidth=0.5)

    ax1 = plt.subplot(322)
    plt.title('Speed (mph)')
    
    ax2 = plt.subplot(324,sharex=ax1)
    plt.title('Servo Position')
    
    ax3 = plt.subplot(326,sharex=ax1)
    plt.title('Power')

    time = range(0, len(records['speed']), 1)
    
    # ax1 = Speed & Servo
    ax1.plot(time, records['speed'])
    ax1.set_ylim(7, 30)

    ax1.plot(time, ma_speed, color='g')

    ax2.plot(time, records['position'], color='r')
    ax2.set_ylim(800, 1700)

    ax3.plot(time, records['power'], 'r')
    ax3.plot(time, records['power_est'], 'orange', linestyle='--', linewidth=2)

    ax3.plot(time, ma_power30, color='b')
    ax3.plot(time, ma_power10, color='lightblue')
    ax3.plot(time, ma_power_est30, color='y')

    ax3.set_ylim(50, 600)
    
    for ix, avg_power, avg_speed in parser.power_ma_crossovers(records):
        plt.scatter(time[ix], avg_power)
        #plt.errorbar(time[ix], avg_power, yerr=records['power_err'][ix])
   
def calc_stdev(records):
    def reject_outliers(data, m=30):
        return data[abs(data - np.mean(data)) < m * np.std(data)]
    
    data = reject_outliers(records['power_err'])
    print("Average Watt Err:", np.mean(data))
   
#
# Main entry point to parse a file.
#        
def main(file_name):
    parser = PositionParser()
    data = []
    
    if os.path.isdir(file_name):
        mag_data = parser.parse_multiple(file_name)
        plot_magonly_linear(mag_data)
    else:
        data = parser.parse(file_name)
        plot_ride(data)
        
        mag_data = parser.parse_magdata(file_name)
        plot_magonly_linear(mag_data)
                
        #print("sum of error:", sum(mag_data['power_err']))
        #print("stdev:", np.std(mag_data['power_err']))
   
        calc_stdev(mag_data)
   
    plt.show()        
    
    
        
if __name__ == "__main__":
    if (len(sys.argv) > 2):
        speed_col = int(sys.argv[2])
    main(sys.argv[1])
    