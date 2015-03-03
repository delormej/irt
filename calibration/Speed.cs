using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IRT.Calibration
{
    class Speed
    {
        private List<CalibrationEvent> m_ticks;
        private double m_lastSpeed = 0;
        
        // 1/2 mile per hour in meters per second +/-.
        readonly double StabilityThresholdMps = 0; 

        public Speed()
        {
            m_ticks = new List<CalibrationEvent>();
        }

        public Speed(double ThresholdMps) : base()
        {
            this.StabilityThresholdMps = ThresholdMps;
        }

        public void AddEvent(byte[] buffer, out double speed, out double seconds, out Motion stability)
        {
            // Event offset skip, every 4th event.
            int SkipOffset = 4; 

            AddEventFromBuffer(buffer);
            
            ushort lastTicks = 0, lastTimestamp = 0;

            if (m_ticks.Count > SkipOffset)
            {
                lastTicks = m_ticks[m_ticks.Count - SkipOffset].Ticks;
                lastTimestamp = m_ticks[m_ticks.Count - SkipOffset].Timestamp;
            }
            else
            {
                // Need to accumulate a couple of events first.
                speed = 0;
                seconds = 0;
                stability = Motion.Undetermined;

                return;
            }

            // Grab the latest event.
            ushort currentTicks = m_ticks[m_ticks.Count - 1].Ticks;
            ushort currentTimestamp = m_ticks[m_ticks.Count - 1].Timestamp;

            int dt, ds; // delta ticks, delta seconds

            if (currentTicks < lastTicks)
                dt = currentTicks + (lastTicks ^ 0xFFFF);
            else
                dt = currentTicks - lastTicks;

            if (lastTimestamp > 0)
            {
                if (currentTimestamp < lastTimestamp)
                    ds = currentTimestamp + (lastTimestamp ^ 0xFFFF);
                else
                    ds = currentTimestamp - lastTimestamp;
            }
            else
            {
                ds = 0;
            }

            /* Each flywheel revolution equals 0.1115 meters travelled by the 
                * bike.  Two ticks are recorded for each revolution.
                * Time is sent in 1/2048 of a second.
                */
            seconds = (ds / 2048.0);
            speed = (dt * 0.1115 / 2.0) / seconds;
            stability = Stability(speed);
        }

        private void AddEventFromBuffer(byte[] buffer)
        {
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
                    m_ticks.Add(new CalibrationEvent(ticks0, time0));
                }
                else
                {
                    // Calculate 125ms in 1/2048s later for the 2nd read.
                    ushort time1 = (ushort)(time0 + (0.125f * 2048));
                    m_ticks.Add(new CalibrationEvent(ticks1, time1));
                }
                i++;
            }
        }

        private Motion Stability(double currentSpeed)
        {
            Motion state = Motion.Undetermined;
            double lowerBounds, upperBounds;

            if (m_lastSpeed != 0)
            {
                lowerBounds = (m_lastSpeed - StabilityThresholdMps);
                upperBounds = (m_lastSpeed + StabilityThresholdMps);

                //
                // Determine stability.
                //
                if (currentSpeed <= upperBounds &&
                    currentSpeed >= lowerBounds)
                {
                    state = Motion.Stable;
                }
                else if (currentSpeed > upperBounds)
                {
                    state = Motion.Accelerating;
                }
                else if (currentSpeed < lowerBounds)
                {
                    state = Motion.Decelerating;
                }
            }

            m_lastSpeed = currentSpeed;
            return state;
        }
    }

    internal class CalibrationEvent
    {
        public CalibrationEvent(ushort timestamp, ushort ticks)
        {
            this.Timestamp = timestamp;
            this.Ticks = ticks;
        }

        public ushort Timestamp;
        public ushort Ticks;
    }
}
