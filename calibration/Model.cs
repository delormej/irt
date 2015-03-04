using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using IRT.Calibration.Globals;

namespace IRT.Calibration
{
    /// <summary>
    /// Represents the current state of acceleration.
    /// </summary>
    public class Model : INotifyPropertyChanged
    {
        private Speed m_speed;
        private List<TickEvent> m_tickEvents;
        private int lastPowerEventCount;
        private ushort lastAccumPower;

        public event PropertyChangedEventHandler PropertyChanged;

        public Model()
        {
            m_speed = new Speed(Settings.StableThresholdSpeedMps);
            m_tickEvents = new List<TickEvent>();
        }

        /// <summary>
        /// Length of time in second that the speed was stable.
        /// </summary>
        public double StableSeconds;

        /// <summary>
        /// Last stable value of speed recorded.
        /// </summary>
        public double StableSpeedMps;

        /// <summary>
        /// At stable speed, these were the stable watts.
        /// </summary>
        public double StableWatts;

        /// <summary>
        /// Current speed in meters per second.
        /// </summary>
        public double SpeedMps;

        /// <summary>
        /// Stage of calibration.
        /// </summary>
        public Stage Stage;

        /// <summary>
        /// Dynamically calculated indication of accelerating/decelerating/stable for each speed tick.
        /// </summary>
        public Motion Motion { get; set; }

        public List<TickEvent> Events { get { return m_tickEvents; } }

        public void AddSpeedEvent(TickEvent tickEvent)
        {
            // Ensure only one thread at a time gets here.
            lock (this)
            {
                // Only record watts if stable.
                if (this.Motion == Globals.Motion.Stable)
                {
                    // Only if the event has not been populated already.
                    if (tickEvent.PowerEventCount == 0 && lastPowerEventCount > 0)
                    {
                        tickEvent.PowerEventCount = lastPowerEventCount;
                        tickEvent.AccumulatedPower = lastAccumPower;
                    }
                }
                else
                {
                    // Add unstable flag.
                    tickEvent.PowerEventCount = -1;
                    tickEvent.AccumulatedPower = 0;
                }

                // Add the event.
                m_tickEvents.Add(tickEvent);

                // Calculate and update state.
                m_speed.Calculate(this);
            }
        }

        public void AddPowerEvent(int eventCount, ushort accumPower)
        {
            // Keep track of last event
            lock (this)
            {
                lastPowerEventCount = eventCount;
                lastAccumPower = accumPower;
            }
        }
    }
}
