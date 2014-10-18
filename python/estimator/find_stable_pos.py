input_file_name = "black_arm_level_1-4_15mph_newdev.csv"
input_file_name = "black_arm_level_1-4_15mph_olddev.csv"
output_file_name = "find_stable_result.csv"
n = 7       # min. sequence length
x = 0.2 * 2 # total range of allowed variation
s = 10  # allowable stdev for watts

from itertools import groupby
import csv
import numpy as np

seqs = [] # list of all found sequences

def find_seq(speeds, watts, offset, pos):
	def check_seq(se): # test range [i..se) for a valid sequence
		if se > speeds.size:
			return False
#   		print watts[se-1], np.mean(watts[i:se])
		return speeds[i:se].ptp() <= x and watts[i:se].all()
		
	last_end = -1 # where last found sequence ended
	for i in range(speeds.size): # possible start of sequence
		seq_end = max(i + n, last_end + 1) # we need a sequence extending beyond the previous one
		if seq_end > speeds.size: # reached the end
			break
		if check_seq(seq_end): # minimum sequence is OK
			while check_seq(seq_end+1): # extend the sequence while we can
				seq_end += 1
			last_end = seq_end
   
			seqs.append({
				'beg': i+offset,
				'end': last_end-1+offset,
				'speed': (speeds[i:last_end].min()+speeds[i:last_end].max())/2,
				'watts': np.mean(watts[i:last_end]),
				'position': pos,
				'stdev': np.std(watts[i:last_end])    
			})

def main():
	speeds, watts, positions = np.loadtxt(input_file_name, delimiter=',', skiprows=1,
		dtype=[('speed', float), ('watts', int), ('position', int)], usecols=[2, 3, 4], unpack=True)
	splits = np.flatnonzero(np.ediff1d(positions, to_begin=1, to_end=1)) # indexes where pos changed, split original sequence there
	for i_beg, i_end in zip(splits[:-1], splits[1:]): # iterate pairs of splits
#		assert positions[i_beg] == positions[i_end-1]
		find_seq(speeds[i_beg:i_end], watts[i_beg:i_end], i_beg, positions[i_beg]) # process each chunk separately
	
	def group_overlaps(seq): # clustering all overlapping sequences together, returns cluster number
		if seq['beg'] > group_overlaps.last_end: # broke overlapping cluster, increase cluster number
			group_overlaps.group_no += 1
		group_overlaps.last_end = seq['end']
		return group_overlaps.group_no
	
	group_overlaps.last_end = -1
	group_overlaps.group_no = -1
	
	lone_seqs = [] # list of non-overlapped sequences
	for i, cluster in groupby(seqs, group_overlaps): # for each cluster of overlapping sequences
		lone_seqs.append(max(cluster, key=lambda seq: seq['end']-seq['beg'])) # find the longest
		
	for seq in lone_seqs:
		seq['speed'] = format(seq['speed'], '.1f')
		seq['watts'] = format(seq['watts'], '.0f')
  		force = round(float(seq['watts']) / (float(seq['speed']) * 0.44704),2)
  		print seq['speed'], ',', seq['watts'], ',', seq['position'], ',', force, ',', seq['stdev'],',',(seq['beg'], seq['end'])
    
	"""
	if lone_seqs:
		with open(output_file_name, 'w') as output_file:
			writer = csv.DictWriter(output_file, ('beg', 'end', 'speed', 'watts', 'position'))
			writer.writeheader()
			writer.writerows(lone_seqs)
	"""
   
main()
