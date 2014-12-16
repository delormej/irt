using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace IRT_GUI
{
    public partial class CalibrationForm : Form
    {
        public CalibrationForm()
        {
            InitializeComponent();
        }

        public void UpdateValues(double seconds, double mph, ushort watts, Motion state)
        {
            Action a = () =>
            {
                lblSeconds.Text = string.Format("{0:0.0}", seconds);
                lblSpeed.Text = string.Format("{0:0.0}", mph);
                lblRefPower.Text = watts.ToString();
                lblStable.Text = state.ToString();
            };

            if (this.InvokeRequired)
            {
                this.BeginInvoke(a);
            }
            else
            {
                a.Invoke();
            }
        }

        private void btnExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
