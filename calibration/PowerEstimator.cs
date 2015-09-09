using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IRT.Calibration
{

    class PowerEstimator
    {
        CalibrationResult m_config;

        public PowerEstimator(CalibrationResult config)
        {
            m_config = config;
        }

        public double Calculate(double speedMps)
        {
            //  Calculate force.
            double F = m_config.Cd * Math.Pow(speedMps, 2) +
                m_config.Slope * speedMps + m_config.Intercept;

            double power = F * speedMps;

            return power;
        }

        /// <summary>
        /// Returns an estimated power curve x:speed mph, y:watts.
        /// </summary>
        /// <returns></returns>
        public double[,] Calculate()
        {
            double max = 40;
            double start = 5;
            int step = 3;
            int size = (int)((max - start) / step);
            double[,] curve = new double[size, 2];
            double speed = start;

            for (int i = 0; i < size; i++)
            {
                double power = Calculate(speed * 0.44704);

                curve[i, 0] = speed;
                curve[i, 1] = power;

                speed += step;
            }

            return curve;
        }
    }

}
