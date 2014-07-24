using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;

namespace ANT_Console.Services
{
    public class SpeedSimulator
    {
        public static void Simulate(float speed_mps)
        {
            // (a) Ratio of Flywheel to Wheel (2.07m) Revolutions = (18.52 * 2)
            // (b) Speed in Mph converted to MPS
            // (c) MPS / 2.07 = # of wheel rotations per second
            // (d) Multiply wheel rotations per second times Ratio (a)
            // (e) Start a timer and every 1/16th second, send rotations / 16

            SerialPort port = new SerialPort("COM3");
            try
            {
                Console.WriteLine("Started: {0:ss-FFFFF}", System.DateTime.Now);
                int i = 0;
                port.Open();
                while (i++ < 100)
                {
                    port.RtsEnable = true;
                    Thread.Sleep(1);
                    port.RtsEnable = false;
                }
                Console.WriteLine("Ended: {0:ss-FFFFF}", System.DateTime.Now);
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
            }
            finally
            {
                port.Close();
            }
        }
    }
}
