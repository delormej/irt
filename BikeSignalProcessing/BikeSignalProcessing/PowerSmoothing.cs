using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MathNet.Filtering;
using MathNet.Filtering.Median;

namespace BikeSignalProcessing
{
    public class PowerSmoothing
    {
        public static double[] Smooth(double[] data)
        {
            double[] filteredData;

            //var filter = OnlineFilter.CreateLowpass(ImpulseResponse.Finite, 30, 1);
            var filter = OnlineFilter.CreateDenoise();
            filteredData = filter.ProcessSamples(data);

            return filteredData;
        }

        public static double[] MovingAverage(double[] data, int duration)
        {
            OnlineMedianFilter filter = new OnlineMedianFilter(duration);
            return filter.ProcessSamples(data);
        }
    }
}
