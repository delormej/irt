using IRT.Calibration;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace IRT.Calibration
{
    public partial class CoastdownForm : Form
    {
        public event Action<Coastdown> Apply;
        private Coastdown m_coastdown;
        private Model m_model;
        private Timer m_timer;
        private int m_secondsUntilApply;

        private readonly string instructions = ""; /*
            "In {0} second(s) calibration parameters will be applied.\r\n" +
            "Close this dialog or begin another coastdown to interrupt."; */

        public CoastdownForm(Coastdown coastdown, Model model)
        {
            InitializeComponent();

            m_coastdown = coastdown;
            m_model = model;

            UpdateValues();
            //StartTimer();
        }

        private void btnApply_Click(object sender, EventArgs e)
        {
            OnApply();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void OnApply()
        {
            if (Apply != null)
            {
                Apply(m_coastdown);
            }

            this.Close();
        }

        private void StartTimer()
        {
            m_timer = new Timer();

            m_secondsUntilApply = 15;
            m_timer.Interval = 1000;
            m_timer.Tick += m_timer_Tick;
            m_timer.Start();
        }

        void m_timer_Tick(object sender, EventArgs e)
        {
            if (m_secondsUntilApply > 0)
            {
                /*lblInstructions.Text =
                    string.Format(instructions, m_secondsUntilApply--);*/
            }
            else
            {
                OnApply();
            }
        }

        private void DrawChart()
        {
            chartCoastdown.ChartAreas.Clear();
            var coastdownArea = chartCoastdown.ChartAreas.Add("Coastdown");
            coastdownArea.AlignmentOrientation = AreaAlignmentOrientations.Horizontal;
            
            chartCoastdown.Series.Clear();

            var series1 = chartCoastdown.Series.Add("Computed");
            series1.ChartType = SeriesChartType.Spline;

            var series2 = chartCoastdown.Series.Add("Actual");
            series2.ChartType = SeriesChartType.Point;

            chartCoastdown.ChartAreas["Coastdown"].AxisY.Minimum = 0;
            chartCoastdown.ChartAreas["Coastdown"].AxisX.Minimum = 0;

            chartCoastdown.Legends[0].Docking = Docking.Bottom;

            chartCoastdown.ChartAreas["Coastdown"].AxisY.Title = "Coastdown (seconds)";
            chartCoastdown.ChartAreas["Coastdown"].AxisX.Title = "Speed (mph)";

            // Plot the calculated curve line first.
            for (double mph = 2; mph < 35; mph++)
            {
                var time = m_coastdown.CoastdownTime(mph * 0.44704);
                series1.Points.AddXY(mph, time);
            }

            // Plot the actual values as points.
            for (int i = 0; i < m_coastdown.Data.SpeedMps.Length; i++)
            {
                series2.Points.AddXY(
                    Math.Round(m_coastdown.Data.SpeedMps[i] * 2.23694, 1),
                    m_coastdown.Data.CoastdownSeconds[i]);
            }

            // Second chart with power curve.
            var powerArea = chartCoastdown.ChartAreas.Add("Power");
            powerArea.AlignmentOrientation = AreaAlignmentOrientations.Horizontal;
            
            var wattSeries = chartCoastdown.Series.Add("Watts");
            wattSeries.ChartArea = "Power";
            wattSeries.ChartType = SeriesChartType.Spline;

            chartCoastdown.ChartAreas["Power"].AxisY.Title = "Power (watts)";
            chartCoastdown.ChartAreas["Power"].AxisX.Title = "Speed (mph)";
            chartCoastdown.ChartAreas["Power"].AxisX.RoundAxisValues();
            //chartCoastdown.ChartAreas["Coastdown"].AxisX = chartCoastdown.ChartAreas["Power"].AxisX;

            chartCoastdown.Series["Watts"].ToolTip = "Watts: #VALY{N0}\nMph: #VALX{N1}";

            // Plot the watts at stable speed.
            int stablePoint = wattSeries.Points.AddXY(m_model.StableSpeedMps * 2.23694, m_model.StableWatts);
            wattSeries.Points[stablePoint].MarkerStyle = MarkerStyle.Star10;
            wattSeries.Points[stablePoint].MarkerSize = 10;
            wattSeries.Points[stablePoint].MarkerColor = Color.Gold;

            for (double mph = 2; mph < 35; mph++)
            {
                var watts = m_coastdown.Watts(mph * 0.44704);
                int i = wattSeries.Points.AddXY(mph, watts);

                if (mph % 5 == 0)
                {
                    wattSeries.Points[i].MarkerStyle = MarkerStyle.Circle;
                    wattSeries.Points[i].MarkerSize = 5;
                }
            }
        }

        private void UpdateValues()
        {
            this.txtDrag.Text = String.Format("{0:0.0000000}",  m_coastdown.Drag);
            this.txtRR.Text = String.Format("{00:0.0000000}", m_coastdown.RollingResistance);
            this.lblStableSeconds.Text = String.Format("{0:0.0}", m_model.StableSeconds);
            this.txtStableSpeed.Text = String.Format("{0:0.0}", m_model.StableSpeedMps * 2.23694);
            this.txtStableWatts.Text = m_model.StableWatts.ToString();

            DrawChart();
        }

        private void btnRecalc_Click(object sender, EventArgs e)
        {
            double stableMph;

            if (double.TryParse(txtStableSpeed.Text, out stableMph) &&
                double.TryParse(txtStableWatts.Text, out m_model.StableWatts))
            {
                m_model.StableSpeedMps = stableMph * 0.44704;
                m_model.Inertia = 0; // 0 out so it recalculates.

                m_coastdown.Calculate(m_model);
                UpdateValues();
            }
        }

        private void btnCalcWatts_Click(object sender, EventArgs e)
        {
            double stableMph;

            if (double.TryParse(txtStableSpeed.Text, out stableMph))
            {
                this.txtStableWatts.Text = 
                    string.Format("{0:0}", m_coastdown.Watts(stableMph * 0.44704));
            }
        }

        private void btnReset_Click(object sender, EventArgs e)
        {
            UpdateValues();
        }
    }
}
