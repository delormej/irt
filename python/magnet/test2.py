import sys, getopt
import log_parser as lp
import matplotlib.pyplot as plt
import fit
import numpy as np

#def calc_force(speed_mps, watts):
    #f, coeff = fit_polynomial(servo_pos, power, c)
    

#
# Main entry point to parse a file.
#        
def main(file_name, drag, rr, offset):
    #x = np.array([12, 16, 20, 28, 32])  # 20 == 73??
    #y = [39 ,54 ,73, 113, 131]
    
    #drag, rr = fit.fit_bike_science(x*0.44704,y)
    #print("using calibration:", drag, rr)
    
    p = lp.LogParser(file_name, drag, rr, force_offset=offset)
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
    
    est, points, avg_err = p.EstimateError()
    print("Error estimate: %f based on: %d points, avg err: %f" % (est, points, avg_err))
    
    p.PlotRide()
    p.PlotMagnet()
    plt.show()
    
        
if __name__ == "__main__":
    file = ""
    drag = 0
    rr = 0
    offset = 0

    try:
      opts, args = getopt.getopt(sys.argv[1:],"hi:d:r:o:",["input=","drag=","rr=","offset="])
    except getopt.GetoptError:
      print('parse_tr.py -i <inputfile> -d <drag> -r <rolling resistance> -o <force offset>')
      sys.exit(2)        
    
    for opt, arg in opts:
        if opt == '-h':
            print('parse_tr.py -i <inputfile> -d <drag> -r <rolling resistance>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            file = arg
        elif opt in ("-d", "--drag"):
            drag = float(arg)
        elif opt in ("-r", "--rr"):
            rr = float(arg)
        elif opt in ("-o", "--offset"):
            offset = int(arg)
            
    main(file, drag, rr, offset)