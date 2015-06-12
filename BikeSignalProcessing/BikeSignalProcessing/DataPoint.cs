using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MathNet.Numerics.Statistics;

namespace BikeSignalProcessing
{
    public class DataPoint
    {
        public int Seconds { get; set; }

        public double PowerWatts { get; set; }

        public double SpeedMph { get; set; }

        public int ServoPosition { get; set; }

        public double SmoothedPowerWatts { get; set; }

        public double SmoothedSpeedMph { get; set; }
    }

    public class Segment
    {
        public int Start;
        public int End;
        public double StdDev;
        public double AveragePower;
        public double AverageSpeed;
        public int ServoPosition;

        public Segment()
        {
            Start = 0;
            End = 0;
        }

        public Segment Copy()
        {
            return this.MemberwiseClone() as Segment;
        }

        /// <summary>
        /// Groups segments of similar speed and returns the segment with the greatest
        /// number of values.
        /// </summary>
        /// <param name="segments"></param>
        /// <returns></returns>
        public static IEnumerable<Segment> FindBestSegments(IEnumerable<Segment> segments)
        {
            var speeds = segments
                .OrderBy(s => s.AverageSpeed)
                .Select(s => s.AverageSpeed);

            double dev = speeds.StandardDeviation();
            var segs = segments.OrderBy(s => s.AverageSpeed);

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
                        best = group.Where(s => (s.End - s.Start) == max).Single();

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
    }
}
