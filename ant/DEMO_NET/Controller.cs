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
        const string FORMAT = "{0:H:mm:ss.fff}, {1:N1}, {2:N1}, {3:N0}, {4:N0}, {5:N0}, {6:g}";

        public DateTime Timestamp;
        public float SpeedReference;
        public float SpeedEMotion;
        public ushort PowerReference;
        public ushort PowerEMotion;
        public ushort ServoPosition;
        public ushort Accelerometer_y;
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
                Accelerometer_y);
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
            // State   
            m_data = new DataPoint();
            //m_services = new Dictionary<AntChannel, AntService>();

            ConfigureServices();
            ConfigureReporter();

            InteractiveConsole();
        }

        private static void ConfigureServices()
        {
            const byte emotion_tranmission_type = 0xA5;
            const byte quarq_tranmission_type = 0x5;

            // Configure reference power.
            AntBikePower refPower = new AntBikePower(
                (int)AntChannel.RefPower, 0, quarq_tranmission_type);
            refPower.PowerReported += (o, e) =>
            {
                m_data.PowerReference = e.Watts;
            };
            //m_services.Add(AntChannel.RefPower, refPower);

            AntBikePower eMotionPower = new AntBikePower(
                (int)AntChannel.EMotionPower, 0, emotion_tranmission_type);
            eMotionPower.PowerReported += (o, e) =>
            {
                m_data.PowerEMotion = e.Watts;
            }; 

        }

        private static void ConfigureReporter()
        {
            // Create a timer, every second spit out the current values from datapoint.
            Timer timer = new Timer(1000);
            timer.Elapsed += (o, e) =>
            {
                Console.WriteLine(m_data);
            };
            timer.Start();
        }

        private static void InteractiveConsole()
        {
            Console.ReadKey();
        }
    }
}
