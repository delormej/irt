using IRT.Calibration;
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
using BikeSignalProcessing.Model;
using MathNet.Numerics;

namespace BikeSignalProcessing.View
{
    public partial class ChartView : Form
    {
        private const string ActualSeriesName = "Actual";
        private const string SmoothSeriesName = "Smoothed";
        private const string ChartAreaMagnet = "Magnet";
        private const int SegmentLineWidth = 3;
        
        private Data mData;
        //private AsyncCsvFactory asyncCsv;

        private int mZoomStart = -1;
        private int mZoomEnd = -1;

        public ChartView(Data data)
        {
            if (data == null)
            {
                mData = new Data();
            }

            mData = data;

            InitializeComponent();
            ClearChart();

            chart1.MouseClick += Chart1_MouseClick;
            chart1.MouseMove += Chart1_MouseMove;

            upDownMinWindow.ValueChanged += SegmentConfigChanged;
            upDownThreshold.ValueChanged += SegmentConfigChanged;

            BindData();
        }

        private void SegmentConfigChanged(object sender, EventArgs e)
        {
            try
            {
                if (upDownThreshold.Value <= 0 || upDownMinWindow.Value <= 0)
                    return;

                this.Cursor = Cursors.WaitCursor;
                mData.Threshold = (double)upDownThreshold.Value;
                mData.Window = (int)upDownMinWindow.Value;

                // Rechart segments.
                ChartSegments(mData.StableSegments);
            }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }

        public void PlotCoastdownPower(bool fit)
        {
            // Attempt to fit the coast down.
            double[] speed, power;

            if (fit)
            {
                mData.EvaluateNoMagnetFit(out speed, out power);
            }
            else
            {
                mData.GetCoastdownFit(out speed, out power);
            }

            if (speed == null || power == null)
            {
                return;
            }

            string seriesName = "Coastdown Power Estimate";

            Series wattSeries = chart1.Series.FindByName(seriesName);

            if (wattSeries != null)
            {
                // Remove if already exists.
                chart1.Series.Remove(wattSeries);
            }

            var powerArea = chart1.ChartAreas[ChartAreaMagnet];

            wattSeries = chart1.Series.Add(seriesName);
            wattSeries.ChartArea = ChartAreaMagnet;
            wattSeries.ChartType = SeriesChartType.Spline;

            //chart1.ChartAreas["Power"].AxisY.Title = "Power (watts)";
            //chart1.ChartAreas["Power"].AxisX.Title = "Speed (mph)";
            //chart1.ChartAreas["Power"].AxisX.RoundAxisValues();
            ////chart1.ChartAreas["Coastdown"].AxisX = chart1.ChartAreas["Power"].AxisX;

            chart1.Series[seriesName].ToolTip = "Watts: #VALY{N0}\nMph: #VALX{N1}";

            for (int j = 0; j < speed.Length; j++)
            {
                int i = wattSeries.Points.AddXY(speed[j], power[j]);
                wattSeries.Points[i].MarkerStyle = MarkerStyle.Circle;
                wattSeries.Points[i].MarkerSize = 5;
            }
        }

        private void Chart1_MouseMove(object sender, MouseEventArgs e)
        {
            HitTestResult result = chart1.HitTest(e.X, e.Y);

            if (result.ChartElementType == ChartElementType.DataPoint)
            {
                DataPoint point = result.Series.Points[result.PointIndex];
                LineAnnotation segment = point.Tag as LineAnnotation;

                if (segment != null)
                {
                    HighlightSegment(segment);
                }
            }
            else if (result.ChartElementType == ChartElementType.Annotation)
            {
                LineAnnotation line = result.Object as LineAnnotation;
                if (line != null)
                {
                    // Reset
                    line.LineWidth = SegmentLineWidth;
                    ResetSegmentArea();
                }
            }
        }
        private void Chart1_MouseClick(object sender, MouseEventArgs e)
        {
            HitTestResult result = chart1.HitTest(e.X, e.Y);

            if (result.ChartElementType == ChartElementType.Annotation)
            {
                Annotation annotation = result.Object as Annotation;

                // Click on text to hide.
                if (annotation is TextAnnotation)
                {
                    annotation.Visible = false;
                }
                else if (annotation is LineAnnotation)
                {
                    Segment segment = annotation.Tag as Segment;
                    if (segment != null)
                    {
                        TextAnnotation text = FindTextAnnotation(segment);
                        if (text != null)
                        {
                            text.Visible = true;
                            ChartSegmentArea(segment);
                        }
                    }
                }
            }
        }

        private TextAnnotation FindTextAnnotation(Segment segment)
        {
            foreach (var a in chart1.Annotations)
            {
                if (a is TextAnnotation && a.X == segment.Start)
                    return a as TextAnnotation;
            }

            return null;
        }

        private TextAnnotation CreateText(Segment segment)
        {
            string summary = string.Format("Duration: {0}\r\nStdDev: {1:N1}\r\nSpeed: " +
                "{2:N1}\r\nWatts: {3:N0}\r\n Position: {4}\r\nStart: {5}",
                (segment.End - segment.Start), segment.StdDev,
                segment.AverageSpeed, segment.AveragePower, segment.MagnetPosition,
                segment.Start);

            var text = new TextAnnotation();
            text.Text = summary;
            text.AxisX = chart1.ChartAreas[0].AxisX;
            text.AxisY = chart1.ChartAreas[0].AxisY;
            text.X = segment.Start;
            text.Y = segment.AveragePower;

            return text;
        }

        private void ChartSegmentArea(Segment segment)
        {
            ChartArea area = chart1.ChartAreas["Segment"];
            area.AxisX.ScaleView.Position = segment.Start;
            area.AxisX.ScaleView.Size = segment.Length;
            area.AxisY.ScaleView.Size = 100;
            area.AxisY.ScaleView.Position = Math.Floor(segment.AveragePower) - (area.AxisY.ScaleView.Size / 2);
            area.AxisY2.ScaleView.Size = 10;
            area.AxisY2.ScaleView.Position = Math.Floor(segment.AverageSpeed) - (area.AxisY2.ScaleView.Size / 2);
            area.AxisY.ScrollBar.Enabled = false;
            area.AxisY2.ScrollBar.Enabled = false;
            //area.AxisX.ScaleView.Zoom(segment.Start, segment.End);
        }

        private void ResetSegmentArea()
        {
            ChartArea area = chart1.ChartAreas["Segment"];
            area.AxisX.ScaleView.Size = 30;
            //area.AxisX.ScaleView.ZoomReset();
        }

        /// <summary>
        /// Shows the segment by highlighting the line.
        /// </summary>
        /// <param name="line"></param>
        private void HighlightSegment(LineAnnotation line)
        {
            Segment segment = line.Tag as Segment;

            if (segment == null)
                return;

            line.LineColor = Color.Red;
            line.LineWidth = 10;

            TextAnnotation text = FindTextAnnotation(segment);
            if (text != null)
                text.Visible = true;

            ChartSegmentArea(segment);
        }

        private void ClearChart()
        {
            RemoveZoomMarkers();

            chart1.Annotations.Clear();
            chart1.Series.Clear();
            chart1.ChartAreas.Clear();
        }

        private void DrawMagLinear(MagnetFit magfit)
        {
            string name = magfit.MagnetPosition.ToString() + " Fit";

            // Find or create the series
            Series magLinear = chart1.Series.FindByName(name);
            if (magLinear == null)
            {
                magLinear = chart1.Series.Add(name);
                magLinear.ChartArea = ChartAreaMagnet;
                magLinear.ChartType = SeriesChartType.Line;

                // Grab original series for the color.
                Series series = chart1.Series.FindByName(magfit.MagnetPosition.ToString());
                if (series != null)
                    magLinear.Color = series.Color;
            }
            else
            {
                magLinear.Points.Clear();
            }

            magLinear.Points.AddXY(magfit.Low().Item1, magfit.Low().Item2);
            magLinear.Points.AddXY(magfit.High().Item1, magfit.High().Item2);
        }
        
        private void DrawMagLinear()
        {
            // Ensure all colors are in place.
            chart1.ApplyPaletteColors();

            mData.EvaluateMagnetFit();

            foreach (MagnetFit fit in mData.MagnetFits)
            {
                DrawMagLinear(fit);
            }
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
            line.LineWidth = SegmentLineWidth;
            line.LineDashStyle = ChartDashStyle.Dot;
            line.EndCap = LineAnchorCapStyle.Round;
            line.AllowMoving = false;
            line.IsInfinitive = false;
            line.Tag = segment;     // Tag the line with the segment.

            chart1.Annotations.Add(line);

            // Create text annotation with details.
            TextAnnotation text = CreateText(segment);
            text.Visible = false;
            chart1.Annotations.Add(text);

            // Look for or create series in mag chart.
            Series mag = chart1.Series.FindByName(segment.MagnetPosition.ToString());
            if (mag == null)
            {
                mag = chart1.Series.Add(segment.MagnetPosition.ToString());
                mag.ChartArea = ChartAreaMagnet;
                mag.ChartType = SeriesChartType.Point;
                mag.ToolTip = "Watts: #VALY{N0}\nMph: #VALX{N1}";
            }

            System.Windows.Forms.DataVisualization.Charting.DataPoint d =
                new System.Windows.Forms.DataVisualization.Charting.DataPoint(
                    segment.AverageSpeed, segment.NoMagnetPower);
            //d.MarkerSize = ((segment.End - segment.Start) / 20) * 5;
            d.Tag = line;  // Tag the segment so we can find it.
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
            if (segments == null)
                return;

            // Remove any old segments.
            RemoveSegments(); 

            // Chart all segments
            foreach (var seg in segments)
            {
                //System.Diagnostics.Debug.WriteLine("Start: {0}, End: {1}, Power: {2}",
                //    seg.Start, seg.End, seg.AveragePower);
                DrawSegment(seg);
            }

            DrawMagLinear();
        }

        private void BindChart(Data data)
        {
            chart1.Series.Clear();
            chart1.ChartAreas.Clear();

            ChartArea rideArea = chart1.ChartAreas.Add("Ride");
            rideArea.Position.Width = 70;
            rideArea.Position.Height = 80;
            rideArea.Position.X = 0;
            rideArea.Position.Y = 20;
            rideArea.AxisX.IntervalType = DateTimeIntervalType.Seconds;
            rideArea.AxisY2.Maximum = 40; // limit to 40 mph.
            rideArea.CursorX.IsUserSelectionEnabled = true;

            ChartArea magArea = chart1.ChartAreas.Add("Magnet");
            magArea.Position.Width = 30;
            magArea.Position.Height = 80;
            magArea.AlignWithChartArea = "Ride";
            magArea.AlignmentOrientation = AreaAlignmentOrientations.Horizontal;
            magArea.AlignmentStyle = AreaAlignmentStyles.AxesView;
            magArea.Position.X = 70;
            magArea.Position.Y = 20;
            magArea.AxisX.Interval = 3;
            magArea.AxisX.LabelStyle.Format = "{0:0} mph";
            magArea.CursorX.IsUserSelectionEnabled = true;

            chart1.DataSource = mData.DataPoints;

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

            //
            // Create an area on the graph to zoom into a segment.
            //
            ChartArea segmentArea = chart1.ChartAreas.Add("Segment");
            segmentArea.Position.Width = 50;
            segmentArea.Position.Height = 20;
            segmentArea.Position.X = 20;
            segmentArea.Position.Y = 0;
            //segmentArea.AlignWithChartArea = "Ride";
            //segmentArea.AlignmentOrientation = AreaAlignmentOrientations.Vertical;
            //segmentArea.AlignmentStyle = AreaAlignmentStyles.AxesView;
            //segmentArea.AxisX.Maximum = 30;
            ResetSegmentArea();
            segmentArea.AxisX.ScrollBar.Enabled = false;
            segmentArea.AxisX.IntervalType = DateTimeIntervalType.Seconds;
            segmentArea.AxisY.MajorGrid.Enabled = false;
            segmentArea.AxisY.MinorGrid.Enabled = false;
            segmentArea.AxisY2.MajorGrid.Enabled = false;
            segmentArea.AxisY2.MinorGrid.Enabled = false;

            Series segmentPower = chart1.Series.Add("Segment Power");
            segmentPower.ChartType = SeriesChartType.FastLine;
            segmentPower.YValueMembers = "PowerWatts";
            segmentPower.YAxisType = AxisType.Primary;
            segmentPower.ChartArea = "Segment";

            Series segmentSpeed = chart1.Series.Add("Segment Speed (mph)");
            segmentSpeed.ChartType = SeriesChartType.FastLine;
            segmentSpeed.YValueMembers = "SpeedMph";
            segmentSpeed.YAxisType = AxisType.Secondary;
            segmentSpeed.ChartArea = "Segment";

            mData.DataPoints.CollectionChanged += (object sender,
                System.Collections.Specialized.NotifyCollectionChangedEventArgs e) =>
            {
                Action a = () => {
                    try
                    {
                        chart1.DataBind();
                        
                        ChartArea area = chart1.ChartAreas["Segment"];

                        if (area.AxisX.Maximum > area.AxisX.ScaleView.Size)
                        {
                            area.AxisX.ScaleView.Scroll(area.AxisX.Maximum + 5);
                        }
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
            };
        }

        private void LoadData(string filename)
        {
            if (filename != null)
            {
                mData = (Data)IrtCsvFactory.Open(filename);
                var f = new System.IO.FileInfo(filename);
                this.Text = f.Name;
            }
        }

        private void BindData()
        {
            // Clear any existing bindings.
            upDownThreshold.DataBindings.Clear();
            upDownMinWindow.DataBindings.Clear();
            txtDrag.DataBindings.Clear();
            txtRR.DataBindings.Clear();

            upDownThreshold.DataBindings.Add("Value", mData, "Threshold");
            upDownMinWindow.DataBindings.Add("Value", mData, "Window");

            txtDrag.DataBindings.Add("Text", mData, "Drag", true, DataSourceUpdateMode.OnPropertyChanged);
            txtRR.DataBindings.Add("Text", mData, "RollingResistance", true, DataSourceUpdateMode.OnPropertyChanged);

            BindChart(mData);
            PlotCoastdownPower(false);
            ChartSegments(mData.StableSegments);
            mData.SegmentDetected += OnSegmentDetected;
        }

        private void OnSegmentDetected(Segment segment)
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
                try
                {
                    this.Cursor = Cursors.WaitCursor;
                    LoadData(dlg.FileName);
                    BindData();
                }
                finally
                {
                    this.Cursor = Cursors.Default;
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

            //System.Diagnostics.Debug.WriteLine("Removing vertical lines.");
            //chart1.UpdateAnnotations();

            //if (chart1.Annotations.Count < 1)
            //    return;

            //foreach (var a in chart1.Annotations)
            //{
            //    if (a is VerticalLineAnnotation)
            //        chart1.Annotations.Remove(a);
            //}
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

        private void ClearMagPoints()
        {
            foreach (Segment seg in mData.StableSegments)
            {
                // Clear any mag points.
                Series series = chart1.Series.FindByName(seg.MagnetPosition.ToString());
                if (series != null)
                {
                    series.Points.Clear();
                }
            }
        }

        private void btnReset_Click(object sender, EventArgs e)
        {
            // Reset
            ClearChart();
            BindData();
            mData.ReEvaluateSegments();
        }

        private void upDownThreshold_ValueChanged(object sender, EventArgs e)
        {
            //Threshold = (double)upDownThreshold.Value;
            //ChartSegments();
        }

        private void btnBest_Click(object sender, EventArgs e)
        {
            ClearMagPoints();
            PlotCoastdownPower(true);
            ChartSegments(mData.StableSegments);

            //// Re-chart only the best segments.
            //ChartSegments(Segment.FindBestSegments(mData.StableSegments));
        }

        private void btnData_Click(object sender, EventArgs e)
        {
            DataView dv = new DataView(mData);
            dv.Text = this.Text;
            dv.Show();
        }
    }
}
