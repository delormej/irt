using System;
using System.IO;

namespace ANT_Console_Demo
{

    public class Reporter : IDisposable
    {
        Collector m_collector;
        Calculator m_calculator;
        StreamWriter m_logFileWriter;
        const string report_format = "{0:H:mm:ss.fff}, {1:N4}, {2:N4}, {3:N1}, {4:g}, {5:g}, {6:g}, {7:g}, {8:g}, {9:g}";

        public Reporter(Collector collector)
        {
            m_calculator = new Calculator();
            m_collector = collector;
            m_logFileWriter = new StreamWriter("log.csv");
            m_logFileWriter.AutoFlush = true;
        }

        public void Start()
        {
            Console.CursorVisible = false;
            
            Console.WriteLine("Starting....");
            m_logFileWriter.WriteLine("event_time, bike_speed_mps, emotion_speed_mps, emotion_speed_mph, emotion_power, quarq_power, calc_power, servo_pos, accelerometer_y, temperature");
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
                eventData.servo_position,
                eventData.accelerometer_y,
                eventData.temperature
                //wheelTorqueEvents != null ? wheelTorqueEvents[wheelTorqueEvents.Length - 1].CumulativeWheelRevs : 0,
                //wheelTorqueEvents != null ? wheelTorqueEvents[wheelTorqueEvents.Length-1].EventTime : 0
                );

            m_logFileWriter.WriteLine(data);

            // Leave 2 rows at the bottom for command.
            int lastLine = Console.CursorTop;

            // Do we need to scroll?
            if (lastLine > Console.WindowHeight - 2)
            {
                // Buffer space to scroll? 
                if ((Console.WindowTop + Console.WindowHeight) >= Console.BufferHeight-1)
                {
                    // We're out of buffer space, so flush.
                    Console.MoveBufferArea(Console.WindowLeft, 
                        Console.WindowTop, 
                        Console.WindowWidth, 
                        Console.WindowHeight,
                        Console.WindowLeft, 
                        0);

                    Console.SetWindowPosition(Console.WindowLeft, 0);
                    lastLine = Console.WindowHeight - 1;
                }
                else
                {
                    // Scroll
                    Console.SetWindowPosition(Console.WindowLeft, Console.WindowTop + 1);
                }
            }

            // Position the cursor at the bottom of the screen to write the command line.
            Console.SetCursorPosition(Console.WindowLeft, Console.WindowTop + Console.WindowHeight - 1);
            Console.Write("<enter cmd>");
            Console.SetCursorPosition(Console.WindowLeft, lastLine);
            Console.WriteLine(data);
        }

        public void Dispose()
        {
            m_logFileWriter.Flush();
            m_logFileWriter.Close();
        }
    }
}
