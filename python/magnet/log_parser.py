import csv
import numpy as np, numpy.ma as ma
from collections import defaultdict
from itertools import groupby, chain

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
        
        with open(file_name, 'r') as f:
            for row in reversed(list(csv.reader(f))):
                if row[0] == 'RR':
                    rr = float(row[1])
                if row[0] == 'Drag':
                    drag = float(row[1])
                if row[0] == 'DeviceID':
                    device_id = int(row[1])
                    return drag, rr, device_id
    
    #
    # Opens the log file and returns arrays: speed (mph), power, servo position.
    #
    def open(self, file_name):
        return np.loadtxt(file_name, delimiter=',', skiprows=self.skip_rows+1,
                dtype=[('speed', float), ('power', int), ('position', int)], usecols=[self.speed_col, self.watts_col, self.servo_col], comments='"',
                converters = {5: lambda s: float(s.strip() or 0)})

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
        self.power = 0
        self.device_id = 0
        
# ----------------------------------------------------------------------------
# 
# Class responsible for parsing stable position, speed, power data.
#
# ----------------------------------------------------------------------------
class PositionParser:
    def __init__(self):
        self.min_seq_len = 7                # min. sequence length
        self.speed_variance_mph = 0.25 * 2  # total range of allowed variation
        self.max_dev = 5                    # maximum deviation of watts
        
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
    # Optionally extend an existing position_list (i.e. if working with multiple files)
    #
    def get_position_dictionary(self, records, position_list = defaultdict(list)):
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
    # Extracts stable [position, speed, watts, device_id] from a log file.        
    #
    def parse(self, file_name):
        util = Util()
        drag, rr, device_id = util.read_calibration(file_name)
        records = util.open(file_name)

        # Gets a dictionary of valid indexes for each position.
        position_dict = self.get_position_dictionary(records)

        stable_data = []
        
        for position, values in position_dict.items():
            for index in values:
                #print(position, records['speed'][index], records['power'][index])
                point = PositionDataPoint()
                point.device_id = device_id
                point.position = position                           # Servo Position
                point.speed_mps = records['speed'][index] * 0.44704 # Convert to meters per second from mph.
                point.power = records['power'][index]
                stable_data.append(point)
        
        return stable_data
