using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using MathNet.Numerics.Statistics;
using MathNet.Numerics.LinearRegression;
using MathNet.Filtering.Median;

namespace BikeSignalProcessing.Model
{
    public enum SegmentState
    {
        Started,
        Finished,
        Invalidated
    }

    public class Segment
    {
        private int mStart;
        private int mEnd;
        private Data mOwner;    // object that owns this segment.
        private double mAveragePower;

        // Hide constructor without Data arg.
        private Segment()
        {
            Start = 0;
            End = 0;
        }

        public Segment(Data owner) : this()
        {
            this.mOwner = owner;
        }

        public int Start
        {
            get { return mStart; }
            set
            {
                mStart = value;
                State = SegmentState.Started;
            }
        }

        public int End
        {
            get { return mEnd; }
            set
            {
                mEnd = value;
                State = SegmentState.Finished;
            }
        }

        public double StdDev;
        public double AveragePower
        {
            get
            {
                if (mOwner.mPowerFit != null)
                {
                    return mAveragePower -
                        mOwner.mPowerFit.Watts(this.AverageSpeed);
                }
                else
                {
                    return mAveragePower;
                }
            }
            set { mAveragePower = value; }
        }

        public double AverageSpeed;
        public int MagnetPosition;

        public SegmentState State { get; set; }

        public Segment Copy()
        {
            return this.MemberwiseClone() as Segment;
        }


        public static IEnumerable<Segment> FindBestNoMagnetSegments(IEnumerable<Segment> segments)
        {
            // Filter for only those that have no magnet.
            return FindBestSegments(
                segments.Where(s => s.MagnetPosition >= 1600) );
        }

        /// <summary>
        /// Groups segments of similar speed and returns the segment with the greatest
        /// number of values.
        /// </summary>
        /// <param name="segments"></param>
        /// <returns></returns>
        public static IEnumerable<Segment> FindBestSegments(IEnumerable<Segment> segments)
        {
            var segs = segments.OrderBy(s => s.AverageSpeed);
            var speeds = segs.Select(s => s.AverageSpeed);

            double dev = speeds.StandardDeviation();

            List<Segment> group = null;

            // Get an enumerator and stash the first record.
            var enumerator = segs.GetEnumerator();
            enumerator.MoveNext();
            Segment last = enumerator.Current;

            do
            {
                Segment seg = null;
                if (enumerator.MoveNext())
                    seg = enumerator.Current;

                if (seg != null && (seg.AverageSpeed - last.AverageSpeed) < dev)
                {
                    // Group close speeds together.
                    if (group == null)
                    {
                        group = new List<Segment>();
                        group.Add(last);
                    }

                    // Defer evalutation until a speed is out of range.
                    group.Add(seg);
                    continue;
                }
                else
                {
                    Segment best = null;

                    // If group exists, select the best from the group.
                    if (group != null)
                    {
                        // Evaluate the group and find the one with the most
                        double minSpeed = group.Min(s => s.AverageSpeed);
                        double maxSpeed = group.Max(s => s.AverageSpeed);

                        int max = group.Max(s => (s.End - s.Start));
                        best = group.Where(s => (s.End - s.Start) == max).First();

                        // Reset group.
                        group = null;

                        System.Diagnostics.Debug.WriteLine("GROUP: Min: {0:N1}, Max:{1:N1}, Count:{2:N0}",
                            minSpeed, maxSpeed, best.End - best.Start);
                    }
                    else
                    {
                        // Not part of a group, return the last one evaluated.
                        best = last;
                    }

                    last = seg;

                    // Return the best segment.
                    yield return best;
                }
            } while (last != null);
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

    }
}
