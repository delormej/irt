input_file_name = "180lb_large_mag_range_adjust_speed.csv"
n = 7       # min. sequence length
x = 0.2 * 2 # total range of allowed variation
max_dev = 8 # maximum deviation of watts
skip_rows = 200 # data rows skipped at the beginning

import sys
from collections import defaultdict
from itertools import groupby, chain
import numpy as np, numpy.ma as ma

if len(sys.argv) > 1:
	input_file_name = sys.argv[1]

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
	
def main():
	speeds, watts, positions = np.loadtxt(input_file_name, delimiter=',', skiprows=skip_rows+1,
		dtype=[('speed', float), ('watts', int), ('position', int)], usecols=[3, 5, 7], unpack=True)

	valid_data = defaultdict(list) # { postion: [indices of all good values] } 
	splits = np.flatnonzero(np.ediff1d(positions, to_begin=1, to_end=1)) # indexes where pos changed, split original sequence there
	for i_beg, i_end in zip(splits[:-1], splits[1:]): # iterate pairs of splits
		assert positions[i_beg] == positions[i_end-1]
		valid_data[positions[i_beg]].extend(
			find_seq(speeds[i_beg:i_end], watts[i_beg:i_end], i_beg) # process each chunk separately
		)

	old_err = np.seterr(divide="ignore", invalid="ignore")
	forces = watts / (speeds * 0.44704)
	np.seterr(**old_err)
	
	id2000 = np.fromiter(chain.from_iterable((ids for p, ids in valid_data.items() if p >= 2000)), dtype=int)
	sp2000 = speeds[id2000]
	f2000 = forces[id2000]
	slope, intercept = np.linalg.lstsq(np.vstack([sp2000, np.ones_like(sp2000)]).T, f2000)[0]
	print("slope, intercept")
	print(slope, intercept)
	
	pos_list = [p for p in valid_data if p < 2000]
	pos_list.sort()

	print("\nposition\tspeed\tforce\tadd_force")
	for p in pos_list:
		for i in valid_data[p]:
			print(p, speeds[i], forces[i], forces[i] - speeds[i]*slope - intercept)

	print("\nposition\tforce\tadd_force")
	for p in pos_list:
		ids = valid_data[p]
		if ids:
			print(p, forces[ids].mean(), (forces[ids] - speeds[ids]*slope - intercept).mean())

main()
