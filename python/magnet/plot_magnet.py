import os
import sys
from log_parser import *
import matplotlib.pyplot as plt
        
#
# Plots an array of PositionDataPoint.
#
def plot(data):
    keyfunc = lambda x: x.position
    data = sorted(data, key=keyfunc)
    for k, g in groupby(data, keyfunc):
        items = list(g)
        
        speed = [x.speed_mps * 2.23694 for x in items]
        power = [x.power for x in items]
        plt.scatter(speed, power)

    plt.show()        
#
# Main entry point to parse a file.
#        
def main(file_name):
    parser = PositionParser()
    data = []
    
    if os.path.isdir(file_name):
        data = parser.parse_multiple(file_name)
    else:
        data = parser.parse(file_name)
    
    plot(data)
    
        
if __name__ == "__main__":
    if (len(sys.argv) > 2):
        speed_col = int(sys.argv[2])
    main(sys.argv[1])
    #main(input_file_name)    