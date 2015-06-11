﻿using System;
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
    /// Encapsulates state and calculations for stable, smoothed data.
    /// </summary>
    public class Data : INotifyPropertyChanged
    {
        /// <summary>
        /// Minimum number of data points in a segment. 
        /// </summary>
        public int Window = 10;

        /// <summary>
        /// Threshold for standard deviation in stable segment.
        /// </summary>
        public double Threshold = 4.0;

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

        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Segments represent stable windows into this collection of data points.
        /// </summary>
        public ObservableCollection<Segment> StableSegments { get; set; }

        /// <summary>
        /// Data points including raw received and smoothed values.
        /// </summary>
        public ObservableCollection<DataPoint> DataPoints { get; set; }

        public Segment CurrentSegment
        {
            get { return mCurrentSegment; }
            private set
            {
                mCurrentSegment = value;
                OnPropertyChanged("CurrentSegment");
            }
        }

        public Data()
        {
            mIndex = 0;

            mCurrentSegment = null;
            StableSegments = new ObservableCollection<Segment>();
            DataPoints = new ObservableCollection<DataPoint>();

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
            DataPoint value = new DataPoint();
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

        private double[] SlicePower(int start, int end)
        {
            int len = end - start;
            double[] power = new double[len];
            for (int i = 0; i < len; i++)
            {
                power[i] = DataPoints[i + start].SmoothedPowerWatts;
            }

            return power;
        }

        private double StandardDeviation(int start, int end)
        {
            double[] power = SlicePower(start, end);
            return Statistics.StandardDeviation(power);
        }

        private double AveragePower(int start, int end)
        {
            double[] power = SlicePower(start, end);
            return power.Average();
        }

        /// <summary>
        /// Evaluates whether to start, end, add to a segment or not.
        /// </summary>
        /// <param name="value"></param>
        private void EvaluateSegment(DataPoint value)
        {
            // Can't evaluate until we have at least enough points in the window.
            if (mIndex < Window || value.PowerWatts == 0)
                return;

            // Ensure that we've accumulated enough points since the last segment.
            if (StableSegments.Count > 0)
            {
                Segment last = StableSegments[StableSegments.Count - 1];
                if (last != null && (last.End + Window) > mIndex)
                    return;
            }

            int start = mCurrentSegment != null ? this.mCurrentSegment.Start :
                mIndex - Window;

            double dev = StandardDeviation(start, mIndex);

            if (dev <= Threshold)
            {
                //
                // Within threshold.
                //
                if (mCurrentSegment == null)
                {
                    mCurrentSegment = new Segment();
                    mCurrentSegment.Start = start;
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
                        mCurrentSegment.End = mIndex - 3;
                        mCurrentSegment.AveragePower = AveragePower(
                            mCurrentSegment.Start, mCurrentSegment.End);

                        if (mCurrentSegment.AveragePower > 0)
                        {
                            Segment copy = mCurrentSegment.Copy();
                            StableSegments.Add(copy);
                        }

                        // Notify that the property changed.
                        OnPropertyChanged("CurrentSegment");
                    }

                    // Now clear the current segment to make way for next.
                    mCurrentSegment = null;
                }
            }
        }
    }
}
