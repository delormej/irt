using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace IntervalParser
{
    class Entry
    {
        private Entry() {}

        public int Watts;
        public string Comments;
        public float ElapsedStart;
        public float ElapsedEnd;

        public static Entry Create(Entry last, float durationMin)
        {
            Entry next = new Entry();
            
            if (last != null)
                next.ElapsedStart = last.ElapsedEnd;
            else
                next.ElapsedStart = 0;
            
            next.ElapsedEnd = next.ElapsedStart + durationMin;
            
            return next;
        }
    }

    class Program
    {
        static List<Entry> ReadInput(string filename)
        {
            var list = new List<Entry>();

            // read input file into array
            using (StreamReader reader = File.OpenText(filename))
            {
                while (!reader.EndOfStream)
                {
                    string line = reader.ReadLine();
                    string[] vals = line.Split(',');
                    float durationMin = 0.0f;
                    float.TryParse(vals[0], out durationMin);

                    var entry = Entry.Create(list.LastOrDefault(), durationMin);
                    int.TryParse(vals[1], out entry.Watts);
                    if (vals.Length > 2 && !string.IsNullOrEmpty(vals[2]))
                        entry.Comments = vals[2];

                    list.Add(entry);
                }
            }

            return list;
        }

        static void WriteOuput(string filename, int ftp, List<Entry> list)
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

        static void Main(string[] args)
        {
            string outputFilename = string.Format("{0:yyyyMMdd-HHmmss-F}.erg",
                DateTime.Now);
            
            var list = ReadInput(args[0]);
            WriteOuput(outputFilename, 300, list);

            Console.Write("DONE.");
            Console.ReadKey();
        }
    }
}
