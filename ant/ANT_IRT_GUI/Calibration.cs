using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

namespace IRT_GUI
{
    public static class Calibration
    {
        private static StreamWriter m_logFileWriter;
        const string report_format = "{0:g}, {1:g}, {2:g}";

        private static bool m_inCalibrationMode = false;
        private static byte m_lastCount = 0;
        private static Stopwatch m_stopwatch;

        public static bool InCalibration { get { return m_inCalibrationMode; } }

        public static void EnterCalibration()
        {
            if (!m_inCalibrationMode)
            {
                m_inCalibrationMode = true;
                m_stopwatch = new Stopwatch();
                m_stopwatch.Start();

                m_inCalibrationMode = true;

                // open up a stream to start logging
                string filename = string.Format("calib_{0}_{1:yyyyMMdd-HHmmss-F}.csv",
                    typeof(Calibration).Assembly.GetName().Version.ToString(3),
                    DateTime.Now);

                m_logFileWriter = new StreamWriter(filename);
                m_logFileWriter.AutoFlush = true;
                m_logFileWriter.WriteLine("timestamp_ms, count, ticks");
            }
        }

        public static void ExitCalibration()
        {
            if (m_inCalibrationMode)
            {
                m_inCalibrationMode = false;
            }

            if (m_logFileWriter != null)
            {
                m_logFileWriter.Flush();
                m_logFileWriter.Close();
            }

            if (m_stopwatch != null)
            {
                m_stopwatch.Stop();
                m_stopwatch = null;
            }
        }

        public static void LogCalibration(byte[] buffer)
        {
            if (!InCalibration || m_logFileWriter == null)
            {
                EnterCalibration();
            }

            // Single entrance.
            object _lock = new object();
            lock (_lock)
            {
                long ms = m_stopwatch.ElapsedMilliseconds;
                

                // If we already saw this message, skip it.
                if (m_lastCount > 0 && m_lastCount == buffer[0])
                {
                    return;
                }

                for (int i = 0; i < buffer.Length - 1; i++)
                {
                    // Each one came at 50ms intervals.
                    long timestamp = ms - (250 - (i*50));

                    if (timestamp < 0)
                        timestamp = 0;

                    m_logFileWriter.WriteLine(string.Format(report_format,
                        timestamp, buffer[0], buffer[1 + i]));
                }

                m_lastCount = buffer[0];
            }
        }
    }
}
