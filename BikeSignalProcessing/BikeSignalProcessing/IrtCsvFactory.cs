using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
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
                reader.ReadLine();

                while (!reader.EndOfStream)
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

                        data.Update(speedMph, power, servoPosition);
                    }
                }
            }

            return data;
        }
    }
}
