using System;
using System.Collections.Generic;
using System.Linq;

namespace IRT.Calibration
{
    internal class PowerFit
    {
        private double[] m_coeff = { 0.0, 0.0 };
        private DecelerationFit m_decelFit;
        private double m_inertia;

        public PowerFit(DecelerationFit decelFit, double stableSpeedMps, double stableWatts)
        {
            m_decelFit = decelFit;
            m_inertia = MomentOfInteria(stableSpeedMps, stableWatts);
        }

        public double Drag { get { return m_coeff[0]; } }

        public double RollingResistance { get { return m_coeff[1]; } }

        public double Watts(double speedMps)
        {
            return fit_drag_rr(speedMps, Drag, RollingResistance);
        }

        /// <summary>
        /// Calculates the moment of inertia a.k.a mass in F=ma.
        /// </summary>
        /// <param name="speed_mps"></param>
        /// <param name="watts"></param>
        /// <returns></returns>
        private double MomentOfInteria(double speedMps, double watts)
        {
            // Where P = F*v, F=ma

            double f = watts / speedMps;
            double a = m_decelFit.Rate(speedMps);

            // Get the rate of deceleration (a) for a given velocity.

            // Solve for m = f/a
            return f / a;
        }

        public void Fit()
        {
            int info;
            double[,] speed;
            double[] watts;

            GeneratePowerData(out speed, out watts);

            alglib.lsfitstate state;
            alglib.lsfitreport report;
            alglib.lsfitcreatef(speed, watts, m_coeff, 0.0001, out state);
            alglib.lsfitfit(state, fit_func, null, null);
            alglib.lsfitresults(state, out info, out m_coeff, out report);
        }

        private void GeneratePowerData(out double[,] speed, out double[] watts)
        {
            speed = new double[30, 1];
            watts = new double[30];

            int ix = 0;
            // Generate power data.
            foreach (var i in Enumerable.Range(5, 30))
            {
                double v = i * 0.44704;
                double a = m_decelFit.Rate(v);
                double f = m_inertia * a;

                speed[ix, 0] = v;
                watts[ix] = f * v;

                ix++;
            }
        }

        static double fit_drag_rr(double v, double K, double rr)
        {
            double p = K * Math.Pow(v, 2) + ((v * rr)); // / contact_patch());

            return p;
        }

        static void fit_func(double[] c, double[] x, ref double func, object obj)
        {
            func = fit_drag_rr(x[0], c[0], c[1]);
        }

        static double contact_patch()
        {
            //# 'Using this method: http://bikeblather.blogspot.com/2013/02/tire-crr-testing-on-rollers-math.html
            double wheel_diameter_cm = 210.7 / Math.PI;
            double drum_diameter_cm = 26 / Math.PI;
            double patch = Math.Pow((1 / (1 + (wheel_diameter_cm / drum_diameter_cm))), 0.7);

            return patch;
        }
    }
}
