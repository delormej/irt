using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;

namespace ANT_Console.Services
{
    public class SpeedSimulator : IDisposable
    {
        Timer m_timer;
        //SerialPort m_port;
        UnmanagedIRSerialPort m_port;
        bool m_state = false;

        public SpeedSimulator() 
        {
            /*
            m_port = new SerialPort("COM3", 38400);
            m_port.RtsEnable = true;
            m_port.StopBits = StopBits.One;
            m_port.Parity = Parity.None;
            m_port.DataBits = 7;
            m_port.Handshake = Handshake.None;

            m_port.Open();
            */

            m_port = new UnmanagedIRSerialPort("COM5", 115200); // 38400);
            m_port.Open();

            m_timer = new Timer(Simulate_Callback);
        }

        /*
        public SpeedSimulator(SerialPort port)
        {
            m_port = port;
            m_timer = new Timer(Simulate_Callback);
        } */

        private void Simulate_Callback(object state)
        {
            // Flip the bit.
            //m_port.RtsEnable = !m_port.RtsEnable;

            //lock (this)
            //{
                if (m_state)
                {
                    m_port.On();
                }
                else
                {
                    m_port.Off();
                }

                // Flip the state.
                m_state = !m_state;
            //}
        }

        public void Simulate(float speed_mps)
        {
            TimeSpan interval = GetInterval(speed_mps);
            m_timer.Change(interval, interval);
        }

        private TimeSpan GetInterval(float speed_mps)
        {
            float wheelsPerSec = speed_mps / 2.07f; // static wheel size in meters.
            float flyWheelPerSec = wheelsPerSec * 18.5218f;

            // 2 events per flywheel
            long ticks = (long)(((1 / (flyWheelPerSec)) * TimeSpan.TicksPerSecond) / 2);

            TimeSpan interval = new TimeSpan(ticks);
            //TimeSpan interval = new TimeSpan(0, 0, 0, 0, 8);

            return interval;
        }

        public void Dispose()
        {
            if (m_timer != null)
            {
                m_timer.Dispose();
            }

            if (m_port != null)
            {
                m_port.Close();
            }
        }
    }
}
