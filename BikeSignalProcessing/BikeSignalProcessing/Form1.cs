using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace BikeSignalProcessing
{
    public partial class Form1 : Form
    {
        private const string ActualSeriesName = "Actual";
        private const string SmoothSeriesName = "Smoothed";
        private Data mData;
        private int mZoomStart = -1;
        private int mZoomEnd = -1;

        public Form1()
        {
            InitializeComponent();
            chart1.MouseClick += Chart1_MouseClick;
            ClearChart();
        }

        private void ClearChart()
        {
            RemoveZoomMarkers();
            chart1.Series.Clear();
        }

        private void ChartSmoothPower(double point)
        {
            Series series = chart1.Series.FindByName(SmoothSeriesName+2);
            if (series == null)
            {
                series = chart1.Series.Add(SmoothSeriesName + 2);
                series.ChartType = SeriesChartType.FastLine;
            }

            series.Points.AddY(point);
        }

        private void Chart(double[] data, string seriesName)
        {
            Series series = chart1.Series.Add(seriesName);
            series.ChartType = SeriesChartType.Line;

            //series.Points.Add(points);

            foreach (double d in data)
            {
                series.Points.AddY(d);
            }
        }

        private void Chart(double[] data, string seriesName, int start, int end)
        {
            double[] subset = new double[end - start];
            Array.Copy(data, start, subset, 0, subset.Length-1);

            Chart(subset, seriesName);
        }

        private void DrawSegmentMarkers(Segment segment)
        {
            var aStart = new HorizontalLineAnnotation();
            //aStart.AnchorDataPoint = chart1.Series[0].Points[segment.Start];

            aStart.AxisX = chart1.ChartAreas[0].AxisX;
            aStart.AxisY = chart1.ChartAreas[0].AxisY;
            aStart.IsSizeAlwaysRelative = false;
            /*aStart.SetAnchor(
                aStart.AnchorDataPoint = chart1.Series[0].Points[segment.Start],
                aStart.AnchorDataPoint = chart1.Series[0].Points[segment.End]); */

            aStart.X = segment.Start; 
            aStart.Right = segment.End; 
            aStart.Y = segment.Power; 

            aStart.ClipToChartArea = chart1.ChartAreas[0].Name;
            aStart.LineColor = Color.Green;
            aStart.LineWidth = 3;
            aStart.LineDashStyle = ChartDashStyle.Dot;
            aStart.EndCap = LineAnchorCapStyle.Round;
            aStart.AllowMoving = false;
            aStart.IsInfinitive = false;

            //aStart.X = segment.Start;
            //aStart.Y = segment.Power;
            //aStart.Bottom = 0; // segment.Power - segment.StdDev;
            //aStart.Right = segment.End;
            //aStart.StartCap = LineAnchorCapStyle.Square;

            chart1.Annotations.Add(aStart);
        }
        
        private void RemoveSegments()
        {
            // hack for now.
            chart1.Annotations.Clear();
        }

        private void ChartSegments(double[] data)
        {
            // Remove any old segments.
            RemoveSegments(); 

            if (mData == null)
                return;

            List<Segment> segments = PowerSmoothing.GetSegments(data,
                mData.Threshold, mData.Window);

            if (segments == null)
                return;

            foreach (var seg in segments)
            {
                System.Diagnostics.Debug.WriteLine("Start: {0}, End: {1}, Power: {2}",
                    seg.Start, seg.End, seg.Power);
                DrawSegmentMarkers(seg);
            }
        }

        private void ChartSegments(int start, int end)
        {
            double[] sample = new double[end - start];
            Array.Copy(mData.SmoothedPower, start, sample, 0, end - start);

            ChartSegments(sample);
        }

        private void ChartSegments()
        {
            ChartSegments(mData.SmoothedPower);
        }

        private void SmoothData(string filename)
        {
            // Open a CSV file, grab speed, power, servo pos columns
            // output smoothed power signal vs. actual power signal
            if (filename != null)
            {
                mData = new Data(filename);
            }

            if (mData != null)
            {
                upDownThreshold.Value = (decimal)mData.Threshold;

                PowerSmoothing smoother = new PowerSmoothing();

                foreach (double d in mData.RawPower)
                {
                    double smoothed = smoother.SmoothPower(d);
                    ChartSmoothPower(smoothed);
                }

                Chart(mData.RawPower, ActualSeriesName);
                //Chart(mData.Power5secMA, "Moving Average (5 sec)");
                Chart(mData.SmoothedPower, SmoothSeriesName);
                //Chart(mData.Power10secMA, "Moving Average (10 sec)");

                ChartSegments();
            }
        }

        private void Load()
        {
            OpenFileDialog dlg = new OpenFileDialog();
            //dlg.InitialDirectory = m_lastPath;
            dlg.Filter = "Ride Logs (*.csv)|*.csv|All files (*.*)|*.*";
            dlg.FilterIndex = 1;
            dlg.RestoreDirectory = false;
            dlg.CheckFileExists = true;
            dlg.Multiselect = false;

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    SmoothData(dlg.FileName);
                }
                catch (Exception ex)
                {
                    //UpdateStatus("Error attempting to parse calibration file.\r\n" +
                    //  ex.Message);
                }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Load();
        }

        private void RemoveZoomMarkers()
        {
            chart1.Annotations.Clear();
            return;

            System.Diagnostics.Debug.WriteLine("Removing vertical lines.");
            chart1.UpdateAnnotations();

            if (chart1.Annotations.Count < 1)
                return;

            foreach (var a in chart1.Annotations)
            {
                if (a is VerticalLineAnnotation)
                    chart1.Annotations.Remove(a);
            }
        }

        /// <summary>
        /// Draws start and end lines for selecting a region to zoom into.
        /// </summary>
        /// <param name="xPosition"></param>
        private void DrawZoomMarker(double xPosition)
        {
            if (mZoomStart > 0 && mZoomEnd > 0)
            {
                // Erase all the lines and start over.
                RemoveZoomMarkers();
                mZoomStart = -1;
                mZoomEnd = -1;
            }

            var line = new VerticalLineAnnotation();

            if (mZoomStart == -1)
            {
                mZoomStart = (int)xPosition;
                line.LineColor = Color.Green;
                
            }
            else
            {
                if ((int)xPosition <= mZoomStart)
                    return;

                mZoomEnd = (int)xPosition;
                line.LineColor = Color.Red;
            }

            line.X = xPosition;
            line.LineWidth = 2;
            line.AxisX = chart1.ChartAreas[0].AxisX;
            line.AllowMoving = true;
            line.IsInfinitive = true;
            line.ClipToChartArea = chart1.ChartAreas[0].Name;

            chart1.Annotations.Add(line);
        }

        private void Zoom()
        {
            if (mZoomStart == -1 || mZoomEnd == -1)
                return;

            ClearChart();

            PowerSmoothing smoother = new PowerSmoothing();

            for (int i = mZoomStart; i < mZoomEnd; i++)
            {
                double d = mData.RawPower[i];

                double smoothed = smoother.SmoothPower(d);
                ChartSmoothPower(smoothed);
            }

            Chart(mData.RawPower, "Actual", mZoomStart, mZoomEnd);
            Chart(mData.SmoothedPower, "Smoothed", mZoomStart, mZoomEnd);
            //Chart(mData.Power5secMA, "Moving Average (5 sec)", x[0], x[1]);
            //Chart(mData.Power10secMA, "Moving Average (10 sec)", x[0], x[1]);

            ChartSegments(mZoomStart, mZoomEnd);
        }

        private void Chart1_MouseClick(object sender, MouseEventArgs e)
        {
            var chartArea = chart1.ChartAreas[0];
            double x = chartArea.AxisX.PixelPositionToValue(e.X);

            DrawZoomMarker(x);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Zoom();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            // Reset
            ClearChart();
            SmoothData(null);
        }

        private void upDownThreshold_ValueChanged(object sender, EventArgs e)
        {
            mData.Threshold = (double)upDownThreshold.Value;
            ChartSegments();
        }
    }
}
