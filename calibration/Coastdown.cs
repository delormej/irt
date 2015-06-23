using System;
using System.Linq;
using System.Collections.Generic;

namespace IRT.Calibration
{
    /// <summary>
    /// Handles the filtering and process of the data files.
    /// Front end interface to the end results.
    /// </summary>
    public class Coastdown
    {
        private CoastdownData m_coastdownData;

        // Fit objects.
        private DecelerationFit m_decelFit;
        private PowerFit m_powerFit;

        public Coastdown()
        {
        }

        /// <summary>
        /// Raw coastdown data points, filtered for just the deceleration phase
        /// from max to min speed.
        /// </summary>
        public CoastdownData Data { get { return m_coastdownData; } }

        /// <summary>
        /// Calculates based on multiple runs of coastdown.
        /// </summary>
        /// <param name="models"></param>
        public Model Calculate(Model[] models)
        {
            m_coastdownData = new CoastdownData();

            foreach (Model m in models)
            {
                // Aggregate all valid coast down data points.
                m_coastdownData.Evaluate(m.Events.ToArray());
                Fit(m);
            }

            Model aggregateModel = new Model();
            
            // Calculate average inertia for new model.
            aggregateModel.Inertia = models.Average(m => m.Inertia);

            //aggregateModel.StableSpeedMps = models.Average(m => m.StableSpeedMps);
            //aggregateModel.StableWatts = models.Average(m => m.StableWatts);
            aggregateModel.StableSeconds = models.Max(m => m.StableSeconds);

            Fit(aggregateModel);

            return aggregateModel;
        }

        /// <summary>
        /// Parses the coast down x,y values and generates coefficients of Drag 
        /// and Rolling Resistance based on stable speed and watts.
        /// </summary>
        /// <returns>true/false if it succeeded.</returns>
        public void Calculate(Model model)
        {
            // Process values to build speed/time arrays.
            m_coastdownData = new CoastdownData();
            m_coastdownData.Evaluate(model.Events.ToArray());

            Fit(model);
            /*
            if (Drag < 0.0)
            {
                throw new CoastdownException(
                    "Drag should not be negative, likely entry speed is wrong.");
            }
            else if (RollingResistance < 10)
            {
                //throw new CoastdownException(
                // "Rolling Resistance seems incorrect, it should be greater.");
            }
             */
        }

        /// <summary>
        /// Calculated coefficient of Drag.
        /// </summary>
        public double Drag
        {
            get { return m_powerFit.Drag; }
            set { m_powerFit.Drag = value; }
        }

        /// <summary>
        /// Calculated coefficient of Rolling Resistance.
        /// </summary>
        public double RollingResistance {
            get { return m_powerFit.RollingResistance; }
            set { m_powerFit.RollingResistance = value; }
        }

        /// <summary>
        /// Returns the time it takes to coastdown in seconds from a given speed.
        /// </summary>
        /// <param name="speedMps"></param>
        /// <returns></returns>
        public double CoastdownTime(double speedMps)
        {
            return m_decelFit.Seconds(speedMps);
        }

        /// <summary>
        /// Calculate the rate of deceleration for a given speed.
        /// </summary>
        /// <param name="speed_mps"></param>
        /// <returns></returns>
        public double Deceleration(double speedMps)
        {
            return m_decelFit.Rate(speedMps);
        }

        /// <summary>
        /// Calculate the power required in watts for a given speed.
        /// </summary>
        /// <param name="speed_mps"></param>
        /// <returns></returns>
        public double Watts(double speedMps)
        {
            return m_powerFit.Watts(speedMps);
        }

        /// <summary>
        /// Performs the deceleration and power fits.
        /// </summary>
        /// <param name="model"></param>
        private void Fit(Model model)
        {
            // Calculate the deceleration.
            m_decelFit = new DecelerationFit();
            m_decelFit.Fit(m_coastdownData.SpeedMps, m_coastdownData.Acceleration);

            // Calculate the power fit.
            m_powerFit = new PowerFit(m_decelFit);
            m_powerFit.CalculateStablePowerFactor(model.StableSpeedMps, model.StableWatts);
            m_powerFit.Fit();
        }
    }

    public class CoastdownException : Exception
    {
        public CoastdownException(string message) : base(message)
        {

        }
    }
}
