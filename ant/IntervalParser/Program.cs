using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Timers;

namespace IntervalParser
{
    public enum ResistanceType : byte
    {
        Erg = 0x42,
        Level = 0x41,
        Position = 0x5B
    }

    // Linked list of steps.
    public class ResistanceStep
    {
        public ResistanceStep() {}

        public ResistanceType Type = ResistanceType.Erg;
        
        public int Watts { get; set; }
        
        public string Comments { get; set; }
        
        public float ElapsedStart 
        {
            get
            {
                if (Previous != null)
                {
                    return Previous.ElapsedEnd; 
                }
                else
                {
                    return 0;
                }
            }
        }

        public float ElapsedEnd
        {
            get
            {
                return ElapsedStart + Duration;
            }
        }

        public float Duration { get; set; }

        public ResistanceStep Previous { get; set; }

        public static ResistanceStep Create(ResistanceStep last, float durationMin)
        {
            ResistanceStep next = new ResistanceStep();
            next.Duration = durationMin;
            next.Previous = last;
            
            return next;
        }
    }

    public class ErgMode
    {
        private Timer m_timer;
        private int m_stepIndex;
        private float m_seconds; 
        private ResistanceStep[] m_steps;

        public event Action<ResistanceStep> ResistanceStepChange;
        public event Action Finished;

        public ResistanceStep[] Steps { get { return m_steps; } }
        public float Seconds { get { return m_seconds; } }

        public ErgMode()
        {
        }

        public ErgMode(ResistanceStep[] steps)
        {
            m_steps = steps;
        }
        
        public void Start()
        {
            m_stepIndex = 0;
            m_seconds = 0;

            // Kick off the first step.
            if (m_steps != null && m_steps.Length > 0)
            {
                if (ResistanceStepChange != null)
                {
                    ResistanceStepChange(m_steps[m_stepIndex]);
                }
            }
            else
            {
                return;
            }

            m_timer = new Timer(1000);
            m_timer.Elapsed += m_timer_Elapsed;
            m_timer.Start();
        }

        void m_timer_Elapsed(object sender, ElapsedEventArgs e)
        {
            // Increment each second.
            m_seconds += ((float)m_timer.Interval / 1000.0f);
            int index = m_stepIndex;

            while (index < m_steps.Length && 
                (m_steps[index].ElapsedEnd /* 60*/) <= m_seconds)
            {
                index++;
            }

            if (index >= m_steps.Length)
            {
                if (Finished != null)
                {
                    Finished();
                }
            }
            else if (index > m_stepIndex)
            {
                m_stepIndex = index;

                if (ResistanceStepChange != null)
                {
                    ResistanceStepChange(m_steps[m_stepIndex]);
                }
            }
        }

        public static ErgMode FromFile(string filename)
        {
            ResistanceStep[] entries = Parser.ReadInput(filename).ToArray();

            ErgMode simulator = new ErgMode(entries);
            return simulator;
        }
    }

    public class Parser
    {
        private Parser() { }

        public static List<ResistanceStep> ReadInput(string filename)
        {
            var list = new List<ResistanceStep>();

            // read input file into array
            using (StreamReader reader = File.OpenText(filename))
            {
                while (!reader.EndOfStream)
                {
                    string line = reader.ReadLine();
                    string[] vals = line.Split(',');
                    float durationMin = 0.0f;
                    float.TryParse(vals[0], out durationMin);

                    var entry = ResistanceStep.Create(list.LastOrDefault(), durationMin);
                    int watts;
                    int.TryParse(vals[1], out watts);
                    entry.Watts = watts;
                    if (vals.Length > 2 && !string.IsNullOrEmpty(vals[2]))
                        entry.Comments = vals[2];

                    list.Add(entry);
                }
            }

            return list;
        }

        public static void WriteOuput(string filename, int ftp, List<ResistanceStep> list)
        {
            const string COURSE_HEADER = "[COURSE HEADER]\r\n" +
                "VERSION=2\r\n" +
                "UNITS=ENGLISH\r\n" +
                "DESCRIPTION={0}\r\n" +
                "FILE NAME={1}\r\n" +
                "FTP={2}\r\n" +
                "MINUTES\tWATTS\r\n" +
                "[END COURSE HEADER]\r\n" +
                "[COURSE DATA]\r\n";
            const string COURSE_LINE = "{0:f2}\t{1:N0}\r\n";   
            const string COURSE_TEXT = "{0:d}\t{1}\t{2:d}\r\n"; // Start time (seconds), Text, Duration (seconds)
            const string COURSE_TEXT_START = "[END COURSE DATA]\r\n[COURSE TEXT]\r\n";
            const string COURSE_TEXT_END = "[END COURSE TEXT]";
            const int DEFAULT_TEXT_DURATION = 15;

            StringBuilder formatted = new StringBuilder();
            formatted.AppendFormat(COURSE_HEADER, filename, filename, ftp);

            foreach (var item in list)
            {
                formatted.AppendFormat(COURSE_LINE,
                    item.ElapsedStart,
                    item.Watts);
                formatted.AppendFormat(COURSE_LINE,
                    item.ElapsedEnd,
                    item.Watts);
            }

            formatted.Append(COURSE_TEXT_START);

            foreach (var item in list)
            {
                if (item.Comments == null)
                    continue;

                formatted.AppendFormat(COURSE_TEXT,
                    (int)(item.ElapsedStart * 60), // Convert to seconds
                    item.Comments,
                    DEFAULT_TEXT_DURATION);
            }

            formatted.Append(COURSE_TEXT_END);

            File.WriteAllText(filename, formatted.ToString());
        }

        public static void Parse(string inputFilename = null, string outputFilename = null)
        {
            if (string.IsNullOrEmpty(inputFilename))
                inputFilename = "Source.txt";
            if (string.IsNullOrEmpty(outputFilename))
                outputFilename = string.Format("{0:yyyyMMdd-HHmmss-F}.erg",
                    DateTime.Now);
            
            var list = ReadInput(inputFilename);
            WriteOuput(outputFilename, 300, list);

            Console.Write("Parsed interval file {0} and output file {1}.",
                inputFilename,
                outputFilename);
        }
    }

    public class Program
    {
        private static bool m_running = false;
        
        public static void Main()
        {
            m_running = true;
            ErgMode erg = ErgMode.FromFile("source.txt");
            erg.ResistanceStepChange += erg_ResistanceChanged;
            erg.Finished += erg_Finished;
            erg.Start();

            // Loop unti finished.
            while (m_running)
                System.Threading.Thread.Sleep(10);
        }

        static void erg_Finished()
        {
            m_running = false;   
        }

        static void erg_ResistanceChanged(ResistanceStep step)
        {
            Console.WriteLine("{0} @ {1} watts for {2} minutes.",
                step.Comments, step.Watts, step.ElapsedEnd - step.ElapsedStart);
        }
    }
}
