using ANT_Console;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace IRT_GUI
{
    public partial class GraphForm : Form, IReporter
    {
        public GraphForm()
        {
            InitializeComponent();

            chart1.ChartAreas[0].AxisX.Maximum = 60*10; // 10 minutes.
            chart1.ChartAreas[0].AxisX.Minimum = 0;
        }

        public void Report(IrtMessages.DataPoint data)
        {
            if (chart1.Series["RefPower"].Points.Count > chart1.ChartAreas[0].AxisX.Maximum)
            {
                chart1.ChartAreas[0].AxisX.Maximum++;
                chart1.ChartAreas[0].AxisX.Minimum++;
                chart1.ChartAreas[0].AxisX.ScaleView.Scroll(chart1.ChartAreas[0].AxisX.Maximum);
            }

            chart1.Series["RefPower"].Points.AddY(data.PowerReference);
            chart1.Series["EMotionPower"].Points.AddY(data.PowerEMotion);
            chart1.Series["Speed"].Points.AddY(data.SpeedEMotion);
            chart1.Series["SpeedMovAvg"].Points.AddY(data.SpeedEmotionAvg);
            chart1.Series["RefPowerMovAvg"].Points.AddY(data.PowerReferenceAvg);
        }

        public void Report(string data) { }
    }
}
