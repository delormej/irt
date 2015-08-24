using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IRT.Calibration
{
    public class PolyPowerFit : PowerFit
    { 
        double[] m_speed;
        double[] m_power;

        public PolyPowerFit()
        {
        }

        public override double Watts(double speedMps)
        {
            double power = m_coeff[3] * Math.Pow(speedMps, 3) +
                m_coeff[2] * Math.Pow(speedMps, 2) +
                m_coeff[1] * speedMps +
                m_coeff[0];

            return power;
        }

        public override void Fit(double[] speed, double[] watts)
        {
            m_speed = speed;
            m_power = watts;
            m_coeff = MathNet.Numerics.Fit.Polynomial(m_speed, m_power, 3);
        }

        /// <summary>
        /// Creates a matrix of speed:power
        /// </summary>
        /// <returns></returns>
        public override void GeneratePowerData(out double[,] speed, out double[] watts)
        {
            double min = Math.Floor(m_speed.Min());
            double max = Math.Ceiling(m_speed.Max());

            double val = min;
            int len = (int)(max - min);

            speed = new double[len,1];
            watts = new double[len];

            for (int i = 0; i < len; i++)
            {
                speed[i,0] = val + i;
                watts[i] = Watts(speed[i,0]);
            } 
        }
    }
}
