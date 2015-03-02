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

namespace IRTCalibration
{
    public partial class CoastdownForm : Form
    {
        public event Action<int> Apply;
        private Coastdown m_coastdown;

        public CoastdownForm(Coastdown coastdown)
        {
            InitializeComponent();

            m_coastdown = coastdown;
            this.lblDrag.Text = coastdown.Drag.ToString();
            this.lblRR.Text = coastdown.RollingResistance.ToString();
            DrawChart();
        }

        private void btnApply_Click(object sender, EventArgs e)
        {
            if (Apply != null)
            {
                Apply(0);
            }

            this.Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void DrawChart()
        {
            var series = this.chartCoastdown.Series.Add("Coastdown");
            series.ChartType = SeriesChartType.Point;

            for (int i = 0; i < m_coastdown.Data.SpeedMps.Length; i++)
            {
                series.Points.AddXY(m_coastdown.Data.SpeedMps[i],
                    m_coastdown.Data.CoastdownSeconds[i]);
            }
        }
    }
}
