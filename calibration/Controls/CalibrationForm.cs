using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using IRT.Calibration.Globals;

namespace IRT.Calibration
{
    public partial class CalibrationForm : Form
    {
        private Model m_model;

        public CalibrationForm(Model model)
        {
            model = m_model;
            InitializeComponent();
        }

        public void UpdateValues(double mph, double stableSeconds, double stableMph, ushort stableWatts, ushort watts, Motion state)
        {
            Action a = () =>
            {
                lblSpeed.Text = string.Format("{0:0.0}", mph);
                lblSeconds.Text = string.Format("{0:0.0}", stableSeconds);
                lblStableSpeed.Text = string.Format("{0:0.0}", stableMph);
                lblRefPower.Text = stableWatts.ToString();
                lblInstantPower.Text = watts.ToString();
                lblStable.Text = state.ToString();


                if (state == Motion.Stable && stableSeconds > 3.0f)
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
        /// Initiate step 2 which is to speed up.
        /// </summary>
        public void Step2()
        {
            lblStep1.ForeColor = SystemColors.InactiveCaption;
            lblStep3.ForeColor = SystemColors.InactiveCaption;

            lblStep2.ForeColor = SystemColors.ControlText;
        }

        /// <summary>
        /// Initiate step 3 which is to coast down.
        /// </summary>
        public void Step3()
        {
            lblStep1.ForeColor = SystemColors.InactiveCaption;
            lblStep2.ForeColor = SystemColors.InactiveCaption;

            lblStep3.ForeColor = SystemColors.ControlText;
        }

        private void btnExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
