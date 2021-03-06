#input_file_name = "activity_612219153-orig.csv"
n = 5       # min. sequence length
x = 0.1 * 2 # total range of allowed variation

import sys, getopt
import numpy as np
from itertools import groupby

def find_stable(input_file_name):
    speeds, watts, positions = np.loadtxt(input_file_name, delimiter=',', skiprows=1,
	    dtype=[('speed', float), ('watts', int), ('position', int)], usecols=[3, 5, 7], unpack=True, comments='"')
    #speeds = np.array([18.8, 19.3, 19.8, 20.4, 21.3, 20.9, 21.4, 20.9, 21.4, 21.4, 21.4, 22, 21.4, 21.4, 21.6, 21.7, 21.4, 20.1, 16.7, 16.3, 16.1, 16.3, 16.4, 16.1, 16.6, 15.9])
    #speeds = np.array([16.1, 16.2, 16.3, 16.4, 16.5, 16.6, 16.7, 16.8, 16.9])
    
    last_end = 0 # where last found sequence ended
    last_position = positions[0]
    
    for i in range(speeds.size - n + 1): # possible start of sequence
    	seq_end = i + n
    	if (speeds[i:seq_end].ptp() <= x): # minimum sequence OK
    		for seq_end in range(seq_end, speeds.size+1):   # try to extend sequence
    			if speeds[i:seq_end+1].ptp() > x or positions[i] != last_position or watts[i] == 0:            # got out of range here - sequence ends
    				last_position = positions[i]
    				break
    		if seq_end > last_end: # a new sequence and not subsequence of the previous one
    			last_end = seq_end
    			"""
    			print([i, seq_end-1],
    				(speeds[i:seq_end].min()+speeds[i:seq_end].max())/2,
    				np.median(watts[i:seq_end]))
    			"""
    			print (speeds[i:seq_end].min()+speeds[i:seq_end].max())/2, ',', np.median(watts[i:seq_end]), ',', 	last_position, ',', [i, seq_end-1], seq_end-i      
    
def main(argv):
   inputfile = ''
   outputfile = ''
   usage = 'find_stable.py -i <inputfile>'
   try:
      opts, args = getopt.getopt(argv,"hi:o:")
   except getopt.GetoptError:
      print(usage)
      sys.exit(2)
   for opt, arg in opts:
      if opt == '-h':
         print(usage)
         sys.exit()
      elif opt in ("-i"):
         inputfile = arg
      elif opt in ("-o"):
         outputfile = arg         

   if not inputfile or inputfile.isspace():
       print(usage)
       sys.exit(2)

   find_stable(inputfile)

if __name__ == "__main__":
   main(sys.argv[1:])