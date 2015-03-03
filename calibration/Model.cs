using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace IRT.Calibration
{
    /// <summary>
    /// Represents the current state of acceleration.
    /// </summary>
    public class Model : INotifyPropertyChanged
    {
        private Speed m_speed;
        private List<TickEvent> m_ticks;

        public event PropertyChangedEventHandler PropertyChanged;

        public Model()
        {
            m_speed = new Speed(Settings.StableThresholdSpeedMps);
            m_ticks = new List<TickEvent>();
        }

        public ushort Ticks;
        public ushort Timestamp2048;

        public double Watts
        {
            get { return 0; }
            set {  }
        }

        public double SpeedMps
        {
            get { return 0; }
            set {  }
        }

        public double StableSeconds
        {
            get { return 0; }
            set { }
        }

        public Stage Stage;

        public Motion Motion { get; set; }

        public List<TickEvent> Events { get { return m_ticks; } }

        public void AddSpeedEvent(byte[] buffer)
        {
            m_speed.AddEvent(buffer, this);
        }

        public void AddPowerEvent(ushort eventCount, ushort accumPower)
        {
            // Only record watts if stable.
            if (Stage == Calibration.Stage.Stable)
            {
                Watts = AveragePower.AddEvent(eventCount, accumPower, m_ticks);
            }
        }
    }
}
