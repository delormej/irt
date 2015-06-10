using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MathNet.Numerics.Statistics;

namespace BikeSignalProcessing
{
    public class Data
    {
        public double[] RawPower;

        public double[] SmoothedPower;

        public double[] Power5secMA;

        public double[] Power10secMA;

        public Data(string filename)
        {
            RawPower = (double[])IrtCsvFactory.Open(filename);
            SmoothedPower = PowerSmoothing.Smooth(RawPower);
            Power5secMA = PowerSmoothing.MovingAverage(SmoothedPower, 5);
            Power10secMA = PowerSmoothing.MovingAverage(SmoothedPower, 10);
        }

        public double StandardDeviation(int start, int end)
        {
            double[] sample = new double[end - start];
            Array.Copy(SmoothedPower, start, sample, 0, end - start);

            return Statistics.StandardDeviation(sample);
        }
    }
}
