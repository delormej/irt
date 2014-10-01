using ANT_Managed_Library;
using AntPlus.Profiles.BikePower;
using AntPlus.Profiles.Components;
using AntPlus.Types;
//using ANT_Console;
//using ANT_Console.Messages;
//using ANT_Console.Services;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace IRT_GUI
{
    public partial class frmIrtGui : Form
    {
        const int EMR_CHANNEL_ID = 0;
        const int REF_PWR_CHANNEL_ID = 1;
        const byte ANT_FREQUENCY = 0x39;     // 2457 Mhz
        readonly byte[] USER_NETWORK_KEY = { 0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45 };

        private BikePowerDisplay m_eMotion;
        private ANT_Device m_ANT_Device;
        private Network m_ANT_Network;

        public object Simulator
        {
            set
            {
                this.m_eMotion = (BikePowerDisplay)value;  // throw type mismatch if not correct.
            }
        }

        public frmIrtGui()
        {
            InitializeComponent();
            
            this.Load += frmIrtGui_Load;
            //this.Load += (o, e) => { frmIrtGui_Load(o, e);  };
        }

        void frmIrtGui_Load(object sender, EventArgs e)
        {
            try
            {
                //m_ANT_Network = new Network(0, USER_NETWORK_KEY, ANT_FREQUENCY);
                //m_ANT_Device = new ANT_Device();
                //m_ANT_Device.ResetSystem();     // Soft reset
                //System.Threading.Thread.Sleep(500);    // Delay 500ms after a reset

            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.Write(ex);
            }

        }
        private void btnCalibrationSet_Click(object sender, EventArgs e)
        {
            // Bounds check.

            UInt32 value = (UInt32) (ushort.Parse(txtSlope.Text) |
                (ushort.Parse(txtOffset.Text) >> 16));

            //m_eMotion.SetParameter(ANT_Console.Messages.SubPages.Crr, value);
        }

        private void ShowInputError(string message)
        {
            MessageBox.Show(message);
        }

        private void btnParamGet_Click(object sender, EventArgs e)
        {
            //m_eMotion.RequestDeviceParameter((SubPages)Enum.Parse(typeof(SubPages),
            //    txtParamGet.Text));
        }

        private void btnParamSet_Click(object sender, EventArgs e)
        {
            //m_eMotion.SetParameter((SubPages)Enum.Parse(typeof(SubPages), txtParamGet.Text),
            //    uint.Parse(txtParamSet.Text));
        }

        private void btnSettingsSet_Click(object sender, EventArgs e)
        {
            if (txtTotalWeight.Modified)
            {
                statusStrip.Text = "Setting total weight...";
                
                float weightKg = float.Parse(txtTotalWeight.Text) / 2.2f;
                //m_eMotion.SetWeight(weightKg);
            }

            if (txtWheelSizeMm.Modified)
            {
                statusStrip.Text += "Setting wheel size...";
                //m_eMotion.SetWheelSize(int.Parse(txtWheelSizeMm.Text));
                
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
            //m_eMotion.MoveServo(ushort.Parse(txtServoPos.Text));
        }


        private void btnResistanceInc_Click(object sender, EventArgs e)
        {
            //m_control.RemoteControl(ANT_Console.Services.AntControl.RemoteControlCommand.Up);
        }

        private void btnResistanceDec_Click(object sender, EventArgs e)
        {
            //m_control.RemoteControl(ANT_Console.Services.AntControl.RemoteControlCommand.Down);
        }

        private void btnDfuEnable_Click(object sender, EventArgs e)
        {
            //m_eMotion.SetFirmwareUpdateMode();
            // Todo: set some other kind of state / notification here.
        }

        private void chkCharge_CheckedChanged(object sender, EventArgs e)
        {
            // Toggles charger... need to then also wait for response back and update display.
            //m_eMotion.SetParameter(SubPages.Charger, 1 /*any value*/ );
        }

        private void btnServoOffset_Click(object sender, EventArgs e)
        {
            // TODO: this is broken, because servo offset can be negative.
            //m_eMotion.SetParameter(SubPages.ServoOffset, uint.Parse(txtServoOffset.Text));
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

        private void btnEmrSearch_Click(object sender, EventArgs e)
        {
            

        }

        
        /*private bool TextBoxRangeCheck<T>(TextBox txt, string name, int min, int max, out T value) where T : IComparable<T>
        {
            
           //TextBoxRangeCheck 
            //return false;
        }*/

        private void configureEMR()
        {
            m_eMotion = new BikePowerDisplay(m_ANT_Device.getChannel(EMR_CHANNEL_ID), m_ANT_Network);
            m_eMotion.StandardWheelTorquePageReceived += m_eMotion_StandardWheelTorquePageReceived;
            m_eMotion.TurnOn();
        }

        void m_eMotion_StandardWheelTorquePageReceived(StandardWheelTorquePage arg1, uint arg2)
        {
            System.Diagnostics.Debug.WriteLine("Page!");
        }
    }
}
