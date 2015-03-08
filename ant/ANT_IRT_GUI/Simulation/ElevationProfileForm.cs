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
        public ElevationProfileForm(SimulationMode simulator)
        {
            InitializeComponent();
            
            this.chart1.Series.Clear();
            this.chart1.Legends.Clear();

            Series series = this.chart1.Series.Add("Altitude");
            series.ChartType = SeriesChartType.FastLine;

            foreach (GpsPoint point in simulator.GpsPoints)
            {
                series.Points.AddXY(point.DistanceM, point.ElevationM);
            }
        }

        private void ElevationProfile_Load(object sender, EventArgs e)
        {
            
        }
    }
}
