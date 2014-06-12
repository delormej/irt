using System;
using System.Collections.Generic;
using ANT_Console.Messages;
using ANT_Console.Services;
using ANT_Managed_Library;
using System.Timers;

namespace ANT_Console
{
    class Controller
    {
        DataPoint m_data;
        AntBikePower m_eMotionPower;
        AntControl m_control;
        InteractiveConsole m_console;

        enum AntChannel : byte // limited to 8 channels per device.
        {
            EMotionPower = 0,
            RefPower,
            RefSpeed,
            AntControl
        }

        public static void Main()
        {
            Console.Title = "IRT Debug Console";

            try
            {
                Controller controller = new Controller();
                controller.Run();
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }

        public Controller()
        {
            m_data = new DataPoint();
        }

        public void Run()
        {
            ConfigureServices();
            m_console = new InteractiveConsole(m_eMotionPower, m_control);
            ConfigureReporters();
            m_console.Run();
        }

        private void ConfigureServices()
        {
            const byte emotion_tranmission_type = 0xA5;
            const byte quarq_tranmission_type = 0x5;

            // Configure reference power.
            AntBikePower refPower = new AntBikePower(
                (int)AntChannel.RefPower, 0, quarq_tranmission_type);
            refPower.StandardPowerEvent += ProcessMessage;
            refPower.TorqueEvent += ProcessMessage;
            refPower.Connected += refPower_Connected;
            refPower.Closing += refPower_Closing;

            // Configure E-Motion power reporting.
            m_eMotionPower = new AntBikePower(
                (int)AntChannel.EMotionPower, 0, emotion_tranmission_type);
            m_eMotionPower.StandardPowerEvent += ProcessMessage;
            m_eMotionPower.TorqueEvent += ProcessMessage;
            m_eMotionPower.ExtraInfoEvent += ProcessMessage;
            m_eMotionPower.ResistanceEvent += ProcessMessage;
            m_eMotionPower.Connected += m_eMotionPower_Connected;
            m_eMotionPower.Closing += m_eMotionPower_Closing;

            // Configure the remote control service.
            m_control = new AntControl((int)AntChannel.AntControl);
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

        private void ConfigureReporters()
        {
            // Temporary reporter.
            IList<IReporter> reporters = new List<IReporter>(2);
            reporters.Add(new LogReporter());
            reporters.Add(m_console);

            Timer timer = new Timer(1000);
            timer.Elapsed += (o, e) =>
            {
                foreach (var r in reporters)
                    r.Report(m_data);
            };

            timer.Start();
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
                    break;
                case AntChannel.RefPower:
                    m_data.Timestamp = m.Source.timeReceived;
                    m_data.PowerReference = m.CalculatedPower;
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
            System.Diagnostics.Debug.Write(string.Format("Received Extra_info: {0:HH:mm:ss.fff}\n", System.DateTime.Now));
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
    }
}
