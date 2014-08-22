using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ANT_Console.Services
{
    public static class TheilSen
    {
        static ArrayList Calc(List<double> x, List<double> y)
        {

            int n = x.Count; 
            //if n < 100 or not sample:

            //        ix = np.argsort( x )
            

            //slopes = np.empty( n*(n-1)*0.5 )
            

            return null;
        }

        private static double Slope(double x_1, double x_2, double y_1, double y_2)
        {
            return (1 - 2 * (x_1 > x_2 ? 1 : 0) * ((y_2 - y_1) / Math.Abs((x_2 - x_1))));
        }
    }
}
