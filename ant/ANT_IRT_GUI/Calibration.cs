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
        const string format = "{0:g}, {1:g}, {2:g}";

        public long TimestampMS;
        public byte Sequence;
        public byte TickDelta;

        public override string ToString()
        {
            return string.Format(format, 
                TimestampMS,
                Sequence,
                TickDelta);
        }
    }

    public class Calibration : IDisposable
    {
        private StreamWriter m_logFileWriter;
        private CalibrationForm m_form;

        private bool m_inCalibrationMode = false;
        private byte m_lastCount = 0;
        private Stopwatch m_stopwatch;
        private List<TickEvent> m_tickEvents;

        public Calibration()
        {
            m_tickEvents = new List<TickEvent>();
            m_stopwatch = new Stopwatch();

            m_inCalibrationMode = true;
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

        public void ShowCalibration(short watts)
        {
            m_form = new CalibrationForm();
            m_form.lblRefPower.Text = watts.ToString();
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

            CloseForm();
        }

        public void LogCalibration(byte[] buffer)
        {
            long ms = 0;
            TickEvent tick = null;

            // Single entrance.
            lock (this)
            {
                ms = m_stopwatch.ElapsedMilliseconds;
                
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

                    tick = new TickEvent() 
                        { TimestampMS = timestamp, Sequence = buffer[0], TickDelta = buffer[1 + i] };

                    m_tickEvents.Add(tick);
                    m_logFileWriter.WriteLine(tick);
                }

                m_lastCount = buffer[0];
            }

            if (m_form != null)
            {
                Action a = () =>
                {
                    double mph = (tick.TickDelta * 20 * 0.11176 / 2) * 2.23694;
                    m_form.lblSeconds.Text = string.Format("{0:0.0}", ms / 1000.0f);
                    m_form.lblSpeed.Text = string.Format("{0:0.0}", mph);
                };

                m_form.BeginInvoke(a);
            }
        }

        public void Dispose()
        {
            ExitCalibration();
        }
    }
}
