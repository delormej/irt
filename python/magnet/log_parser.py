import sys
import os
import csv
import numpy as np, numpy.ma as ma
from collections import defaultdict
import itertools
from itertools import groupby, chain
import statistics as stats
from numpy.lib.recfunctions import append_fields
import magnet as mag
import fit as fit
import matplotlib.pyplot as plt
import math as math


# ----------------------------------------------------------------------------
#
#  Works with IRT & TrainerRoad log files.
#
# ----------------------------------------------------------------------------
class LogParser:
    
    def CalibrationFit(self):
        # Runs a calibration fit 
        drag, rr = fit.fit_nonlinear_calibration(self.MagOffPower())
        return drag, rr
    
    def MagnetFit(self):
        # Fits slope/intercept for each magnet position.
        return self.positions
        
    def MagOffPower(self):
        # Returns an array of all stable records where the magnet is off.
        return self.records[self.__stable_by_position(2000)]
        
    def MagOnlyPower(self):
        # Returns array of stable: position, speed, watt records
        return [r in self.stable_records ];

    def PlotRide(self):
        self.__create_ride_plot()
        # returns chart area... 
        
    def PlotMagnet(self):
        #self.__create_magnet_plot()
        #self.__create_model_mag_plot()
        self.__create_mag_force_plot()
        
    # ------------------------------------------------------------------------
    #  Internal methods
    # ------------------------------------------------------------------------

    def __init__(self, file_name, drag=0, rr=0, force_offset=0):
        self.cc = ChartColor()
        self.file_name = file_name
        self.records = []
        self.stable_records = []    # index, avg_power, avg_speed for stable data points.
        self.positions = []         # tuple (position, slope, intercept, stable_records_ix[])
        
        self.drag = drag
        self.rr = rr
        self.force_offset = force_offset
        self.device_id = 0
        
        # Grab all the records and config settings.
        self.__open()
        self.__read_config()
        
        # Initializes the co-efficients for magnet power.
        fit.init_mag(force_offset)
        
        self.__enrich()
        
    #
    # Opens the log file and returns arrays: speed (mph), power, servo position.
    #
    def __open(self, speed_col = 3, watts_col = 5, servo_col = 7, skip_rows = 20):
    
        self.records = np.loadtxt(self.file_name, delimiter=',', skiprows=skip_rows+1,
                dtype=[('speed', float), ('power', int), ('position', int)], usecols=[speed_col, watts_col, servo_col], comments='"',
                converters = {5: lambda s: float(s.strip() or 0)})
                
    #
    # Get the configuration settings from the log file.
    #
    def __read_config(self):
        
        if not self.file_name.startswith('irt_'):
            # it's not an IRT log file, so skip.
            return 
        
        try:
            with open(self.file_name, 'r') as f:
                for row in reversed(list(csv.reader(f))):
                    if row[0] == 'RR' and self.rr == 0:
                        self.rr = float(row[1])
                    if row[0] == 'Drag' and self.drag == 0:
                        self.drag = float(row[1])
                    if row[0] == 'DeviceID':
                        self.device_id = int(row[1])
                        # when we get here we've read all the config records
                        break
        except:
            print("Unable to parse calibration.")

    # 
    # Enriches the records with moving averages and other functions.
    # Add additional calls in here to further enrich.
    #
    def __enrich(self):

        # Identify the stable records.
        self.__find_stable()
    
        # Calibrate if drag & rr are not present based on stable records.
        if self.drag == 0 or self.rr == 0:
            self.drag, self.rr = self.CalibrationFit()
        
        # Enrich the stable records with mag only power.
        magonly_col = self.__stable_magonly_power()
        self.stable_records = append_fields(self.stable_records, 'magonly_power', magonly_col, usemask=False)
        
        # Calculate the slope/intercept of each magnet position.
        self.__calculate_positions()
        
        # Add power estimate.
        power_est_col = self.__power_estimate()
        self.records = append_fields(self.records, 'power_est', power_est_col, usemask=False)
        
        # Add actual vs. estimate error column.
        err_col = self.records['power_est'] - self.records['power']
        self.records = append_fields(self.records, 'power_err', err_col, usemask=False)

    #
    # Returns a list of valid indexes of stable data by servo position.
    #
    def __stable_by_position(self, position):
        #return [i for i, x in enumerate(self.records[self.stable_records['index']]) if x['position']==position]
        return [x for x in self.stable_records if x['position'] == position]
        
    def __stable_magonly_power(self):
        magonly_col = []
        
        for r in self.stable_records:
            magonly_power = fit.magonly_power(r['speed_mps'], r['power'], self.drag, self.rr)    
            magonly_col.append(magonly_power)
            
        return magonly_col
        
    def __calculate_positions(self):
        records = []
        
        # find all unique servo positions
        positions, counts = np.unique(self.stable_records['position'], return_counts=True)
        
        for i,p in enumerate(positions):
            # Need at least 2 records.
            if p >= 1600 or counts[i] < 2:
                continue
        
            ix = [i for i,x in enumerate(self.stable_records) if x['position'] == p]
            slope, intercept = fit.fit_lin_regress(self.stable_records[ix]['speed_mps'], self.stable_records[ix]['magonly_power'])
            records.append((p, slope, intercept, ix))

        #dtp = np.dtype([('position','i4'), ('slope','f4'), ('intercept','f4'), ('ix')])
        #self.positions = np.array(records, dtype=dtp)
        self.positions = records
            
    # 
    # Builds a single dimension array, same length as records with power estimate.
    #
    def __power_estimate(self):
        power_est_col = []
        
        for r in self.records:
            power_est = fit.power_estimate(r['speed'] * 0.44704, r['position'], self.drag, self.rr)
            power_est_col.append(power_est)
        
        return power_est_col       

    #
    # Calculates the Force for the magnet portion of the data.
    #
    def __calculate_mag_force(self, records):        
        #stable_force = [calc_force_actual(x['speed_mps'], x['magonly_power']) for x in self.stable_records if x['position'] == position]
        #stable_speed = [x['speed_mps'] for x in self.stable_records if x['position'] == position]        
        
        force = lambda r: r['magonly_power'] / r['speed_mps']
        force_records = [force(records)]
        
        return force_records
        
    #
    # Identifies stable data.
    #
    def __find_stable(self, skip=420):
        # Skip first 7 minutes of data (7*60 = 420 records).
        if len(self.records) < skip:
            raise "Not enough rows to find stable data."
    
        stable = []
    
        for i, power, speed in fit.power_ma_crossovers(self.records, skip):
            position = self.records[i]['position']
            speed_mps = speed * 0.44704
            stable.append((i, position, speed_mps, power)) 
            
        dtp = np.dtype([('index','i4'), ('position','i4'), ('speed_mps','f4'), ('power','f4')])
        self.stable_records = np.array(stable, dtype=dtp)
        
        #print(len(self.records),len(self.stable_records))

    def __create_ride_plot(self):
        plt.rc('axes', grid=True)
        plt.rc('grid', color='0.75', linestyle='-', linewidth=0.5)

        ax1 = plt.subplot(322)
        plt.title('Speed (mph)')
        
        ax2 = plt.subplot(324,sharex=ax1)
        plt.title('Servo Position')
        
        ax3 = plt.subplot(326,sharex=ax1)
        plt.title('Power')

        time = range(0, len(self.records['speed']), 1)
        
        # ax1 = Speed & Servo
        ax1.plot(time, self.records['speed'])
        ax1.set_ylim(7, 30)

        # 15 second moving average for speed.
        ax1.plot(time, fit.speed_moving_average(self.records['speed'], 15), color='g')

        ax2.plot(time, self.records['position'], color='r')
        ax2.set_ylim(800, 1700)

        ax3.plot(time, self.records['power'], 'r')
        ax3.plot(time, self.records['power_est'], 'orange', linestyle='--', linewidth=2)

        ax3.plot(time, fit.moving_average(self.records['power'], 30), color='b')
        ax3.plot(time, fit.moving_average(self.records['power'], 10), color='lightblue')
        ax3.plot(time, fit.moving_average(self.records['power_est'], 30), color='y')

        ax3.set_ylim(50, 600)

    def __create_magnet_plot(self):

        plt.subplot(121)
        plt.title('Linear Magnet Power')
        plt.grid(b=True, which='both', color='0.65', linestyle='-')
        plt.axhline(y=0, c='black', linewidth=2)
        plt.xlabel('Speed (mph)')
        plt.ylabel('Mag Only Power')
        
        for p in self.positions:
            position = p[0]
            slope = p[1]
            intercept = p[2]
            ix = p[3]
            speed = self.stable_records[ix]['speed_mps']
            power = self.stable_records[ix]['magonly_power']
        
            color = self.cc.get_color(position)
        
            # Scatter plot all stable magonly power values by position.
            plt.scatter(speed*2.23694, power, color=color, label=(('Position: %i' % (position))), marker='o')
            
            # Draw linear slope/intercept for a given position.
            lin_power = lambda x: x * slope + intercept
            plt.plot(speed*2.23694, lin_power(speed), color=color, linestyle='--')
        
        plt.legend()

    def __create_model_mag_plot(self):
        # Draw the model power 
        model_speed_mps = np.array( [ min(self.stable_records['speed_mps']), max(self.stable_records['speed_mps']) ] )

        def mag_watts(speeds, position):
            watts = []
            for s in speeds:
                watts.append(mag.watts(s, position))
                
            return watts

        for x in range(820, 1420, 10):
            color = self.cc.get_color(x)
            
            if x % 100 == 0:
                # Every 100 positions make a stronger line
                linewidth=4
                plt.text(model_speed_mps[1]*2.23694, mag.watts(model_speed_mps[1], x)-4, x, color=color)
            else:
                linewidth=.5
                
            plt.plot(model_speed_mps*2.23694, mag_watts(model_speed_mps, x), color=color, linestyle='-.', linewidth=linewidth)
        
    def __create_mag_force_plot(self):
        
        def calc_force(speed_mps, slope, intercept):
            power = speed_mps * slope + intercept
                   
            force = power / speed_mps
            return force
        
        def calc_force_actual(speed_mps, magonly_power):
            return (magonly_power / speed_mps)
            
        def model_force(speed_mps, position):
            force = []
            for v in speed_mps:
                w = mag.watts(v, position)
                f = (w / v)
                force.append(f) 
            return force
        
        ax = plt.subplot(121)
        
        for p in self.positions:
            position = p[0]
            slope = p[1]
            intercept = p[2]
            ix = p[3]
            
            # Create a range of speeds
            speed = np.linspace(0, 15, 50)
            f = lambda x: calc_force(x, slope, intercept)
            force = [f(speed)][0]
            
            # Plot linear data as force
            ax.plot(speed, force, marker='o')
            
            # Fit force to a model.
            # indexes where force is > 0
            ix = [i for i,x in enumerate(force) if x > 0]
            a,b = fit.fit_force(speed[ix], force[ix])
            #force2 = [fit.get_force(x, a, b) for x in speed]
            force2 = model_force(speed, position)
            ax.plot(speed, force2, color='orange')
            
            # Get the actual force by position.
            stable_force = [calc_force_actual(x['speed_mps'], x['magonly_power']) for x in self.stable_records if x['position'] == position]
            stable_speed = [x['speed_mps'] for x in self.stable_records if x['position'] == position]
            
            for i, f in enumerate(stable_force):
                print((position, stable_speed[i], f))
            
            ax.plot(stable_speed, stable_force, color='red', marker='*')
                        
    
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
        keyfunc = lambda x: x[0]
        for c, p in enumerate(sorted(self.positions, key=keyfunc)):
            if p[0] == position:
                return p[1]
        
        color = self.colors[self.index]
        if (self.index == 10):
            self.index = 0
        else:
            self.index = self.index + 1
        
        self.positions.append((position, color))
        
        return color            
            