using System;
using System.Linq;

namespace Calibration
{
    class Program
    {
        // mps
        static double[] x = {  14.12333333, 13.2685, 12.27698925, 11.15, 10.258, 9.366, 8.474, 7.4705, 6.8015, 5.798, 5.129, 4.3485, 3.568, 2.899, 2.1185 };

        // coastdown_sec
        static double[] y = { 6.99951172, 6.49951172, 6.0, 5.5, 5.0, 4.5, 4.0, 3.5, 3.0, 2.5, 2.0, 1.5, 1.0, 0.5, 0.0 };

        static double[] speed = { 2.50889796 ,2.78259592,   3.05629388,   3.32999184,   3.6036898,
           3.87738776,   4.15108571,   4.42478367   ,4.69848163   ,4.97217959,
           5.24587755   ,5.51957551   ,5.79327347   ,6.06697143   ,6.34066939,
           6.61436735   ,6.88806531   ,7.16176327   ,7.43546122   ,7.70915918,
           7.98285714   ,8.2565551    ,8.53025306   ,8.80395102   ,9.07764898,
           9.35134694   ,9.6250449    ,9.89874286  ,10.17244082  ,10.44613878,
          10.71983673  ,10.99353469  ,11.26723265  ,11.54093061  ,11.81462857,
          12.08832653  ,12.36202449  ,12.63572245  ,12.90942041  ,13.18311837,
          13.45681633  ,13.73051429  ,14.00421224  ,14.2779102   ,14.55160816,
          14.82530612  ,15.09900408  ,15.37270204  ,15.6464    };
        
        static double[] watts = { 49.63705235   ,55.26569699   ,60.93820893   ,66.65510302   ,72.41690217,
           78.22413755   ,84.07734874   ,89.97708388   ,95.92389989  ,101.91836259,
          107.96104691  ,114.05253707  ,120.19342676  ,126.38431935  ,132.62582806,
          138.91857619  ,145.26319732  ,151.66033549  ,158.11064549  ,164.61479301,
          171.17345491  ,177.78731943  ,184.45708646  ,191.18346778  ,197.96718728,
          204.80898126  ,211.70959867  ,218.66980141  ,225.69036458  ,232.7720768,
          239.91574046  ,247.12217207  ,254.39220256  ,261.72667757  ,269.12645781,
          276.59241938  ,284.12545414  ,291.72647002  ,299.39639142  ,307.1361596,
          314.94673301  ,322.82908772  ,330.78421785  ,338.81313594  ,346.9168734,
          355.09648097  ,363.35302913  ,371.68760863  ,380.10133089};

        static double contact_patch()
        {
	        //# 'Using this method: http://bikeblather.blogspot.com/2013/02/tire-crr-testing-on-rollers-math.html
	        double wheel_diameter_cm = 210.7 / Math.PI;
	        double drum_diameter_cm = 26 / Math.PI;
	        double patch = Math.Pow((1/(1+ (wheel_diameter_cm/drum_diameter_cm))), 0.7);
	
	        return patch;
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

        static void Main(string[] args)
        {
            Coastdown coastdown = Coastdown.FromFile(
                "C:\\temp\\coastdown\\calib_1.4.5_20150108-164127-3.csv");
                
            coastdown.Calculate(15.0, 137.0);

            System.Console.WriteLine("Drag: {0}, RollingResistance: {1}",
                coastdown.Drag, coastdown.RollingResistance);

            foreach (var i in Enumerable.Range(5, 35))
            {
                System.Console.WriteLine("Speed: {0}, Decel: {1}",
                    i, coastdown.Deceleration(i * 0.44704));
            }
            
            /*foreach (var i in Enumerable.Range(5, 35))
            {
                System.Console.WriteLine("Speed: {0}, Power: {1}",
                    i, coastdown.Watts(i));
            }*/

            /*
            double[,] speed_x = new double[speed.Length, 1];
            
            for (int ix = 0; ix < speed.Length; ix++)
            {
                speed_x[ix, 0] = speed[ix];
            }

            double[] c = { 0.0, 10.0 };
            alglib.lsfitstate state;
            alglib.lsfitreport report3;
            alglib.lsfitcreatef(speed_x, watts, c, 0.0001, out state);
            alglib.lsfitfit(state, fit_func, null, null);
            alglib.lsfitresults(state, out info, out c, out report3);
            */
            
            
            /*
            alglib.polynomialfit(speed, watts, watts.Length, 3, out info, out p2, out report2);
            alglib.polynomialbar2pow(p2, out coeff2);
             */

            Console.ReadKey();
        }
    }
}
