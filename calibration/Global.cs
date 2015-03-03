﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IRT.Calibration
{
    public enum Motion
    {
        Undetermined,
        Accelerating,
        Stable,
        Decelerating
    }

    public enum Stage
    {
        Ready = 0,
        Stable,
        Accelerating,
        SpeedThresholdReached,
        Coasting,
        Processing,
        Finished,
        Failed
    }

    /// <summary>
    /// Configuration settings, can't be changed right now.
    /// </summary>
    public class Settings
    {
        // Stability is considered after 15 seconds consitent.
        public const double StableThresholdSeconds = 15;

        // Amount speed can vary and still be considered stable.
        public const double StableThresholdSpeedMps = 0.11176f;

        // Minimum of 25mph to reach peak speed.
        public const double MinAccelerationSpeedMps = 25.0f * 0.44704;
    }
}