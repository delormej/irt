using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MathNet.Filtering;
using MathNet.Filtering.Median;
using MathNet.Numerics.Statistics;
using System.ComponentModel;
using IRT.Calibration;

namespace BikeSignalProcessing.Model
{
    /// <summary>
    /// Signature of a method that receives new segments.
    /// </summary>
    /// <param name="segment"></param>
    public delegate void SegmentDetectedEventHandler(Segment segment);

    /// <summary>
    /// Encapsulates state and calculations for stable, smoothed data.
    /// </summary>
    public class Data : INotifyPropertyChanged
    {
        private int m_window = 10;
        private double m_threshold = 4.0;
        private double m_drag;
        private double m_rr;
        private int m_skipRows;

        private object m_updateLock = null;

        internal PowerFit m_powerFit;

        /// <summary>
        /// Minimum number of data points in a segment. 
        /// </summary>
        public int Window
        {
            get { return m_window; }
            set
            {
                if (value != m_window)
                {
                    m_window = value;
                    // Force re-evaluation.
                    ReEvaluateSegments();

                    OnPropertyChanged("Window");
                }
            }
        }

        /// <summary>
        /// Threshold for standard deviation in stable segment.
        /// </summary>
        public double Threshold
        {
            get { return m_threshold; }
            set
            {
                if (value != m_threshold)
                {
                    m_threshold = value;
                    // Force re-evaluation.
                    ReEvaluateSegments();

                    OnPropertyChanged("Threshold");
                }
            }
        }

        /// <summary>
        /// Sets or gets the # of initial rows to skip.
        /// </summary>
        public int SkipRows
        {
            get { return m_skipRows; }
            set
            {
                m_skipRows = value;
                OnPropertyChanged("SkipRows");
            }
        }

        public double Drag
        {
            get
            {
                return m_drag;
            }
            set
            {
                m_drag = value;
                OnPropertyChanged("Drag");
            }
        }
        public double RollingResistance
        {
            get
            {
                return m_rr;
            }
            set
            {
                m_rr = value;
                OnPropertyChanged("RollingResistance");
            }
        }

        /// <summary>
        /// Hangs on to the active segment.
        /// </summary>
        private Segment mCurrentSegment;

        /// <summary>
        /// Index of the last data point added to the collection.
        /// </summary>
        private int mIndex;

        /// <summary>
        /// Filter for smoothing speed data.
        /// </summary>
        private IOnlineFilter mSpeedFilter;

        /// <summary>
        /// Filter for smoothing power data.
        /// </summary>
        private IOnlineFilter mPowerFilter;

        private enum Field
        {
            Power,
            Speed
        }

        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Invoked when a stable segment is added.
        /// </summary>
        public event SegmentDetectedEventHandler SegmentDetected;

        /// <summary>
        /// Segments represent stable windows into this collection of data points.
        /// </summary>
        public ObservableCollection<Segment> StableSegments { get; private set; }

        /// <summary>
        /// Data points including raw received and smoothed values.
        /// </summary>
        public ObservableCollection<BikeDataPoint> DataPoints { get; private set; }

        public ObservableCollection<MagnetFit> MagnetFits { get; private set; }

        public Data()
        {
            m_updateLock = new object();
            mIndex = 0;

            mCurrentSegment = null;
            StableSegments = new ObservableCollection<Segment>();
            DataPoints = new ObservableCollection<BikeDataPoint>();

            mSpeedFilter = OnlineFilter.CreateDenoise();
            mPowerFilter = OnlineFilter.CreateDenoise();
        }

        /// <summary>
        /// Update the collection with key data values, calculate smoothness factor and
        /// evaluate whether it's in a smooth window.
        /// </summary>
        /// <param name="speedMph"></param>
        /// <param name="powerWatts"></param>
        /// <param name="servoPosition"></param>
        public void Update(double speedMph, double powerWatts, int servoPosition)
        {
            lock (m_updateLock)
            {
                BikeDataPoint value = new BikeDataPoint();
                value.Seconds = mIndex;

                value.SpeedMph = speedMph;
                value.PowerWatts = powerWatts;
                value.ServoPosition = servoPosition;

                // Calculate smoothed values.
                value.SmoothedPowerWatts = Smooth(mPowerFilter, powerWatts);
                value.SmoothedSpeedMph = Smooth(mSpeedFilter, speedMph);

                // Maintain segment state.
                EvaluateSegment(value);

                // Update the collection.
                DataPoints.Add(value);

                // Increment internal index.
                mIndex++;
            }
        }

        public void ReEvaluateSegments()
        {
            lock (m_updateLock)
            {
                // Reset internal counter
                mIndex = SkipRows > 0 && DataPoints.Count > SkipRows ? SkipRows : 0;
                
                // Clear existing segments.
                StableSegments.Clear();
                mCurrentSegment = null;

                foreach (BikeDataPoint point in DataPoints.Skip(mIndex))
                {
                    EvaluateSegment(point);
                    mIndex++;
                }
            }
        }

        public void EvaluateMagnetFit()
        {
            // Filter for only the magnet ON positions.
            var magSegments = this.StableSegments.Where(s => s.MagnetPosition < 1600);

            MagnetFits = new ObservableCollection<MagnetFit>(
                MagnetFit.FitMagnet(magSegments));
        }

        public void GetCoastdownFit(out double[] speedMph, out double[] watts)
        {
            if (!(Drag > 0 && RollingResistance > 0))
            {
                speedMph = null;
                watts = null;
                return;
            }

            int start = 5;
            int len = 35 - start;

            speedMph = new double[len];
            watts = new double[len];

            for (int i = 0; i < len; i++)
            {
                speedMph[i] = i+start;
                watts[i] = 0; // PowerFit.Power(speedMph[i], Drag, RollingResistance);
#warning "Not calculating estimated power."
            }
        }

        public void EvaluateNoMagnetFit(out double[] speedModified, out double[] powerData)
        {
            speedModified = new double[0];
            powerData = new double[0];
            return;

            List<double> speed = new List<double>();
            List<double> watts = new List<double>();

            var best = Segment.FindBestNoMagnetSegments(StableSegments);

            if (best == null || best.Count() < 1)
            {
                speedModified = null;
                powerData = null;
                return;
            }

            foreach (var segment in best)
            {
                if (segment != null && segment.MagnetPosition >= 1600)
                {
                    speed.Add(segment.AverageSpeed);
                    watts.Add(segment.AveragePower);
                }
            }

            if (speed.Count() > 2)
            {
                try
                {
                    m_powerFit = new PowerFit();
                    m_powerFit.Fit(speed.ToArray(), watts.ToArray());

                    Drag = m_powerFit.Drag;
                    RollingResistance = m_powerFit.RollingResistance;

                    double[,] speedData;
                    //m_powerFit.GeneratePowerData(out speedData, out powerData);
#warning "Not generating estimated power."
                    /*
                    speedModified = new double[powerData.Length];

                    int i = 0;
                    foreach (double d in speedData)
                    {
                        speedModified[i++] = d;
                    }*/
                }
                catch
                {
                    m_powerFit = null;
                    speedModified = null;
                    powerData = null;
                }
            }
            else
            {
                powerData = null;
                speedModified = null;
            }
        }

        /// <summary>
        /// Returns a smoothed value for a given filter.
        /// </summary>
        /// <param name="filter"></param>
        /// <param name="sample"></param>
        /// <returns></returns>
        private double Smooth(IOnlineFilter filter, double sample)
        {
            return filter.ProcessSample(sample);
        }

        private double[] SliceData(Field field, int start, int end)
        {
            int len = end - start;

            if (len < 1)
                return null;

            double[] data = new double[len];
            for (int i = 0; i < len; i++)
            {
                switch (field)
                {
                    case Field.Power:
                        data[i] = DataPoints[i + start].SmoothedPowerWatts;
                        break;

                    case Field.Speed:
                        data[i] = DataPoints[i + start].SmoothedSpeedMph;
                        break;

                    default:
                        break;
                }
            }

            return data;
        }

        private double StandardDeviation(int start, int end)
        {
            double[] power = SliceData(Field.Power, start, end);
            return Statistics.StandardDeviation(power);
        }

        private double AveragePower(int start, int end)
        {
            double[] power = SliceData(Field.Power, start, end);
            return power.Average();
        }

        private double AverageSpeed(int start, int end)
        {
            double[] speed = SliceData(Field.Speed, start, end);
            return speed.Average();
        }

        private int GetLastSegmentEnd()
        {
            int end = 0;

            if (StableSegments != null)
            {
                int count = StableSegments.Count;
                if (count > 0)
                    end = StableSegments[count-1].End;
            }

            return end;
        }

        /// <summary>
        /// Evaluates whether to start, end, add to a segment or not.
        /// </summary>
        /// <param name="value"></param>
        private void EvaluateSegment(BikeDataPoint value)
        {
            // Get the last end.
            int end = GetLastSegmentEnd();
            
            // Ensure that we've accumulated enough points since the last segment.
            if ((end + Window) > mIndex)
                return;

            // Determine where the window starts.
            int start = (mCurrentSegment != null) ? mCurrentSegment.Start
                 : mIndex - Window;

            // Determine variation inside the window.
            double dev = StandardDeviation(start, mIndex);

            //
            // 3 possible states: (1) New/Invalidated, (2) Segment started, (3) Segment Ended
            //

            if (dev <= Threshold)
            {
                //
                // Within threshold.
                //
                if (mCurrentSegment == null)
                {
                    // Starting a new segment.
                    mCurrentSegment = new Segment(this);
                    mCurrentSegment.Start = start;

                    // Notify that we started a segment.
                    if (SegmentDetected != null)
                        SegmentDetected(mCurrentSegment);
                }
                else
                {
                    mCurrentSegment.StdDev = dev;
                }
            }
            else
            {
                //
                // Outside of threshold
                //
                if (mCurrentSegment != null)
                {
                    // Make sure segment is at least as big as Window
                    if (mIndex > (mCurrentSegment.Start + Window))
                    {
                        // Back up 3 data points.
                        mCurrentSegment.End = mIndex - 3;

                        mCurrentSegment.AveragePower = AveragePower(
                            mCurrentSegment.Start, mCurrentSegment.End);

                        mCurrentSegment.AverageSpeed = AverageSpeed(
                            mCurrentSegment.Start, mCurrentSegment.End);

                        // Not interested in data points below these thresholds.
                        if (mCurrentSegment.AveragePower > 40 &&
                            mCurrentSegment.AverageSpeed > 4.0)
                        {
                            Segment copy = mCurrentSegment.Copy();
                            StableSegments.Add(copy);

                            // Notify that a segment was added.
                            if (SegmentDetected != null)
                                SegmentDetected(copy);
                        }
                    }
                }
                else
                {
                    // Invalidate the segment.
                    // Notify that a segment was added.
                    if (SegmentDetected != null && mCurrentSegment != null)
                    {
                        Segment copy = mCurrentSegment.Copy();
                        copy.State = SegmentState.Invalidated;
                        SegmentDetected(copy);
                    }
                }

                mCurrentSegment = null;
            }
        }

        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

    }
}
