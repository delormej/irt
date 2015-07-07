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
        protected double[] m_coeff = { 0.0, 0.0 };

        public virtual double[] Coeff
        {
            get
            {
                return m_coeff;
            }
        }

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
            //return alglib.barycentriccalc(m_interpolant, speedMps);
            double acceleration = Rate(speedMps);
            double seconds = speedMps / acceleration;

            return seconds;
        }

        /// <summary>
        /// Returns the rate of deceleration in meters per second for a given speed.
        /// </summary>
        /// <param name="speed_mps"></param>
        /// <returns></returns>
        public double Rate(double speedMps)
        {
            return fit_coastdown(speedMps, m_coeff[0], m_coeff[1]);
        }

        /// <summary>
        /// Smoothes and fits raw coast down data to a curve.  This method must 
        /// be called before evaluating any deceleration rates.
        /// Speed and decleration should come in fastest first, i.e. element 0 is the fastest
        /// speed and the longest duration to coastdown.
        /// </summary>
        public virtual void Fit(double[] speedMps, double[] acceleration)
        {
            // internal implementation of curve fitting.
            int info = 0;

            if (speedMps.Length != acceleration.Length)
            {
                throw new ArgumentException("speedMps must have the same length as coastdownSeconds");
            }

            // Copy speed to jagged array as per alglib requirement.
            int size = speedMps.Length;
            double[,] speed = new double[size, 1];

            for (int i = 0; i < size; i++)
            {
                // Copy speed into the appropriate array shape for the function.
                speed[i, 0] = speedMps[i];
            }

            // Fit a curve in the function of y = mx^2+b, or { a = c1+c2*v^2 }

            alglib.lsfitstate state;
            alglib.lsfitreport report;
            alglib.lsfitcreatef(speed, acceleration, m_coeff, 0.0000001, out state);
            //alglib.lsfitsetbc(state, new double[] { 0.0, 0.0 }, new double[] { 0.0, 0.0 });
            alglib.lsfitfit(state, fit_func, null, null);
            alglib.lsfitresults(state, out info, out m_coeff, out report);
        }

        /// <summary>
        /// Fits a=c1+c2*v^2
        /// </summary>
        /// <param name="velocity"></param>
        /// <param name="c1"></param>
        /// <param name="c2"></param>
        /// <returns></returns>
        static double fit_coastdown(double v, double c1, double c2)
        {
            // Two coefficients of coastdown.
            return c1 + c2 * Math.Pow(v, 2);
        }

        /// <summary>
        /// Fits acceleration to velocity. 
        /// </summary>
        /// <param name="c"></param>
        /// <param name="x"></param>
        /// <param name="func"></param>
        /// <param name="obj"></param>
        static void fit_func(double[] c, double[] x, ref double func, object obj)
        {
            func = fit_coastdown(x[0], c[0], c[1]);
        }
    }
}
