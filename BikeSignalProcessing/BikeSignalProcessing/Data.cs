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

namespace BikeSignalProcessing
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
        private int mWindow = 10;
        private double mThreshold = 4.0;

        private object mUpdateLock = null;

        /// <summary>
        /// Minimum number of data points in a segment. 
        /// </summary>
        public int Window
        {
            get { return mWindow; }
            set
            {
                if (value != mWindow)
                {
                    mWindow = value;
                    // Force re-evaluation.
                    ReEvaluateSegments();
                }
            }
        }

        /// <summary>
        /// Threshold for standard deviation in stable segment.
        /// </summary>
        public double Threshold
        {
            get { return mThreshold; }
            set
            {
                if (value != mThreshold)
                {
                    mThreshold = value;
                    // Force re-evaluation.
                    ReEvaluateSegments();
                }
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
        public ObservableCollection<Segment> StableSegments { get; set; }

        /// <summary>
        /// Data points including raw received and smoothed values.
        /// </summary>
        public ObservableCollection<BikeDataPoint> DataPoints { get; set; }

        public Data()
        {
            mUpdateLock = new object();
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
            lock (mUpdateLock)
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
            lock (mUpdateLock)
            {
                // Reset internal counter
                mIndex = 0;

                // Clear existing segments.
                StableSegments.Clear();
                mCurrentSegment = null;

                foreach (BikeDataPoint point in DataPoints)
                {
                    EvaluateSegment(point);
                    mIndex++;
                }
            }
        }

        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
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
                    mCurrentSegment = new Segment();
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
                            mCurrentSegment.MagnetPosition =
                                DataPoints[mCurrentSegment.Start].ServoPosition;

                            Segment copy = mCurrentSegment.Copy();
                            StableSegments.Add(copy);

                            // Try fitting linear regression if there are multiple points for this position.
                            copy.FitMagnet(this.StableSegments);

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
    }
}
