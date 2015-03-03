using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IRT.Calibration
{
    public class TickEvent
    {
        public TickEvent(ushort timestamp, ushort ticks)
        {
            this.Timestamp = timestamp;
            this.Ticks = ticks;
        }

        public ushort Timestamp;
        public ushort Ticks;
        public ushort PowerEventCount;
        public ushort AccumulatedPower;

        public override string ToString()
        {
            const string format = "{0:g}, {1:g}, {2:g}, {3:g}, {4:g}, {5:g}";

            return string.Format(format,
                Timestamp,
                0, // was sequence, but no longer used
                Ticks,
                0,  // was watts, but no longer used
                PowerEventCount,
                AccumulatedPower);
        }
    }
}
