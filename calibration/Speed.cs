﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using IRT.Calibration.Globals;

namespace IRT.Calibration
{
    class Speed
    {
        private double m_lastSpeed = 0;
        private double m_stableStartSeconds;
        
        // 1/2 mile per hour in meters per second +/-.
        readonly double StabilityThresholdMps = 0; 

        public Speed()
        {
        }

        public Speed(double ThresholdMps) : base()
        {
            this.StabilityThresholdMps = ThresholdMps;
        }

        /// <summary>
        /// Calcualtes latest state based on tick events.
        /// </summary>
        /// <param name="tickEvent"></param>
        /// <param name="model"></param>
        public void Calculate(Model model)
        {
            TickEvent[] events = model.Events;
            TickEvent tickEvent = events.Last();

            // Event offset skip, every 4th event.
            int SkipOffset = 4;
            int currentIndex = events.Length;
            
            ushort lastTicks = 0, lastTimestamp = 0;

            if (currentIndex > SkipOffset)
            {
                lastTicks = events[currentIndex - SkipOffset].Ticks;
                lastTimestamp = events[currentIndex - SkipOffset].Timestamp;
            }
            else
            {
                // Need to accumulate a couple of events first.
                model.SpeedMps = 0;
                model.StableSeconds = 0;
                model.Motion = Motion.Undetermined;

                return;
            }

            int dt, ds; // delta ticks, delta seconds

            if (tickEvent.Ticks < lastTicks)
                dt = tickEvent.Ticks + (lastTicks ^ 0xFFFF);
            else
                dt = tickEvent.Ticks - lastTicks;

            if (lastTimestamp > 0)
            {
                if (tickEvent.Timestamp < lastTimestamp)
                    ds = tickEvent.Timestamp + (lastTimestamp ^ 0xFFFF);
                else
                    ds = tickEvent.Timestamp - lastTimestamp;
            }
            else
            {
                ds = 0;
            }

            /* Each flywheel revolution equals 0.1115 meters travelled by the 
                * bike.  Two ticks are recorded for each revolution.
                * Time is sent in 1/2048 of a second.
                */
            double seconds = (ds / 2048.0);
            model.SpeedMps = (dt * 0.1115 / 2.0) / seconds;
            
            Motion motion = Stability(model.SpeedMps); 
            
            // Track a transition into stablity.
            if (motion == Motion.Stable)
            {
                if (model.Motion != Motion.Stable)
                {
                    m_stableStartSeconds = seconds;
                    model.StableSeconds = 0;
                }
                else 
                {
                    model.StableSeconds = seconds - m_stableStartSeconds;
                    model.StableSpeedMps = model.SpeedMps;
                }
            }
            else
            {
                model.StableSeconds = 0;
            }

            model.Motion = motion;
        }

        private Motion Stability(double currentSpeed)
        {
            Motion state = Motion.Undetermined;
            double lowerBounds, upperBounds;

            if (m_lastSpeed != 0)
            {
                lowerBounds = (m_lastSpeed - StabilityThresholdMps);
                upperBounds = (m_lastSpeed + StabilityThresholdMps);

                //
                // Determine stability.
                //
                if (currentSpeed <= upperBounds &&
                    currentSpeed >= lowerBounds)
                {
                    state = Motion.Stable;
                }
                else if (currentSpeed > upperBounds)
                {
                    state = Motion.Accelerating;
                }
                else if (currentSpeed < lowerBounds)
                {
                    state = Motion.Decelerating;
                }
            }

            m_lastSpeed = currentSpeed;
            return state;
        }
    }
}
