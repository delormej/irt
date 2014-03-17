using System;
using System.IO;

namespace ANT_Console_Demo
{

    public class Reporter : IDisposable
    {
        Collector m_collector;
        Calculator m_calculator;
        StreamWriter m_logFileWriter;
        const string report_format = "{0:H:mm:ss.fff}, {1:N4}, {2:N4}, {3:N1}, {4:N0}, {5:N0}, {6:N0}, {7:N0}";

        public Reporter(Collector collector)
        {
            m_calculator = new Calculator();
            m_collector = collector;
            m_logFileWriter = new StreamWriter("log.csv");
            m_logFileWriter.AutoFlush = true;
        }

        public void Start()
        {
            Console.WriteLine("Starting....");
            m_logFileWriter.WriteLine("event_time, bike_speed_mps, emotion_speed_mps, emotion_speed_mph, emotion_power, quarq_power, calc_power, mag_level");
            System.Timers.Timer timer = new System.Timers.Timer(1000);
            timer.Elapsed += Reporter_Elapsed;
            timer.Start();
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
                eventData.resistance_level
                //wheelTorqueEvents != null ? wheelTorqueEvents[wheelTorqueEvents.Length - 1].CumulativeWheelRevs : 0,
                //wheelTorqueEvents != null ? wheelTorqueEvents[wheelTorqueEvents.Length-1].EventTime : 0
                );

            m_logFileWriter.WriteLine(data);

            Console.WriteLine(data);
        }

        public void Dispose()
        {
            m_logFileWriter.Flush();
            m_logFileWriter.Close();
        }
    }
}
