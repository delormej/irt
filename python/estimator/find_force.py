#input_file_name = "180lb_large_mag_range_adjust_speed.csv"
n = 5       # min. sequence length
x = 0.1 * 2 # total range of allowed variation
max_dev = 8 # maximum deviation of watts
skip_rows = 20 # data rows skipped at the beginning
txt_offset = 250
speed_col = 3
watts_col = 5
servo_col = 7
labels = [] # Labels for chart legend
#xsl_filename = '../../tcx-to-csv.xslt'

import sys
import os
from collections import defaultdict
from itertools import groupby, chain
import numpy as np, numpy.ma as ma
import scipy.optimize as spo, scipy.stats as stats
import bottleneck
import itertools
import matplotlib.pyplot as plt
import math
import traceback
#import lxml.etree as ET

class position:
    def __init__(self):
        self.servo = 0
        self.slope = 0.0
        self.intercept = 0.0

if len(sys.argv) > 1:
    input_file_name = sys.argv[1]

speed_threshold = 0.2
contiguous_count = 3

def in_threshold(current, last):
    return ( (current <= (last + speed_threshold)) and (current >= (last - speed_threshold)) )


def get_mean(speeds, watts):
    lastId = 0
    firstId = 0

    #print("count:", len(speeds))

    # look for when the speed varies more than threshold
    for id in range(len(speeds)):
        #print(speeds[id], watts[id], id, lastId, firstId)

        if firstId == 0:
            firstId = id

        if lastId > 0:
            if not in_threshold(speeds[id], speeds[lastId]):
            # average over what we have so far
                if (lastId - firstId) > contiguous_count:
                    #print("basing calc on:", lastId, firstId) #, speeds[983])
                    yield (speeds[firstId:lastId].mean(), watts[firstId:lastId].mean())
                    firstId = 0

        lastId = id

    #print("end")
    yield (speeds[firstId:lastId].mean(), watts[firstId:lastId].mean())

# used to fit curve by drag (K) an rolling resistance (rr) variables
# to return power in watts given speed (v) in mps
def drag_rr_func(v, K, rr):
    p = K * v**2 + (v * rr)
    return p

"""
Get the stable speed and watts for each servo position.
"""
def get_positions(valid_data, speeds, watts, cal_slope, cal_intercept):
    pos_list = [p for p in valid_data if p < 2000]
    pos_list.sort()

    positions = []

    for p in pos_list:
        speed = []
        watt = []

        ids = valid_data[p]
        if ids:
            #print(p, forces[ids].mean(), (forces[ids] - ((flywheel_mps[ids]*slope - intercept)/flywheel_mps[ids])).mean())
            for i in get_mean(speeds[ids], watts[ids]):
                speed.append(0.44704 * i[0])
                base_watts = (0.44704 * i[0]) * cal_slope + cal_intercept
                ##get_base_watts(0.44704 * i[0]) #
                #print("base", 0.44704 * i[0], base_watts)
                watt.append(i[1] - base_watts)
                #print(p, round(i[0],1), round(i[1],0))
                #plt.scatter(round(i[0],1), round(i[1],0))

            slope, intercept, r_val, p_val, stderr = stats.linregress(speed, watt)
            print((p, slope, intercept))
            entry = position()
            entry.servo = p
            entry.slope = slope
            entry.intercept = intercept
            positions.append(entry)
            mph = [x * 2.23694 for x in speed]
            plt.plot(mph, watt)

    return positions

def xsl(xml_filename):
    """
    parses garmin tcx file and outputs csv file
    """

    try:
        print(xml_filename)
        xslt = ET.parse(xsl_filename)
        transform = ET.XSLT(xslt)
        print('Parsed xsl')
        dom = ET.parse(xml_filename)
        newdom = transform(dom)
        #ET.write(sys.stdout)
        print('transformed')
        print(ET.tostring(newdom, pretty_print=True))
    except:
        print("Unexpected error:", sys.exc_info())

def bike_science_func(v, drag, rr):
    #m = 72.57 # hard coded for now.
    #return  ( k*(v**2) + (m * 9.81 * Crr) ) * v
    return  ( drag*(v**2) + (rr) ) * v

def fit_bike_science(x_new, x, y):
    # convert x to meters per second from mph
    global labels
    x1 = x * 0.44704
    x_new1 = x_new * 0.44704

    pars, covar = spo.curve_fit(bike_science_func, x1, y, p0 = [0.3, 0.005])
    print('bike', pars)
    plt.plot(x_new, bike_science_func(x_new1, *pars), 'b+', zorder=100, linewidth=3)
    labels.append(r'%s' % ('Bike Function'))

def power_func(x, a, b):
    return a*(x**b)

# fit to a 2nd degree polynomial
def fit_poly2d(x_new, x, y):
    global labels
    coefficients = np.polyfit(x, y, 2)
    polynomial = np.poly1d(coefficients)
    y_new = polynomial(x_new)
    # y_new = ax^2 + bx + c
    f = ("poly2d: y = %sx^2 + %sx + %s" % (coefficients[0], coefficients[1], coefficients[2]))
    print(f)
    plt.plot(x_new, y_new, 'g-')
    labels.append(r'%s' % ('2D polynomial'))

"""
Non linear fit of base speed/watt data.
"""
def fit_nonlinear_calibration(data):
    #data = [(22.600000000000001, 204), (25.5, 247), (16.0, 129), (16.0, 139), (15.9, 126), (16.0, 132), (16.699999999999999, 133), (16.800000000000001, 134), (23.0, 200), (23.0, 219)]
    global txt_offset
    global labels

    """
    Cluster speeds and find the median watts for these speeds.
    """
    groups = []

    #print("\r\ndata:")
    keyfunc = lambda x: float(x[0])
    data = sorted(data, key=keyfunc)
    for k, g in groupby(data, keyfunc):
        items = []
        for i in g:
            items.append(i[1])
        med = np.median(items)
        groups.append((k, med))
        #print(k, med)

    npgroups = np.array(groups)
    x = npgroups[:,0]
    y = npgroups[:,1]

    # Calculate power function (y = ax^b)
    pars, covar = spo.curve_fit(power_func, x, y)

    # Create a new set of speed points between 5, 40 mph
    #x_new = np.linspace(x[0], x[-1], len(x))
    x_new = np.linspace(5, 40, 50)
    plt.plot(x_new, power_func(x_new, *pars), 'r--')
    labels.append(r'%s' % ('Power Function'))

    # Calculate bicycle power function
    #pars2, covar2 = spo.curve_fit(bicycle_func, x, y)
    #plt.plot(x_new, bicycle_func(x_new, *pars2), 'b')
    #txt_offset+=20
    #bike_func_txt = ("F = %f(V^2)+%f" % (pars2[0], pars2[1]))
    #plt.text(7, txt_offset, bike_func_txt)

    # advance the text offset vertically
    txt_offset+=20

    #func_txt = ("y = %fx^%f" % (pars[0], pars[1]))
    #plt.text(7, txt_offset, func_txt)

    # calculate the polynomial
    fit_poly2d(x_new, x, y)

    fit_bike_science(x_new, x, y)

    # calculate new x / y
    #x_new = np.linspace(x[0], x[-1], len(x))
    #y_new = f(x_new)

    # plot them
    #plt.plot(x,y,'o', x_new, y_new)
    #plt.xlim(x[0]-1, x[-1]+1)
    #txt_offset = txt_offset + 20
    #plt.text(15, txt_offset, "a: %f, b: %f, c: %f" % (z[0], z[1], z[2]))
    #plt.show()

    return pars[0], pars[1]

def graph(speeds_mph, watts, slope, intercept, color1='b', color2='r'):
    global txt_offset
    global labels
    # convert slope to wheel speed in mph from flywheel mps
    #slope = slope * (0.4/0.115) * 0.44704
    #plt.xlim([speeds_mph.min(),speeds_mph.max()])
    plt.xlim(5,40)
    plt.ylim(0,400)
    plt.scatter(speeds_mph, watts, c=color1)
    plt.plot(speeds_mph, (speeds_mph*0.44704)*slope + intercept, color2)
    labels.append(r'%s' % ('Linear'))
    txt_offset = txt_offset + 20
    plt.text(7, txt_offset, "slope: %s, offset: %i" % (math.trunc(slope*1000), math.trunc(abs(intercept)*1000)))

def fit_linear_calibration(x,y, sample= "auto", n_samples = 1e7):
    """
    Computes the Theil-Sen estimator for 2d data.
    parameters:
        x: 1-d np array, the control variate
        y: 1-d np.array, the ind variate.
        sample: if n>100, the performance can be worse, so we sample n_samples.
                Set to False to not sample.
        n_samples: how many points to sample.

    This complexity is O(n**2), which can be poor for large n. We will perform a sampling
    of data points to get an unbiased, but larger variance estimator.
    The sampling will be done by picking two points at random, and computing the slope,
    up to n_samples times.

    """
    assert x.shape[0] == y.shape[0], "x and y must be the same shape."
    n = x.shape[0]

    if n < 100 or not sample:
        ix = np.argsort( x )
        slopes = np.empty( n*(n-1)*0.5 )
        for c, pair in enumerate(itertools.combinations( range(n),2 ) ): #it creates range(n) =(
            i,j = ix[pair[0]], ix[pair[1]]
            slopes[c] = slope( x[i], x[j], y[i],y[j] )
    else:
        i1 = np.random.randint(0, n, n_samples)
        i2 = np.random.randint(0, n, n_samples)
        slopes = slope( x[i1], x[i2], y[i1], y[i2] )
        #pdb.set_trace()

    slope_ = bottleneck.nanmedian( slopes )
    #find the optimal b as the median of y_i - slope*x_i
    intercepts = np.empty( n )
    for c in range(n):
        intercepts[c] = y[c] - slope_*x[c]
    intercept_ = bottleneck.median( intercepts )

    return np.array( [slope_, intercept_] )

def slope( x_1, x_2, y_1, y_2):
    if np.abs(x_2-x_1) == 0:
        return np.nan
    return (1 - 2*(x_1>x_2) )*( (y_2 - y_1)/np.abs((x_2-x_1)) )

def find_seq(speeds, watts, offset):
    def check_seq(se): # test range [i..se) for a valid sequence
        if se > speeds.size:
            return False
        return watts[i:se].all() and speeds[i:se].all() and speeds[i:se].ptp() <= x

    seqs = []
    last_end = -1 # where last found sequence ended
    for i in range(speeds.size): # possible start of sequence
        seq_end = max(i + n, last_end + 1) # we need a sequence extending beyond the previous one
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
            if longest_end - longest_beg < n:
                return
            left = []; right = []
            for beg, end in cluster: # adjust each sequence overlapped by the longest
                if beg < longest_beg:
                    end = min(end, longest_beg)
                    if end-beg >= n:
                        left.append((beg, end))
                elif end > longest_end:
                    beg = max(beg, longest_end)
                    if end-beg >= n:
                        right.append((beg, end))

            split_cluster(left)

            m_watts = ma.array(watts[longest_beg:longest_end])
            while m_watts.std(ddof=1) > max_dev: # strip outliers until stdev in range
                edges = np.array([m_watts.min(), m_watts.max()])
                distances = abs(edges - m_watts.mean())
                outlier = edges[distances.argmax()]
                m_watts[m_watts == outlier] = ma.masked
            if m_watts.count() >= n:
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

def get_nomagnet_data(valid_data, speeds, watts):
    old_err = np.seterr(divide="ignore", invalid="ignore")
    #forces = watts / (speeds * 0.44704)
    #forces = watts / flywheel_mps
    
    np.seterr(**old_err)

    id2000 = np.fromiter(chain.from_iterable((ids for p, ids in valid_data.items() if p >= 2000)), dtype=int)
    #sp2000 = speeds[id2000]
    #f2000 = forces[id2000]
    #w2000 = watts[id2000]
    
    return id2000
    
"""
Returns array of median speed and median watts.
"""
def fit_calibration(id2000, speeds, watts):
    speeds_mps = (speeds * 0.44704)

    #slope, intercept = np.linalg.lstsq(np.vstack([sp2000, np.ones_like(sp2000)]).T, w2000)[0]
    slope, intercept =  fit_linear_calibration(speeds_mps[id2000], watts[id2000], False)

    print("\r\nslope, intercept")
    #print(slope * (0.4/0.115), intercept)
    #plt.text(10, 200, "slope: %s, offset: %i" % (round((slope * (0.4/0.115)),3), round(intercept,3)))
    print(slope, intercept)

    data_tuples = []

    # For all valid speed/watt points at servo position 2000.
    for id in id2000:
        data_tuples.append((speeds[id], watts[id]))

    a, b = fit_nonlinear_calibration(data_tuples)

    # return the linear and non-linear calibration.
    return (slope, intercept, a, b)

def process_file(input_file_name):
    """
    if input_file_name.endswith('.tcx'):
            print('Parsing tcx file...')
            xsl(input_file_name)

    exit()
    """

    speeds, watts, positions = np.loadtxt(input_file_name, delimiter=',', skiprows=skip_rows+1,
            dtype=[('speed', float), ('watts', int), ('position', int)], usecols=[speed_col, watts_col, servo_col], unpack=True, comments='"',
            converters = {5: lambda s: float(s.strip() or 0)})

    minval = np.min(speeds[np.nonzero(speeds)])
    maxval = np.max(speeds[np.nonzero(speeds)])
    if (maxval - minval) < 7:
        print("not enough speed diff.")
        return

    valid_data = defaultdict(list) # { postion: [indices of all good values] }
    splits = np.flatnonzero(np.ediff1d(positions, to_begin=1, to_end=1)) # indexes where pos changed, split original sequence there
    for i_beg, i_end in zip(splits[:-1], splits[1:]): # iterate pairs of splits
        assert positions[i_beg] == positions[i_end-1]
        valid_data[positions[i_beg]].extend(
                find_seq(speeds[i_beg:i_end], watts[i_beg:i_end], i_beg) # process each chunk separately
        )

    id2000 = get_nomagnet_data(valid_data, speeds, watts)
    sp2000 = speeds[id2000]
    w2000 = watts[id2000]

    # fit both linear and non-linear calibration.
    slope, intercept, a, b = fit_calibration(id2000, speeds, watts)

    positions = get_positions(valid_data, speeds, watts, slope, intercept)

    return sp2000, w2000, slope, intercept

def get_files(rootdir):
    for root, dirs, files in os.walk(rootdir):
        for filename in files:
            if filename.endswith('.csv'):
                filepath = os.path.join(root, filename)
                yield filepath

def graph_file(file):
    print('Processing: ' + file)
    try:
        s, w, sl, i = process_file(file)
        graph(s, w, sl, i, color1=np.random.rand(3,1))

    except:
        print("Had to skip that one because: ", traceback.format_exc())

def main(input_file_name):
    if os.path.isdir(input_file_name):
        dir = input_file_name
        for file in get_files(input_file_name):
            graph_file(file)
    else:
        graph_file(input_file_name)
        dir = os.path.dirname(os.path.realpath(input_file_name))

    #if (os.path.isdir(dir)):
    (fig_name, ext) = os.path.splitext(input_file_name)

    plt.legend(labels, loc='upper right')

    #plt.savefig(os.path.join(dir, 'slope.png'))
    plt.savefig(fig_name + '.png')
    plt.show()

if __name__ == "__main__":
    """
    Usage: parse_tr.py filename [-l]
    -l to calculate linear
    """

    if (len(sys.argv) > 2):
        speed_col = int(sys.argv[2])
    main(sys.argv[1])
    #main(input_file_name)
