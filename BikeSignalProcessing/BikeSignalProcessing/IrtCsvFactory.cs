using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace BikeSignalProcessing
{


    /// <summary>
    /// Reads CSV file and returns an object of the appropriate type.
    /// </summary>
    public class IrtCsvFactory
    {
        public static object Open(string filename)
        {
            Data data = new Data();

            using (StreamReader reader = File.OpenText(filename))
            {
                // Read headers... do nothing with them right now.
                LogParserType type = ChooseParser(reader.ReadLine());

                int speedCol, powerCol, magCol;

                switch (type)
                {
                    case LogParserType.Irt:
                        speedCol = 3;
                        powerCol = 5;
                        magCol = 7;
                        break;

                    case LogParserType.TrainerRoad:
                        speedCol = 8;
                        powerCol = 2; magCol = 11;
                        break;

                    default:
                        throw new InvalidDataException("Couldn't match a parser.");
                }

                while (!reader.EndOfStream)
                {
                    string line = reader.ReadLine();
                    string[] vals = line.Split(',');

                    if (vals.Length > 7)
                    {
                        // Parse power data.
                        double power;
                        double.TryParse(vals[powerCol], out power);

                        // Parse speed
                        double speedMph;
                        double.TryParse(vals[speedCol], out speedMph);

                        if (type == LogParserType.TrainerRoad)
                        {
                            // Convert from kmh to mph
                            speedMph = speedMph * 0.621371;
                        }

                        // Parse servo position
                        int servoPosition;
                        int.TryParse(vals[magCol], out servoPosition);

                        data.Update(speedMph, power, servoPosition);
                    }
                }
            }

            return data;
        }

        private static LogParserType ChooseParser(string header)
        {
            const string irtLogToken = "event_time";
            const int TRTokenColumn = 1;
            LogParserType type = LogParserType.Invalid;

            if (string.IsNullOrEmpty(header))
                return type;

            if (header.StartsWith(irtLogToken))
            {
                type = LogParserType.Irt;
            }

            string[] fields = header.Split(',');
            Guid guid;

            if (fields.Length >= TRTokenColumn && 
                Guid.TryParse(fields[TRTokenColumn],
                    out guid)) //6th column is a date
            {
                type = LogParserType.TrainerRoad;
            }

            return type;
        }
    }

    public enum LogParserType
    {
        Invalid,
        Irt,
        TrainerRoad
    }
    
    public class AsyncCsvFactory
    {
        private Data mData;
        private Timer mTimer;

        public AsyncCsvFactory()
        {
            mData = new Data();
        }

        public Data Open(string filename)
        {
            StreamReader reader = File.OpenText(filename);
            
            // Read headers... do nothing with them right now.
            reader.ReadLine();

            for (int i = 0; i < 1000; i++)
                reader.ReadLine();

            mTimer = new Timer(Callback, reader, 0, 250);

            return mData;
        }

        private void Callback(object state)
        {
            StreamReader reader = (StreamReader)state;

            if (!reader.EndOfStream)
            {
                string line = reader.ReadLine();
                string[] vals = line.Split(',');

                if (vals.Length > 7)
                {
                    // Parse power data.
                    double power;
                    double.TryParse(vals[5], out power);

                    // Parse speed
                    double speedMph;
                    double.TryParse(vals[3], out speedMph);

                    // Parse servo position
                    int servoPosition;
                    int.TryParse(vals[7], out servoPosition);

                    mData.Update(speedMph, power, servoPosition);
                }
            }
            else
            {
                reader.Close();
                mTimer.Dispose();
            }
        }
    }
}
