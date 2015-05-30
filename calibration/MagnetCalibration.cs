using System;

namespace IRT.Calibration
{
    public delegate void MagnetCalibrationEventHandler(object sender, MagnetCalibrationEventArgs e);

    public class MagnetCalibration
    {
        public MagnetCalibration()
        {
            LowSpeedFactors = new float[4];
            HighSpeedFactors = new float[4];
        }

        public float LowSpeedMps;
        public float HighSpeedMps;

        public float[] LowSpeedFactors;
        public float[] HighSpeedFactors;
    }

    public class MagnetCalibrationEventArgs : EventArgs
    {
        public MagnetCalibration Calibration;

        public MagnetCalibrationEventArgs(MagnetCalibration calibration)
        {
            this.Calibration = calibration;
        }
    }
}
