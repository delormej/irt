import sys, getopt
import os
import math
import numpy as np
import matplotlib.pyplot as plt
import csv
import magnet as mag
import fit as fit

"""
Calculates a moving average of an array of values.
"""
def moving_average(x, n, type='simple'):
    """
    compute an n period moving average.

    type is 'simple' | 'exponential'

    """
    x = np.asarray(x)
    if type=='simple':
        weights = np.ones(n)
    else:
        weights = np.exp(np.linspace(-1., 0., n))

    weights /= weights.sum()


    a =  np.convolve(x, weights, mode='full')[:len(x)]
    a[:n] = a[n]
    return a

"""
Calculates power based coast down fit (drag & rr), speed and magnet position.
"""
def get_power(speed_mps, servo_pos, drag, rr):
    # Calculates non-mag power based on speed in meters per second.
    no_mag_watts = (drag*speed_mps**2)+(speed_mps * rr) # Force = K(V^2) + (mgCrr)  
    
    # If the magnet is ON the position will be less than 1600
    if servo_pos < 1600:
        # Calculates the magnet-only power based on speed in meters per second.
        mag_watts = mag.watts(speed_mps, servo_pos)
        #print(servo_pos, mag_watts)
    else:
        mag_watts = 0   
    
    return no_mag_watts + mag_watts
    
"""
Reads an irt log file and returns the Drag and RR settings.
"""
def read_calibration(file_name):
    drag = 0
    rr = 0
    with open(file_name, 'r') as f:
        for row in reversed(list(csv.reader(f))):
            if row[0] == 'RR':
                rr = float(row[1])
            if row[0] == 'Drag':
                drag = float(row[1])
                return drag, rr

"""
Returns an array of the same dimension of mph & servo_pos with estimated power values.
"""
def estimate_power(mph, servo_pos, drag, rr):
    #assert mph.shape[0] == servo_pos.shape[0], "Speed and servo must be the same shape."

    est_watts = [] #mph.shape[0]
    
    for i, speed in enumerate(mph):
        #print(speed)
        est_watts.append(get_power(speed * 0.44704, servo_pos[i], drag, rr))
    
    return est_watts
    
def main(file_name, drag, rr, offset):
    
    if (os.path.basename(file_name).startswith("irt_")):
        #
        # load the irt csv file
        #
        #print(read_calibration(file_name))
        power_watts, kmh, target_watts, emr_watts, servo_pos = \
            np.loadtxt(file_name, delimiter=',', skiprows=3,
                            dtype=[('emr_watts', float), ('kmh', float), ('target_watts', float), ('power_watts', float), ('servo_pos', int)], 
                            usecols=[5, 3, 6, 6, 7], unpack=True, comments='"')
        minutes = range(0, power_watts.shape[0])
        drag, rr = read_calibration(file_name)
        print(drag, rr)

    else:
        #
        # load a TrainerRoad log file
        #
        # "Watts" == Power Meter
        # "TargetData" == TR's erg target
        # "PowerMeterData" == E-Motion Virtual Power
        input = np.genfromtxt(file_name, delimiter=',', skiprows=3, missing_values=0, \
                usecols=[1, 2, 8, 9, 10, 11], dtype=[('minutes', float), ('power_watts', float), \
                ('kmh', float), ('target_watts', float), ('emr_watts', float), ('servo_pos', int)])
                                
        minutes = input['minutes']
        power_watts = input['power_watts']
        kmh = input['kmh']
        target_watts = input['target_watts']
        emr_watts = input['emr_watts']
        servo_pos = input['servo_pos']
        
        kmh = kmh * 0.621371

    labels = []

    ma_speed = moving_average(kmh, 30)
    ma_power30 = moving_average(power_watts, 30)
    ma_power10 = moving_average(power_watts, 10)
    ma_est_power = moving_average(emr_watts, 30)
    
    plt.rc('axes', grid=True)
    plt.rc('grid', color='0.75', linestyle='-', linewidth=0.5)

    #left, width = 1, 0.8
    #rect1 = [left, 1, width, 1]
    #fig = plt.figure()
    #ax1 = fig.add_axes(rect1)
    #ax2 = fig.add_axes(rect1, sharex=ax1)
    ax1 = plt.subplot(311)
    ax2 = plt.subplot(312,sharex=ax1)
    #ax2 = ax1.twinx()
    ax3 = plt.subplot(313,sharex=ax1)

    # ax1 = Speed & Servo
    ax1.plot(minutes, kmh)
    #labels.append(r'y = %s' % ('kmh'))
    ax1.set_ylim(7, 30)

    ax1.plot(minutes, ma_speed)

    ax2.plot(minutes, servo_pos, color='r')
    ax2.set_ylim(750, 1700)

    ax3.plot(minutes, power_watts, 'r')
    labels.append(r'%s' % ('Actual'))
    
    ax3.plot(minutes, emr_watts, color='c')
    labels.append(r'%s' % ('Estimated'))

    ax3.plot(minutes, ma_power30, color='b')
    labels.append(r'%s' % ('30 sec MA'))
    ax3.plot(minutes, ma_power10, color='lightblue')
    labels.append(r'%s' % ('10 sec MA'))
    ax3.plot(minutes, ma_est_power, color='y')
    labels.append(r'%s' % ('Est 30 sec MA'))

    #ax2.stackplot(minutes, target_watts)
    ax3.plot(minutes, target_watts, linestyle='--', linewidth='2', color='g', zorder=100)
    labels.append(r'%s' % ('Target'))
    ax3.set_ylim(50, 600)

    if drag > 0 and rr > 0:
        #fit.init_mag(offset)
        est_watts = estimate_power(kmh, servo_pos, drag, rr)
        ax3.plot(minutes, est_watts, linestyle=':', color='orange', linewidth='3', zorder=200)
        labels.append(r'%s' % ('Revised Estimate'))
    
    plt.legend(labels, loc='upper right')   
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

