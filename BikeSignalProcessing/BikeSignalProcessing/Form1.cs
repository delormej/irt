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
using MathNet.Numerics.Statistics;

namespace BikeSignalProcessing
{
    public partial class Form1 : Form
    {
        private const string ActualSeriesName = "Actual";
        private const string SmoothSeriesName = "Smoothed";
        private Data mData;
        private Data mData2;
        private AsyncCsvFactory asyncCsv;

        private int mZoomStart = -1;
        private int mZoomEnd = -1;

        public double Threshold = 4.0;
        public int Window = 10;

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

            int i = mZoomStart > 0 ? mZoomStart : 0;
            int end = mZoomEnd > 0 ? mZoomEnd : data.Length;

            do
            {
                double d = data[i];
                series.Points.AddY(d);
            } while (i++ < end);
        }

        private void DrawSegment(Segment segment)
        {
            if (segment == null || segment.End == 0)
                return;

            var line = new HorizontalLineAnnotation();
            //aStart.AnchorDataPoint = chart1.Series[0].Points[segment.Start];

            line.AxisX = chart1.ChartAreas[0].AxisX;
            line.AxisY = chart1.ChartAreas[0].AxisY;
            line.IsSizeAlwaysRelative = false;
            /*aStart.SetAnchor(
                aStart.AnchorDataPoint = chart1.Series[0].Points[segment.Start],
                aStart.AnchorDataPoint = chart1.Series[0].Points[segment.End]); */

            line.X = segment.Start; 
            line.Right = segment.End; 
            line.Y = segment.AveragePower; 

            line.ClipToChartArea = chart1.ChartAreas[0].Name;
            line.LineColor = Color.Green;
            line.LineWidth = 3;
            line.LineDashStyle = ChartDashStyle.Dot;
            line.EndCap = LineAnchorCapStyle.Round;
            line.AllowMoving = false;
            line.IsInfinitive = false;

            chart1.Annotations.Add(line);

            string summary = string.Format("Duration: {0}\r\nStdDev: {1:N1}\r\nSpeed: {2:N1}\r\nWatts: {3:N0}\r\n Position: {4}",
                (segment.End - segment.Start), segment.StdDev,
                segment.AverageSpeed, segment.AveragePower, segment.ServoPosition);

            /*var text = new TextAnnotation();
            text.Text = summary;
            text.AxisX = chart1.ChartAreas[0].AxisX;
            text.AxisY = chart1.ChartAreas[0].AxisY;
            text.X = segment.Start;
            text.Y = segment.AveragePower;

            chart1.Annotations.Add(text);*/

            // Look for or create series in mag chart.
            Series mag = chart1.Series.FindByName(segment.ServoPosition.ToString());
            if (mag == null)
            {
                mag = chart1.Series.Add(segment.ServoPosition.ToString());
                mag.ChartArea = "Magnet";
                mag.ChartType = SeriesChartType.Point;
            }

            System.Windows.Forms.DataVisualization.Charting.DataPoint d =
                new System.Windows.Forms.DataVisualization.Charting.DataPoint(
                    segment.AverageSpeed, segment.AveragePower);
            d.MarkerSize = ((segment.End - segment.Start) / 20) * 5;
            //d.ToolTip = summary;
            //d.Label = summary;
            mag.Points.Add(d);
        }
        
        private void RemoveSegments()
        {
            // hack for now.
            chart1.Annotations.Clear();
        }

        private void ChartSegments(IEnumerable<Segment> segments)
        {
            // Remove any old segments.
            RemoveSegments(); 

            if (segments == null)
                return;

            foreach (var seg in segments)
            {
                System.Diagnostics.Debug.WriteLine("Start: {0}, End: {1}, Power: {2}",
                    seg.Start, seg.End, seg.AveragePower);
                DrawSegment(seg);
            }
        }

        
        private void ChartSegments(int start, int end)
        {
            /*
            double[] sample = new double[end - start];
            Array.Copy(mData.SmoothedPower, start, sample, 0, end - start);

            ChartSegments(sample);
            */
        }

        private void ChartSegments()
        {
            //ChartSegments(mData.SmoothedPower);
        }

        private void BindChart(Data data)
        {
            chart1.Series.Clear();
            chart1.ChartAreas.Clear();

            ChartArea rideArea = chart1.ChartAreas.Add("Ride");
            rideArea.Position.Width = 70;
            rideArea.Position.Height = 100;
            rideArea.Position.X = 0;
            rideArea.AxisX.IntervalType = DateTimeIntervalType.Seconds;
            rideArea.AxisY2.Maximum = 40; // limit to 40 mph.

            ChartArea magArea = chart1.ChartAreas.Add("Magnet");
            magArea.Position.Width = 30;
            magArea.Position.Height = 100;
            magArea.AlignWithChartArea = "Ride";
            magArea.AlignmentOrientation = AreaAlignmentOrientations.Horizontal;
            magArea.AlignmentStyle = AreaAlignmentStyles.AxesView;
            magArea.Position.X = 70;
            magArea.AxisX.Interval = 3;
            magArea.AxisX.LabelStyle.Format = "{0:0} mph";

            chart1.DataSource = mData2.DataPoints;

            mData2.DataPoints.CollectionChanged += (object sender, 
                System.Collections.Specialized.NotifyCollectionChangedEventArgs e) =>
            {
                Action a = () => {
                    try
                    {
                        chart1.DataBind();
                    }
                    catch (Exception ex)
                    {
                        System.Diagnostics.Debug.WriteLine(ex);
                    }
                };

                // Chart doesn't seem to catch collection changed, so force update.
                if (this.InvokeRequired)
                {
                    this.BeginInvoke(a);
                }
                else
                {
                    this.Invoke(a);
                }

                //chart1.DataBind();
            };

            Series actualPower = chart1.Series.Add(ActualSeriesName);
            actualPower.ChartType = SeriesChartType.FastLine;
            actualPower.YAxisType = AxisType.Primary;
            actualPower.YValueMembers = "PowerWatts";
            //actualPower.Points.DataBind(mData2, "Seconds", "PowerWatts", "");
            //actualPower.Points.DataBindXY(mData2, "Seconds", mData2, "PowerWatts");
            actualPower.ChartArea = "Ride";

            Series smoothPower = chart1.Series.Add(SmoothSeriesName);
            smoothPower.ChartType = SeriesChartType.FastLine;
            smoothPower.YValueMembers = "SmoothedPowerWatts";
            smoothPower.ChartArea = "Ride";

            Series actualSpeed = chart1.Series.Add("Speed (mph)");
            actualSpeed.ChartType = SeriesChartType.FastLine;
            actualSpeed.YValueMembers = "SpeedMph";
            actualSpeed.YAxisType = AxisType.Secondary;
            actualSpeed.ChartArea = "Ride";

            Series smoothSpeed = chart1.Series.Add("Smoothed Speed");
            smoothSpeed.ChartType = SeriesChartType.FastLine;
            smoothSpeed.YValueMembers = "SmoothedSpeedMph";
            smoothSpeed.YAxisType = AxisType.Secondary;
            smoothSpeed.ChartArea = "Ride";
        }

        private void SmoothData(string filename)
        {
            // Open a CSV file, grab speed, power, servo pos columns
            // output smoothed power signal vs. actual power signal
            if (filename != null)
            {
                mData2 = (Data)IrtCsvFactory.Open(filename);

                //asyncCsv = new AsyncCsvFactory();
                //mData2 = asyncCsv.Open(filename);

                BindChart(mData2);
                ChartSegments(mData2.StableSegments);
                mData2.SegmentDetected += MData2_SegmentDetected;

                foreach (Segment seg in mData2.StableSegments.OrderBy(s => s.AverageSpeed))
                {
                    System.Diagnostics.Debug.WriteLine("Speed: {0:N1}, Len: {1:N0}",
                        seg.AverageSpeed, seg.End - seg.Start);
                }

                System.Diagnostics.Debug.WriteLine("BEST...");

                var best = FindBest(mData2.StableSegments);
                foreach (Segment seg in best)
                {
                    System.Diagnostics.Debug.WriteLine("Speed: {0:N1}, Len: {1:N0}",
                        seg.AverageSpeed, seg.End - seg.Start);
                }

                /*
                double[] d = mData2.StableSegments.Select(s => s.AverageSpeed).ToArray();
                MathNet.Numerics.Statistics.Histogram h = new MathNet.Numerics.Statistics.Histogram(d, 5);
                int x = h.BucketCount;
                */
                return;
            }
            /*
            if (mData != null)
            {
                upDownThreshold.Value = (decimal)Threshold;

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
            */
        }

        private IEnumerable<Segment> FindBest(IEnumerable<Segment> segments)
        {
            var speeds = segments
                .OrderBy(s => s.AverageSpeed)
                .Select(s => s.AverageSpeed);

            double dev = speeds.StandardDeviation();
            var segs = mData2.StableSegments.OrderBy(s => s.AverageSpeed);
            
            List<Segment> group = null;

            // Get an enumerator and stash the first record.
            var enumerator = segs.GetEnumerator();
            enumerator.MoveNext();
            Segment last = enumerator.Current;

            do
            {
                Segment seg = null;
                if (enumerator.MoveNext())
                    seg = enumerator.Current;

                //Start:
                if (seg != null && (seg.AverageSpeed - last.AverageSpeed) < dev)
                {
                    // group-em
                    if (group == null)
                    {
                        group = new List<Segment>();
                        group.Add(last);
                    }

                    group.Add(seg);
                    continue;
                }
                else
                {
                    Segment best = null;

                    // If group exists, select the best from the group.
                    if (group != null)
                    {
                        // Evaluate the group and find the one with the most
                        double minSpeed = group.Min(s => s.AverageSpeed);
                        double maxSpeed = group.Max(s => s.AverageSpeed);

                        int max = group.Max(s => (s.End - s.Start));
                        best = group.Where(s => (s.End - s.Start) == max).Single();

                        group = null;
                        System.Diagnostics.Debug.WriteLine("GROUP: Min: {0:N1}, Max:{1:N1}, Count:{2:N0}",
                            minSpeed, maxSpeed, best.End - best.Start);
                        // Loop back to the begining of this loop without advancing to next seg.       
                        //goto Start;
                    }
                    else
                    {
                        best = last;
                    }

                    last = seg;

                    yield return best;
                }
            } while (last != null);
        }

        private void MData2_SegmentDetected(Segment segment)
        {
            Action a = () => { DrawSegment(segment); };

            // Chart doesn't seem to catch collection changed, so force update.
            if (this.InvokeRequired)
            {
                this.BeginInvoke(a);
            }
            else
            {
                this.Invoke(a);
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
                SmoothData(dlg.FileName);
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

            /*
            TextAnnotation text = new TextAnnotation();
            text.Text = xPosition.ToString("N0");
            text.X = xPosition;
            text.Y = 10;
            text.ClipToChartArea = chart1.ChartAreas[0].Name;
            text.AxisX = chart1.ChartAreas[0].AxisX;
            text.AxisY = chart1.ChartAreas[0].AxisY;
            chart1.Annotations.Add(text);
            */
        }

        private void Zoom()
        {
            mData2.Update(50, 510, 1000);
            return;
            /*
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

            Chart(mData.RawPower, "Actual");
            Chart(mData.SmoothedPower, "Smoothed");
            //Chart(mData.Power5secMA, "Moving Average (5 sec)", x[0], x[1]);
            //Chart(mData.Power10secMA, "Moving Average (10 sec)", x[0], x[1]);

            ChartSegments(mZoomStart, mZoomEnd);
            */
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
            Threshold = (double)upDownThreshold.Value;
            ChartSegments();
        }
    }
}
