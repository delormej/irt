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

        private Queue<ScriptSegment> m_queue;
        private Timer m_timer;

        // Hide constructor
        private ScriptHandler() 
        {
            m_queue = new Queue<ScriptSegment>();
            m_timer = new Timer();
            m_timer.AutoReset = false;
            m_timer.Elapsed += (o, e) => { ProcessSegment(); };
        }

        public void Start(StreamReader input)
        {
            // Parses the input file and creates a queue of segments.
            while (!input.EndOfStream)
            {
                string line = input.ReadLine();
                string[] vals = line.Split(',');

                ScriptSegment segment = new ScriptSegment();
                segment.IntervalDuration = double.Parse(vals[0]);
                segment.ServoPosition = int.Parse(vals[1]);

                m_queue.Enqueue(segment);
            }

            // Process the first item.
            if (m_queue.Count > 0)
                ProcessSegment();
        }
        
        protected virtual void ProcessSegment()
        {
            var segment = m_queue.Dequeue();

            if (segment == null)
            {
                // Raise event that we're done.
                if (ScriptComplete != null)
                    ScriptComplete(this, null);

                return;
            }

            // raise event
            SetServoEvent(segment.ServoPosition);

            // set next interval
            m_timer.Interval = segment.IntervalDuration;
            m_timer.Start();
        }
    }
}
