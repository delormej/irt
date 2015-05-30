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

        public float[] Fit(float speedMps, List<MagnetPosition> magSlopeIntercepts)
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
