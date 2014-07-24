using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {
            short speed = 1;
            SerialPort port = new SerialPort("COM5");
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
            finally 
            {
                port.Close();
            }

            Console.Read();
        }
    }
}
