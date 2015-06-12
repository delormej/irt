using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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

        public static IEnumerable<Segment> FindBestSegments(IEnumerable<Segment> segments)
        {
            List<Segment> matches = new List<Segment>();

            // Sort segments by position.
            var positionGroups = segments.GroupBy(s => s.ServoPosition);

            foreach (var positionGroup in positionGroups)
            {
                // More than one segment in a servo position group.
                if (positionGroup.Count() > 1)
                {
                    // Order by speed.
                    var ordered = positionGroup.OrderBy(s => s.AverageSpeed)
                        .ThenByDescending(s => (s.End - s.Start));



                    //var ordered = group.OrderByDescending(s => (s.End - s.Start));

                    // If there is more than one segment by position, group by speed.

                }
            }

            

            // Take the longest segment when more than 1 for a given speed.

            return matches;
        }

        private static bool InVariance(double originalValue, double newValue, double variance)
        {
            return (newValue < (originalValue + variance) &&
                newValue > (originalValue - variance));
        }

        private IEnumerable<Segment> FindBestInGroup(IEnumerable<Segment> ordered)
        {


            const double variance = 1.5; // +/- 1.5 mph.

            Segment last = ordered.First();

            foreach (Segment segment in ordered)
            {
                // Group by speeds within variance.
                //ordered.GroupBy(s => InVariance(s.AverageSpeed));



                //
                // If this segment's speed is within the range of the last, take
                // the largest segment (end-start).
                //
                if (InVariance(segment.AverageSpeed, last.AverageSpeed, variance))
                {
                    yield return last;
                }
                else
                {
                    yield return segment;
                }

                last = segment;
            }
        }
    }
}
