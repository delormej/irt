using ANT_Console;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ANT_IRT_GUI
{
    public partial class frmIrtGui : Form
    {
        Controller m_controller;

        public frmIrtGui()
        {
            InitializeComponent();
            m_controller = new Controller();
            this.Load += frmIrtGui_Load;
        }

        void frmIrtGui_Load(object sender, EventArgs e)
        {
            // TODO: this might be best done after the DeviceId is set?
            m_controller.ConfigureServices();
        }
    }
}
