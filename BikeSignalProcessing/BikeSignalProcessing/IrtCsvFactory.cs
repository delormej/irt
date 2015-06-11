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
            List<double> powerData = new List<double>();
            const int dataColumn = 5;

            using (StreamReader reader = File.OpenText(filename))
            {
                // Read headers... do nothing with them right now.
                reader.ReadLine();

                while (!reader.EndOfStream)
                {
                    string line = reader.ReadLine();
                    string[] vals = line.Split(',');

                    if (vals.Length > dataColumn)
                    {
                        // Parse power data.
                        double power;
                        double.TryParse(vals[dataColumn], out power);
                        powerData.Add(power);
                    }
                }
            }

            return powerData.ToArray();
        }
    }
}
