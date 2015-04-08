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
            
            // Get max records, plus the last ones if not divisible by 4.
            int records = timestamp.Length; //  (int)Math.Ceiling(timestamp.Length / 4.0d);

            double[] seconds = new double[records];
            double[] speed = new double[records];

            for (int ix = 0; ix < records; ix++)
            {
                int idx = ix; //  (ix * 4);

                if (idx == 0)
                {
                    // Advance to the next record to start.
                    idx++; //= 4;
                }
                // If we overrun, cap at max.
                else if (idx > flywheel.Length)
                {
                    idx = flywheel.Length - 1;
                }

                int val = flywheel[idx];
                int dt, ds;

                // Handle rollover
                if (val < flywheel[idx - 1])
                    dt = val + (flywheel[idx - 1] ^ 0xFFFF);
                else
                    dt = val - flywheel[idx - 1];

                if (timestamp[idx] < timestamp[idx - 1])
                    ds = timestamp[idx] + (timestamp[idx - 1] ^ 0xFFFF);
                else
                    ds = timestamp[idx] - timestamp[idx - 1];

                if (ix > 0)
                    seconds[ix] = (ds / 2048.0) + seconds[ix - 1];
                else
                    seconds[ix] = (ds / 2048.0);

                /* Each flywheel revolution equals 0.115 meters travelled by the 
                 * bike.  Two ticks are recorded for each revolution.
                 * Time is sent in 1/2048 of a second.
                 */
                speed[ix] = (dt * 0.115 / 2.0) / (ds / 2048.0);
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

            if (m_coastdownSeconds.Count < 10)
            {
                throw new ApplicationException("Unable to detect suitable deceleration, please retry.");
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
                System.Diagnostics.Debug.WriteLine("{0}: {1:0.0} mps", i, speeds[i]);

                // At least 15mph and faster than the previous?
                if (speeds[i] > 15 * 0.44704 &&
                    speeds[i] >= speeds[i - 1])
                {
                    // We've found the start.
                    return i;
                }
            }

            // We shouldn't really get here.
            return 0;
        }

        private int FindMinSpeedIndex(double[] speeds)
        {
            // Always returns the end of the file.
            return speeds.Length-1;
        }
    }
}
