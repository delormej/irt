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

        public void Update(double mph, double ms, int watts, CalibrationFormState state)
        {
            string instructions = "";

            switch (state)
            {
                case CalibrationFormState.BelowSpeed:
                    instructions = "Increase your speed...";
                    break;
                case CalibrationFormState.Stabilizing:
                    instructions = "Stablize your speed...";
                    break;
                case CalibrationFormState.Ready:
                    instructions = "Stop pedaling and coast...";
                    break;
                case CalibrationFormState.Coasting:
                    instructions = "Coasting...";
                    break;
                case CalibrationFormState.Done:
                    instructions = "Data Captured!";
                    Exit();
                    break;
                default:
                    instructions = "";
                    break;
            }

            Action a = () =>
            {
                lblSeconds.Text = string.Format("{0:0.0}", ms / 1000.0f);
                lblSpeed.Text = string.Format("{0:0.0}", mph);
                lblInstructions.Text = instructions;
            };

            this.BeginInvoke(a);
        }

        

        private void btnExit_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        public void Exit()
        {
            System.Threading.Thread.Sleep(1000);
            this.Close();
        }

    }
}
