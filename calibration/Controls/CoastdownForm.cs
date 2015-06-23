using IRT.Calibration;
using IRT.Calibration.Controls;
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
            recalculateToolStripMenuItem.Click += btnRecalc_Click;
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
            
            chartCoastdown.Series.Clear();
            chartCoastdown.Legends[0].Docking = Docking.Bottom;

            coastdownArea.AxisX.Title = "Coastdown (seconds)";
            coastdownArea.AxisY.Title = "Speed (mph)";

            // Second chart with power curve.
            var powerArea = chartCoastdown.ChartAreas.Add("Power");

            powerArea.AlignWithChartArea = "Coastdown";

            PlotActualCoastDown(m_coastdown.Data.SpeedMps, 
                m_coastdown.Data.Acceleration);
            PlotComputedCoastDown();

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
            series1.ChartArea = "Coastdown";

            // Get the starting point.
            double mph = Math.Floor(m_coastdown.Data.SpeedMps.Min() * 2.23694);
            double max = Math.Ceiling(m_coastdown.Data.SpeedMps.Max() * 2.23694);

            // Plot the calculated curve line first.
            while (mph < max)
            {
                //var time = m_coastdown.Data.SpeedMps(mph * 0.44704);
                double acceleration = m_coastdown.Deceleration(mph * 0.44704);
                series1.Points.AddXY(mph, acceleration);
                mph++;
            }
        }

        /// <summary>
        /// Plot actual coast down time/speed values.
        /// </summary>
        /// <param name="speedMps"></param>
        /// <param name="acceleration"></param>
        public void PlotActualCoastDown(double[] speedMps, double[] acceleration, string name = "Actual")
        {
            Series series2 = null;

            if (!chartCoastdown.Series.IsUniqueName(name))
            {
                name = chartCoastdown.Series.NextUniqueName();
            }

            series2 = chartCoastdown.Series.Add(name);
            series2.ChartType = SeriesChartType.Point;
            series2.ChartArea = "Coastdown";
            series2.ToolTip = "Acceleration: #VALY{N2}\nMph: #VALX{N1}";

            chartCoastdown.ChartAreas["Coastdown"].AxisY.Minimum = acceleration.Min();
            chartCoastdown.ChartAreas["Coastdown"].AxisY.Maximum = acceleration.Max();

            //chartCoastdown.ChartAreas["Coastdown"].AxisX.Minimum = 0;

            // Plot the actual values as points.
            for (int i = 0; i < acceleration.Length; i++)
            {
                series2.Points.AddXY(
                    Math.Round(speedMps[i] * 2.23694, 1),
                    acceleration[i]);
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
            Series wattSeries = chartCoastdown.Series.FindByName(seriesName);
            
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

        private void RecalculateCoastdown()
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

        private void RecalculatePower()
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
            RecalculateCoastdown();
        }

        private void btnCalcWatts_Click(object sender, EventArgs e)
        {
            RecalculatePower();
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
                RecalculatePower();
            }
        }

        private void txtDrag_Leave(object sender, EventArgs e)
        {
            if (txtDrag.Modified)
            {
                double drag;
                if (double.TryParse(txtDrag.Text, out drag))
                {
                    m_coastdown.Drag = drag;
                    RecalculatePower();
                }
            }
        }

        private void txtRR_Leave(object sender, EventArgs e)
        {
            if (txtRR.Modified)
            {
                double rr;
                if (double.TryParse(txtRR.Text, out rr))
                {
                    m_coastdown.RollingResistance = rr;
                    RecalculatePower();
                }
            }
        }

        private void dataToolStripMenuItem_Click(object sender, EventArgs e)
        {
            CoastdownDataForm form = new CoastdownDataForm(m_coastdown.Data);
            form.Show();
        }
    }
}
