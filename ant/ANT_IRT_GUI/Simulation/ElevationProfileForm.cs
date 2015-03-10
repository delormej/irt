using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace IRT_GUI.Simulation
{
    public partial class ElevationProfileForm : Form
    {
        private VerticalLineAnnotation m_positionLine;
        private TextAnnotation m_slopeText;
        private SimulationMode m_simulator;

        /// <summary>
        /// Current position in meters.
        /// </summary>
        public double Position
        {
            get 
            {
                if (m_positionLine != null)
                {
                    return m_positionLine.X;
                }
                else
                {
                    return 0;
                }
            }
            set 
            {
                if (m_positionLine != null)
                {
                    try
                    {
                        m_positionLine.X = value;
                    }
                    finally
                    {
                        // When closing the form in the middle of a simulation, a funky state can occur here.
                    }
                }
            } 
        }

        public ElevationProfileForm(SimulationMode simulator)
        {
            if (simulator == null || simulator.GpsPoints.Length == 0)
            {
                return;
            }

            InitializeComponent();
            
            this.chart1.Series.Clear();
            this.chart1.Legends.Clear();

            Series series = this.chart1.Series.Add("Altitude");
            series.ChartType = SeriesChartType.FastLine;

            this.chart1.ChartAreas[0].AxisX.MajorGrid.Enabled = false;
            this.chart1.ChartAreas[0].AxisX.MajorTickMark.Enabled = false;
            this.chart1.ChartAreas[0].AxisX.LabelStyle.Enabled = false;

            this.chart1.ChartAreas[0].AxisY.MajorGrid.LineWidth = 1;
            this.chart1.ChartAreas[0].AxisY.MajorGrid.LineDashStyle = ChartDashStyle.Dot;
            this.chart1.ChartAreas[0].AxisY.MajorGrid.LineColor = Color.LightGray;

            /*
            this.chart1.ChartAreas[0].AxisX.LabelStyle.Format = "{0}";
            this.chart1.ChartAreas[0].AxisX.LabelStyle.Interval = 1000;
            this.chart1.ChartAreas[0].AxisX.IntervalOffset = 3; */

            foreach (GpsPoint point in simulator.GpsPoints)
            {
                series.Points.AddXY(point.DistanceM, point.ElevationM);
            }

            m_positionLine = new VerticalLineAnnotation();
            m_positionLine.ClipToChartArea = this.chart1.ChartAreas[0].Name;
            m_positionLine.IsInfinitive = true;
            m_positionLine.Width = 3;
            m_positionLine.LineColor = Color.Red;
            m_positionLine.AxisX = this.chart1.ChartAreas[0].AxisX;

            // Initial position
            m_positionLine.X = 0;

            this.chart1.Annotations.Add(m_positionLine);

            // Text annotation to show the current slope.
            m_slopeText = new TextAnnotation();
            m_slopeText.Font = new System.Drawing.Font("Microsoft Sans Serif", 18.0f);
            m_slopeText.AxisX = this.chart1.ChartAreas[0].AxisX;
            m_slopeText.AxisY = this.chart1.ChartAreas[0].AxisY;
            m_slopeText.X = 0;
            m_slopeText.Y = 0;

            this.chart1.Annotations.Add(m_slopeText);

            m_simulator = simulator;

            m_simulator.PositionChanged += simulator_PositionChanged;
            m_simulator.SlopeChanged += simulator_SlopeChanged;
            m_simulator.SimulationEnded += simulator_SimulationEnded;
        }

        void simulator_SimulationEnded()
        {
            if (m_simulator != null)
            {
                // Unsubscribe from events.
                m_simulator.PositionChanged -= simulator_PositionChanged;
                m_simulator.SlopeChanged -= simulator_SlopeChanged;
                m_simulator.SimulationEnded -= simulator_SimulationEnded;
            }
            // Close the form.
            this.Close();
        }

        void simulator_SlopeChanged(double slope)
        {
            m_slopeText.Text = string.Format("{0:0.0} %", slope);
            m_slopeText.Y = m_simulator.GpsPoints[m_simulator.Index].ElevationM;
        }

        void simulator_PositionChanged(double distance)
        {
            this.Position = distance;
            m_slopeText.X = distance;
        }

        private void ElevationProfile_Load(object sender, EventArgs e)
        {
            
        }

        private void ElevationProfileForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            // Unsubscribe from event, so we're not invoked twice.
            m_simulator.SimulationEnded -= simulator_SimulationEnded;

            // End the simulator
            m_simulator.End();
        }
    }
}
