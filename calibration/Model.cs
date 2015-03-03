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

        public event PropertyChangedEventHandler PropertyChanged;

        public Model()
        {
            m_speed = new Speed(Settings.StableThresholdSpeedMps);
            m_tickEvents = new List<TickEvent>();
        }

        public double StableWatts;

        public double StableSeconds;

        public double StableSpeedMps;

        public double SpeedMps;

        public Stage Stage;

        public Motion Motion { get; set; }

        public TickEvent[] Events { get { return m_tickEvents.ToArray(); } }

        public void AddSpeedEvent(TickEvent tickEvent)
        {
            // Add the event.
            m_tickEvents.Add(tickEvent);
            
            // Calculate and update state.
            m_speed.Calculate(this);
        }

        public void AddPowerEvent(int eventCount, ushort accumPower)
        {
            // Only record watts if stable.
            if (Stage == Stage.Stable)
            {
                StableWatts = AveragePower.AddEvent(eventCount, accumPower, m_tickEvents);
            }
            else
            {
                // Add unstable flag.
                AveragePower.AddEvent(-1, 0, m_tickEvents);
            }
        }
    }
}
