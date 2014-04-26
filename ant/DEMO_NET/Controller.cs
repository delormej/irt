using System;
using System.Collections.Generic;
using ANT_Console.Messages;
using ANT_Console.Services;
using ANT_Managed_Library;
using System.Timers;

namespace ANT_Console
{
    // Represents the datapoints being collected from various sensors.
    class DataPoint
    {
        const string FORMAT = "{0:H:mm:ss.fff}, {1:N1}, {2:N1}, {3:g}, {4:g}, {5:g}, {6:g}, {7:g}";

        public DateTime Timestamp;
        public float SpeedReference;
        public float SpeedEMotion;
        public short PowerReference;
        public short PowerEMotion;
        public ushort ServoPosition;
        public short Accelerometer_y;
        public byte Temperature;

        public override string ToString()
        {
            return string.Format(FORMAT,
                Timestamp,
                SpeedReference,
                SpeedEMotion,
                PowerReference,
                PowerEMotion,
                ServoPosition,
                Accelerometer_y,
                Temperature);
        }
    }

    class Controller
    {
        static DataPoint m_data;
        //static Dictionary<AntChannel, AntService> m_services;

        enum AntChannel : byte // limited to 8 channels per device.
        {
            EMotionPower = 0,
            RefPower,
            RefSpeed,
            AntControl
        }

        public static void Main()
        {
            try
            {
                // State   
                m_data = new DataPoint();
                //m_services = new Dictionary<AntChannel, AntService>();

                ConfigureServices();
                ConfigureReporter();

                InteractiveConsole();
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }

        private static void ConfigureServices()
        {
            const byte emotion_tranmission_type = 0xA5;
            const byte quarq_tranmission_type = 0x5;

            // Configure reference power.
            AntBikePower refPower = new AntBikePower(
                (int)AntChannel.RefPower, 0, quarq_tranmission_type);
            refPower.StandardPowerEvent += ProcessMessage;
            refPower.TorqueEvent += ProcessMessage;

            // Configure E-Motion power reporting.
            AntBikePower eMotionPower = new AntBikePower(
                (int)AntChannel.EMotionPower, 0, emotion_tranmission_type);
            eMotionPower.StandardPowerEvent += ProcessMessage;
            eMotionPower.TorqueEvent += ProcessMessage;
            eMotionPower.ExtraInfoEvent += ProcessMessage;
            eMotionPower.ResistanceEvent += ProcessMessage;
        }

        private static void ConfigureReporter()
        {
            Reporter reporter = new Reporter();

            Timer timer = new Timer(1000);
            timer.Elapsed += (o, e) =>
            {
                reporter.Report(m_data);
            };

            timer.Start();
        }

        private static void ProcessMessage(StandardPowerMessage m)
        {
            switch ((AntChannel)m.Source.antChannel)
            {
                case AntChannel.EMotionPower:
                     //m_data.Timestamp = m.Source.timeReceived;
                     //m_data.PowerEMotion = m.Watts;
                     break;
                case AntChannel.RefPower:
                     m_data.PowerReference = m.Watts;
                     break;
                default:
                     break;
            }
        }

        private static void ProcessMessage(TorqueMessage m)
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
        private static void ProcessMessage(ExtraInfoMessage m)
        {
            m_data.Timestamp = m.Source.timeReceived;
            m_data.ServoPosition = m.ServoPosition;
            m_data.Temperature = m.Temperature;
            m_data.Accelerometer_y = m.Accelerometer_y;
        }

        /// <summary>
        /// Wahoo messages for resistance control, plus additional messages added
        /// by IRT and Trainer Road.
        /// </summary>
        /// <param name="m"></param>
        private static void ProcessMessage(ResistanceMessage m)
        {
            //
        }

        private static void InteractiveConsole()
        {
            Console.ReadKey();
        }
    }
}
