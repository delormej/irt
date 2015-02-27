using System;
using System.Linq;

namespace Calibration
{
    class Program
    {
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
            
            foreach (var i in Enumerable.Range(5, 35))
            {
                System.Console.WriteLine("Speed: {0}, Power: {1}",
                    i, coastdown.Watts(i));
            }
           
            /*
            alglib.polynomialfit(speed, watts, watts.Length, 3, out info, out p2, out report2);
            alglib.polynomialbar2pow(p2, out coeff2);
             */

            Console.ReadKey();
        }
    }
}
