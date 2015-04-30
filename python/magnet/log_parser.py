import sys
import os
import csv
import numpy as np, numpy.ma as ma
from collections import defaultdict
from itertools import groupby, chain
import statistics as stats
from numpy.lib.recfunctions import append_fields
import magnet as mag
import fit as fit
import matplotlib.pyplot as plt


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

    def MagOffPower(self):
        # Returns an array of all stable records where the magnet is off.
        return self.records[self.magoff_ix]
        
    def MagnetFit(self, servo_offset=0):
        # Returns a series of position, slope, intercepts
        return 0
        
    def MagOnlyPower(self):
        # returns array of stable: position, speed, watt records
        return list(fit.power_ma_crossovers(self.records))

    # ------------------------------------------------------------------------
    #  Internal methods
    # ------------------------------------------------------------------------

    def __init__(self, file_name, drag=0, rr=0, servo_offset=0):
        self.file_name = file_name
        self.records = []
        self.stable_records = []
        
        self.drag = drag
        self.rr = rr
        self.servo_offset = servo_offset
        self.device_id = 0
        
        # Grab all the records and config settings.
        self.__open()
        self.__read_config()
        
        # Initializes the co-efficients for magnet power.
        fit.init_mag()
        
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
                    if row[0] == 'AddServoOffset' and self.servo_offset == 0:
                        self.servo_offset = int(row[1])
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
        self.magoff_ix = [i for i, x in enumerate(self.records[self.stable_records['index']]) if x['position']==2000]
    
        # Calibrate if drag & rr are not present.
        if self.drag == 0 or self.rr == 0:
            self.drag, self.rr = self.CalibrationFit()

        # Add speed in meters per second.
        speed_mps_col = self.records['speed'] * 0.44704
        self.records = append_fields(self.records, 'speed_mps', speed_mps_col, usemask=False)
            
        # Add magnet only power.
        mag_col = self.__magonly_power()
        self.records = append_fields(self.records, 'magonly_power', mag_col, usemask=False)
        
        # Add power estimate.
        power_est_col = self.__power_estimate()
        self.records = append_fields(self.records, 'power_est', power_est_col, usemask=False)
        
        # Add actual vs. estimate error column.
        err_col = self.records['power_est'] - self.records['power']
        self.records = append_fields(self.records, 'power_err', err_col, usemask=False)
    
    # TODO: not sure if this is needed??
    def __magonly_power(self):
        result = []
        
        for r in self.records:
            magonly_power = 0
            
            position = r['position']  
            power = r['power']
            speed_mps = r['speed_mps'] 
            
            if position < 1600:
                magoff = fit.magoff_power(speed_mps, self.drag, self.rr)
                magonly_power = power - magoff

            if magonly_power < 0:
                magonly_power = 0
                
            result.append(magonly_power)
        
        return result
        
    # 
    # Builds a single dimension array, same length as records with power estimate.
    #
    def __power_estimate(self):
        power_est_col = []
        
        for r in self.records:
            power_est = fit.power_estimate(r['speed_mps'], r['position'], self.drag, self.rr)
            power_est_col.append(power_est)
        
        return power_est_col       

    #
    # Identifies stable data.
    #
    def __find_stable(self):
        # Skip first 7 minutes of data (7*60 = 420 records).
        if len(self.records) < 420:
            raise "Not enough rows to find stable data."
    
        stable = []
    
        for i, power, speed in fit.power_ma_crossovers(self.records[420:]):
            stable.append((i,power,speed)) 
            
        dtp = np.dtype([('index','i4'), ('power','f4'),('speed','f4')])
        self.stable_records = np.array(stable, dtype=dtp)
            