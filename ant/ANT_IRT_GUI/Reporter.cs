using BikeSignalProcessing;
using IRT_GUI.IrtMessages;
using System;
using System.IO;

namespace ANT_Console
{
    public interface IReporter
    {
        void Report(DataPoint data);
        void Report(string message);
    }

    public class ReportHelper
    {
        public static readonly string Header = "Time         |  mph  | Watts | Watts2| Servo  | Target | Flywheel\r\n";

        public static string Format(DataPoint data)
        {
            const string format = "{0:H:mm:ss.fff} | {1,5:N1} | {2,5:N0} | {3,5:N0} | {4,6:N0} | {5,4}:{6} | {7}\r\n";
            string value = string.Format(format,
                data.Timestamp,
                //data.SpeedReference,
                data.SpeedEMotionMph,
                data.PowerEMotion,
                data.PowerReference,
                data.ServoPosition,
                data.ResistanceMode == 0x41 ? "S" : data.ResistanceMode == 0x42 ? "E" : "",
                data.TargetLevel,
                data.FlywheelRevs);

            return value;
        }
    }

    public class BikeSignalProcessingReporter : IReporter
    {
        BikeSignalProcessing.Form1 mForm;
        BikeSignalProcessing.Data mData;

        public event EventHandler Closing;

        public BikeSignalProcessingReporter()
        {
            mData = new Data();
            BikeSignalProcessing.Form1 mForm = new Form1(mData);
            mForm.FormClosing += MForm_FormClosing;
            mForm.Show();
        }

        private void MForm_FormClosing(object sender, System.Windows.Forms.FormClosingEventArgs e)
        {
            if (this.Closing != null)
            {
                Closing(this, EventArgs.Empty);
            }
        }

        public void Report(string message)
        {
            return;
        }

        public void Report(DataPoint data)
        {
            mData.Update(data.SpeedEMotionMph, data.PowerReference, data.ServoPosition);
        }
    }

    public class LogReporter : IReporter, IDisposable
    {
        StreamWriter m_logFileWriter;
        const string report_format = "{0:H:mm:ss.fff}, {1:N4}, {2:N4}, {3:N1}, {4:g}, {5:g}, {6:g}, {7:g}, {8:g}, {9:g}, {10:g}, {11:g}, {12:g}";

        public LogReporter()
        {
            string filename = string.Format("irt_{0}_{1:yyyyMMdd-HHmmss-F}.csv",
                typeof(LogReporter).Assembly.GetName().Version.ToString(3),
                DateTime.Now);

            m_logFileWriter = new StreamWriter(filename);
            m_logFileWriter.AutoFlush = true;
            m_logFileWriter.WriteLine("event_time, ref_speed_mph, emotion_speed_mps, emotion_speed_mph, emotion_power, quarq_power, calc_power, servo_pos, flywheel, temperature, ref_torque, ref_wheel_event, ref_torque");
        }

        public void Report(DataPoint data)
        {
            string message = String.Format(report_format,
                data.Timestamp,
                data.SpeedReference,
                data.SpeedMPSEMotion,
                data.SpeedEMotionMph,
                data.PowerEMotion,
                data.PowerReference,
                data.PowerEMotion,
                data.ServoPosition,
                data.FlywheelRevs,
                data.Temperature,
                data.RefPowerAccumTorque,
                data.RefPowerWheelEvents,
                data.RefTorque
                //wheelTorqueEvents != null ? wheelTorqueEvents[wheelTorqueEvents.Length - 1].CumulativeWheelRevs : 0,
                //wheelTorqueEvents != null ? wheelTorqueEvents[wheelTorqueEvents.Length-1].EventTime : 0
                );

            m_logFileWriter.WriteLine(message);
        }

        public void Report(string message)
        {
            m_logFileWriter.Write(message);
        }

        public void Dispose()
        {
            m_logFileWriter.Flush();
            m_logFileWriter.Close();
        }
    }
}
