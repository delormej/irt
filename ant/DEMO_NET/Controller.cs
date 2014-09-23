using System;
using System.Collections.Generic;
using ANT_Console.Messages;
using ANT_Console.Services;
using ANT_Managed_Library;
using System.Timers;

namespace ANT_Console
{
    public class Controller
    {
        ushort m_eMotionDeviceId = 0;
        DataPoint m_data;
        SummaryInfo m_summary;
        AntControl m_control;
        AntBikePower m_eMotionPower;
        AntBikeSpeed m_refSpeed;
        AntBikePower m_refPower;
        IList<IReporter> m_reporters;
        CalibrationSpeed m_last_calibration;
        Timer m_ReportingTimer;

        enum AntChannel : byte // limited to 8 channels per device.
        {
            EMotionPower = 0,
            RefPower,
            RefSpeed,
            AntControl
        }

        public Controller()
        {
            m_data = new DataPoint();
            m_summary = new SummaryInfo();
            m_reporters = new List<IReporter>();
        }

        public void Shutdown()
        {
            m_ReportingTimer.Stop();
            // Report out summary.
            Report(m_summary);
        }

        public IList<IReporter> Reporters
        {
            get { return m_reporters;  }
        }

        public AntBikePower EMotionBikePower
        {
            get { return m_eMotionPower; }
        }
            
        public AntControl AntRemoteControl
        {
            get { return m_control; }
        }

        public AntBikeSpeed RefBikeSpeed
        {
            get { return m_refSpeed; }
        }

        public AntBikePower RefBikePower
        {
            get { return RefBikePower;  }
        }

        private void GetSummaryInfo()
        {
            // Try until we can connect to E-Motion Rollers.
            while (m_eMotionDeviceId == 0)
            {
                Console.WriteLine("Searching for E-Motion rollers...");
                System.Threading.Thread.Sleep(1000);
                m_eMotionDeviceId = m_eMotionPower.GetDeviceNumber();
            }

            Console.WriteLine("E-Motion Device: {0}", m_eMotionDeviceId);
            Console.WriteLine("Reference Power: {0}", m_refPower.GetDeviceNumber());
            Console.WriteLine("Reference Speed: {0}", m_refSpeed.GetDeviceNumber());

            m_summary.EmotionDeviceId = m_eMotionDeviceId;

            // Set a timer for 5 minutes from now to collect product and manufacturer data which should have
            // Arrived.
            Timer t = new Timer(5 * 60 * 1000);
            t.AutoReset = true;
            t.Elapsed += (o, e) => 
            {
                if (m_summary.Crr != 0 && m_summary.Settings != 0 && m_summary.TotalWeight != 0)
                {
                    // Disable if we already have all the info.
                    t.Enabled = false;
                    return;
                }

                m_summary.EmotionFirmwareVersion = m_eMotionPower.FirmwareVersion;

                if (m_eMotionPower.Product != null)
                {
                    m_summary.EmotionSerialNo = m_eMotionPower.Product.SerialNumber;
                }

                if (m_eMotionPower.Manufacturer != null)
                {
                    m_summary.EmotionModel = m_eMotionPower.Manufacturer.Model;
                }

                if (m_refPower.Manufacturer != null)
                {
                    m_summary.RefPowerManfId = m_refPower.Manufacturer.Manufacturer;
                    m_summary.RefPowerModel = m_refPower.Manufacturer.Model;
                    m_summary.EmotionHWRev = m_refPower.Manufacturer.HardwareRevision;
                }

                // Request these settings.
                m_refPower.RequestDeviceParameter(SubPages.Crr);
                System.Threading.Thread.Sleep(500);
                m_refPower.RequestDeviceParameter(SubPages.TotalWeight);
                System.Threading.Thread.Sleep(500);
                m_refPower.RequestDeviceParameter(SubPages.Settings);
                System.Threading.Thread.Sleep(500);

            };
            t.Start();

        }


        public void ConfigureServices(ushort deviceId = 0)
        {
            const byte emotion_transmission_type = 0xA5;
            const byte quarq_transmission_type = 0x5;

            // Open a background scanning channel for devices (speed, power, etc...)

            /*

            // Configure reference power.
            m_refPower = new AntBikePower(
                (int)AntChannel.RefPower, 0, quarq_transmission_type);
            m_refPower.StandardPowerEvent += ProcessMessage;
            m_refPower.TorqueEvent += ProcessMessage;
            m_refPower.Connected += refPower_Connected;
            m_refPower.Closing += refPower_Closing;

            m_refSpeed = new AntBikeSpeed(
                (int)AntChannel.RefSpeed, 0);
            m_refSpeed.SpeedEvent += ProcessMessage;
            */

            // Configure E-Motion power reporting.
            m_eMotionPower = new AntBikePower(
                (int)AntChannel.EMotionPower, deviceId, emotion_transmission_type);
            m_eMotionPower.StandardPowerEvent += ProcessMessage;
            m_eMotionPower.TorqueEvent += ProcessMessage;
            m_eMotionPower.ExtraInfoEvent += ProcessMessage;
            m_eMotionPower.ResistanceEvent += ProcessMessage;
            m_eMotionPower.GetSetParameterEvent += ProcessMessage;
            m_eMotionPower.BatteryStatusEvent += ProcessMessage;
            m_eMotionPower.Connected += m_eMotionPower_Connected;
            m_eMotionPower.MeasureOutputEvent += ProcessMessage;
            m_eMotionPower.Closing += m_eMotionPower_Closing;

            // Configure the remote control service.
            //m_control = new AntControl((int)AntChannel.AntControl);
        }

        //
        // These things never seem to get called, in here for future implementation.
        //
        void refPower_Closing(object sender, EventArgs e)
        {
            Console.WriteLine("refPower_Closing");
        }

        void refPower_Connected(object sender, EventArgs e)
        {
            Console.WriteLine("refPower_Connected");
        }

        void m_eMotionPower_Closing(object sender, EventArgs e)
        {
            Console.WriteLine("eMotionPower_Closing");
        }

        void m_eMotionPower_Connected(object sender, EventArgs e)
        {
            Console.WriteLine("eMotionPower_Connected");
        }
        //
        // End
        //

        public void ConfigureReporters()
        {
            // Temporary reporter.
            m_reporters.Add(new LogReporter());

            m_ReportingTimer = new Timer(1000);
            m_ReportingTimer.Elapsed += (o, e) =>
            {
                Report();
            };

            m_ReportingTimer.Start();
        }

        private void Report()
        {
            foreach (var r in m_reporters)
                r.Report(m_data);
        }

        private void Report(SummaryInfo data)
        {
            string message = data.ToString();

            foreach (var r in m_reporters)
                r.Report(message);
        }

        private void Report(CalibrationSpeed calibration)
        {
            DataPoint data = new DataPoint();
            data.Timestamp = DateTime.Now;
            data.FlywheelRevs = calibration.FlywheelRotations;
            
            if (m_last_calibration != null)
            {
                if (calibration.Seconds < m_last_calibration.Seconds)
                {
                    // Rollover.
                    data.RefPowerAccumTorque = (ushort)((m_last_calibration.Seconds ^ 0xFFFF) +
                            calibration.Seconds);
                }
                else 
                {
                    // Just using the torque field randomly for a place right now.
                    data.RefPowerAccumTorque = (ushort)(calibration.Seconds - m_last_calibration.Seconds);
                }
            }
            else
            {
                data.RefPowerAccumTorque = 0;
            }

            foreach (var r in m_reporters)
                r.Report(data);

            m_last_calibration = calibration;
        }

        private void ProcessMessage(StandardPowerMessage m)
        {
            switch ((AntChannel)m.Source.antChannel)
            {
                case AntChannel.EMotionPower:
                     m_data.Timestamp = m.Source.timeReceived;
                     m_data.PowerEMotion = m.Watts;
                     break;
                case AntChannel.RefPower:
                     m_data.Timestamp = m.Source.timeReceived;
                     m_data.PowerReference = m.Watts;
                     System.Diagnostics.Debug.Write(
                                     string.Format("Received Ref Power: {0:HH:mm:ss.fff}, watts: {1}\n",
                                     System.DateTime.Now, m.Watts));
                     break;
                default:
                     break;
            }
        }

        private void ProcessMessage(TorqueMessage m)
        {
            switch ((AntChannel)m.Source.antChannel)
            {
                case AntChannel.EMotionPower:
                    m_data.Timestamp = m.Source.timeReceived;
                    m_data.PowerEMotion = m.CalculatedPower;
                    m_data.SpeedEMotion = m.SpeedMph;
                    m_data.SpeedMPSEMotion = m.SpeedMps;
                    break;
                case AntChannel.RefPower:
                    m_data.Timestamp = m.Source.timeReceived;
                    m_data.RefPowerAccumTorque = m.AccumTorque;
                    //m_data.PowerReference = m.CalculatedPower;
                    System.Diagnostics.Debug.Write(
                                    string.Format("Received Ref Power: {0:HH:mm:ss.fff}, torque: {1}\n",
                                    System.DateTime.Now, m.AccumTorque));
                    break;
                default:
                    break;
            }
        }

        /// <summary>
        /// Represents messages that have additional debug information in them, 
        /// such as Servo Position, temperature and accelerometer reading.
        /// </summary>
        /// <param name="m"></param>
        private void ProcessMessage(ExtraInfoMessage m)
        {
            /*System.Diagnostics.Debug.Write(
                string.Format("Received Extra_info: {0:HH:mm:ss.fff}, Flywheel: {1}\n", 
                System.DateTime.Now, m.FlyweelRevs));*/
            m_data.Timestamp = System.DateTime.Now;
            m_data.ServoPosition = m.ServoPosition;
            m_data.Temperature = m.Temperature;
            m_data.FlywheelRevs = m.FlyweelRevs;
            m_data.TargetLevel = m.Level;
            m_data.ResistanceMode = m.Mode;
        }

        /// <summary>
        /// Wahoo messages for resistance control, plus additional messages added
        /// by IRT and Trainer Road.
        /// </summary>
        /// <param name="m"></param>
        private void ProcessMessage(ResistanceMessage m)
        {
            m_data.Timestamp = m.Source.timeReceived;
            m_data.TargetLevel = m.Level;
            m_data.ResistanceMode = m.Mode;
        }

        private void ProcessMessage(SpeedMessage m)
        {
            m_data.SpeedReference = m.SpeedMph;
            m_data.RefSpeedWheelRevs = m.WheelRevs;
        }

        /// <summary>
        /// Happens when the get/set parameters is sent from the device.
        /// </summary>
        /// <param name="m"></param>
        private void ProcessMessage(GetSetMessage m)
        {
            byte[] buffer = m.AsBytes();

            // Sub page
            switch((SubPages)m.SubPage)
            {
                case SubPages.Crr:
                    Console.WriteLine("CRR Response: Slope: {0}, Intercept: {1}",
                        Message.BigEndian(buffer[2], buffer[3]),
                        Message.BigEndian(buffer[4], buffer[5]));
                    m_summary.Crr = (UInt32)(Message.BigEndian(buffer[6], buffer[7]) << 16) |
                        Message.BigEndian(buffer[4], buffer[5]);
                    break;

                case SubPages.TotalWeight:
                    int grams = Message.BigEndian(buffer[2], buffer[3]);
                    m_summary.TotalWeight = grams;
                    Console.WriteLine("Total Weight: {0:N2} kg", grams / 100.0);
                    break;

                case SubPages.ServoOffset:
                    Console.WriteLine("Servo Offset: {0}",
                        Message.BigEndian(buffer[2], buffer[3]));
                    break;

                case SubPages.CalibrationSpeed:
                    var calibration = new CalibrationSpeed(buffer);
                    Console.WriteLine(calibration);
                    Report(calibration);
                    break;

                default:
                    Console.WriteLine("Received Parameters page: {0} - [{1:x2}][{2:x2}][{3:x2}][{4:x2}][{5:x2}][{6:x2}]", m.SubPage,
                        buffer[2],
                        buffer[3],
                        buffer[4],
                        buffer[5],
                        buffer[6],
                        buffer[7]);
                    break;
            }
        }

        private void ProcessMessage(BatteryStatusMessage m)
        {
            if (m_summary != null)
                m_summary.BatteryTime = m.OperatingTime;

            Console.WriteLine("Battery: {0:0.####} volts, Operating Time: {1:0.00} hours.", 
                m.Voltage, m.OperatingTime);
        }

        private void ProcessMessage(MeasureOutputMessage m)
        {
            Console.WriteLine("Temperature returned: {0}.", (m.Value / 1024.0));
        }
    }
}
