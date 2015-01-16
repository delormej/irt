using AntPlus.Profiles.BikePower;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace IRT_GUI
{
    public class TickEvent
    {
        const string format = "{0:g}, {1:g}, {2:g}, {3:g}, {4:g}, {5:g}";

        public long TimestampMS;
        public byte Sequence;
        public ushort TickDelta;
        public ushort Watts;
        public byte PowerEventCount;
        public ushort AccumulatedPower;

        public override string ToString()
        {
            return string.Format(format, 
                TimestampMS,
                Sequence,
                TickDelta, 
                Watts,
                PowerEventCount,
                AccumulatedPower);
        }
    }

    public enum Motion
    {
        Undetermined,
        Accelerating,
        Stable,
        Decelerating
    }

    /* Compares tick count over time to the last recorded. */

    public class Stability
    {
        private ushort lastTicks = 0;
        private ushort lastPeriod = 0;
        private double lastVelocity = 0.0f;
        private int threshold = 1;  // default tick threshold per period in determing stability.

        public Stability()
        {

        }

        public Stability(int threshold)
        {
            this.threshold = threshold;
        }

        public Motion Check(ushort ticks, ushort period, out double velocity)
        {
            Motion state = Motion.Undetermined;
            velocity = 0.0f;

            int deltaPeriod = 0;
            int deltaTicks = 0;

            if (lastTicks > 0)
            {
                if (period < lastPeriod) // rollover
                {
                    deltaPeriod = (lastPeriod ^ 0xFFFF) + period;
                }
                else
                {
                    deltaPeriod = (period - lastPeriod);
                }

                if (ticks < lastTicks) // rollover
                {
                    deltaTicks = (lastTicks ^ 0xFFFF) + ticks;
                }
                else
                {
                    deltaTicks = (ticks - lastTicks);
                }

                // Period is in 1/2048 of a second.
                velocity = deltaTicks / (deltaPeriod / 2048.0f);
            }

            //
            // Allow threshold (ticks) * 2.048 (1/2048 of a second) +/- 
            // 

            if ( lastVelocity <= (velocity + (threshold * 2.048f)) &&
                lastVelocity >= (velocity - (threshold * 2.048f)))
            {
                state = Motion.Stable;
            }
            else if (velocity > lastVelocity)
            {
                state = Motion.Accelerating;
            }
            else if (velocity < lastVelocity)
            {
                state = Motion.Decelerating;
            }

            lastVelocity = velocity;
            lastTicks = ticks;
            lastPeriod = period;

            return state;
        }
    }

    // Newer version of calibration.
    public class Calibration12 : Calibration
    {
        private ushort m_startTime = 0;
        private ushort m_startTicks = 0;
        private byte m_stableCount = 0;
        private Motion m_lastState = Motion.Undetermined;
        private Stability m_stable = new Stability();

        public Calibration12()
        {
            m_tickEvents = new List<TickEvent>();
        }

        public override void LogCalibration(byte[] buffer)
        {
            /*
            tx_buffer[2] = LOW_BYTE(time_2048);
            tx_buffer[3] = HIGH_BYTE(time_2048);
            tx_buffer[4] = LOW_BYTE(flywheel_ticks[0]);
            tx_buffer[5] = HIGH_BYTE(flywheel_ticks[0]);
            tx_buffer[6] = LOW_BYTE(flywheel_ticks[1]);
            tx_buffer[7] = HIGH_BYTE(flywheel_ticks[1]);
             */
            ushort time = (ushort)(buffer[0] | buffer[1] << 8);
            ushort ticks0 = (ushort)(buffer[2] | buffer[3] << 8);
            ushort ticks1 = (ushort)(buffer[4] | buffer[5] << 8);
            double velocity = 0.0f;
            double coastdownSeconds = 0.0f;

            int i = 0;
            TickEvent te = null;

            while (i < 2) // do this loop 2 times
            {
                te = new TickEvent();

                if (m_refPower != null && m_refPower.StandardPowerOnly != null)
                {
                    te.Watts = m_refPower.StandardPowerOnly.InstantaneousPower;
                    te.PowerEventCount = m_refPower.StandardPowerOnly.EventCount;
                    te.AccumulatedPower = m_refPower.StandardPowerOnly.AccumulatedPower;
                }

                // Alternate between the two tick counts.
                if (i % 2 == 0)
                {
                    te.TickDelta = ticks0;
                    te.TimestampMS = time;
                }
                else
                {
                    te.TickDelta = ticks1;
                    // Calculate 125ms in 1/2048s later for the 2nd read.
                    te.TimestampMS = (ushort)(time + (0.125f * 2048));
                }

                m_tickEvents.Add(te);
                i++;
            }

            if (m_stableCount++ % 2 == 0)
            {
                // Grab the first tick record and compare against the last packet.
                Motion state = m_stable.Check(ticks0, time, out velocity);

                // Velocity is in ticks per second, convert into meters per second, miles per hour.
                double mps = (velocity / 2) * 0.11176f;
                double mph = mps * 2.23694f;

                // We've started decelerating or we just got stable.
                if ((m_lastState != Motion.Decelerating && state == Motion.Decelerating) ||
                    (m_lastState != Motion.Stable && state == Motion.Stable))
                {
                    // start recording the time.
                    m_startTime = time;
                    coastdownSeconds = 0;
                }
                else
                {
                    coastdownSeconds = 0;
                }
                
                if (state == Motion.Decelerating || state == Motion.Stable)
                {
                    // Calculate the amount of time we've been in this state.
                    if (time < m_startTime) // rollover
                    {
                        coastdownSeconds = ((m_startTime ^ 0xFFFF) + time) / 2048.0f;
                    }  
                    else
                    {
                        coastdownSeconds = (time - m_startTime) / 2048.0f;
                    }
                }
                else
                {
                    coastdownSeconds = 0;
                }

                if (m_form != null)
                {
                    m_form.UpdateValues(coastdownSeconds, mph, te.Watts, state);
                }

                m_lastState = state;
            }
        }
    }

    public class Calibration : IDisposable
    {
        protected StreamWriter m_logFileWriter;
        protected CalibrationForm m_form;

        protected bool m_inCalibrationMode = false;
        protected byte m_lastCount = 0;
        protected BikePowerDisplay m_refPower;
        protected Stopwatch m_stopwatch;
        protected List<TickEvent> m_tickEvents;

        public Calibration()
        {
            m_tickEvents = new List<TickEvent>();
            m_stopwatch = new Stopwatch();

            m_inCalibrationMode = true;
            m_stopwatch.Start();

            m_inCalibrationMode = true;
        }

        public void ShowCalibration(BikePowerDisplay refPower)
        {
            m_form = new CalibrationForm();
            m_refPower = refPower;
            m_form.Show();
        }

        private void CloseForm()
        {
            Action a = () =>
            {
                m_form.Close();
            };

            m_form.BeginInvoke(a);
        }

        public void ExitCalibration()
        {
            // open up a stream to start logging
            string filename = string.Format("calib_{0}_{1:yyyyMMdd-HHmmss-F}.csv",
                typeof(Calibration).Assembly.GetName().Version.ToString(3),
                DateTime.Now);

            using (m_logFileWriter = new StreamWriter(filename))
            {
                m_logFileWriter.WriteLine("timestamp_ms, count, ticks, watts, pwr_events, accum_pwr");

                foreach (var tick in m_tickEvents)
                {
                    m_logFileWriter.WriteLine(tick);
                }

                m_logFileWriter.Flush();
                m_logFileWriter.Close();
            }

            if (m_stopwatch != null)
            {
                m_stopwatch.Stop();
                m_stopwatch = null;
            }

            CloseForm();
        }

        public virtual void LogCalibration(byte[] buffer)
        {
            long ms = 0;
            TickEvent tick = null;
            byte pwrEventCount = 0;
            ushort watts = 0;

            // Single entrance.
            lock (this)
            {
                ms = m_stopwatch.ElapsedMilliseconds;

                if (m_refPower != null && m_refPower.StandardPowerOnly != null)
                {
                    watts = m_refPower.StandardPowerOnly.InstantaneousPower;
                    pwrEventCount = m_refPower.StandardPowerOnly.EventCount;
                }

                // If we already saw this message, skip it.
                if (m_lastCount > 0 && m_lastCount == buffer[0])
                {
                    return;
                }

                for (int i = 0; i < buffer.Length - 1; i++)
                {
                    // Each one came at 50ms intervals.
                    long timestamp = ms - (200 - (i * 50));

                    if (timestamp < 0)
                        timestamp = 0;

                    tick = new TickEvent()
                    {
                        TimestampMS = timestamp,
                        Sequence = buffer[0],
                        TickDelta = buffer[1 + i],
                        Watts = watts,
                        PowerEventCount = pwrEventCount
                    };

                    m_tickEvents.Add(tick);
                    //m_logFileWriter.WriteLine(tick);
                }

                // Byte 0 has the event count, store it.
                m_lastCount = buffer[0];
            }

            if (m_form != null)
            {
                double mph = CalculateSpeed(m_tickEvents);

                Action a = () =>
                {
                    m_form.lblStable.Visible = false;
                    m_form.lblSeconds.Text = string.Format("{0:0.0}", ms / 1000.0f);
                    m_form.lblSpeed.Text = string.Format("{0:0.0}", mph);
                    m_form.lblRefPower.Text = watts.ToString();
                };

                m_form.BeginInvoke(a);
            }
        }

        public void Dispose()
        {
            ExitCalibration();
        }

        private double CalculateSpeed(List<TickEvent> events)
        {
            byte tickDelta;
            long ms;

            // 1/2 second Average 
            if (events.Count < 5)
                return 0.0;

            var last20 = events.Skip(Math.Max(0, events.Count() - 5)).Take(5);
            var sum = last20.Sum(e => e.TickDelta);

            // Take into consideration that the first data point assumes it was read after 50ms of reading.
            ms = (last20.Last().TimestampMS - events[events.Count() - 5].TimestampMS) + 50;

            // double mph = (tickDelta * 20 * 0.11176 / 2) * 2.23694;
            double distance_M = (sum / 2.0f) * 0.11176f;
            double mps = distance_M / (ms / 1000.0f);

            double mph = mps * 2.23694f;

            return mph;
        }

        public static int EncodeFloat(float value)
        {
            int i = 0;
            bool sign;
            float fractional;
            float intpart;
            int binvalue;
            int exponent;

            //sign = *((uint32_t*)&value) >> 31;
            sign = value < 0.0f;

            // Strip the sign if it's negative.
            if (sign)
            {
                value = value * -1;
            }

            // Determine the exponent size required.
            for (i = 0; i < 24; i++) // max bits to use are 23
            {
                exponent = (int)Math.Pow(2, i);

                // Just get the fractional portion of a number
                fractional = modff(value * exponent, &intpart);

                // Keep going until fraction is 0 or we used all the bits.
                if (fractional == 0.0f)
                    break;
            }

            binvalue = (sign << 31) | (127 + i << 23) | (int32_t)intpart;

            memcpy(&p_buffer[2], &binvalue, sizeof(uint32_t));

            printf("f2b::sign is: %i, exponent is: %i, binvalue: %i, intpart:%i\r\n",
                sign, i, binvalue, (int32_t)intpart);

            /*
                modf stores the integer part in *integer-part, and returns the 
                fractional part. For example, modf (2.5, &intpart) returns 0.5 and 
                stores 2.0 into intpart.*/
        }

    }
}
