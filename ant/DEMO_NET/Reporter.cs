using System;
using System.IO;

namespace ANT_Console_Demo
{

    public class Reporter : IDisposable
    {
        const int REPORT_INTERVAL_MS = 1000;

        Collector m_collector;
        Calculator m_calculator;
        StreamWriter m_logFileWriter;
        System.Timers.Timer m_timer;
        const string report_format = "{0:H:mm:ss.fff}, {1:N4}, {2:N4}, {3:N1}, {4:g}, {5:g}, {6:g}, {7:g}, {8:g}, {9:g}";

        public delegate void DataEventHandler(object o, CollectorEventData data);

        public event DataEventHandler OnReport;

        public Reporter(Collector collector)
        {
            m_calculator = new Calculator();
            m_collector = collector;
            m_logFileWriter = new StreamWriter("log.csv");
            m_logFileWriter.AutoFlush = true;
            m_timer = new System.Timers.Timer(REPORT_INTERVAL_MS);
        }

        public void Start()
        {
            m_logFileWriter.WriteLine("event_time, bike_speed_mps, emotion_speed_mps, emotion_speed_mph, emotion_power, quarq_power, calc_power, servo_pos, accelerometer_y, temperature");
          
            m_timer.Elapsed += Reporter_Elapsed;
            m_timer.Start();
        }

        void Reporter_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            Report(m_collector.EventData);
        }

        public void Report(CollectorEventData eventData)
        {
            eventData.bike_speed = m_calculator.GetBikeSpeed(
                m_collector.PopBikeSpeedEvents());

            SpeedEvent[] wheelTorqueEvents = m_collector.PopTorqueSpeedEvents();
            eventData.emotion_speed = m_calculator.GetSpeed(wheelTorqueEvents);
            int calculatedWatts = m_calculator.GetPowerFromAccumTorque(wheelTorqueEvents);

            string data = String.Format(report_format,
                DateTime.Now,
                eventData.bike_speed,
                eventData.emotion_speed,
                eventData.emotion_speed * 2.23693629f,
                eventData.emotion_power,
                eventData.quarq_power,
                calculatedWatts,
                eventData.servo_position,
                eventData.accelerometer_y,
                eventData.temperature
                //wheelTorqueEvents != null ? wheelTorqueEvents[wheelTorqueEvents.Length - 1].CumulativeWheelRevs : 0,
                //wheelTorqueEvents != null ? wheelTorqueEvents[wheelTorqueEvents.Length-1].EventTime : 0
                );

            m_logFileWriter.WriteLine(data);

            if (OnReport != null)
            {
                OnReport(this, eventData);
            }
        }

        public void Dispose()
        {
            m_timer.Stop();
            m_logFileWriter.Flush();
            m_logFileWriter.Close();
        }
    }
}
