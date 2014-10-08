using System;
using System.Collections.Generic;
using System.Linq;
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
            Application.Run(form);
        }

        public static void Stop()
        {
            Application.Exit();
        }
    }
}
