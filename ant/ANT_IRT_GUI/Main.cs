﻿using ANT_Managed_Library;
using AntPlus.Profiles.BikePower;
using AntPlus.Profiles.Common;
using AntPlus.Profiles.Components;
using AntPlus.Types;
using IRT_GUI.IrtMessages;
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
        readonly byte[] USER_NETWORK_KEY = { 0xb9, 0xa5, 0x21, 0xfb, 0xbd, 0x72, 0xc3, 0x45 };

        
        private BikePowerDisplay m_eMotion;
        private ANT_Device m_ANT_Device;
        private Network m_ANT_Network;

        /* 
        AntBikePower m_eMotion;
        AntBikeSpeed m_refSpeed;
        AntControl m_control;
        SpeedSimulator m_speedSim;
        */

        public frmIrtGui()
        {
            InitializeComponent();
            
            this.Load += frmIrtGui_Load;
            this.Load += (o, e) =>
            {
                if (m_eMotion == null)
                {
                    m_ANT_Device = new ANT_Device();
                    m_ANT_Device.setNetworkKey(0x00, USER_NETWORK_KEY);

                    var r = m_ANT_Device.requestMessageAndResponse(ANT_ReferenceLibrary.RequestMessageID.VERSION_0x3E, 300);
                    System.Diagnostics.Debug.WriteLine(new ASCIIEncoding().GetString(r.messageContents, 0, r.messageContents.Length - 1));

                    m_ANT_Network = new Network(0x00, USER_NETWORK_KEY, ANT_FREQUENCY);
                    ANT_Channel channel = m_ANT_Device.getChannel(0);



                    channel.channelResponse += channel_channelResponse;
                    channel.DeviceNotification += channel_DeviceNotification;
                    channel.rawChannelResponse += channel_rawChannelResponse;

                    m_ANT_Device.serialError += m_ANT_Device_serialError;
                    m_ANT_Device.deviceResponse += m_ANT_Device_deviceResponse;

                    m_eMotion = new BikePowerDisplay(channel, m_ANT_Network);
                }
                
                m_eMotion.TurnOn();
                BindSimulator();
            };
        }

        public frmIrtGui(object simulator)
            : this()
        {
            if (simulator != null && (simulator is BikePowerDisplay))
            {
                m_eMotion = (BikePowerDisplay)simulator;
            }
        }

        void m_ANT_Device_deviceResponse(ANT_Response response)
        {
            try
            {
                System.Diagnostics.Debug.WriteLine(response.getMessageID());
            }
            catch 
            {
                System.Diagnostics.Debug.WriteLine(response.messageContents);
            }
        }

        void m_ANT_Device_serialError(ANT_Device sender, ANT_Device.serialErrorCode error, bool isCritical)
        {
            
        }

        void channel_rawChannelResponse(ANT_Device.ANTMessage message, ushort messageSize)
        {
            if (message.ucharBuf[1] == 0x24)
            {
                // it's our custom message, including servo position.
            }

            //throw new NotImplementedException();
        }

        void channel_DeviceNotification(ANT_Device.DeviceNotificationCode notification, object notificationInfo)
        {
            //throw new NotImplementedException();
        }

        void channel_channelResponse(ANT_Response response)
        {
            if (response.messageContents[1] == 0x24)
            {
                HandleExtraInfo(response);
            }

            //throw new NotImplementedException();
        }

        void HandleExtraInfo(ANT_Response response)
        {
            ExtraInfoMessage message = new ExtraInfoMessage(response);
            UpdateText(txtServoPos, message.ServoPosition);

            if (this.InvokeRequired)
            {
                this.BeginInvoke((Action)delegate()
                {
                    switch ((ResistanceMode)message.Mode)
                    {
                        case ResistanceMode.Standard:
                            cmbResistanceMode.SelectedIndex = 0;
                            UpdateText(lblResistanceStdLevel, message.Level);
                            break;
                        case ResistanceMode.Percent:
                            cmbResistanceMode.SelectedIndex = 1;
                            break;
                        case ResistanceMode.Erg:
                            cmbResistanceMode.SelectedIndex = 2;
                            break;
                        case ResistanceMode.Sim:
                            cmbResistanceMode.SelectedIndex = 3;
                            break;
                        default:
                            break;
                    }
                });
            }
        }

        void BindSimulator()
        {
            m_eMotion.SensorFound += m_eMotion_SensorFound;

            m_eMotion.ManufacturerIdentificationPageReceived += m_eMotion_ManufacturerIdentificationPageReceived;
            m_eMotion.ProductInformationPageReceived += m_eMotion_ProductInformationPageReceived;

            m_eMotion.BatteryStatusPageReceived += m_eMotion_BatteryStatusPageReceived;
            m_eMotion.DataPageReceived += m_eMotion_DataPageReceived;
            m_eMotion.ManufacturerSpecificPageReceived += m_eMotion_ManufacturerSpecificPageReceived;
            m_eMotion.GetSetParametersPageReceived += m_eMotion_GetSetParametersPageReceived;
            m_eMotion.TemperatureSubPageReceived += m_eMotion_TemperatureSubPageReceived;

            m_eMotion.StandardWheelTorquePageReceived += m_eMotion_StandardWheelTorquePageReceived;
            m_eMotion.StandardPowerOnlyPageReceived += m_eMotion_StandardPowerOnlyPageReceived;
        }

        void UpdateText(Control control, object obj)
        {
            if (this.InvokeRequired)
            {
                this.BeginInvoke((Action)delegate()
                {
                    control.Text = obj.ToString();
                });
            }
        }
        
        void m_eMotion_ManufacturerSpecificPageReceived(ManufacturerSpecificPage arg1, uint arg2)
        {
            
        }

        void m_eMotion_SensorFound(ushort arg1, byte arg2)
        {
            UpdateText(txtEmrDeviceId, arg1);
        }

        void m_eMotion_TemperatureSubPageReceived(AntPlus.Profiles.Common.TemperatureSubPage arg1, uint arg2)
        {
            //throw new NotImplementedException();
        }


        void m_eMotion_GetSetParametersPageReceived(GetSetParametersPage arg1, uint arg2)
        {
            // Don't think we use this on the display side today.
            //throw new NotImplementedException();
        }

        void m_eMotion_DataPageReceived(DataPage arg1, uint arg2)
        {
            //throw new NotImplementedException();
        }

        void m_eMotion_BatteryStatusPageReceived(AntPlus.Profiles.Common.BatteryStatusPage arg1, uint arg2)
        {
            float volts = (float)arg1.CoarseBatteryVoltage + 
                ((float)arg1.FractionalBatteryVoltage / 255);

            UpdateText(lblEmrBattVolt, volts.ToString("0.00"));
            // Update the color of the battery status, Red, Green, Yellow.

            double hours = 0.0; 
            int minutes = 0;

            if (arg1.CumulativeOperatingTimeResolution == Common.Resolution.TwoSeconds)
            {
                // CumulativeOperatingTime represents a tick every 2 seconds.
                hours = arg1.CumulativeOperatingTime / (3600 / 2);
            }
            else
            {
                // CumulativeOperatingTime represents a tick every 16 seconds.
                hours = arg1.CumulativeOperatingTime / (3600 / 16);
            }

            minutes = (int)Math.Round((hours - Math.Floor(hours)) / 60, 0);

            UpdateText(lblEmrBattTime, string.Format("{0}:{1:D2}", hours, minutes));
        }

        void m_eMotion_StandardPowerOnlyPageReceived(StandardPowerOnlyPage arg1, uint arg2)
        {
            UpdateText(lblEmrWatts, arg1.InstantaneousPower);
        }


        void m_eMotion_StandardWheelTorquePageReceived(StandardWheelTorquePage arg1, uint arg2)
        {
            // Convert to mph from km/h.
            double mph = m_eMotion.AverageSpeedWheelTorque * 0.621371;
            UpdateText(lblEmrMph, mph.ToString("00.0"));
        }

        void m_eMotion_ProductInformationPageReceived(AntPlus.Profiles.Common.ProductInformationPage arg1, uint arg2)
        {
            UpdateText(lblEmrSerialNo, arg1.SerialNumber.ToString("X"));
            
            string firmwareRev = string.Format("{0}.{1}.{2}", 0, arg1.SoftwareRevision, arg1.SupplementalSoftwareRevision);
            UpdateText(lblEmrFirmwareRev, firmwareRev);
        }

        void m_eMotion_ManufacturerIdentificationPageReceived(AntPlus.Profiles.Common.ManufacturerIdentificationPage arg1, uint arg2)
        {
            UpdateText(lblEmrHardwareRev, arg1.HardwareRevision);
            UpdateText(lblEmrModel, arg1.ModelNumber);
        }
        
        void frmIrtGui_Load(object sender, EventArgs e)
        {
            try
            {
                //m_eMotion = controller.EMotionBikePower;
                //m_control = controller.AntRemoteControl;
                //m_refSpeed = controller.RefBikeSpeed;


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

                case ResistanceMode.Percent:
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

                case ResistanceMode.Sim:
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

    }
}
