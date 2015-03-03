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

        public double Seconds
        {
            get { return 0; }
            set { }
        }

        public Stage Stage;

        public Motion Motion { get; set; }

        public void AddSpeedEvent(byte[] buffer)
        {
            m_speed.AddEvent(buffer, this);
        }

        public void AddPowerEvent(ushort eventCount, ushort accumPower)
        {
            Watts = m_power.AddEvent(eventCount, accumPower);
        }
    }
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
