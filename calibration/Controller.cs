using System;
using System.Linq;
using AntPlus.Profiles.BikePower;

namespace IRT.Calibration
{
    /// <summary>
    /// Orchestrates and manages the state of the calibration workflow end to end.
    /// </summary>
    public class Controller
    {
        BikePowerDisplay m_emotionPower, m_refPower;
        Model m_model;
        Coastdown m_coastdown;

        // Forms
        CoastdownForm m_coastdownForm;
        CalibrationForm m_calibrationForm;

        public event Action<Coastdown> SetCalibrationValues;

        /// <summary>
        /// Creates the controller, should be created on the UI Thread.
        /// </summary>
        /// <param name="emotionPower"></param>
        /// <param name="refPower"></param>
        public Controller(BikePowerDisplay emotionPower, BikePowerDisplay refPower)
        {
            m_model = new Model();
            m_coastdown = new Coastdown();

            m_calibrationForm = new CalibrationForm(m_model);
            m_coastdownForm = new CoastdownForm(m_coastdown);

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
            m_model.AddPowerEvent(arg1.EventCount, arg1.AccumulatedPower);
        }

        void m_emotionPower_GeneralCalibrationResponseFailPageReceived(
            GeneralCalibrationResponseFailPage arg1, uint arg2)
        {
            OnFail("General calibration fail, please retry.");
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
                OnFail("Premature exit, please retry.");
            }
        }

        void m_emotionPower_CalibrationCustomParameterResponsePageReceived(
            CustomCalibrationParameterResponsePage arg1, uint arg2)
        {
            m_model.AddSpeedEvent(arg1.CalibrationDataArray.ToArray());
            m_coastdown.Add(m_model.Timestamp2048, m_model.Ticks);

            // 
            // Determine stage and process state transitions.
            //
            if (m_model.Stage == Stage.Ready && 
                m_model.Motion == Motion.Stable &&
                m_model.Seconds >= Settings.StableThresholdSeconds)
            {
                OnStable();
            }
            else if (m_model.Stage == Stage.Stable && 
                m_model.Motion == Motion.Accelerating)
            {
                OnAccelerating();
            }
            else if (m_model.Stage == Stage.Accelerating &&
                m_model.SpeedMps > Settings.MinAccelerationSpeedMps)
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
         * Manage state transitions.
         * 
         */

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
            try
            {
                m_coastdown.Calculate(m_model.SpeedMps, m_model.Watts);

                OnFinished();
            }
            catch (Exception e)
            {
                OnFail(e.Message);
            }
        }

        private void OnFinished()
        {
            m_model.Stage = Stage.Finished;

            if (SetCalibrationValues != null)
            {
                // Kick off a timer, countdown from n seconds and then invoke.
                //SetCalibrationValues(m_coastdown);
            }
        }

        private void OnFail(string message)
        {
            // TODO: Raise an event here indicating we failed.

            m_model.Stage = Stage.Failed;
        }

        /// <summary>
        /// Closes the dialogs and resets the state.
        /// </summary>
        public void Cancel()
        {
            
        }
    }
}
