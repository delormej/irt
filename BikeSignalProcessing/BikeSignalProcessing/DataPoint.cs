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
    }
}
