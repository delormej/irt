using System;
using System.Collections.Generic;
using System.Linq;

namespace IRT.Calibration
{
    public class PowerFit
    {
        protected double[] m_coeff = { 0.0, 0.0 };
        private double m_mass = 0.0;

        public PowerFit()
        {
        }

        public double Drag { get; set; }

        public double RollingResistance { get; set; }

        public virtual void Fit(double[] speedMph, double[] watts)
        {
            double[,] speed;
            speed = new double[speedMph.Length, 1];

            for (int i = 0; i < speedMph.Length; i++)
            {
                speed[i, 0] = speedMph[i] * 0.44704;
            }

            Fit(speed, watts);
        }

        private void Fit(double[,] speed, double[] watts)
        {
            int info;

            alglib.lsfitstate state;
            alglib.lsfitreport report;
            alglib.lsfitcreatef(speed, watts, m_coeff, 0.0001, out state);
            alglib.lsfitsetbc(state, new double[] { 0.0, 0.0 }, new double[] { 5.0, 50.0 });
            alglib.lsfitfit(state, fit_func, null, null);
            alglib.lsfitresults(state, out info, out m_coeff, out report);

            Drag = m_coeff[0];
            RollingResistance = m_coeff[1];
        }

        /// <summary>
        /// Model for how power from rolling resistance and drag (no magnet) is calculated.
        /// </summary>
        /// <param name="v"></param>
        /// <param name="K"></param>
        /// <param name="rr"></param>
        /// <returns></returns>
        static double fit_drag_rr(double v, double K, double rr)
        {
            double p = (K * Math.Pow(v, 2) + rr) * v; // / contact_patch());

            return p;
        }

        static double fit_power(double K, double v, double m, double Crr)
        {
            double g = 9.0867;
            double w = (K * Math.Pow(v, 2) + m * g * Crr) * v;

            return w;
        }

        static void fit_power_func()
        {

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
