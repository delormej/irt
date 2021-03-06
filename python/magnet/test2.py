import os
import sys, getopt
import log_parser as lp
import matplotlib.pyplot as plt
import fit
import numpy as np

#
# Enumerator for recursive file search.
#
def get_files(rootdir):
    for root, dirs, files in os.walk(rootdir):
        if root.find("BCI") > -1 or root.find("Mag_Calibration") > -1:
            continue
        for filename in files:
            if filename.startswith('irt_') and filename.endswith('.csv'):
                filepath = os.path.join(root, filename)
                yield filepath

#
#
#
def isBeta2(file):
    p = lp.LogParser(file)
    if (p.firmware_rev.startswith("1.")):
        try:
            print("processing:", file)
            
            if p.drag > 0.1:
                # Represents old calibration.
                main(file, drag = 0, rr = 0)
            else:
                main(file)
        except:
            print("skipping:", file)
                    				

#
# Main entry point to parse a file.
#        
def main(file_name, drag=0, rr=0, offset=0):
    #x = np.array([12, 16, 20, 28, 32])  # 20 == 73??
    #y = [39 ,54 ,73, 113, 131]
    
    #x = np.array([10.5, 14.9, 16.4, 19.8, 26])	
    #y = [79, 125, 140, 182, 255]
    
    #y = [258.4117647,194.0487805,149.7560976,140.3333333,123.7303371,108.902439,92,76.43333333]
    #x = np.array([28.21176471,22.85853659,18.99512195,16.98666667,15.79550562,14.04634146,12.66923077,10.35333333])

    
    #drag, rr = fit.fit_bike_science(x*0.44704,y)
    #print("using calibration:", drag, rr)
    #return
    
    p = lp.LogParser(file_name)
    
    #p.servo_offset = 320
    
    # Override config if passed here.
    if drag > 0:
        p.drag = drag
    if rr > 0:
        p.rr = rr
    if offset > 0:
        p.gap_offset = offset

    p.print_slope_intercept = True   

    p.Parse()
    #print(p.MagOnlyPower())
    """
    BCI values:
    Drag: 0.01305838
    RR: 7.530708
    """
    #print(len(p.records))
    #print(p.records[p.stable_records['index']][10],)
    #plt.plot(p.records['speed'], p.records['power'])
    #plt.show()
    
    #est, points, avg_err = p.EstimateError()
    print("Error estimate: ", p.EstimateError())
    #print("Error estimate: %f based on: %d points, avg err: %f" % (est, points, avg_err))
    
    p.PlotRide()
    p.PlotMagnet()
    plt.title(file_name, fontsize=6)
    plt.show()
    
        
if __name__ == "__main__":
    usage = 'parse_tr.py -i <inputfile> -d <drag> -r <rolling resistance> -g <gap offset>'
    file = ""
    drag = 0
    rr = 0
    offset = 0
    
    try:
      opts, args = getopt.getopt(sys.argv[1:],"hi:d:r:g:",["input=","drag=","rr=","gap_offset="])
    except getopt.GetoptError:
      print(usage)
      sys.exit(2)        
    
    for opt, arg in opts:
        if opt == '-h':
            print(usage)
            sys.exit()
        elif opt in ("-i", "--ifile"):
            file = arg
        elif opt in ("-d", "--drag"):
            drag = float(arg)
        elif opt in ("-r", "--rr"):
            rr = float(arg)
        elif opt in ("-g", "--gap_offset"):
            offset = float(arg)
            
    if os.path.isdir(file):
        for f in get_files(file):
            isBeta2(f)
    else:
        main(file, drag, rr, offset)
        