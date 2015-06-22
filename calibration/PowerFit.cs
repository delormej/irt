using System;
using System.Collections.Generic;
using System.Linq;

namespace IRT.Calibration
{
    public class PowerFit
    {
        protected double[] m_coeff = { 0.0, 0.0 };
        private double m_coastdownToPowerRatio;

        private DecelerationFit m_decelFit;

        public static double Power(double speedMph, double drag, double rr)
        {
            double speedMps = speedMph * 0.44704;
            return fit_drag_rr(speedMps, drag, rr);
        }

        public PowerFit()
        {
            m_decelFit = null;
        }

        public PowerFit(DecelerationFit decelFit)
        {
            m_decelFit = decelFit;
        }

        public double Drag { get; set; }

        public double RollingResistance { get; set; }

        public virtual double Watts(double speedMps)
        {
            return fit_drag_rr(speedMps, Drag, RollingResistance);
            //double force = (m_decelFit.Coeff[0] / m_coastdownToPowerRatio) +
            //    (m_decelFit.Coeff[1] / m_coastdownToPowerRatio) *
            //    Math.Pow(speedMps, 2);

            //double watts = force * speedMps;

            //return watts;
        }

        /// <summary>
        /// Calculates the moment of inertia a.k.a mass in F=ma.
        /// </summary>
        /// <param name="speed_mps"></param>
        /// <param name="watts"></param>
        /// <returns></returns>
        public double CalculateStablePowerFactor(double speedMps, double watts)
        {
            if (m_decelFit == null)
            {
                throw new InvalidOperationException(
                    "PowerFit must be constructed with DecelerationFit to use this method.");
            }

            // Where P = F*v
            double f = watts / speedMps;
            
            // Get the rate of deceleration (a) for a given velocity.
            double a = m_decelFit.Rate(speedMps);

            // Calcualte the ratio of coastdown to power coefficients.
            m_coastdownToPowerRatio = a / f;
            
            return m_coastdownToPowerRatio;
        }

        public virtual void Fit()
        {
            if (m_decelFit == null)
            {
                throw new InvalidOperationException(
                    "PowerFit must be constructed with DecelerationFit to use this method.");
            }

            RollingResistance = (m_decelFit.Coeff[0] / m_coastdownToPowerRatio);
            Drag = (m_decelFit.Coeff[1] / m_coastdownToPowerRatio); 
        }

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

        public virtual void GeneratePowerData(out double[,] speed, out double[] watts)
        {
            if (m_decelFit == null)
            {
                throw new InvalidOperationException(
                    "PowerFit must be constructed with DecelerationFit to use this method.");
            }

            speed = new double[30, 1];
            watts = new double[30];

            int ix = 0;
            // Generate power data.
            foreach (var i in Enumerable.Range(5, 30))
            {
                double v = i * 0.44704;

                speed[ix, 0] = v;
                watts[ix] = fit_drag_rr(v, Drag, RollingResistance);

                ix++;
            }
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
