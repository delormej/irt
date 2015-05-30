using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;

namespace IRT.Calibration
{
    public struct MagnetPosition
    {
        public int Position;
        public float Slope;
        public float Intercept;

        public static bool operator ==(MagnetPosition lhs, MagnetPosition rhs)
        {
            return (lhs.Position == rhs.Position);
        }

        public static bool operator !=(MagnetPosition lhs, MagnetPosition rhs)
        {
            return (lhs.Position != rhs.Position);
        }

        public override bool Equals(object obj)
        {
            return obj is MagnetPosition && this == (MagnetPosition)obj;
        }

        public override int GetHashCode()
        {
            if (Position > 0)
            {
                return Position.GetHashCode();
            }
            else
            {
                return base.GetHashCode();
            }
        }

        public static List<MagnetPosition> GetMagnetPositions()
        {
            var assembly = Assembly.GetExecutingAssembly();
            var resourceName = "IRT.Calibration.position_slope_intercept.csv";

            using (Stream stream = assembly.GetManifestResourceStream(resourceName))
            {
                StreamReader reader = new StreamReader(stream);
                return GetMagnetPositions(reader);
            }
        }

        public static List<MagnetPosition> GetMagnetPositions(string filename)
        {
            StreamReader reader = File.OpenText(filename);
            return GetMagnetPositions(reader);
        }

        public static List<MagnetPosition> GetMagnetPositions(StreamReader reader)
        {
            List<MagnetPosition> positions = new List<MagnetPosition>();

            using (reader)
            {
                while (!reader.EndOfStream)
                {
                    MagnetPosition position = new MagnetPosition();

                    string line = reader.ReadLine();
                    string[] vals = line.Split(',');

                    int.TryParse(vals[0], out position.Position);
                    float.TryParse(vals[1], out position.Slope);
                    float.TryParse(vals[2], out position.Intercept);

                    positions.Add(position);
                }
            }

            return positions;
        }

        public static MagnetPosition Find(int position)
        {
            return MagnetPosition.GetMagnetPositions().FindPosition(position);
        }
    }

    public static class MagnetPositionExtensions
    {
        public static MagnetPosition FindPosition(this List<MagnetPosition> list, int magPosition)
        {
            return list.Where(x => x.Position == magPosition).First();

            /*
            if (magPosition != 1200)
            {
                throw new ArgumentOutOfRangeException("Invalid magnet position.", "magPosition");
            }

            float slope = 14.3916520783f;
            float intercept = -17.4455956139f;

            var p = new MagnetPosition { Slope = slope, Intercept = intercept, Position = magPosition };

            return p;
            */
        }
    }
}
