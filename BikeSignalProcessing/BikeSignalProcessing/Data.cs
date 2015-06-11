using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BikeSignalProcessing
{
    public class Segment
    {
        public int Start;
        public int End;
        public double StdDev;
        public double Power;

        public Segment Copy()
        {
            return this.MemberwiseClone() as Segment;
        }
    }

    public class Data
    {
        public double Threshold = 4.0;
        public int Window = 10;

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
    }
}
