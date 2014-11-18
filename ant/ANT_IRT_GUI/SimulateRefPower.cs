using AntPlus.Profiles.BikePower;
using IRT_GUI.IrtMessages;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace IRT_GUI
{
    public enum SimulateState
    {
        Invalid,
        Loaded,
        Running,
        Cancelling,
        Stopped
    }

    public class SimulateRefPower
    {
        private List<ushort> m_power;
        private Timer m_timer;
        private int m_position;

        public SimulateState m_state = SimulateState.Invalid;
        public event Action<StandardPowerOnlyPage, uint> SimluatedPowerMessage;
        public event Action<SimulateState> StateChanged;

        private void m_timer_Tick(object sender, EventArgs e)
        {
            if (m_state == SimulateState.Cancelling)
            {
                m_timer.Stop();
                OnStateChange(SimulateState.Stopped);
                return;

            }

            if (m_position < m_power.Count)
            {
                var msg = new StandardPowerOnlyPage() { InstantaneousPower = m_power[m_position++] };

                if (SimluatedPowerMessage != null)
                {
                    SimluatedPowerMessage(msg, 0);
                }
            }
            else
            {
                m_timer.Stop();
                OnStateChange(SimulateState.Stopped);
            }
        }

        private void OnStateChange(SimulateState newState)
        {
            m_state = newState;
            if (StateChanged != null)
            {
                StateChanged(newState);
            }
        }

        public SimulateState State { get { return m_state; } }

        public void Load()
        {
            Stream stream = null;
            OpenFileDialog dlg = new OpenFileDialog();
            m_power = new List<ushort>();

            dlg.InitialDirectory = Environment.CurrentDirectory;
            dlg.Filter = "Ride Logs (*.csv)|*.csv|All files (*.*)|*.*";
            dlg.FilterIndex = 1;
            dlg.RestoreDirectory = true;

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    if ((stream = dlg.OpenFile()) != null)
                    {
                        using (var reader = new StreamReader(stream))
                        {
                            /* Advance through first line.
                            if (!reader.EndOfStream)
                                reader.ReadLine(); */

                            while (!reader.EndOfStream)
                            {
                                var line = reader.ReadLine();
                                var values = line.Split(',');

                                if (values.Length > 5)
                                {
                                    ushort power = 0;
                                    ushort.TryParse(values[5], out power);
                                    m_power.Add(power);
                                }
                            }
                        }
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error: Could not read file from disk. Original error: " + ex.Message);
                }
            }

            OnStateChange(SimulateState.Loaded);
        }

        public void Run()
        {
            if (m_state != SimulateState.Loaded)
            {
                throw new Exception("Must be loaded to run.");
            }

            m_timer = new Timer();
            m_timer.Interval = 1000; // 1 second interval
            m_timer.Tick += m_timer_Tick;
            m_timer.Start();

            OnStateChange(SimulateState.Running);
        }

        public void Stop()
        {
            if (m_state == SimulateState.Running)
            {
                m_timer.Stop();
                OnStateChange(SimulateState.Stopped);
            }
        }
    }
}
