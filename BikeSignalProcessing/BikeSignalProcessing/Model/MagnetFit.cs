using MathNet.Numerics.LinearRegression;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BikeSignalProcessing.Model
{
    public class MagnetFit
    {
        public double Slope;
        public double Intercept;
        public int MagnetPosition;

        private double lowSpeed = 10.0; 
        private double highSpeed = 25.0; 

        public Tuple<double, double> Low()
        {
            Tuple<double, double> value = new Tuple<double, double>(
                lowSpeed,
                Slope * lowSpeed + Intercept);

            return value;
        }

        public Tuple<double, double> High()
        {
            Tuple<double, double> value = new Tuple<double, double>(
                highSpeed,
                Slope * highSpeed + Intercept);

            return value;
        }

        /// <summary>
        /// Fits this segment against others for a given magnet position.
        /// </summary>
        /// <param name="segments"></param>
        /// <returns></returns>
        public static MagnetFit[] FitMagnet(IEnumerable<Segment> segments)
        {
            List<MagnetFit> fits = new List<MagnetFit>();

            var groups = segments.GroupBy(s => s.MagnetPosition);
            foreach (var group in groups)
            {
                var best = Segment.FindBestSegments(group);

                // Need at least 2 members in a group to fit.
                if (best == null || best.Count() < 2)
                    continue;

                List<double> x, y;

                x = new List<double>();
                y = new List<double>();

                foreach (Segment segment in best)
                {
                    x.Add(segment.AverageSpeed);
                    y.Add(segment.AveragePower);
                }

                Tuple<double, double> fit = SimpleRegression.Fit(x.ToArray(), y.ToArray());

                // Assign fit.
                MagnetFit magfit = new MagnetFit();
                magfit.MagnetPosition = group.First().MagnetPosition;
                magfit.Intercept = fit.Item1;
                magfit.Slope = fit.Item2;

                fits.Add(magfit);
            }

            return fits.ToArray();

        }

    }
}
