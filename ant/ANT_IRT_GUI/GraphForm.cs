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

            chart1.ChartAreas[0].AxisX.Maximum = 60 * 10; // 10 minutes.
            chart1.ChartAreas[0].AxisX.Minimum = 0;

            chart1.ChartAreas[0].Position = new ElementPosition(5, 10, 90, 90);
            chart1.ChartAreas[0].InnerPlotPosition = new ElementPosition(5, 0, 85, 85);
            //chart1.ChartAreas[0].AxisY.TitleAlignment = StringAlignment.Near;

            ChartArea area = chart1.ChartAreas[0];
            ChartArea area2 = new ChartArea("Torque");
            area2.AxisY.Title = "Torque (Nm)";
            //area2.AxisY.TitleAlignment = StringAlignment.Far;
            area2.AxisY.LabelStyle.ForeColor = Color.Blue;
            area2.AxisY.TitleForeColor = area2.AxisY.LabelStyle.ForeColor;
            area2.AxisX.Maximum = area.AxisX.Maximum;
            chart1.ChartAreas.Add(area2);

            Series refTorqueSeries = new Series("RefTorque");
            refTorqueSeries.ChartType = SeriesChartType.FastLine;
            refTorqueSeries.ChartArea = "Torque";
            //refTorqueSeries.YAxisType = AxisType.Secondary;
            chart1.Series.Add(refTorqueSeries);

            //area2.AlignmentOrientation = AreaAlignmentOrientations.All;
            //area2.AlignWithChartArea = area.Name;
            //area2.AlignmentStyle = AreaAlignmentStyles.All;

            area2.AxisX.IsMarginVisible = false;
            area2.RecalculateAxesScale();
            area2.BackColor = Color.Transparent;
            area2.BorderColor = Color.Transparent;
            area2.Position.FromRectangleF(area.Position.ToRectangleF());
            area2.InnerPlotPosition.FromRectangleF(area.InnerPlotPosition.ToRectangleF());

            area2.AxisX.LineWidth = 0;
            area2.AxisX.MajorGrid.Enabled = false;
            area2.AxisX.MajorTickMark.Enabled = false;
            area2.AxisX.LabelStyle.Enabled = false;
            area2.AxisY.MajorGrid.Enabled = false;
            area2.AxisY.IsStartedFromZero = area.AxisY.IsStartedFromZero;
            area2.AxisY.LabelStyle.Font = area.AxisY.LabelStyle.Font;

            //area2.Position.X = area2.Position.X - 5;
            //area2.InnerPlotPosition.X = area.InnerPlotPosition.X + 5; // +15;
            
        }

        public void Report(IrtMessages.DataPoint data)
        {
            if (chart1.Series["RefPower"].Points.Count > chart1.ChartAreas[0].AxisX.Maximum)
            {
                chart1.ChartAreas[0].AxisX.Title = "Time";
                chart1.ChartAreas[0].AxisX.TitleAlignment = StringAlignment.Near;
                chart1.ChartAreas[0].AxisX.TextOrientation = TextOrientation.Horizontal;
                chart1.ChartAreas[0].AxisX.Maximum++;
                chart1.ChartAreas[0].AxisX.Minimum++;
                chart1.ChartAreas[0].AxisX.ScaleView.Scroll(chart1.ChartAreas[0].AxisX.Maximum);

                chart1.ChartAreas["Torque"].AxisX.Title = "Time";
                chart1.ChartAreas["Torque"].AxisX.TitleAlignment = StringAlignment.Near;
                chart1.ChartAreas["Torque"].AxisX.TextOrientation = TextOrientation.Horizontal;
                chart1.ChartAreas["Torque"].AxisX.Maximum++;
                chart1.ChartAreas["Torque"].AxisX.Minimum++;
                chart1.ChartAreas["Torque"].AxisX.ScaleView.Scroll(chart1.ChartAreas["Torque"].AxisX.Maximum);
            }

            chart1.Series["Speed"].Points.AddY(data.SpeedEMotionMph);
            chart1.Series["RefPower"].Points.AddY(data.PowerReference);
            chart1.Series["EMotionPower"].Points.AddY(data.PowerEMotion);
            chart1.Series["SpeedMovAvg"].Points.AddY(data.SpeedEmotionAvg);
            chart1.Series["RefPowerMovAvg"].Points.AddY(data.PowerReferenceAvg);
            
            chart1.Series["RefTorque"].Points.AddY(data.RefTorque);
        }

        public void Report(string data) { }
    }
}
