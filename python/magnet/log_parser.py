import os
import csv
import numpy as np, numpy.ma as ma
from collections import defaultdict
from itertools import groupby, chain
import statistics as stats
from numpy.lib.recfunctions import append_fields
import magnet as mag
import calibration_fit as fit
import find_slope as fs

# ----------------------------------------------------------------------------
#
# Encapsulates common utility functions, mainly dealing with reading log files.
#
# ----------------------------------------------------------------------------
class Util:
    def __init__(self):
        #
        #Column position definitions
        #
        self.speed_col = 3
        self.watts_col = 5
        self.servo_col = 7
        
        # Data rows skipped at the beginning
        self.skip_rows = 20 
    
    #
    # Reads an irt log file and returns the Drag and RR settings.
    #
    def read_calibration(self, file_name):
        device_id = 0
        drag = 0
        rr = 0
        offset = 0
        
        try:
            with open(file_name, 'r') as f:
                for row in reversed(list(csv.reader(f))):
                    if row[0] == 'AddServoOffset':
                        offset = int(row[1])
                    if row[0] == 'RR':
                        rr = float(row[1])
                    if row[0] == 'Drag':
                        drag = float(row[1])
                    if row[0] == 'DeviceID':
                        device_id = int(row[1])
                        # when we get here we've read all the config data
                        break
        except:
            print("Unable to parse calibration.")
                        
        return drag, rr, offset, device_id
    
    #
    # Opens the log file and returns arrays: speed (mph), power, servo position.
    #
    def open(self, file_name):
        return np.loadtxt(file_name, delimiter=',', skiprows=self.skip_rows+1,
                dtype=[('speed', float), ('power', int), ('position', int)], usecols=[self.speed_col, self.watts_col, self.servo_col], comments='"',
                converters = {5: lambda s: float(s.strip() or 0)})

    #
    # Builds a list of matching *.csv filenames.
    #
    def get_csv_files(self, rootdir):
        for root, dirs, files in os.walk(rootdir):
            for filename in files:
                if filename.endswith('.csv'):
                    filepath = os.path.join(root, filename)
                    yield filepath    
                
    #
    # Prints comma delimited device, position, speed, power.
    #
    def print_csv(data):
        for p in data:
            print(('%i,%i,%.2f,%i' % (p.device_id, p.position, p.speed_mps, p.power)))


# ----------------------------------------------------------------------------
# 
# Encapsulates stable position data.
#
# ----------------------------------------------------------------------------
class PositionDataPoint:
    def __init__(self):
        self.position = 0
        self.speed_mps = 0
        self.speed_mph = 0
        self.power = 0
        self.magonly_power = 0
        self.device_id = 0
        
# ----------------------------------------------------------------------------
# 
# Class responsible for parsing stable position, speed, power data.
#
# ----------------------------------------------------------------------------
class PositionParser:
    def __init__(self):
        # 
        # Configuration variables.
        #
        self.min_seq_len = 10               # min. sequence length
        self.speed_variance_mph = 0.5       # total range of allowed variation
        self.max_dev = 7                    # maximum deviation of watts
        self.servo_lag = 6                  # don't take any data points within # of seconds of a servo change.
        self.cal = fit.CalibrationFit()

    #
    # Calculates a moving average of an array of values.
    #
    def moving_average(self, x, n, type='simple'):
        """
        compute an n period moving average.

        type is 'simple' | 'exponential'

        """
        x = np.asarray(x)
        
        if type=='simple':
            weights = np.ones(n)
        else:
            weights = np.exp(np.linspace(-1., 0., n))

        weights /= weights.sum()


        a =  np.convolve(x, weights, mode='full')[:len(x)]
        a[:n] = a[n]
        return a
        
    #
    # Calculates a moving average of speed which eliminates outliers where
    # we may have had data drop.
    #
    def speed_moving_average(self, speed, n):
        stdev_speed = stats.stdev(speed)
        
        # look backward and forward to determine if the speed is an
        # outlier or it's a legitimate change in speed.
        def normalize_speed(speed):
            for ix in range(0, len(speed), 1):
                if ix < 1 or ix+1 > len(speed):
                    yield speed[ix]
                else:
                    if abs(speed[ix-1] - speed[ix]) > stdev_speed and \
                            abs(speed[ix+1] - speed[ix]) > stdev_speed:
                        # Return previous speed.
                        yield speed[ix-1]
                    else:
                        yield speed[ix]
        
        normal_speed = normalize_speed(speed)
        ma = self.moving_average(list(normal_speed), n)
        
        return ma
        
    #
    # Returns the index into the power array where the longer moving average
    # crosses the shorter moving average.
    #
    def power_ma_crossovers(self, records):
        power = records['power']
        
        ma_speed = self.speed_moving_average(records['speed'], 15)
        ma_long = self.moving_average(power, 15)
        ma_short = self.moving_average(power, 5) 
        
        for i in range(1, len(power)-2, 1):
            # if the last long average was less than short average
            # and the current long average is higher than short
            # we've crossed over.
            if ma_long[i-1] < ma_short[i-1] and ma_long[i] > ma_short[i]:
                # Only include if the servo position hasn't changed for a few seconds.
                # This eliminates the issue with averages appearing on the edge.
                if i > self.servo_lag and records['position'][i-self.servo_lag] == records['position'][i]:
                    # We've crossed over return a tuple of index and long ma
                    yield i, ma_long[i], ma_speed[i]
        
    #
    # Returns a sequence of contiguous stable speed and power data. 
    #
    def find_seq(self, speeds, watts, offset):
        def check_seq(se): # test range [i..se) for a valid sequence
            if se > speeds.size:
                return False
            return watts[i:se].all() and speeds[i:se].all() and speeds[i:se].ptp() <= self.speed_variance_mph

        seqs = []
        last_end = -1 # where last found sequence ended
        for i in range(speeds.size): # possible start of sequence
            seq_end = max(i + self.min_seq_len, last_end + 1) # we need a sequence extending beyond the previous one
            if seq_end > speeds.size: # reached the end
                break
            if check_seq(seq_end): # minimum sequence is OK
                while check_seq(seq_end+1): # extend the sequence while we can
                    seq_end += 1
                last_end = seq_end
                seqs.append((i, last_end))

        result = [] # list of indices of valid values

        def split_cluster(cluster): # splits a cluster of overlapped sequences into non-overlapped
            if len(cluster) > 0:
                longest_beg, longest_end = max(cluster, key=lambda s: s[1]-s[0])
                if longest_end - longest_beg < self.min_seq_len:
                    return
                left = []; right = []
                for beg, end in cluster: # adjust each sequence overlapped by the longest
                    if beg < longest_beg:
                        end = min(end, longest_beg)
                        if end-beg >= self.min_seq_len:
                            left.append((beg, end))
                    elif end > longest_end:
                        beg = max(beg, longest_end)
                        if end-beg >= self.min_seq_len:
                            right.append((beg, end))

                split_cluster(left)

                m_watts = ma.array(watts[longest_beg:longest_end])
                while m_watts.std(ddof=1) > self.max_dev: # strip outliers until stdev in range
                    edges = np.array([m_watts.min(), m_watts.max()])
                    distances = abs(edges - m_watts.mean())
                    outlier = edges[distances.argmax()]
                    m_watts[m_watts == outlier] = ma.masked
                if m_watts.count() >= self.min_seq_len:
                    result.extend(np.flatnonzero(~ma.getmaskarray(m_watts))+longest_beg+offset)
                split_cluster(right)

        last_end = -1
        cluster = []
        for s in seqs:
            if s[0] >= last_end: # broke overlapping cluster
                split_cluster(cluster)
                cluster = []
            cluster.append(s)
            last_end = s[1]
        split_cluster(cluster)
        return sorted(result)    

    #
    # Returns a dictionary keyed by servo position with an array of indexes 
    # pointing to valid data.
    #
    def get_position_dictionary(self, records):
        position_list = defaultdict(list)
        
        # Create an array for each column.
        positions = records['position']
        speeds = records['speed']
        watts = records['power']

        #valid_data = defaultdict(list) # { postion: [indices of all good values] }
        splits = np.flatnonzero(np.ediff1d(positions, to_begin=1, to_end=1)) # indexes where pos changed, split original sequence there
        for i_beg, i_end in zip(splits[:-1], splits[1:]): # iterate pairs of splits
            assert positions[i_beg] == positions[i_end-1]
            position_list[positions[i_beg]].extend(
                    self.find_seq(speeds[i_beg:i_end], watts[i_beg:i_end], i_beg) # process each chunk separately
            )

        #
        # Removes positions from the dictionary that do not have at least 2 data points.
        #
        def remove_empty(list):
            empty = []
            for p in list:
                if len(list[p]) < 3:
                    empty.append(p)
                    
            for p in empty:
                del(list[p])
            
        # remove positions that don't contain any data.
        remove_empty(position_list)
            
        return position_list

    #
    # Returns array of data which represents only the consistent speed data
    # when the magnet is off.
    #
    def magoff_records(self, records):
        position_dict = self.get_position_dictionary(records)
        magoff = records[position_dict[2000]]
        return magoff
        
    #
    # Calculates the magnet only portion of power based on non-linear calibration.
    #
    def magonly(self, records, drag, rr):
        result = []
        
        for r in records:
            magonly_power = 0
            
            position = r['position']  
            power = r['power']
            speed_mps = r['speed'] * 0.44704
            
            if position < 1600:
                magoff = self.cal.magoff_power(speed_mps, drag, rr)
                magonly_power = power - magoff

            if magonly_power < 0:
                magonly_power = 0
                
            result.append(magonly_power)
        
        return result

    #
    # Initializes the co-efficients for magnet power.
    #
    def init_mag(self):
        
        positions = fs.get_position_data()
        values = fs.fit_3rd_poly(positions)
        """
[(10, array([ -3.01904366e-12,   1.71079155e-08,  -3.77162970e-05,  4.05289009e-02,  -2.15718725e+01,   4.77771480e+03])), 
 (15, array([ -2.35669382e-12,   1.11676725e-08,  -1.83936624e-05,  1.13010366e-02,  -9.23325540e-01,  -6.00964020e+02])), 
 (20, array([ -1.69434399e-12,   5.22742936e-09,   9.28972322e-07, -1.79268277e-02,   1.97252215e+01,  -5.97964284e+03])), 
 (25, array([ -1.03199415e-12,  -7.12813721e-10,   2.02516070e-05, -4.71546920e-02,   4.03737685e+01,  -1.13583217e+04]))]
        """
        
        low_speed = values[1][0]*0.44704
        low_a = values[1][1][0]
        low_b = values[1][1][1]
        low_c = values[1][1][2]
        low_d = values[1][1][3]
        low_e = values[1][1][4]
        low_f = values[1][1][5]

        high_speed = values[3][0]*0.44704
        high_a = values[3][1][0]
        high_b = values[3][1][1]
        high_c = values[3][1][2]
        high_d = values[3][1][3]
        high_e = values[3][1][4]
        high_f = values[3][1][5]
        
        """
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
        """
        
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

    #
    # Calculates power based coast down fit (drag & rr), speed and magnet position.
    #
    def get_power(self, speed_mps, servo_pos, drag, rr):
        # Calculates non-mag power based on speed in meters per second.
        no_mag_watts = self.cal.magoff_power(speed_mps, drag, rr)
        
        # If the magnet is ON the position will be less than 1600
        if servo_pos < 1600:
            # Calculates the magnet-only power based on speed in meters per second.
            mag_watts = mag.watts(speed_mps, servo_pos)
            #print(servo_pos, mag_watts)
        else:
            mag_watts = 0	
        
        return no_mag_watts + mag_watts        
    
    #
    # Returns only the cleaned up magnet position data.
    #
    def parse_magdata(self, file):
        util = Util()
        parser = PositionParser()
        cal = fit.CalibrationFit()
        
        data = self.parse(file)
        drag, rr, offset, device_id = util.read_calibration(file)
        
        valid = []
        
        for ix, power, speed in parser.power_ma_crossovers(data):
            # don't take any data less than ~7 mph
            if data[ix]['speed_mps'] > 3:
                valid.append(ix)
                data[ix]['power'] = power
                data[ix]['speed'] = speed
                data[ix]['magonly_power'] = power - cal.magoff_power(speed * 0.44704, drag, rr)      
                
                #if data[ix]['position'] != 2000:
                    #print(data[ix]['position'], data[ix]['speed_mps'], data[ix]['magonly_power'], power, cal.magoff_power(speed * 0.44704, drag, rr))
            
        return data[valid]
    
    #
    # Extracts stable [position, speed, watts, device_id] from a log file.        
    #
    def parse(self, file_name):
        util = Util()
        
        # Read configuration values
        drag, rr, servo_offset, device_id = util.read_calibration(file_name)
                
        # Read all data.
        records = util.open(file_name)
       
        # Enrich data with meters per second and magnet only power.
        speed_mps = records['speed'] * 0.44704
        records = append_fields(records, 'speed_mps', speed_mps, usemask=False)

        def offset(x):
            if x < 2000: 
                return x+servo_offset
            else:
                return x
        
        # Implement a servo offset:
        records['position'] = [offset(x) for x in records['position']]
        
        if drag == 0 or rr == 0:
            # Perform a calibration against the file.
            drag, rr = self.cal.fit_nonlinear_calibration(self.magoff_records(records))
            #x = [8.761984,7.063232,5.677408]
            #y = [177,154,106]
            #drag, rr = self.cal.fit_bike_science(x, y)


        print("drag,rr:", drag, rr)
        
        mag_col = self.magonly(records, drag, rr)
        records = append_fields(records, 'magonly_power', mag_col, usemask=False)
        
        
        
        # TODO: move this out of parse.
        # Calculate estimated power.
        #
        self.init_mag()
        power_est_col = []
        
        for r in records:
            power_est = self.get_power(r['speed_mps'], r['position'], drag, rr)
            power_est_col.append(power_est)
        
        records = append_fields(records, 'power_est', power_est_col, usemask=False)
        
        return records
        """
        #
        # Gets a dictionary keyed by servo position containing a list of indexes
        # for contiguous stable speed, power data at a given servo position.
        #
        position_dict = self.get_position_dictionary(records)

        #
        # Flatten data out into an array of PositionDataPoint objects.
        #
        stable_data = []
        
        for position, values in position_dict.items():
            for index in values:
                #print(position, records['speed'][index], records['power'][index])
                point = PositionDataPoint()
                point.device_id = device_id
                point.position = position                           # Servo Position
                point.speed_mps = records['speed'][index] * 0.44704 # Convert to meters per second from mph.
                point.speed_mph = records['speed'][index]
                point.power = records['power'][index]
                if magonly_calc is not None:
                    point.magonly_power = magonly_calc(point, drag, rr)
                stable_data.append(point)
                
        return stable_data
        """
        
     #
     # Parses a directory of *.csv log files.  This only looks at plotting linear
     # magnet data.
     #
    def parse_multiple(self, rootdir):
        records = np.ndarray(0)
        util = Util()
        for file in util.get_csv_files(rootdir):
            print("parsing: ", file)
            
            data = self.parse_magdata(file)
            
            if len(records) == 0:
                records = data
            else:
                records = np.concatenate((records, data), axis=0)
  
        return records
