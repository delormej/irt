using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IRT.Calibration
{
    public class TickEvent
    {
        public TickEvent()
        { }

        public TickEvent(ushort timestamp, ushort ticks)
        {
            this.Timestamp = timestamp;
            this.Ticks = ticks;
        }

        public ushort Timestamp;
        public ushort Ticks;
        public int PowerEventCount;
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

        /// <summary>
        /// Creates 2 TickEvent objects from a calibration message.
        /// </summary>
        /// <param name="buffer"></param>
        /// <returns></returns>
        public static TickEvent[] FromBuffer(byte[] buffer)
        {
            TickEvent[] ticks = new TickEvent[2];
            /*
                tx_buffer[0] = 		ANT_BP_PAGE_CALIBRATION;
                tx_buffer[1] = 		ANT_BP_CAL_PARAM_RESPONSE;
                tx_buffer[2] = 		LOW_BYTE(time_2048);
                tx_buffer[3] = 		HIGH_BYTE(time_2048);
                tx_buffer[4] = 		LOW_BYTE(flywheel_ticks[0]);
                tx_buffer[5] = 		HIGH_BYTE(flywheel_ticks[0]);
                tx_buffer[6] = 		LOW_BYTE(flywheel_ticks[1]);
                tx_buffer[7] = 		HIGH_BYTE(flywheel_ticks[1]);
            */
            ushort time0 = (ushort)(buffer[0] | buffer[1] << 8);
            ushort ticks0 = (ushort)(buffer[2] | buffer[3] << 8);
            ushort ticks1 = (ushort)(buffer[4] | buffer[5] << 8);

            int i = 0;

            //
            // 2 events are embedded in each buffer message.
            //
            while (i < 2) // do this loop 2 times
            {
                // Alternate between the two tick counts.
                if (i % 2 == 0)
                {
                    ticks[0] = new TickEvent(ticks0, time0);
                }
                else
                {
                    // Calculate 125ms in 1/2048s later for the 2nd read.
                    ushort time1 = (ushort)(time0 + (0.125f * 2048));
                    ticks[1] = new TickEvent(ticks1, time1);
                }
                i++;
            }

            return ticks;
        }
    }
}
