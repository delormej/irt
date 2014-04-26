using System;
using System.Timers;

namespace ANT_Console
{
    class Reporter
    {
        DateTime m_lastReport = DateTime.Now;

       public void Report(DataPoint data)
        {
            if (data.Timestamp > m_lastReport)
            {
                Console.WriteLine(data);
                m_lastReport = data.Timestamp;
            }
        }
    }
}
