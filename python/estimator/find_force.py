#input_file_name = "180lb_large_mag_range_adjust_speed.csv"
n = 7       # min. sequence length
x = 0.2 * 2 # total range of allowed variation
max_dev = 8 # maximum deviation of watts
skip_rows = 600 # data rows skipped at the beginning
#xsl_filename = '../../tcx-to-csv.xslt'

import sys
from collections import defaultdict
from itertools import groupby, chain
import numpy as np, numpy.ma as ma
import bottleneck 
import itertools
import matplotlib.pyplot as plt
#import lxml.etree as ET

if len(sys.argv) > 1:
	input_file_name = sys.argv[1]

def xsl(xml_filename):
    """
    parses garmin tcx file and outputs csv file
    """
    print(xml_filename)
    xslt = ET.parse(xsl_filename)
    transform = ET.XSLT(xslt)

    dom = ET.parse(xml_filename)
    newdom = transform(dom)
    #ET.write(sys.stdout)
    print(ET.tostring(newdom)) #, pretty_print=True))

def graph(speeds_mph, watts, slope, intercept, color1='b', color2='r'):
	# convert slope to wheel speed in mph from flywheel mps
	slope = slope * (0.4/0.115) * 0.44704
	plt.scatter(speeds_mph, watts, c=color1)
	plt.plot(speeds_mph, speeds_mph*slope + intercept, color2)

def theil_sen(x,y, sample= "auto", n_samples = 1e7):
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

def process_file(input_file_name):
	"""
    if input_file_name.endswith('.tcx'):
        print("Parsing tcx file...")        
        xsl(input_file_name)
        #input_file_name.replace()        
    else:
        exit()
	"""    

	speeds, watts, positions = np.loadtxt(input_file_name, delimiter=',', skiprows=skip_rows+1,
		dtype=[('speed', float), ('watts', int), ('position', int)], usecols=[3, 5, 7], unpack=True, comments='"')

	# convert to meters per second, then to flywheel meters per second
	speeds_mps = (speeds * 0.44704)
	flywheel_mps = (speeds_mps * (0.4/0.115))

	valid_data = defaultdict(list) # { postion: [indices of all good values] } 
	splits = np.flatnonzero(np.ediff1d(positions, to_begin=1, to_end=1)) # indexes where pos changed, split original sequence there
	for i_beg, i_end in zip(splits[:-1], splits[1:]): # iterate pairs of splits
		assert positions[i_beg] == positions[i_end-1]
		valid_data[positions[i_beg]].extend(
			find_seq(speeds[i_beg:i_end], watts[i_beg:i_end], i_beg) # process each chunk separately
		)

	old_err = np.seterr(divide="ignore", invalid="ignore")
	#forces = watts / (speeds * 0.44704)
	forces = watts / flywheel_mps
	np.seterr(**old_err)
	
	id2000 = np.fromiter(chain.from_iterable((ids for p, ids in valid_data.items() if p >= 2000)), dtype=int)
	sp2000 = speeds[id2000]
	f2000 = forces[id2000]
	w2000 = watts[id2000]
	#slope, intercept = np.linalg.lstsq(np.vstack([sp2000, np.ones_like(sp2000)]).T, w2000)[0]
	slope, intercept = theil_sen(flywheel_mps[id2000], watts[id2000], False)

	print("slope, intercept")
	print(slope * (0.4/0.115), intercept)
	#print(slope, intercept)
	
	pos_list = [p for p in valid_data if p < 2000]
	pos_list.sort()

	"""
	print("\nposition\tspeed\tforce\tadd_force")
	for p in pos_list:
		for i in valid_data[p]:
			print(p, flywheel_mps[i], forces[i], forces[i] - ((flywheel_mps[i]*slope - intercept)/flywheel_mps[i]))
	"""

	print("\nposition\tforce\tadd_force")
	for p in pos_list:
		ids = valid_data[p]
		if ids:
			#print(p, forces[ids].mean(), (forces[ids] - ((flywheel_mps[ids]*slope - intercept)/flywheel_mps[ids])).mean())
			print(p, bottleneck.nanmedian(forces[ids]), bottleneck.nanmedian(forces[ids] - ((flywheel_mps[ids]*slope - intercept)/flywheel_mps[ids])))

	
def main(input_file_name):
	
	process_file(input_file_name)
	#graph(speeds[id2000], w2000, slope, intercept)
	#plt.show()

if __name__ == "__main__":
   main(sys.argv[1])
   #main(input_file_name)
