using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Timers;

namespace ANT_Console.Services
{
    public delegate void SetServoHandler(int ServoPosition);

    public class ScriptSegment
    {
        public double IntervalDuration;
        public int ServoPosition;
    }

    public class ScriptHandler
    {
        public event SetServoHandler SetServo;
        public event EventHandler ScriptComplete;

        // OnExecute Event for when the script is complete
        // OnEnd Event for when a segment is executed

        private List<ScriptSegment> m_queue;
        private Timer m_timer;
        private int m_index;

        // Hide constructor
        public ScriptHandler() 
        {
            m_queue = new List<ScriptSegment>();
            m_timer = new Timer();
            m_timer.AutoReset = false;
            m_timer.Elapsed += (o, e) => { ProcessSegment(); };
        }

        public void Start()
        {
            // Process the first item.
            if (m_queue.Count > 0)
            {
                m_index = 0;
                ProcessSegment();
            }
            else
            {
                throw new InvalidOperationException("No script segments to process.");
            }
        }

        public void ParseInput(StreamReader input)
        {
            // Clear existing queue if one exists.
            if (m_queue.Count > 0)
            {
                m_queue.Clear();
            }

            // Parses the input file and creates a queue of segments.
            while (!input.EndOfStream)
            {
                string line = input.ReadLine();
                string[] vals = line.Split(',');

                ScriptSegment segment = new ScriptSegment();
                segment.IntervalDuration = double.Parse(vals[0]);
                segment.ServoPosition = int.Parse(vals[1]);

                m_queue.Add(segment);
            }
        }

        public bool IsRunning { get { return (m_timer != null && m_timer.Enabled); } }

        public void TogglePause()
        {
            lock (m_timer)
            {
                if (m_timer.Enabled)
                    m_timer.Stop();
                else
                    m_timer.Start();
            }
        }

        protected virtual void ProcessSegment()
        {
            if (m_index < m_queue.Count)
            {
                var segment = m_queue[m_index++];

                // raise event
                if (SetServo != null)
                    SetServo(segment.ServoPosition);

                // set next interval
                m_timer.Interval = segment.IntervalDuration * 1000;  // convert to milliseconds
                m_timer.Start();
            }
            else
            {
                // Raise event that we're done.
                if (ScriptComplete != null)
                    ScriptComplete(this, null);
            }
        }
    }
}
