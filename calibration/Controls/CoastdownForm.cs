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

            evaluateToolStripMenuItem.Click += btnCalcWatts_Click;
            resetToolStripMenuItem.Click += btnReset_Click;
            recalculateToolStripMenuItem.Click += btnReset_Click;
            applyToolStripMenuItem.Click += btnApply_Click;
            exitToolStripMenuItem.Click += btnCancel_Click;

            m_coastdown = coastdown;
            m_model = model;

            UpdateValues();
            DrawChart();
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
            //
            // Setup the chart.
            //
            chartCoastdown.ChartAreas.Clear();
            var coastdownArea = chartCoastdown.ChartAreas.Add("Coastdown");
            coastdownArea.AlignmentOrientation = AreaAlignmentOrientations.Horizontal;
            
            chartCoastdown.Series.Clear();

            chartCoastdown.ChartAreas["Coastdown"].AxisY.Minimum = 0;
            chartCoastdown.ChartAreas["Coastdown"].AxisX.Minimum = 0;

            chartCoastdown.Legends[0].Docking = Docking.Bottom;

            chartCoastdown.ChartAreas["Coastdown"].AxisX.Title = "Coastdown (seconds)";
            chartCoastdown.ChartAreas["Coastdown"].AxisY.Title = "Speed (mph)";

            // Second chart with power curve.
            var powerArea = chartCoastdown.ChartAreas.Add("Power");
            powerArea.AlignmentOrientation = AreaAlignmentOrientations.Horizontal;

            // Plot data on the chart.
            PlotComputedCoastDown();
            PlotActualCoastDown(m_coastdown.Data.CoastdownSeconds, 
                m_coastdown.Data.SpeedMps);
            PlotStableWatts(m_model.StableSpeedMps * 2.23694, m_model.StableWatts);
            PlotWatts("Watts");
        }

        /// <summary>
        /// Plots coastdown data.
        /// </summary>
        private void PlotComputedCoastDown()
        {
            var series1 = chartCoastdown.Series.Add("Computed");
            series1.ChartType = SeriesChartType.Spline;

            // Plot the calculated curve line first.
            for (double mph = 2; mph < 35; mph++)
            {
                var time = m_coastdown.CoastdownTime(mph * 0.44704);
                series1.Points.AddXY(time, mph);
            }
        }

        /// <summary>
        /// Plot actual coast down time/speed values.
        /// </summary>
        /// <param name="time"></param>
        /// <param name="speed"></param>
        public void PlotActualCoastDown(double[] time, double[] speed, string name = "Actual")
        {
            Series series2 = null;

            if (!chartCoastdown.Series.IsUniqueName(name))
            {
                name = chartCoastdown.Series.NextUniqueName();
            }

            series2 = chartCoastdown.Series.Add(name);
            series2.ChartType = SeriesChartType.Point;

            // Plot the actual values as points.
            for (int i = 0; i < speed.Length; i++)
            {
                series2.Points.AddXY(
                    time[i],
                    Math.Round(speed[i] * 2.23694, 1));
            }
        }

        /// <summary>
        /// Plots a marker showing the stable watts.
        /// </summary>
        public void PlotStableWatts(double speed_mph, double watts)
        {
            // Plot the watts at stable speed.
            Series stablePowerSeries = null;
            string name = "StablePower";

            if (!chartCoastdown.Series.IsUniqueName(name))
            { 
                // Add additional series name
                name = chartCoastdown.Series.NextUniqueName();
            }

            stablePowerSeries = chartCoastdown.Series.Add(name);

            stablePowerSeries.ChartArea = "Power";
            stablePowerSeries.ChartType = SeriesChartType.Point;

            int stablePoint = stablePowerSeries.Points.AddXY(speed_mph, watts);
            stablePowerSeries.Points[stablePoint].MarkerStyle = MarkerStyle.Diamond;
            stablePowerSeries.Points[stablePoint].MarkerSize = 8;
            stablePowerSeries.ToolTip = "Watts: #VALY{N0}\nMph: #VALX{N1}";
        }

        /// <summary>
        /// Plots power data on the chart.
        /// </summary>
        /// <param name="seriesName"></param>
        private void PlotWatts(string seriesName)
        {
            Series wattSeries = chartCoastdown.Series.FirstOrDefault(x => x.Name == seriesName);

            if (wattSeries != null)
            {
                // Remove if already exists.
                chartCoastdown.Series.Remove(wattSeries);
            }

            var powerArea = chartCoastdown.ChartAreas["Power"];

            wattSeries = chartCoastdown.Series.Add(seriesName);
            wattSeries.ChartArea = "Power";
            wattSeries.ChartType = SeriesChartType.Spline;

            chartCoastdown.ChartAreas["Power"].AxisY.Title = "Power (watts)";
            chartCoastdown.ChartAreas["Power"].AxisX.Title = "Speed (mph)";
            chartCoastdown.ChartAreas["Power"].AxisX.RoundAxisValues();
            //chartCoastdown.ChartAreas["Coastdown"].AxisX = chartCoastdown.ChartAreas["Power"].AxisX;

            chartCoastdown.Series[seriesName].ToolTip = "Watts: #VALY{N0}\nMph: #VALX{N1}";

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
        }

        private void RecalculateWatts()
        {
            double stableMph;

            if (double.TryParse(txtStableSpeed.Text, out stableMph))
            {
                this.txtStableWatts.Text =
                    string.Format("{0:0}", m_coastdown.Watts(stableMph * 0.44704));
            }
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
                PlotWatts("Recalculated");
            }
        }

        private void btnCalcWatts_Click(object sender, EventArgs e)
        {
            RecalculateWatts();
        }

        private void btnReset_Click(object sender, EventArgs e)
        {
            UpdateValues();
            DrawChart();
        }

        private void txtStableSpeed_Leave(object sender, EventArgs e)
        {
            if (txtStableSpeed.Modified)
            {
                RecalculateWatts();
            }
        }
    }
}
