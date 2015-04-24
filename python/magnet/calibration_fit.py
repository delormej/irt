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
    def fit_linear(self, x,y, sample= "auto", n_samples = 1e7):
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
