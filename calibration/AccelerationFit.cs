using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using MathNet.Numerics;

namespace IRT.Calibration
{
    public class AccelerationFit : DecelerationFit
    {
        public AccelerationFit()
        {

        }

        public override void Fit(double[] speedMps, double[] acceleration)
        {
            //MathNet.Numerics.LinearRegression.SimpleRegression
            double[] vSquared = new double[speedMps.Length];

            // Smoothing
            

            for (int i = 0; i < speedMps.Length; i++)
            {
                vSquared[i] = Math.Pow(speedMps[i], 2);
            }

            var c = MathNet.Numerics.Fit.Line(vSquared, acceleration);

            m_coeff[0] = c.Item1;
            m_coeff[1] = c.Item2;

            double a = c.Item1;
            double b = c.Item2;

            var g = MathNet.Numerics.GoodnessOfFit.R(
                speedMps.Select(x => a + b * x),
                acceleration);

            System.Diagnostics.Debug.WriteLine(
                "a={0:N5},b={1:N5},good={2:N5}",
                a, b, g);
        }
    }
}
