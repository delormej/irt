using System;
using System.Linq;
using System.Collections.Generic;
using System.IO;


namespace IRT.Calibration
{
    /// <summary>
    /// Handles the filtering and process of the data files.
    /// Front end interface to the end results.
    /// </summary>
    public class Coastdown
    {
        private List<int> m_timestamps, m_flywheel_ticks;
        private CoastdownData m_coastdownData;

        // Fit objects.
        private DecelerationFit m_decelFit;
        private PowerFit m_powerFit;

        public Coastdown()
        {
            m_timestamps = new List<int>();
            m_flywheel_ticks = new List<int>();
        }

        /// <summary>
        /// Raw coastdown data point.
        /// </summary>
        public CoastdownData Data { get { return m_coastdownData; } }

        /// <summary>
        /// Add a coast down tick event.
        /// </summary>
        /// <param name="timestamp_2048"></param>
        /// <param name="flywheel_ticks"></param>
        public void Add(int timestamp_2048, int flywheel_ticks)
        {
            m_timestamps.Add(timestamp_2048);
            m_flywheel_ticks.Add(flywheel_ticks);
        }

        /// <summary>
        /// Parses the coast down x,y values and generates coefficients of Drag 
        /// and Rolling Resistance based on stable speed and watts.
        /// </summary>
        /// <returns>true/false if it succeeded.</returns>
        public void Calculate(double stableSpeedMps, double stableWatts)
        {
            // Process values to build speed/time arrays.
            m_coastdownData = new CoastdownData(m_timestamps.ToArray(),
                m_flywheel_ticks.ToArray());
            m_coastdownData.Evaluate();

            // Calculate the deceleration.
            m_decelFit = new DecelerationFit(m_coastdownData.SpeedMps,
                m_coastdownData.CoastdownSeconds);
            m_decelFit.Fit();
            m_powerFit = new PowerFit(m_decelFit, stableSpeedMps, stableWatts);
            m_powerFit.Fit();

            /*
            if (Drag < 0.0)
            {
                throw new CoastdownException(
                    "Drag should not be negative, likely entry speed is wrong.");
            }
            else if (RollingResistance < 10)
            {
                //throw new CoastdownException(
                // "Rolling Resistance seems incorrect, it should be greater.");
            }
             */
        }

        /// <summary>
        /// Calculated coefficient of Drag.
        /// </summary>
        public double Drag { get { return m_powerFit.Drag; } }

        /// <summary>
        /// Calculated coefficient of Rolling Resistance.
        /// </summary>
        public double RollingResistance { get { return m_powerFit.RollingResistance; } }

        /// <summary>
        /// Returns the time it takes to coastdown in seconds from a given speed.
        /// </summary>
        /// <param name="speedMps"></param>
        /// <returns></returns>
        public double CoastdownTime(double speedMps)
        {
            return m_decelFit.Seconds(speedMps);
        }

        /// <summary>
        /// Calculate the rate of deceleration for a given speed.
        /// </summary>
        /// <param name="speed_mps"></param>
        /// <returns></returns>
        public double Deceleration(double speedMps)
        {
            return m_decelFit.Rate(speedMps);
        }

        /// <summary>
        /// Calculate the power required in watts for a given speed.
        /// </summary>
        /// <param name="speed_mps"></param>
        /// <returns></returns>
        public double Watts(double speedMps)
        {
            return m_powerFit.Watts(speedMps);
        }

        /// <summary>
        /// Creates an instance of the class by parsing a coastdown file.
        /// </summary>
        /// <param name="filename"></param>
        /// <returns></returns>
        public static Coastdown FromFile(string filename)
        {
            int timestamp, ticks;
            Coastdown coastdown = new Coastdown();

            using (StreamReader reader = File.OpenText(filename))
            {
                // Advance through header.
                reader.ReadLine();

                while (!reader.EndOfStream)
                {
                    ParseLine(reader.ReadLine(), out timestamp, out ticks);
                    coastdown.Add(timestamp, ticks);
                }
            }
            
            return coastdown;
        }

        /// <summary>
        /// Parses a line from the csv file.
        /// </summary>
        /// <param name="line"></param>
        /// <param name="timestamp"></param>
        /// <param name="ticks"></param>
        private static void ParseLine(string line, out int timestamp, out int ticks)
        {
            string[] column = line.Split(',');
            
            if (!int.TryParse(column[0], out timestamp))
            {
                throw new FormatException("CSV not properly formatted.");
            }

            if (!int.TryParse(column[2], out ticks))
            {
                throw new FormatException("CSV not properly formatted.");
            }
        }    
    }

    public class CoastdownException : Exception
    {
        public CoastdownException(string message) : base(message)
        {

        }
    }
}
