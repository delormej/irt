using IRT.Calibration;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace IRTCalibration
{
    public partial class CoastdownForm : Form
    {
        public event Action<int> Apply;

        public CoastdownForm(Coastdown coastdown)
        {
            InitializeComponent();
            
            this.lblDrag.Text = coastdown.Drag.ToString();
            this.lblRR.Text = coastdown.RollingResistance.ToString();
        }

        private void btnApply_Click(object sender, EventArgs e)
        {
            if (Apply != null)
            {
                Apply(0);
            }

            this.Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
