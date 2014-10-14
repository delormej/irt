using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace IRT_GUI
{
    public static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        public static void Main() 
        {
            Main(null);
        }

        public static void Main(object simulator)
        {
            Utility.PreventShutdown();

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            frmIrtGui form = null;
            if (simulator != null)
            {
                 form = new frmIrtGui(simulator);
            }
            else
            {
                form = new frmIrtGui();
            }

            string[] args = Environment.GetCommandLineArgs();
            if (args.Length > 1 && args[1] == "-a")
            {
                // Enable admin mode.
                form.AdminEnabled = true;
            }

            Application.Run(form);
        }

        public static void Stop()
        {
            Application.Exit();
        }
    }

    public static class Utility
    {
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern ThreadExecutionState SetThreadExecutionState(ThreadExecutionState esFlags);
        [FlagsAttribute]
        public enum ThreadExecutionState : uint
        {
            CONTINUOUS = 0x80000000,
            DISPLAY_REQUIRED = 0x00000002,
            SYSTEM_REQUIRED = 0x00000001
        }

        public static void PreventShutdown()
        {
            Utility.SetThreadExecutionState(
                Utility.ThreadExecutionState.CONTINUOUS |
                Utility.ThreadExecutionState.DISPLAY_REQUIRED |
                Utility.ThreadExecutionState.SYSTEM_REQUIRED);
        }
    }

}
