using System;
using System.Collections.Generic;

namespace IRT.Calibration
{
    public delegate void MagnetCalibrationEventHandler(object sender, MagnetCalibrationEventArgs e);

    public class MagnetCalibration
    {
        public MagnetCalibration()
        {
            LowSpeedFactors = new float[4];
            HighSpeedFactors = new float[4];
        }

        public float LowSpeedMps;
        public float HighSpeedMps;

        public float[] LowSpeedFactors;
        public float[] HighSpeedFactors;

        public float[] FitLowSpeed(float speedMps, List<MagnetPosition> magSlopeIntercepts)
        {
            LowSpeedMps = speedMps;
            LowSpeedFactors = Fit(speedMps, magSlopeIntercepts);
            return LowSpeedFactors;
        }

        public float[] FitHighSpeed(float speedMps, List<MagnetPosition> magSlopeIntercepts)
        {
            HighSpeedMps = speedMps;
            HighSpeedFactors = Fit(speedMps, magSlopeIntercepts);
            return HighSpeedFactors;
        }

        /// <summary>
        /// Returns the servo position where the power curve crosses 0 on the Y axis (a "root").
        /// This indicates the position where the curve effectively turns upwards and is no longer
        /// valid.
        /// </summary>
        /// <returns></returns>
        public int GetRootPosition()
        {
            // Iterate through all positions from max to min, until one is bigger.
            const float speedMps = 6; // random any speed.
            float lastPower = float.MaxValue;
            float power = 0;
            int position = 800;

            do
            {
                power = MagnetWatts(speedMps, position);

                // Iterate until power increases, which is where we hit the root.
                if (power > lastPower)
                    break;
                else
                    lastPower = power;
            } while (position++ < 1600);

            return position;
        }

        private float[] Fit(float speedMps, List<MagnetPosition> magSlopeIntercepts)
        {
            double[] x;
            double[] y;
            double[] coeff;
            int m = 4; // polynomial_degree + 1
            int info;
            alglib.barycentricinterpolant p;
            alglib.polynomialfitreport rep;

            // Generate sample data.
            GeneratePowerData(speedMps, magSlopeIntercepts, out x, out y);

            // Fit to the curve.
            alglib.polynomialfit(x, y, m, out info, out p, out rep);

            // Get the polynomial coefficients.
            alglib.polynomialbar2pow(p, out coeff);

            // Reverse the array as it is return backwards from the way we use it.
            Array.Reverse(coeff);

            // Convert to float.
            float[] coeff_f = Array.ConvertAll(coeff, c => (float)c);

            return coeff_f;
        }

        /// <summary>
        /// Returns an array of power values for the spectrum of positions from 800 - 1600 at 100
        /// position increments.
        /// </summary>
        /// <param name="speedMps"></param>
        /// <returns></returns>
        public Tuple<int, float>[] MagnetWatts(float speedMps)
        {
            const int min_position = 1600;
            const int max_position = 800;
            const int step = 50;

            int len = (min_position - max_position) / step;

            Tuple<int, float>[] positions = new Tuple<int, float>[len];

            for (int i = 0; i < len; i++)
            {
                int position = max_position + (i * step);
                float watts = MagnetWatts(speedMps, position);

                positions[i] = new Tuple<int, float>(position, watts);
            }

            return positions;
        }

        public float MagnetWatts(float speedMps, int position)
        {
            float lowWatts = CalculateWattsFromFactors(LowSpeedFactors, position);
            float highWatts = CalculateWattsFromFactors(HighSpeedFactors, position);

            // Interopolate.
            float watts = lowWatts + ((speedMps - LowSpeedMps) / (HighSpeedMps - LowSpeedMps)) *
                (highWatts - lowWatts);

            return watts;
        }

        private float CalculateWattsFromFactors(float[] factors, int position)
        {
            float watts = factors[0] * (float)Math.Pow(position, 3) +
                factors[1] * (float)Math.Pow(position, 2) +
                factors[2] * position +
                factors[3];

            return watts;
        }

        private void GeneratePowerData(double speedMps, 
            List<MagnetPosition> magSlopeIntercepts,
            out double[] position, out double[] watts)
        {
            int positions = magSlopeIntercepts.Count;

            // for each position, speed * slope + intercept
            position = new double[positions];
            watts = new double[positions];

            for (int i = 0; i < positions; i++)
            {
                position[i] = magSlopeIntercepts[i].Position;
                watts[i] = speedMps * magSlopeIntercepts[i].Slope + 
                    magSlopeIntercepts[i].Intercept;
            }
        }
    }

    public class MagnetCalibrationEventArgs : EventArgs
    {
        public MagnetCalibration Calibration;

        public MagnetCalibrationEventArgs(MagnetCalibration calibration)
        {
            this.Calibration = calibration;
        }
    }
}
