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
        Paused,
        Stopped
    }

    public class SimulateRefPower
    {
        private List<ushort> m_power;
        private Timer m_timer;
        private int m_position;
        private string m_lastPath = Environment.CurrentDirectory;

        public SimulateState m_state = SimulateState.Invalid;
        public event Action<StandardPowerOnlyPage, uint> SimluatedPowerMessage;
        public event Action<SimulateState> StateChanged;
        public event Action<int, int> Progress;

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

                if (Progress != null)
                {
                    Progress(m_position, m_power.Count);
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

        private Single CalculateHz(float speedMph)
        {
            const float flywheelDistPerRev = 0.11176f;
            float hz = 0.0f;

            hz = ((((speedMph * 0.44704f) / flywheelDistPerRev) / 2.0f) * 4.0f) / 2.0f;

            return hz;
        }

        public SimulateState State { get { return m_state; } }

        public void Load()
        {
            const string waveFormat = "{0}\t{1:f}\t3.3\t1\r\n";

            Stream stream = null;
            OpenFileDialog dlg = new OpenFileDialog();
            m_power = new List<ushort>();
            StringBuilder wave = new StringBuilder();

            dlg.InitialDirectory = m_lastPath;
            dlg.Filter = "Ride Logs (*.csv)|*.csv|All files (*.*)|*.*";
            dlg.FilterIndex = 1;
            dlg.RestoreDirectory = false;

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    if ((stream = dlg.OpenFile()) != null)
                    {
                        using (var reader = new StreamReader(stream))
                        {
                            // Advance through first line.
                            if (!reader.EndOfStream)
                            {
                                reader.ReadLine();
                                 // Dummy first line @1hz, 0 power
                                wave.AppendFormat(waveFormat, 2, 1.0f);
                                m_power.Add(0);
                            }

                            while (!reader.EndOfStream)
                            {
                                var line = reader.ReadLine();
                                var values = line.Split(',');

                                if (values.Length > 5)
                                {
                                    // Reference Power
                                    ushort power = 0;
                                    ushort.TryParse(values[5], out power);
                                    m_power.Add(power);

                                    // Speed
                                    float speed = 0;
                                    Single hz = 1.0f; // Pclab doesn't like 0 

                                    if (float.TryParse(values[3], out speed))
                                    {
                                        // Valid speed?
                                        if (!float.IsInfinity(speed) && !float.IsNaN(speed) && speed > 0.0f)
                                        {
                                            hz = CalculateHz(speed);
                                        }
                                        else
                                        {
                                            hz = 1.0f; // doesn't like 0
                                        }
                                    }
                                    
                                    wave.AppendFormat(waveFormat, 0, hz);
                                }
                            }

                            // Add 0's to finish it off.
                            m_power.Add(0);
                            wave.AppendFormat(waveFormat, 0, 1.0f);
                        }

                        var result = MessageBox.Show("Copy speed wave to clipboard?", "Wave Generator",
                            MessageBoxButtons.YesNo, MessageBoxIcon.Question, MessageBoxDefaultButton.Button1);

                        if (result == DialogResult.Yes)
                        {
                            Clipboard.SetText(wave.ToString());
                        }

                        m_lastPath = Path.GetDirectoryName(dlg.FileName);
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
            if (m_state != SimulateState.Paused)
            {
                m_position = 0;
                m_timer = new Timer();
                m_timer.Interval = 1000; // 1 second interval
                m_timer.Tick += m_timer_Tick;
            }

            m_timer.Start();

            OnStateChange(SimulateState.Running);
        }

        public void Stop(bool pause = false)
        {
            if (m_state == SimulateState.Running)
            {
                m_timer.Stop();
            }

            if (pause)
            {
                OnStateChange(SimulateState.Paused);
            }
            else
            {
                OnStateChange(SimulateState.Stopped);
            }
        }
    }
}
