input_file_name = "black_arm_level_1-4_15mph_newdev.csv"
n = 5       # min. sequence length
x = 0.1 * 2 # total range of allowed variation

import numpy as np

def find_seq(speeds, watts, offset, pos):
	last_end = -1 # where last found sequence ended
	for i in range(speeds.size): # possible start of sequence
		seq_end = max(i + n, last_end + 1) # we need a sequence extending beyond the previous one
		if seq_end > speeds.size: # reached the end
			break
		if (speeds[i:seq_end].ptp() <= x): # minimum sequence OK
			while seq_end < speeds.size and (speeds[i:seq_end+1].ptp() <= x): # try to extend sequence
				seq_end += 1
			last_end = seq_end
			print([i+offset, seq_end-1+offset],
				round((speeds[i:seq_end].min()+speeds[i:seq_end].max())/2, 1),
				np.median(watts[i:seq_end]), pos)

def main():
	speeds, watts, pos = np.loadtxt(input_file_name, delimiter=',', skiprows=1, usecols=[2, 3, 4], unpack=True)
	splits = np.flatnonzero(np.ediff1d(pos, to_begin=1, to_end=1)) # indexes where pos changed, split original sequence there 
	for i_beg, i_end in zip(splits[:-1], splits[1:]): # iterate pairs of splits
		find_seq(speeds[i_beg:i_end], watts[i_beg:i_end], i_beg, pos[i_beg]) # process each chunk separately

main()