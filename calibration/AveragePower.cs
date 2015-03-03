using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IRT.Calibration
{
    public class AveragePower
    {
        // Average roughly every 15 seconds, we should get an event per second.
        private int EventSkipOffset = 15;

        private List<PowerEvent> accumulatedWatts; 

        public AveragePower()
        {
            accumulatedWatts = new List<PowerEvent>();
        }

        /// <summary>
        /// Decodes and records a power event.
        /// </summary>
        /// <param name="buffer"></param>
        /// <param name="watts"></param>
        public double AddEvent(ushort eventCount, ushort accumWatts)
        {
            accumulatedWatts.Add(new PowerEvent(eventCount, accumWatts));

            // Search for a record 'n' indexes ago to average from.
            int index = accumulatedWatts.Count - 2;
            
            while (index-- > 0 && (accumulatedWatts[index].Index <
                    eventCount - EventSkipOffset));

            // Returns as much of an average as data as we have
            // until we get to 15 seconds.
            return (accumulatedWatts[index].AccumPower - accumWatts) /
                accumulatedWatts[index].Index - eventCount;
        }
    }

    internal class PowerEvent
    {
        public PowerEvent(ushort index, ushort accumPower)
        {
            this.Index = index;
            this.AccumPower = accumPower;
        }

        public ushort Index;
        public ushort AccumPower;
    }
}
