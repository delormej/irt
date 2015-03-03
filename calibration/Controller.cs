using System;
using System.Linq;
using AntPlus.Profiles.BikePower;
using System.IO;
using IRT.Calibration.Globals;

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

        public Controller()
        {
            m_model = new Model();
            m_coastdown = new Coastdown();
        }

        /// <summary>
        /// Creates the controller, should be created on the UI Thread.
        /// </summary>
        /// <param name="emotionPower"></param>
        /// <param name="refPower"></param>
        public Controller(BikePowerDisplay emotionPower, BikePowerDisplay refPower) : base()
        {
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

            OnReady();
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
            // Generates 2 events from the buffer.
            TickEvent[] events = TickEvent.FromBuffer(arg1.CalibrationDataArray.ToArray());
            
            for (int i = 0; i < 2; i++)
            {
                OnEvent(events[i]);
            }

        }

        /// <summary>
        /// Add tick events to the model which recalculates state.
        /// </summary>
        /// <param name="tickEvent"></param>
        public void OnEvent(TickEvent tickEvent)
        {
            m_model.AddSpeedEvent(tickEvent);

            // 
            // Determine stage and process state transitions.
            //
            if (m_model.Stage == Stage.Ready &&
                m_model.Motion == Motion.Stable &&
                m_model.StableSeconds >= Settings.StableThresholdSeconds)
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

        private void OnReady()
        {
            m_calibrationForm = new CalibrationForm(m_model);
            m_calibrationForm.Show();
        }

        private void OnStable()
        {
            // Called when we've reached stable speed for threshold time.
            m_model.Stage = Stage.Stable;

            // Indicate to user it's time to accelerate to threshold speed.
        }

        private void OnAccelerating()
        {
            // Called when we end the acceleration stage.
            m_model.Stage = Stage.Accelerating;

            // Stop recording power events.
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
                m_coastdown.Calculate(m_model);
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

            //
            // Process this on the UI thread.
            //
            Action a = () => 
            { 
                // Close calibration form.
                m_calibrationForm.Close();

                // Open form to show results.
                m_coastdownForm = new CoastdownForm(m_coastdown);
                m_coastdownForm.Show();
            };

            if (m_calibrationForm.InvokeRequired)
            {
                m_calibrationForm.BeginInvoke(a);
            }
            else
            {
                m_calibrationForm.Invoke(a);
            }

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

        private void WriteLog()
        {
            StreamWriter log;

            // open up a stream to start logging
            string filename = string.Format("calib_{0}_{1:yyyyMMdd-HHmmss-F}.csv",
                this.GetType().Assembly.GetName().Version.ToString(3),
                DateTime.Now);

            using (log = new StreamWriter(filename))
            {
                log.WriteLine("timestamp_ms, count, ticks, watts, pwr_events, accum_pwr");

                foreach (var tick in m_model.Events)
                {
                    log.WriteLine(tick);
                }

                log.Flush();
                log.Close();
            }
        }
    }
}
