using System;
using System.Linq;
using AntPlus.Profiles.BikePower;

namespace IRT.Calibration
{
    public enum Stage
    {
        Ready = 0,
        Stable,
        Accelerating,
        SpeedThresholdReached,
        Coasting,
        Processing,
        Finished,
        Failed
    }

    /// <summary>
    /// Orchestrates and manages the state of the calibration workflow end to end.
    /// </summary>
    public class Controller
    {
        BikePowerDisplay m_emotionPower, m_refPower;
        Speed m_speed;
        AveragePower m_power;
        Model m_model;

        // Stability is considered after 15 seconds consitent.
        const double StableThresholdSeconds = 15;
        
        // Amount speed can vary and still be considered stable.
        const double StableThresholdSpeedMps = 0.11176f;

        // Minimum of 25mph to reach peak speed.
        const double MinAccelerationSpeedMps = 25.0f * 0.44704; 

        public event Action<Coastdown> SetCalibrationValues;

        /// <summary>
        /// Must be created on the UI Thread.
        /// </summary>
        /// <param name="emotionPower"></param>
        /// <param name="refPower"></param>
        public Controller(BikePowerDisplay emotionPower, BikePowerDisplay refPower)
        {
            m_speed = new Speed(StableThresholdSpeedMps);
            m_power = new AveragePower();
            m_model = new Model();

            // Listeners for ANT+ events.
            m_emotionPower = emotionPower;
            m_refPower = refPower;

            // Register calibration events from the emotion rollers.
            m_emotionPower.CalibrationCustomParameterResponsePageReceived += 
                m_emotionPower_CalibrationCustomParameterResponsePageReceived;
            m_emotionPower.GeneralCalibrationResponseSuccessReceived += 
                m_emotionPower_GeneralCalibrationResponseSuccessReceived;
            m_emotionPower.GeneralCalibrationResponseFailPageReceived += 
                m_emotionPower_GeneralCalibrationResponseFailPageReceived;

            // Register standard power messages from the external power meter.
            m_refPower.StandardPowerOnlyPageReceived += m_refPower_StandardPowerOnlyPageReceived;
        }

        void m_refPower_StandardPowerOnlyPageReceived(StandardPowerOnlyPage arg1, uint arg2)
        {
            m_model.Watts = m_power.AddEvent(arg1.EventCount, arg1.AccumulatedPower);
        }

        void m_emotionPower_GeneralCalibrationResponseFailPageReceived(
            GeneralCalibrationResponseFailPage arg1, uint arg2)
        {
            OnFail();
        }

        void m_emotionPower_GeneralCalibrationResponseSuccessReceived(
            GeneralCalibrationResponseSuccessPage arg1, uint arg2)
        {
            if (m_model.Stage == Stage.Coasting)
            {
                OnProcessing();
            }
            else
            {
                // Raise an event that we ended prematurely.
                OnFail();
            }
        }

        void m_emotionPower_CalibrationCustomParameterResponsePageReceived(
            CustomCalibrationParameterResponsePage arg1, uint arg2)
        {
            byte[] buffer = arg1.CalibrationDataArray.ToArray();
            double speed, seconds;
            Motion stability;
            m_speed.AddEvent(buffer, out speed, out seconds, out stability);
            m_model.SpeedMps = speed;
            m_model.Seconds = seconds;
            m_model.Motion = stability;

            // 
            // Determine stage and process state transitions.
            //
            if (m_model.Stage == Stage.Ready && 
                stability == Motion.Stable &&
                seconds >= StableThresholdSeconds)
            {
                OnStable();
            }
            else if (m_model.Stage == Stage.Stable && 
                m_model.Motion == Motion.Accelerating)
            {
                OnAccelerating();
            }
            else if (m_model.Stage == Stage.Accelerating &&
                m_model.SpeedMps > MinAccelerationSpeedMps)
            {
                OnSpeedThresholdReached();
            }
            else if (m_model.Stage == Stage.SpeedThresholdReached &&
                m_model.Motion == Motion.Decelerating)
            {
                OnCoasting();
            }
        }

        /*
         * State transitions.
         */

        private void OnFail()
        {
            // TODO: Raise an event here indicating we failed.

            m_model.Stage = Stage.Failed;
        }

        private void OnStable()
        {
            // Called when we've reached stable speed for threshold time.
            m_model.Stage = Stage.Stable;

            // Grab the stable speed and power.

        }

        private void OnAccelerating()
        {
            // Called when we end the acceleration stage.
            m_model.Stage = Stage.Accelerating;
        }

        private void OnSpeedThresholdReached()
        {
            m_model.Stage = Stage.SpeedThresholdReached;
        }

        private void OnCoasting()
        {
            m_model.Stage = Stage.Coasting;
        }

        private void OnProcessing()
        {
            m_model.Stage = Stage.Processing;
            
            // Kick off processing.
            
            // Transition state to finished if success.
            if (true) 
                m_model.Stage = Stage.Finished;
            else
                OnFail();
        }

        /// <summary>
        /// Closes the dialogs and resets the state.
        /// </summary>
        public void Cancel()
        {
            
        }
    }
}
