using System;
using System.Collections.Generic;
using System.IO;

namespace IRT.Calibration
{

    public class MagGapCalibration
    {
        public MagGapCalibration() { }

        /// <summary>
        /// Given a stable speed/power tuple at a known magnet position, calculate
        /// the magnet offset.
        /// </summary>
        /// <param name="speedMps"></param>
        /// <param name="power"></param>
        /// <returns></returns>
        public float CalculateOffset(float speedMps, int magOnlyPower, int magPosition)
        {
            if (speedMps <= 0)
            {
                throw new ArgumentOutOfRangeException("speedMps");
            }
            if (magOnlyPower <= 0)
            {
                throw new ArgumentOutOfRangeException("magOnlyPower");
            }
            
            // Load slope & intercept for magPosition
            MagnetPosition position = MagnetPosition.Find(magPosition);
            
            if (position == null)
            {
                throw new ArgumentOutOfRangeException("magPosition",
                    string.Format("Unable to locate slope/intercept for {0}", magPosition));
            }

            float force = magOnlyPower / speedMps;

            // Calculate mag only estimate.
            float estMagOnly = speedMps * position.Slope + position.Intercept;
            float estForce = estMagOnly / speedMps;

            // Calculate mag offset as a percentage of force.
            float magOffset = force / estForce; 

            return magOffset;
        }
    }
}
