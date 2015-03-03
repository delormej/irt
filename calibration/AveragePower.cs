using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IRT.Calibration
{
    /// <summary>
    /// Encapsulates average power calculation from power meter events.
    /// </summary>
    public static class AveragePower
    {
        /// <summary>
        /// Decodes and records a power event.
        /// </summary>
        /// <param name="buffer"></param>
        /// <param name="watts"></param>
        public static void AddEvent(int eventCount, ushort accumWatts, List<TickEvent> events)
        {
            if (events.Count == 0)
                return;

            // Append power event to the last tick record.
            TickEvent powerEvent = events.Last();

            // Event count is actually only 1 byte on the wire, handle rollover.
            if (eventCount != -1 && eventCount < powerEvent.PowerEventCount)
            {
                powerEvent.PowerEventCount += eventCount;
            }
            else
            {
                powerEvent.PowerEventCount = eventCount;
            }
             
            powerEvent.AccumulatedPower = accumWatts;
        }

        public static double CalculateAverage(List<TickEvent> events)
        {
            double watts = 0;

            if (events.Count > 2)
            {
                // Search for a record 'n' indexes ago to average from, starting here:
                int index = events.Count - 1;
                TickEvent currentEvent = events.Last();

                // Keep going backwards until we hit an instable flag ('-1') or the end.
                while (index > 0 && events[index - 1].PowerEventCount != -1)
                    index--;

                if (events[index].PowerEventCount > 0 && 
                    currentEvent.PowerEventCount > events[index].PowerEventCount)
                {
                    // Returns as much of an average as data as we have
                    // until we get to 15 seconds.
                    watts = (currentEvent.AccumulatedPower - events[index].AccumulatedPower) /
                        (currentEvent.PowerEventCount - events[index].PowerEventCount);
                }
            }

            return watts;
        }
    }
}
