using System;
using System.Collections.Generic;
using System.Linq;
using MathNet.Numerics;

namespace IRT.Calibration
{
    /// <summary>
    /// Business object which encapsulates all logic for how to calculate coastdown results.  
    /// This is not a data object.  The data from the coastdown is stored in CoastdownModel.
    /// </summary>
    public class Coastdown
    {
        // 1. Fit time : speed (should be a nice fit)
        // 2. Solve for the differential equation dV / dT for real acceleration at each step.
        // 3. Fit 2nd order polynomial acceleration : speed. (3 coefficients)

        // 4a. Given weight, wheel and Crr diameter,
        //      solve for mass and resolve to 3 coefficients for F
        // 4b. Given stable power & speed
        //      f = p/v
        //      m = f/a
        //      foreach coefficent of a; multiply by m

        private double[] speed;
        private double[] seconds;
        private double stableSpeedMps;
        private double stableWatts;

        private Func<double, double> funcAccelTime;
        private Func<double, double> funcTimeSpeed;
        private Func<double, double> funcAccelSpeed;

        private CoastdownModel m_model;

        private double m_goodnessOfFit;

        public Coastdown(CoastdownModel model)
        {
            // TODO: should we depend on model here instead of copying? 
            stableSpeedMps = model.StableSpeedMps;
            stableWatts = model.StableWatts;
            speed = model.Data.SpeedMps;
            seconds = model.Data.CoastdownSeconds;

            // Only saving this so that we can populate acceleration.  This should be fixed.
            m_model = model;
        }

        /// <summary>
        /// Fits a curve to time (y) given speed (x).  The slope between each point
        /// represents acceleration.
        /// </summary>
        private void FitTimeSpeed()
        {
            // Fit time : speed (should be a nice fit)
            funcTimeSpeed = Fit.PolynomialFunc(seconds, speed, 2);

            // We want goodness of fit to be as close to 1.0 as possible.  At least 0.999.
            m_goodnessOfFit = MathNet.Numerics.GoodnessOfFit.R(
                seconds.Select(x => funcTimeSpeed(x)), speed);

            if (m_goodnessOfFit < 0.999)
            {
                // throw a warning out here? 
            }
        }

        /// <summary>
        /// Calculates the derivative of Time:Speed based on the function of Time:Speed.
        /// </summary>
        private void CalculateAcceleration()
        {
            // Returns the derivative function.
            funcAccelTime = Differentiate.FirstDerivativeFunc(funcTimeSpeed);
        }

        /// <summary>
        /// Fits a curve to acceleration (y) given speed (x).
        /// </summary>
        /// <returns></returns>
        private void FitAccelerationSpeed(ref double[] coeff)
        {
            // Get the first derivative of the time : speed function to solve for acceleration.
            CalculateAcceleration();

            // Build up a collection of time, speed pairs, then fit to a curve.
            double[] accel = new double[this.speed.Length];

            for (int i = 0; i < this.speed.Length; i++)
            {
                accel[i] = funcAccelTime(seconds[i]);
            }

            // Assign coeff and return the function of acceleration to speed.
            coeff = Fit.Polynomial(speed, accel, 2);
            funcAccelSpeed = Fit.PolynomialFunc(speed, accel, 2);
        }

        /// <summary>
        /// Given stable speed, power and acceleration function, solve for the coefficients
        /// of Force.
        /// </summary>
        /// <param name="accelSpeedCoeff"></param>
        /// <returns></returns>
        private double[] FitForceSpeed(double[] accelSpeedCoeff)
        {
            // Solve for mass in F=ma
            // Calculate stable reference acceleration.
            double F = stableWatts / stableSpeedMps;
            double accel = funcAccelSpeed(stableSpeedMps);
            double m = F / accel;

            //
            // Coeff of force are equal to mass * coeff of acceleration.
            //
            double[] coeff = new double[3];

            for (int i = 0; i < coeff.Length; i++)
            {
                coeff[i] = accelSpeedCoeff[i] * m;
            }

            return coeff;
        }

        public CalibrationResult Calculate()
        {
            double[] accelCoeff = new double[3];

            // Fit curves from time (y) : speed (x) and then acceleration : speed, and force : speed.
            FitTimeSpeed();
            FitAccelerationSpeed(ref accelCoeff);
            double[] forceCoeff = FitForceSpeed(accelCoeff);

            CalibrationResult config = new CalibrationResult();
            config.GoodnessOfFit = m_goodnessOfFit;
            config.Cd = forceCoeff[2];
            config.Slope = forceCoeff[1];
            config.Intercept = forceCoeff[0];

            m_model.Data.Acceleration = EstimateAcceleration();

            return config;
        }

        /// <summary>
        /// Returns an estimated fit of the coast down curve x:speed, y:time.
        /// </summary>
        public double[,] EstimateCoastdown()
        {
            double max = 10; // max 10 seconds
            double start = 0; // coastdown to 0
            int step = 1; // step every second
            int size = (int)((max - start) / step);
            double[,] curve = new double[size, 2];
            double time = start;

            for (int i = 0; i < size; i++)
            {
                double speed = funcTimeSpeed(time);

                curve[i, 0] = speed;
                curve[i, 1] = time;

                time += step;
            }

            return curve;
        }

        public double[] EstimateAcceleration()
        {
            // x = speed, y = accel
            double[] speedAccel = new double[speed.Length];
            for (int i = 0; i < speed.Length; i++)
            {
                speedAccel[i] = funcAccelSpeed(speed[i]);
            }

            return speedAccel;
        }
    }
}