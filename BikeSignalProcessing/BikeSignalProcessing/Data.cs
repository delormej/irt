using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MathNet.Numerics.Statistics;

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
        public const double Threshold = 4.0;
        public const int Window = 10;

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

        // From a 10 point moving average, calculate STDDEV if less than threshold 
        // save the starting point.
        // As soon as you hit 
        public List<Segment> GetSegments()
        {
            List<Segment> segments = new List<Segment>();
            Segment segment = null;

            for (int i = Window; i < SmoothedPower.Length; i++)
            {
                int start = segment != null ? segment.Start : i - Window;

                // Move on if power is 0.
                if (SmoothedPower[start] == 0)
                    continue;

                double dev = StandardDeviation(start, i);

                if (dev <= Threshold)
                {
                    //
                    // Within threshold.
                    //
                    if (segment == null)
                    {
                        segment = new Segment();
                        segment.Start = start;
                    }
                    else
                    {
                        segment.StdDev = dev;
                    }
                }
                else
                {
                    //
                    // Outside of threshold
                    //
                    if (segment != null)
                    {
                        // Make sure segment is at least as big as Window
                        if (i > (segment.Start + Window))
                        {
                            segment.End = i-1;
                            segment.Power = PowerSmoothing.Average(
                                SmoothedPower, segment.Start, Window);

                            if (segment.Power > 0)
                            {
                                Segment copy = segment.Copy();
                                segments.Add(copy);

                                // Advance the Window.
                                i += Window;
                            }
                        }
                        segment = null;
                    }
                }
            }

            return segments;
        }
    }
}
