using System;
using System.IO;
using ANT_Console.Messages;

namespace ANT_Console
{
    public interface IReporter
    {
        void Report(DataPoint data);
    }

    public class LogReporter : IReporter, IDisposable
    {
        StreamWriter m_logFileWriter;
        System.Timers.Timer m_timer;
        const string report_format = "{0:H:mm:ss.fff}, {1:N4}, {2:N4}, {3:N1}, {4:g}, {5:g}, {6:g}, {7:g}, {8:g}, {9:g}, {10:g}, {11:g}";

        public LogReporter()
        {
            string filename = string.Format("irt_{0}_{1:yyyyMMdd-HHmmss-F}.csv",
                typeof(LogReporter).Assembly.GetName().Version.ToString(3),
                DateTime.Now);

            m_logFileWriter = new StreamWriter(filename);
            m_logFileWriter.AutoFlush = true;
            m_logFileWriter.WriteLine("event_time, ref_speed_mph, emotion_speed_mps, emotion_speed_mph, emotion_power, quarq_power, calc_power, servo_pos, flywheel, temperature, ref_torque, ref_wheel_rev");
        }

        public void Report(DataPoint data)
        {
            string message = String.Format(report_format,
                data.Timestamp,
                data.SpeedReference,
                data.SpeedMPSEMotion,
                data.SpeedEMotion,
                data.PowerEMotion,
                data.PowerReference,
                data.PowerEMotion,
                data.ServoPosition,
                data.FlywheelRevs,
                data.Temperature,
                data.RefPowerAccumTorque,
                data.RefSpeedWheelRevs
                //wheelTorqueEvents != null ? wheelTorqueEvents[wheelTorqueEvents.Length - 1].CumulativeWheelRevs : 0,
                //wheelTorqueEvents != null ? wheelTorqueEvents[wheelTorqueEvents.Length-1].EventTime : 0
                );

            m_logFileWriter.WriteLine(message);
        }

        public void Dispose()
        {
            m_timer.Stop();
            m_logFileWriter.Flush();
            m_logFileWriter.Close();
        }
    }
}
