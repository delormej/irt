using System;
using System.Collections.Generic;
using System.Linq;

namespace IRT.Calibration
{
    /// <summary>
    /// Calculates rate of deceleration for a given speed.
    /// </summary>
    public class DecelerationFit
    {
        alglib.barycentricinterpolant m_interpolant;
        double m_lastSpeed;
        double[] m_coeff;

        public DecelerationFit()
        {
        }

        /// <summary>
        /// Returns the calclation of seconds to coastdown to minimum speed from
        /// a given speed in meters per second.
        /// </summary>
        /// <param name="speed_mps"></param>
        /// <returns></returns>
        public double Seconds(double speedMps)
        {
            // Get the amount of time it takes to coast down from this speed.
            return alglib.barycentriccalc(m_interpolant, speedMps);
        }

        /// <summary>
        /// Returns the rate of deceleration in meters per second for a given speed.
        /// </summary>
        /// <param name="speed_mps"></param>
        /// <returns></returns>
        public double Rate(double speedMps)
        {
            if (speedMps < m_lastSpeed)
            {
                return 0;
            }

            double dTime = Seconds(speedMps);

            // Get the change in speed.
            double dSpeed = speedMps - m_lastSpeed;

            // Get the rate of change in m/s^2.
            double acceleration = dSpeed / dTime;

            return acceleration;
        }


        /// <summary>
        /// Smoothes and fits raw coast down data to a curve.  This method must 
        /// be called before evaluating any deceleration rates.
        /// </summary>
        public void Fit(double[] speedMps, double[] coastdownSeconds)
        {
            // internal implementation of curve fitting.
            int info = 0;
            m_lastSpeed = speedMps.Last();

            alglib.polynomialfitreport report;

            alglib.polynomialfit(speedMps, coastdownSeconds, speedMps.Length,
                4, out info, out m_interpolant, out report);
            alglib.polynomialbar2pow(m_interpolant, out m_coeff);

            if (info != 1)
            {
                throw new CoastdownException("Unable to fit deceleration curve.");
            }
        }
    }
}
