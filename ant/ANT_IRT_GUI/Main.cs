using ANT_Console;
using ANT_Console.Messages;
using ANT_Console.Services;
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
    public partial class frmIrtGui : Form, IReporter
    {
        Controller m_controller;
        AntBikePower m_eMotion;
        AntBikeSpeed m_refSpeed;
        AntControl m_control;


        public frmIrtGui()
        {
            InitializeComponent();
            m_controller = new Controller();
            this.Load += frmIrtGui_Load;
            //this.Load += (o, e) => { frmIrtGui_Load(o, e);  };
        }

        void frmIrtGui_Load(object sender, EventArgs e)
        {
            // TODO: this might be best done after the DeviceId is set?
            m_controller.ConfigureServices(ushort.Parse(txtEmrDeviceId.Text));

            m_eMotion = m_controller.EMotionBikePower;
            m_control = m_controller.AntRemoteControl;
            m_refSpeed = m_controller.RefBikeSpeed;

            m_controller.Reporters.Add(this);
            m_controller.ConfigureReporters();

            this.Validating += frmIrtGui_Validating;
        }

        void frmIrtGui_Validating(object sender, CancelEventArgs e)
        {
            throw new NotImplementedException();
        }

        public void Report(ANT_Console.Messages.DataPoint data)
        {
            AppendLog(ReportHelper.Format(data));
        }

        public void Report(string message)
        {
            AppendLog(message + "\r\n");
        }

        public void AppendLog(string message)
        {
            this.Invoke(new Action(() => { txtLog.AppendText(message); }));
        }

        private void btnCalibrationSet_Click(object sender, EventArgs e)
        {
            // Bounds check.

            UInt32 value = (UInt32) (ushort.Parse(txtSlope.Text) |
                (ushort.Parse(txtOffset.Text) >> 16));

            m_eMotion.SetParameter(ANT_Console.Messages.SubPages.Crr, value);
        }

        private void ShowInputError(string message)
        {
            MessageBox.Show(message);
        }

        private void btnParamGet_Click(object sender, EventArgs e)
        {
            m_eMotion.RequestDeviceParameter((SubPages)Enum.Parse(typeof(SubPages), 
                txtParamGet.Text));
        }

        private void btnParamSet_Click(object sender, EventArgs e)
        {
            m_eMotion.SetParameter((SubPages)Enum.Parse(typeof(SubPages), txtParamGet.Text),
                uint.Parse(txtParamSet.Text));
        }

        private void btnSettingsSet_Click(object sender, EventArgs e)
        {
            if (txtTotalWeight.Modified)
            {
                statusStrip.Text = "Setting total weight...";
                
                float weightKg = float.Parse(txtTotalWeight.Text) / 2.2f;
                m_eMotion.SetWeight(weightKg);
            }

            if (txtWheelSizeMm.Modified)
            {
                statusStrip.Text += "Setting wheel size...";
                m_eMotion.SetWheelSize(int.Parse(txtWheelSizeMm.Text));
                
            }
            
            if (chkLstSettings.CheckedItems.Count > 0)
            {
                // Todo: iterate through the settings.
                //UInt32 value = chkLstSettings.CheckedItems;
                // m_eMotion.SetParameter(SubPaegs.Settings, value);

            }
        }

        private void btnResistanceSet_Click(object sender, EventArgs e)
        {
            m_eMotion.MoveServo(ushort.Parse(txtServoPos.Text));
        }


        private void btnResistanceInc_Click(object sender, EventArgs e)
        {
            m_control.RemoteControl(ANT_Console.Services.AntControl.RemoteControlCommand.Up);
        }

        private void btnResistanceDec_Click(object sender, EventArgs e)
        {
            m_control.RemoteControl(ANT_Console.Services.AntControl.RemoteControlCommand.Down);
        }

        private void btnDfuEnable_Click(object sender, EventArgs e)
        {
            m_eMotion.SetFirmwareUpdateMode();
            // Todo: set some other kind of state / notification here.
        }

        private void chkCharge_CheckedChanged(object sender, EventArgs e)
        {
            // Toggles charger... need to then also wait for response back and update display.
            m_eMotion.SetParameter(SubPages.Charger, 1 /*any value*/ );
        }

        private void btnServoOffset_Click(object sender, EventArgs e)
        {
            // TODO: this is broken, because servo offset can be negative.
            m_eMotion.SetParameter(SubPages.ServoOffset, uint.Parse(txtServoOffset.Text));
        }

        public enum ResistanceMode
        {
            Standard = 0,
            Percentage,
            Erg,
            Simluation
        }

        private void cmbResistanceMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch ((ResistanceMode)cmbResistanceMode.SelectedIndex)
            {
                case ResistanceMode.Standard:
                    pnlResistanceSim.Hide();
                    pnlErg.Hide();
                    //pnlResistanceStd.BringToFront();
                    pnlResistanceStd.Show();
                    System.Diagnostics.Debug.WriteLine("Standard selected.");
                    break;

                case ResistanceMode.Percentage:
                    pnlResistanceSim.Hide();
                    pnlErg.Hide();
                    pnlResistanceStd.BringToFront();
                    pnlResistanceStd.Show();
                    System.Diagnostics.Debug.WriteLine("Percentage selected.");
                    break;

                case ResistanceMode.Erg:
                    pnlResistanceSim.Hide();
                    pnlResistanceStd.Hide();
                    pnlErg.BringToFront();
                    pnlErg.Show();
                    System.Diagnostics.Debug.WriteLine("Erg selected.");
                    break;

                case ResistanceMode.Simluation:
                    /*pnlErg.Hide();
                    pnlResistanceStd.Hide();
                    pnlResistanceSim.BringToFront();*/
                    pnlResistanceSim.Show();
                    System.Diagnostics.Debug.WriteLine("Sim selected.");
                    break;

                default:
                    pnlResistanceSim.Hide();
                    pnlResistanceStd.Hide();
                    pnlErg.Hide();
                    System.Diagnostics.Debug.WriteLine("No selection.");
                    break;
            }
        }

        
        /*private bool TextBoxRangeCheck<T>(TextBox txt, string name, int min, int max, out T value) where T : IComparable<T>
        {
            
           //TextBoxRangeCheck 
            //return false;
        }*/
    }
}
