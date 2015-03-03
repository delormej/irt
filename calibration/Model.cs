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
        private AveragePower m_power;

        public event PropertyChangedEventHandler PropertyChanged;

        public Model()
        {
            m_speed = new Speed(Settings.StableThresholdSpeedMps);
            m_power = new AveragePower();
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

        public List<TickEvent> Events { get { return m_speed.Ticks; } }

        public void AddSpeedEvent(byte[] buffer)
        {
            m_speed.AddEvent(buffer, this);
        }

        public void AddPowerEvent(ushort eventCount, ushort accumPower)
        {
            // Always keep track of events.
            double watts = m_power.AddEvent(eventCount, accumPower);

            // Only publish watts if stable.
            if (Stage == Calibration.Stage.Stable)
            {
                Watts = watts;
            }
        }
    }
}
