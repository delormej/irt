using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MathNet.Filtering;
using MathNet.Filtering.Median;
using MathNet.Numerics.Statistics;

namespace BikeSignalProcessing
{
    public class PowerSmoothing
    {
        private IOnlineFilter powerFilter;

        public PowerSmoothing()
        {
            powerFilter = OnlineFilter.CreateDenoise();
        }

        public double SmoothPower(double sample)
        {
            return powerFilter.ProcessSample(sample);
        }

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

        public static double Average(double[] data, int start, int window)
        {
            double[] sample = new double[window];
            Array.Copy(data, start, sample, 0, window);
            return sample.Average();
        }

        public static double StandardDeviation(double[] data, int start, int end)
        {
            double[] sample = new double[end - start];
            Array.Copy(data, start, sample, 0, end - start);

            return Statistics.StandardDeviation(sample);
        }


        // From a 10 point moving average, calculate STDDEV if less than threshold 
        // save the starting point.
        // As soon as you hit 
        public static List<Segment> GetSegments(double[] data, double threshold, int window)
        {
            List<Segment> segments = new List<Segment>();
            Segment segment = null;

            for (int i = window; i < data.Length; i++)
            {
                int start = segment != null ? segment.Start : i - window;

                // Move on if power is 0.
                if (data[start] == 0)
                    continue;

                double dev = StandardDeviation(data, start, i);

                if (dev <= threshold)
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
                        if (i > (segment.Start + window))
                        {
                            segment.End = i - 3;
                            segment.Power = PowerSmoothing.Average(
                                data, segment.Start, segment.End - segment.Start);

                            if (segment.Power > 0)
                            {
                                Segment copy = segment.Copy();
                                segments.Add(copy);

                                // Advance the Window.
                                i = segment.End + window;
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
