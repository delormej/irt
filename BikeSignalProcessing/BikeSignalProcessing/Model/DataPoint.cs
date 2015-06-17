using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;


namespace BikeSignalProcessing.Model
{
    public class BikeDataPoint
    {
        public int Seconds { get; set; }

        public double PowerWatts { get; set; }

        public double SpeedMph { get; set; }

        public int ServoPosition { get; set; }

        public double SmoothedPowerWatts { get; set; }

        public double SmoothedSpeedMph { get; set; }
    }   
}
