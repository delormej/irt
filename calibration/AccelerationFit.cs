using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using MathNet.Numerics;

namespace IRT.Calibration
{
    public class AccelerationFit : DecelerationFit
    {
        private double m_goodnessOfFit;

        public AccelerationFit()
        {

        }

        public double GoodnessOfFit
        {
            get
            {
                return m_goodnessOfFit;
            }
        }

        public double Slope
        {
            get
            {
                return m_coeff[1];
            }

            set
            {
                m_coeff[1] = value;
            }
        }

        public double Intercept
        {
            get
            {
                return m_coeff[0];
            }

            set
            {
                m_coeff[0] = value;
            }
        }

        public override double Rate(double speedMps)
        {
            // y = mx + b
            return Slope * speedMps + Intercept;
        }

        public override void Fit(double[] speedMps, double[] acceleration)
        {
            // Smoothing
            var filterAccel = MathNet.Filtering.OnlineFilter.CreateDenoise();
            var filteredAccel = filterAccel.ProcessSamples(acceleration);

            var filterSpeed = MathNet.Filtering.OnlineFilter.CreateDenoise();
            var filteredSpeed = filterSpeed.ProcessSamples(speedMps);

            var c = MathNet.Numerics.Fit.Line(speedMps, filteredAccel);

            Intercept = c.Item1;
            Slope = c.Item2;

            m_goodnessOfFit = MathNet.Numerics.GoodnessOfFit.R(
                speedMps.Select(x => Intercept + Slope * x),
                filteredAccel);

            System.Diagnostics.Debug.WriteLine(
                "a={0:N5},b={1:N5},good={2:N5}",
                Intercept, Slope, m_goodnessOfFit);
        }
    }
}
