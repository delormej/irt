using System;
using System.Collections.Generic;
using System.Linq;

namespace IRT.Calibration
{
    /// <summary>
    /// Filters and processes raw data into usable speed/coastdown seconds.
    /// </summary>
    public class CoastdownData
    {
        private List<double> m_speedMps, m_coastdownSeconds;

        public CoastdownData()
        {}

        public double[] SpeedMps { get {  return m_speedMps.ToArray(); } }

        public double[] CoastdownSeconds { get { return m_coastdownSeconds.ToArray();  } }

        /// <summary>
        /// Filters and processes raw coastdown records into only the records that
        /// represent the deceleration phase.
        /// </summary>
        public void Evaluate(TickEvent[] tickEvents)
        {
            int[] timestamp, flywheel;

            // Create timestamp / flywheel records from tickEvents.
            timestamp = new int[tickEvents.Length];
            flywheel = new int[tickEvents.Length];

            for (int i = 0; i < tickEvents.Length; i++)
            {
                timestamp[i] = tickEvents[i].Timestamp;
                flywheel[i] = tickEvents[i].Ticks;
            }
            
            int records = timestamp.Length / 4;

            if (timestamp.Length % 4 != 0)
            {
                // If there was a remainder, add 1.
                records += 1;
            }

            double[] seconds = new double[records];
            double[] speed = new double[records];

            int ix = 0;

            for (int idx = 0; idx < flywheel.Length; idx++)
            {
                int val = flywheel[idx];
                int dt, ds;

                // Evaluate every 4th record AND the last record.
                if (idx > 0 && idx % 4 == 0 || idx == flywheel.Length - 1)
                {
                    if (val < flywheel[idx - 4])
                        dt = val + (flywheel[idx - 4] ^ 0xFFFF);
                    else
                        dt = val - flywheel[idx - 4];

                    if (timestamp[idx] < timestamp[idx - 4])
                        ds = timestamp[idx] + (timestamp[idx - 4] ^ 0xFFFF);
                    else
                        ds = timestamp[idx] - timestamp[idx - 4];

                    if (ix > 0)
                        seconds[ix] = (ds / 2048.0) + seconds[ix - 1];
                    else
                        seconds[ix] = (ds / 2048.0);

                    /* Each flywheel revolution equals 0.1115 meters travelled by the 
                     * bike.  Two ticks are recorded for each revolution.
                     * Time is sent in 1/2048 of a second.
                     */
                    speed[ix++] = (dt * 0.1115 / 2.0) / (ds / 2048.0);
                }
            }      

            int maxSpeedIdx = FindDecelerationIndex(speed);
            int minSpeedIdx = FindMinSpeedIndex(speed);
            //int len = minSpeedIdx - maxSpeedIdx;

            // Copy to the array.
            if (m_speedMps == null)
            {
                m_speedMps = new List<double>();
                m_coastdownSeconds = new List<double>();
            }

            //Array.Copy(speed, maxSpeedIdx, SpeedMps, 0, len);
            for (int j = maxSpeedIdx; j <= minSpeedIdx; j++)
            {
                m_speedMps.Add(speed[j]);
                // Invert the timestamp seconds to record seconds to min speed.
                m_coastdownSeconds.Add(seconds[minSpeedIdx] - seconds[j]);
            }
        }

        /// <summary>
        /// Finds the point at which deceleration begins.
        /// </summary>
        /// <param name="values"></param>
        /// <returns></returns>
        private int FindDecelerationIndex(double[] speeds)
        {
            // Progress from the end of the array to the beginning.
            // Ensure that the values continue to increase.
            for (int i = speeds.Length - 1; i > 0; i--)
            {
                if (speeds[i] >= speeds[i - 1])
                {
                    return i + 1;
                }
            }

            // We shouldn't really get here.
            return 0;
        }

        private int FindMinSpeedIndex(double[] speeds)
        {
            // Get the first occurrence of 1 tick delta.
            // Array.IndexOf<double>(speeds, 1.0);

            return speeds.Length-1;
        }
    }
}
