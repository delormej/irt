using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace IRT_GUI.Simulation
{
    public class GpsPoint
    {
        public int Index;
        public double DistanceM;
        public double ElevationM;
        public double Slope;

        /// <summary>
        /// Parses GpsPoint from comma seperated line in the following format:
        /// Index, Name, Longitude, Latitude, Altitude, Distance, GradientToNext, BearingToNext, Time
        /// </summary>
        /// <param name="csv"></param>
        /// <returns></returns>
        public static GpsPoint FromString(string csv)
        {
            string[] fields = csv.Split(',');

            if (fields == null || fields.Length < 8)
                return null;

            GpsPoint point;

            try 
            {
                point = new GpsPoint()
                {
                    Index = int.Parse(fields[0]),
                    DistanceM = double.Parse(fields[5]),
                    ElevationM = double.Parse(fields[4]),
                    Slope = double.Parse(fields[6])
                };
            }
            catch
            {
                return null;
            }

            return point;
        }
    }

    public class SimulationMode
    {
        private GpsPoint[] m_gpsPoints;
        private int m_currentIndex;

        public event Action<double> SlopeChanged;

        public GpsPoint[] GpsPoints { get { return m_gpsPoints; } }

        public SimulationMode()
        {
            m_currentIndex = 0;
        }

        /// <summary>
        /// Current distance location in meters.
        /// </summary>
        public double CurrentLocation { get { return m_gpsPoints[m_currentIndex].DistanceM; } }

        public void DistanceEvent(double distance)
        {
            int index = m_currentIndex;

            //
            // Advance through points until current distance.
            //
            while (m_gpsPoints[index].DistanceM >= distance)
                index++;

            if (index > m_currentIndex)
            {
                SlopeChanged(m_gpsPoints[index].Slope);
            }
        }

        /// <summary>
        /// Loads GPS points from a csv file.
        /// </summary>
        /// <param name="filename"></param>
        public void Load(string filename)
        {
            List<GpsPoint> points = new List<GpsPoint>();
            m_currentIndex = 0;

            using (StreamReader reader = new StreamReader(filename))
            {
                // Advance past first line.
                reader.ReadLine();

                while (!reader.EndOfStream)
                {
                    GpsPoint point = GpsPoint.FromString(reader.ReadLine());
                    points.Add(point);
                }
            }

            if (points.Count > 0)
            {
                m_gpsPoints = points.ToArray();
            }
        }


    }
}
