﻿using System;
using System.Linq;
using System.Collections.Generic;
using System.IO;


namespace Calibration
{
    /// <summary>
    /// Calculates rate of deceleration for a given speed.
    /// </summary>
    internal class DecelerationFit
    {
        alglib.barycentricinterpolant m_interpolant;
        double[] m_speedMps, m_coastdownSeconds;  // x, y coordinates.

        public DecelerationFit(double[] speedMps, double[] coastdownSeconds)
        {
            m_speedMps = speedMps;
            m_coastdownSeconds = coastdownSeconds;
        }

        /// <summary>
        /// Returns the rate of deceleration in meters per second for a given speed.
        /// </summary>
        /// <param name="speed_mps"></param>
        /// <returns></returns>
        public double Rate(double speed_mps)
        {
            return alglib.barycentriccalc(m_interpolant, speed_mps);
        }


        /// <summary>
        /// Smoothes and fits raw coast down data to a curve.  This method must 
        /// be called before evaluating any deceleration rates.
        /// </summary>
        public void Fit()
        {
            // internal implementation of curve fitting.
            int info = 0;
            double[] coeff;

            alglib.polynomialfitreport report;

            alglib.polynomialfit(m_speedMps, m_coastdownSeconds, m_speedMps.Length, 
                3, out info, out m_interpolant, out report);
            alglib.polynomialbar2pow(m_interpolant, out coeff);

            if (info != 1)
            {
                throw new CoastdownException("Unable to fit deceleration curve.");
            }
        }
    }

    internal class PowerFit
    {
        private double[] m_coeff = { 0.0, 0.0 };
        private DecelerationFit m_decelFit;
        private double m_inertia;

        public PowerFit(DecelerationFit decelFit, double stableSpeedMps, double stableWatts)
        {
            m_decelFit = decelFit;
            m_inertia = MomentOfInteria(stableSpeedMps, stableWatts);
        }

        public double Drag { get { return m_coeff[0]; } }
        
        public double RollingResistance { get { return m_coeff[1]; } }

        public double Watts(double speedMps)
        {
            return fit_drag_rr(speedMps, Drag, RollingResistance);
        }

        /// <summary>
        /// Calculates the moment of inertia a.k.a mass in F=ma.
        /// </summary>
        /// <param name="speed_mps"></param>
        /// <param name="watts"></param>
        /// <returns></returns>
        private double MomentOfInteria(double speedMps, double watts)
        {
            // Where P = F*v, F=ma

            double f = watts / speedMps;
            double a = m_decelFit.Rate(speedMps);

            // Get the rate of deceleration (a) for a given velocity.
            
            // Solve for m = f/a
            return f / a;
        }

        public void Fit()
        {
            int info;
            double[,] speed;
            double[] watts;

            GeneratePowerData(out speed, out watts);

            alglib.lsfitstate state;
            alglib.lsfitreport report;
            alglib.lsfitcreatef(speed, watts, m_coeff, 0.0001, out state);
            alglib.lsfitfit(state, fit_func, null, null);
            alglib.lsfitresults(state, out info, out m_coeff, out report);
        }

        private void GeneratePowerData(out double[,] speed, out double[] watts)
        {
            speed = new double[30, 1];
            watts = new double[30];

            int ix = 0;
            // Generate power data.
            foreach (var i in Enumerable.Range(5, 30))
            {
                double v = i * 0.44704;
                double a = m_decelFit.Rate(v);
                double f = m_inertia * a;
                
                speed[ix, 0] = v;
                watts[ix] = f * v;

                ix++;
            }
        }

        static double fit_drag_rr(double v, double K, double rr)
        {
            double p = K * Math.Pow(v, 2) + ((v * rr)); // / contact_patch());
            // p = Kv^2 + rrv + c

            return p;
        }

        static void fit_func(double[] c, double[] x, ref double func, object obj)
        {
            func = fit_drag_rr(x[0], c[0], c[1]);
        }

        static double contact_patch()
        {
            //# 'Using this method: http://bikeblather.blogspot.com/2013/02/tire-crr-testing-on-rollers-math.html
            double wheel_diameter_cm = 210.7 / Math.PI;
            double drum_diameter_cm = 26 / Math.PI;
            double patch = Math.Pow((1 / (1 + (wheel_diameter_cm / drum_diameter_cm))), 0.7);

            return patch;
        }
    }

    /// <summary>
    /// Filters and processes raw data into usable speed/coastdown seconds.
    /// </summary>
    internal class CoastdownData
    {
        private int[] m_timestamp, m_flywheel;

        public CoastdownData(int[] timestamp_2048, int[] flywheel_ticks)
        {
            m_timestamp = timestamp_2048;
            m_flywheel = flywheel_ticks;
        }

        public double[] SpeedMps;
        public double[] CoastdownSeconds;

        public void Evaluate()
        {
            double[] seconds = new double[m_timestamp.Length/4];
            double[] speed = new double[m_flywheel.Length/4];

            int ix = 0;
            for (int idx = 0; idx < m_flywheel.Length; idx++)
            {
                int val = m_flywheel[idx];
                int dt, ds;

                // Evaluate every 4th record.
		        if (idx > 0 && idx % 4 == 0)
                {
			        if (val < m_flywheel[idx-4])
				        dt = val + (m_flywheel[idx-4] ^ 0xFFFF);
			        else
				        dt = val-m_flywheel[idx-4];

			        if (m_timestamp[idx] < m_timestamp[idx-4])
				        ds = m_timestamp[idx] + (m_timestamp[idx-4] ^ 0xFFFF);
			        else
				        ds = m_timestamp[idx]-m_timestamp[idx-4];

			        if (ix > 0)
                        seconds[ix] = (ds / 2048.0) + seconds[ix - 1];
			        else
                        seconds[ix] = (ds / 2048.0);

                    /* Each flywheel revolution equals 0.1115 meters travelled by the 
                     * bike.  Two ticks are recorded for each revolution.
                     * Time is sent in 1/2048 of a second.
                     */
                    speed[ix++] = (dt * 0.1115 / 2.0) / (ds / 2048.0);
                }
            }

            int max = FindDecelerationIndex(speed);
            int min = FindMinSpeedIndex(speed);
            int len = min - max;

            SpeedMps = new double[len];
            CoastdownSeconds = new double[len];
            Array.Copy(speed, max, SpeedMps, 0, len);
            
            // Invert the timestamp seconds to record seconds to min speed.
            for (int i = 0; i < len; i++)
            {
                CoastdownSeconds[i] = seconds[len - 1] - seconds[i];
            }
        }

        /// <summary>
        /// Finds the point at which deceleration begins.
        /// </summary>
        /// <param name="values"></param>
        /// <returns></returns>
        private int FindDecelerationIndex(double[] speeds)
        {
            // Progress from the end of the array to the beginning.
            // Ensure that the values continue to increase.
            for (int i = speeds.Length-1; i > 0; i--)
            {
                if (speeds[i] >= speeds[i - 1])
                {
                    return i+1;
                }
            }

            // We shouldn't really get here.
            return 0;
        }

        private int FindMinSpeedIndex(double[] speeds)
        {
            // Get the first occurrence of 1 tick delta.
            // Array.IndexOf<double>(speeds, 1.0);
            
            return speeds.Length;
        }
    }


    /// <summary>
    /// Handles the filtering and process of the data files.
    /// Front end interface to the end results.
    /// </summary>
    public class Coastdown
    {
        private List<int> m_timestamps, m_flywheel_ticks;

        // Fit objects.
        private DecelerationFit m_decelFit;
        private PowerFit m_powerFit;

        public Coastdown()
        {
            m_timestamps = new List<int>();
            m_flywheel_ticks = new List<int>();
        }

        /// <summary>
        /// Add a coast down tick event.
        /// </summary>
        /// <param name="timestamp_2048"></param>
        /// <param name="flywheel_ticks"></param>
        public void Add(int timestamp_2048, int flywheel_ticks)
        {
            m_timestamps.Add(timestamp_2048);
            m_flywheel_ticks.Add(flywheel_ticks);
        }

        /// <summary>
        /// Parses the coast down x,y values and generates coefficients of Drag 
        /// and Rolling Resistance based on stable speed and watts.
        /// </summary>
        public void Calculate(double stableSpeed, double stableWatts)
        {
            // Process values to build speed/time arrays.
            CoastdownData data = new CoastdownData(m_timestamps.ToArray(), 
                m_flywheel_ticks.ToArray());
            data.Evaluate();

            // Calculate the deceleration.
            m_decelFit = new DecelerationFit(data.SpeedMps, data.CoastdownSeconds);
            m_decelFit.Fit();
            m_powerFit = new PowerFit(m_decelFit, stableSpeed, stableWatts);
            m_powerFit.Fit();

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
        }

        /// <summary>
        /// Calculated coefficient of Drag.
        /// </summary>
        public double Drag { get { return m_powerFit.Drag; } }

        /// <summary>
        /// Calculated coefficient of Rolling Resistance.
        /// </summary>
        public double RollingResistance { get { return m_powerFit.RollingResistance; } }

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
        /// Creates an instance of the class by parsing a coastdown file.
        /// </summary>
        /// <param name="filename"></param>
        /// <returns></returns>
        public static Coastdown FromFile(string filename)
        {
            int timestamp, ticks;
            Coastdown coastdown = new Coastdown();

            using (StreamReader reader = File.OpenText(filename))
            {
                // Advance through header.
                reader.ReadLine();

                while (!reader.EndOfStream)
                {
                    ParseLine(reader.ReadLine(), out timestamp, out ticks);
                    coastdown.Add(timestamp, ticks);
                }
            }
            
            return coastdown;
        }

        /// <summary>
        /// Parses a line from the csv file.
        /// </summary>
        /// <param name="line"></param>
        /// <param name="timestamp"></param>
        /// <param name="ticks"></param>
        private static void ParseLine(string line, out int timestamp, out int ticks)
        {
            string[] column = line.Split(',');
            
            if (!int.TryParse(column[0], out timestamp))
            {
                throw new FormatException("CSV not properly formatted.");
            }

            if (!int.TryParse(column[2], out ticks))
            {
                throw new FormatException("CSV not properly formatted.");
            }
        }    
    }

    public class CoastdownException : Exception
    {
        public CoastdownException(string message) : base(message)
        {

        }
    }
}
