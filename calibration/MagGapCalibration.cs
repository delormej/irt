using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IRT.Calibration
{
    public struct MagnetPosition
    {
        public int Position;
        public float Slope;
        public float Intercept;

        public static bool operator ==(MagnetPosition lhs, MagnetPosition rhs)
        {
            return (lhs.Position == rhs.Position);
        }

        public static bool operator !=(MagnetPosition lhs, MagnetPosition rhs)
        {
            return (lhs.Position != rhs.Position);
        }

        public override bool Equals(object obj)
        {
            return obj is MagnetPosition && this == (MagnetPosition)obj;
        }

        public override int GetHashCode()
        {
            if (Position > 0)
            {
                return Position.GetHashCode();
            }
            else
            {
                return base.GetHashCode();
            }
        }
    }

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
            MagnetPosition position = GetSlopeIntercept(magPosition);
            
            float force = magOnlyPower / speedMps;

            // Calculate mag only estimate.
            float estMagOnly = speedMps * position.Slope + position.Intercept;
            float estForce = estMagOnly / speedMps;

            // Calculate mag offset as a percentage of force.
            float magOffset = force / estForce; 

            return magOffset;
        }

        private MagnetPosition GetSlopeIntercept(int magPosition)
        {
            if (magPosition != 1200)
            {
                throw new ArgumentOutOfRangeException("Invalid magnet position.", "magPosition");
            }
             
            float slope = 14.3916520783f;
            float intercept = -17.4455956139f;

            var p = new MagnetPosition { Slope = slope, Intercept = intercept, Position = magPosition };
            
            return p;
        }
    }
}
