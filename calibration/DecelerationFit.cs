﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Calibration
{
    /// <summary>
    /// Calculates rate of deceleration for a given speed.
    /// </summary>
    internal class DecelerationFit
    {
        alglib.barycentricinterpolant m_interpolant;
        double[] m_speedMps, m_coastdownSeconds;  // x, y coordinates.
        double[] m_coeff;

        public DecelerationFit(double[] speedMps, double[] coastdownSeconds)
        {
            m_speedMps = speedMps;
            m_coastdownSeconds = coastdownSeconds;
        }

        /// <summary>
        /// Returns the rate of deceleration in meters per second for a given speed.
        /// </summary>
        /// <param name="speed_mps"></param>
        /// <returns></returns>
        public double Rate(double speed_mps)
        {
            // Get the amount of time it takes to coast down from this speed.
            double dTime = alglib.barycentriccalc(m_interpolant, speed_mps);

            // Get the change in speed.
            double dSpeed = speed_mps - m_speedMps.Last();

            // Get the rate of change.
            double acceleration = dSpeed / dTime;

            return acceleration;
        }


        /// <summary>
        /// Smoothes and fits raw coast down data to a curve.  This method must 
        /// be called before evaluating any deceleration rates.
        /// </summary>
        public void Fit()
        {
            // internal implementation of curve fitting.
            int info = 0;

            alglib.polynomialfitreport report;

            alglib.polynomialfit(m_speedMps, m_coastdownSeconds, m_speedMps.Length,
                3, out info, out m_interpolant, out report);
            alglib.polynomialbar2pow(m_interpolant, out m_coeff);

            if (info != 1)
            {
                throw new CoastdownException("Unable to fit deceleration curve.");
            }
        }
    }
}
