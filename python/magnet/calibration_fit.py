import math
import traceback
from collections import defaultdict
from itertools import groupby, chain
import numpy as np, numpy.ma as ma
import scipy.optimize as spo, scipy.stats as stats
import bottleneck
import itertools

# ----------------------------------------------------------------------------
#
# Encapsulates all the calibration and magnet data fit functions.
#
# ----------------------------------------------------------------------------
class CalibrationFit:
    def fit_lin_theilsen(self, x,y, sample= "auto", n_samples = 1e7):
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
        
        def slope( x_1, x_2, y_1, y_2):
            if np.abs(x_2-x_1) == 0:
                return np.nan
            return (1 - 2*(x_1>x_2) )*( (y_2 - y_1)/np.abs((x_2-x_1)) )     

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

        return slope_, intercept_
        
    #
    # Fits slope & intercept using a linear regression.
    #
    def fit_lin_regress(self, speed, power):
        slope, intercept, r_val, p_val, stderr = stats.linregress(np.asarray(speed), np.asarray(power))
            
        speed_new = np.arange(5,30)
        speed_new = speed_new * 0.44704
        power_new = lambda x: x * slope + intercept
        
        return slope, intercept, speed_new, power_new(speed_new)
    
    #
    # Returns power as calculated from calibration (drag & speed_mps) with no
    #
    def magoff_power(self, speed_mps, drag, rr):
        return (( drag*(speed_mps**2) + (rr) ) * speed_mps )    
    
    #
    # x = speed_mps
    # y = power
    #
    def fit_bike_science(self, x, y):
        pars, covar = spo.curve_fit(self.magoff_power, x, y)
        print('bike', pars)
        #plt.plot(x_new, bike_science_func(x_new1, *pars), 'b+', zorder=100, linewidth=3)
        #labels.append(r'%s' % ('Bike Function'))        
        
        return pars[0], pars[1]
        
    #
    # Fits calibration data for all records at servo position 2000.
    #
    def fit_nonlinear_calibration(self, records):
        
        # Skip first 7 minutes of data (7*60 = 420 records).
        if len(records) < 420:
            raise "Not enough rows to calibrate."
    
        # Only use data where position == 2000
        #id2000 = [i for i, x in enumerate(records[420:]) if x['position']==2000]
    
        """
        Cluster speeds and find the median watts for these speeds.
        """
        groups = []

        keyfunc = lambda x: float(x['speed_mps'])
        data = sorted(records, key=keyfunc)
        for k, g in groupby(data, keyfunc):
            items = []
            for i in g:
                items.append(i[1])
            med = np.mean(items) #TODO: change this to mean/average?
            groups.append((k, med))

        npgroups = np.array(groups)
        x = npgroups[:,0]
        y = npgroups[:,1]

        #x = [10.1* 0.44704, 15.1* 0.44704, 20.1* 0.44704, 25.2* 0.44704]
        #y = [74, 113, 158, 217]
        
        return self.fit_bike_science(x, y)
            
            