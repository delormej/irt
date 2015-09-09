using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IRT.Calibration
{
    public class CalibrationResult
    {
        /// <summary>
        /// Co-Efficient of drag.
        /// </summary>
        public double Cd;

        /// <summary>
        /// Linear portion of Slope of force for a given speed.
        /// </summary>
        public double Slope;

        /// <summary>
        /// Linear portion of Force.  Offset / intercept of force given speed.
        /// </summary>
        public double Intercept;

        /// <summary>
        /// Indicator of how good the fit was.
        /// </summary>
        public double GoodnessOfFit;
    }
}
