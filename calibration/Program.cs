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
                
            coastdown.Calculate(15.0 * 0.44704, 134.0);

            System.Console.WriteLine("Drag: {0}, RollingResistance: {1}",
                coastdown.Drag, coastdown.RollingResistance);

            // Display rate of deceleration.
            foreach (var i in Enumerable.Range(5, 35))
            {
                System.Console.WriteLine("Speed: {0}, Decel: {1}",
                    i, coastdown.Deceleration(i * 0.44704));
            }
            
            // Display power for speed.
            foreach (var i in Enumerable.Range(5, 35))
            {
                System.Console.WriteLine("Speed: {0}, Power: {1}",
                    i, coastdown.Watts(i * 0.44704));
            }
           
            Console.ReadKey();
        }
    }
}
