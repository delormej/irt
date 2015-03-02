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

                if (state == Motion.Stable && seconds > 3.0f)
                {
                    lblSeconds.ForeColor = Color.Green;
                }
                else
                {
                    lblSeconds.ForeColor = SystemColors.ControlText;
                }

                lblStable.ForeColor = lblSeconds.ForeColor;
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

        /// <summary>
        /// Initiate step 2 which is to coastdown.
        /// </summary>
        public void Step2()
        {
            lblStep1.ForeColor = SystemColors.InactiveCaption;
            lblStep2.ForeColor = SystemColors.ControlText;
        }

        private void btnExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
