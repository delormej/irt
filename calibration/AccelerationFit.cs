using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using MathNet.Numerics;

namespace IRT.Calibration
{
    public class AccelerationFit
    {
        private double m_goodnessOfFit;
        private Func<double, double> m_accelFunc;

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
        
        /// <summary>
        /// Returns acceleration for a given speed.
        /// </summary>
        /// <param name="mps"></param>
        public double Acceleration(double mps)
        {
            if (m_accelFunc == null)
            {
                throw new InvalidOperationException("Must call 'Fit' function first.");
            }

            return m_accelFunc(mps);
        }

        public void Fit(CoastdownData data)
        {
            // Fit time : speed (should be a nice fit)
            Func<double, double> timeSpeedFunc = MathNet.Numerics.Fit.PolynomialFunc(
                data.CoastdownSeconds, data.SpeedMps, 2);

            // We want goodness of fit to be as close to 1.0 as possible.  At least 0.999.
            m_goodnessOfFit = MathNet.Numerics.GoodnessOfFit.R(
                data.CoastdownSeconds.Select(x => timeSpeedFunc(x)),
                data.SpeedMps);

            if (m_goodnessOfFit < 0.999)
            {
                // throw a warning out here? 
            }

            // Get the coefficients, just for debugging purposes.
            double[] timeSpeedCoeff = MathNet.Numerics.Fit.Polynomial(
                data.CoastdownSeconds, data.SpeedMps, 2);

            System.Diagnostics.Debug.WriteLine("Goodness of fit: {0:N5}", m_goodnessOfFit);
            System.Diagnostics.Debug.WriteLine("a,mps,time");

            //
            // Solve for the differential equation dV / dT for real acceleration at each step.
            //
            for (int i = 0; i < data.CoastdownSeconds.Length; i++)
            {
                double accel = MathNet.Numerics.Differentiate.FirstDerivative(timeSpeedFunc, data.CoastdownSeconds[i]);
                data.Acceleration[i] = accel;

                //System.Diagnostics.Debug.WriteLine(
                //    "{0:N5},{1:N5},{2:N5}",
                //    accel, data.SpeedMps[i], data.CoastdownSeconds[i]);
            }

            // Fit acceleration : speed.
            double[] speedAccelCoeff = MathNet.Numerics.Fit.Polynomial(data.SpeedMps, data.Acceleration, 2);

            m_accelFunc = MathNet.Numerics.Fit.PolynomialFunc(data.SpeedMps, data.Acceleration, 2);

            // We want goodness of fit to be as close to 1.0 as possible.  At least 0.999.
            m_goodnessOfFit = MathNet.Numerics.GoodnessOfFit.R(
                data.SpeedMps.Select(x => m_accelFunc(x)),
                data.Acceleration);

            System.Diagnostics.Debug.WriteLine(
                "a={0:N5},b={1:N5},c={2:N5},r={3:N5}",
                speedAccelCoeff[2], speedAccelCoeff[1], speedAccelCoeff[0], m_goodnessOfFit);
        }
    }
}
