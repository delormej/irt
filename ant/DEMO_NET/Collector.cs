using ANT_Managed_Library;
using System;
using ANT_Console.Messages;
using System.Collections;
using System.Collections.Generic;

namespace ANT_Console.Collectors
{
    class Reporter
    {
       public void Report(DataPoint data)
        {
            Console.WriteLine(data);
        }
    }
}
