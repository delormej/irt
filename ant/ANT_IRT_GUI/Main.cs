using ANT_Managed_Library;
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
using Message = IRT_GUI.IrtMessages.Message;

namespace IRT_GUI
{
    public partial class frmIrtGui : Form
    {
        const int EMR_CHANNEL_ID = 0;
        const int REF_PWR_CHANNEL_ID = 1;
        const byte ANT_FREQUENCY = 0x39;     // 2457 Mhz
        readonly byte[] USER_NETWORK_KEY = { 0xb9, 0xa5, 0x21, 0xfb, 0xbd, 0x72, 0xc3, 0x45 };

        const uint ACK_TIMEOUT = 1000;
        const uint REQUEST_RETRY = 3;
        byte m_sequence;

        // Commands
        enum Command : byte
        {
            SetWeight = 0x60,
            MoveServo = 0x61,
            SetButtonStops = 0x62,
            SetDFUMode = 0x64
        };

        private BikePowerDisplay m_eMotion;
        private BikePowerDisplay m_refPower;
        private ANT_Device m_ANT_Device;
        private Network m_ANT_Network;
        private ANT_Channel m_eMotionChannel;
        private ANT_Channel m_refChannel;

        // Wrapper for executing on the UI thread.
        private void ExecuteOnUI(Action a)
        {
            if (this.InvokeRequired)
            {
                this.BeginInvoke(a);
            }
            else
            {
                a.Invoke();
            }
        }


        public frmIrtGui()
        {
            InitializeComponent();
            this.Load += frmIrtGui_Load;
        }

        public bool AdminEnabled
        {
            get;
            set;
        }

        public frmIrtGui(object simulator)
            : this()
        {
            if (simulator != null && (simulator is BikePowerDisplay))
            {
                m_eMotion = (BikePowerDisplay)simulator;
            }
        }


        void frmIrtGui_Load(object sender, EventArgs e)
        {
            // Admin only features.
            if (AdminEnabled)
            {
                txtServoOffset.Enabled = true;
                btnServoOffset.Visible = true;
                btnServoOffset.Enabled = true;
            }
            
            // Setup the settings checklist box.
            chkLstSettings.Items.Clear();
            chkLstSettings.Items.AddRange(Enum.GetNames(typeof(Settings)));

            // Configure and start listening on ANT+.
            StartANT();
        }

        void m_ANT_Device_deviceResponse(ANT_Response response)
        {
            try
            {
                //System.Diagnostics.Debug.WriteLine(response.getMessageID());
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
            try
            {
                // Handle E-Motion raw channel responses.
                if (response.antChannel == EMR_CHANNEL_ID)
                {
                    switch (response.messageContents[1])    // Page Number
                    {
                        case GetSetMessage.Page:
                            ProcessMessage(new GetSetMessage(response));
                            break;

                        case ExtraInfoMessage.Page:
                            ProcessMessage(new ExtraInfoMessage(response));
                            break;

                        default:
                            break;

                    }
                }
            }
            catch (Exception e)
            {
                // log
            }
        }

        private void ProcessMessage(ExtraInfoMessage message)
        {
            /*
            m_data.Timestamp = System.DateTime.Now;
            m_data.ServoPosition = m.ServoPosition;
            m_data.Temperature = m.Temperature;
            m_data.FlywheelRevs = m.FlyweelRevs;
            m_data.TargetLevel = m.Level;
            m_data.ResistanceMode = m.Mode;
             */
            
            UpdateText(txtServoPos, message.ServoPosition);
            UpdateText(lblFlywheel, message.FlyweelRevs);

            ExecuteOnUI(() =>
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

        /// <summary>
        /// Happens when the get/set parameters is sent from the device.
        /// </summary>
        /// <param name="m"></param>
        private void ProcessMessage(GetSetMessage m)
        {
            byte[] buffer = m.AsBytes();

            // Sub page
            switch ((SubPages)m.SubPage)
            {
                case SubPages.Crr:
                    ushort slope = Message.BigEndian(buffer[2], buffer[3]);
                    ushort intercept = Message.BigEndian(buffer[4], buffer[5]);

                    if (slope == short.MaxValue)
                        slope = 0;
                    if (intercept == short.MaxValue)
                        intercept = 0;

                    UpdateText(txtSlope, slope);
                    UpdateText(txtOffset, intercept);
                    break;

                case SubPages.TotalWeight:
                    int grams = Message.BigEndian(buffer[2], buffer[3]);
                    double kg = grams / 100.0;
                    UpdateText(txtTotalWeight, kg.ToString("N2"));

                    break;

                case SubPages.ServoOffset:
                    ushort servoOffset = Message.BigEndian(buffer[2], buffer[3]);
                    UpdateText(txtServoOffset, servoOffset);
                    break;

                case SubPages.CalibrationSpeed:
                    //var calibration = new CalibrationSpeed(buffer);
                    //Console.WriteLine(calibration);
                    //Report(calibration);
                    break;

                case SubPages.WheelSize:
                    //m_eMotion.WheelCircumference
                    ushort wheelSize = Message.BigEndian(buffer[2], buffer[3]);
                    UpdateText(txtWheelSizeMm, wheelSize);
                    break;

                case SubPages.Settings:
                    ushort settings = Message.BigEndian(buffer[2], buffer[3]);
                    UpdateSettings(settings);
                    break;

                case SubPages.Charger:
                    ExecuteOnUI(() =>
                    {
                        if (buffer[2] == 0x02)
                        {
                            chkCharge.Checked = true;
                        }
                        else
                        {
                            chkCharge.Checked = false;
                        }
                    });
                    break;

                default:
                    Console.WriteLine("Received Parameters page: {0} - [{1:x2}][{2:x2}][{3:x2}][{4:x2}][{5:x2}][{6:x2}]", m.SubPage,
                        buffer[2],
                        buffer[3],
                        buffer[4],
                        buffer[5],
                        buffer[6],
                        buffer[7]);
                    break;
            }
        }

        void UpdateSettings(ushort settings)
        {
            ExecuteOnUI(() =>
            {
                for (int i = 0; i < chkLstSettings.Items.Count; i++)
                {
                    // Get the setting
                    Settings setting;
                    Enum.TryParse<Settings>(chkLstSettings.Items[i].ToString(), out setting);

                    bool check = ((settings & (ushort)setting) == (ushort)setting);
                    chkLstSettings.SetItemChecked(i, check);
                }
            });
        }

        void StartANT()
        {
            m_ANT_Device = new ANT_Device();
            m_ANT_Device.ResetSystem(500);

            m_ANT_Device.setNetworkKey(0x00, USER_NETWORK_KEY);

            m_ANT_Network = new Network(0x00, USER_NETWORK_KEY, ANT_FREQUENCY);
            m_eMotionChannel = m_ANT_Device.getChannel(EMR_CHANNEL_ID);

            // Temporary - not sure how much of this I need.
            m_eMotionChannel.channelResponse += channel_channelResponse;
            m_eMotionChannel.DeviceNotification += channel_DeviceNotification;
            m_eMotionChannel.rawChannelResponse += channel_rawChannelResponse;

            m_ANT_Device.serialError += m_ANT_Device_serialError;
            m_ANT_Device.deviceResponse += m_ANT_Device_deviceResponse;

            m_eMotion = new BikePowerDisplay(m_eMotionChannel, m_ANT_Network);
            m_eMotion.ChannelParameters.TransmissionType = 0xA5;

            m_eMotion.SensorFound += m_eMotion_SensorFound;
            m_eMotion.ChannelStatusChanged += m_eMotion_ChannelStatusChanged;

            m_eMotion.ManufacturerIdentificationPageReceived += m_eMotion_ManufacturerIdentificationPageReceived;
            m_eMotion.ProductInformationPageReceived += m_eMotion_ProductInformationPageReceived;

            m_eMotion.BatteryStatusPageReceived += m_eMotion_BatteryStatusPageReceived;
            m_eMotion.DataPageReceived += m_eMotion_DataPageReceived;
            m_eMotion.ManufacturerSpecificPageReceived += m_eMotion_ManufacturerSpecificPageReceived;
            m_eMotion.GetSetParametersPageReceived += m_eMotion_GetSetParametersPageReceived;
            m_eMotion.TemperatureSubPageReceived += m_eMotion_TemperatureSubPageReceived;

            m_eMotion.StandardWheelTorquePageReceived += m_eMotion_StandardWheelTorquePageReceived;
            m_eMotion.StandardPowerOnlyPageReceived += m_eMotion_StandardPowerOnlyPageReceived;

            // Start looking for e-motion.
            m_eMotion.TurnOn();

            // Configure reference power channel, but don't start it.
            m_refChannel = m_ANT_Device.getChannel(REF_PWR_CHANNEL_ID);
            m_refPower = new BikePowerDisplay(m_refChannel, m_ANT_Network);

            m_refPower.ChannelParameters.TransmissionType = 0x5;
            m_refPower.StandardPowerOnlyPageReceived += m_refPower_StandardPowerOnlyPageReceived;
            m_refPower.ManufacturerIdentificationPageReceived += m_refPower_ManufacturerIdentificationPageReceived;
            m_refPower.SensorFound += m_refPower_SensorFound;
            m_refPower.ChannelStatusChanged += m_refPower_ChannelStatusChanged;
        }

        void m_refPower_ChannelStatusChanged(ChannelStatus status)
        {
            ExecuteOnUI(() =>
            {
                switch (status)
                {
                    case ChannelStatus.Closed:
                        btnRefPwrSearch.Enabled = true;
                        txtRefPwrDeviceId.Enabled = true;
                        btnRefPwrSearch.Text = "Search";
                        UpdateStatus("Closed reference power meter channel.");
                        break;

                    case ChannelStatus.Searching:
                        btnRefPwrSearch.Enabled = false;
                        UpdateStatus("Searching for power meter.");
                        break;

                    case ChannelStatus.Tracking:
                        btnRefPwrSearch.Enabled = true;
                        btnRefPwrSearch.Text = "Close";
                        txtRefPwrDeviceId.Enabled = false;
                        UpdateStatus("Found power meter.");
                        break;

                    default:
                        break;
                }
            });
        }

        void m_eMotion_ChannelStatusChanged(ChannelStatus status)
        {
            UpdateStatus("E-Motion channel status changed: " + status.ToString());

            ExecuteOnUI(() =>
            {
                switch (status)
                {
                    case ChannelStatus.Closed:
                        btnEmrSearch.Enabled = true;
                        txtEmrDeviceId.Enabled = true;
                        UpdateText(txtEmrDeviceId, 0);
                        btnEmrSearch.Text = "Search";
                        break;

                    case ChannelStatus.Searching:
                        btnEmrSearch.Enabled = false;
                        break;

                    case ChannelStatus.Tracking:
                        btnEmrSearch.Enabled = true;
                        UpdateText(btnEmrSearch, "Close");
                        txtEmrDeviceId.Enabled = false;
                        break;

                    default:
                        break;
                }
            });
        }

        void m_refPower_StandardPowerOnlyPageReceived(StandardPowerOnlyPage arg1, uint arg2)
        {
            UpdateText(lblRefPwrWatts, arg1.InstantaneousPower);
        }

        void m_refPower_SensorFound(ushort arg1, byte arg2)
        {
            UpdateText(txtRefPwrDeviceId, arg1);
        }

        void m_refPower_ManufacturerIdentificationPageReceived(ManufacturerIdentificationPage arg1, uint arg2)
        {
            UpdateText(lblRefPwrModel, arg1.ModelNumber);
            UpdateText(lblRefPwrManuf, arg1.ManufacturerIdentification);
        }

        void UpdateText(Control control, object obj)
        {
            ExecuteOnUI(() => control.Text = obj.ToString());
        }

        void m_eMotion_ManufacturerSpecificPageReceived(ManufacturerSpecificPage arg1, uint arg2)
        {
            // wahoo resistance ack pages come through here.
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
            // This isn't going to work because GetSetParametersPage doesn't contain the value.
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
                hours = arg1.CumulativeOperatingTime / (3600.0d / 2.0d);
            }
            else
            {
                // CumulativeOperatingTime represents a tick every 16 seconds.
                hours = arg1.CumulativeOperatingTime / (3600.0d / 16.0d);
            }

            // Convert to minutes.
            minutes = (int)((hours - Math.Floor(hours)) * 60);

            UpdateText(lblEmrBattTime, string.Format("{0:N0}:{1:D2}", hours, minutes));
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


        private void ShowInputError(string message)
        {
            MessageBox.Show(message);
        }

        private void btnParamGet_Click(object sender, EventArgs e)
        {
            byte param = GetParameter();
            if (param != 0)
            {
                RequestDeviceParameter((SubPages)param);
            }
        }

        private void btnParamSet_Click(object sender, EventArgs e)
        {
            UInt32 value = 0;

            byte param = GetParameter();
            if (param == 0 || String.IsNullOrEmpty(txtParamSet.Text))
            {
                UpdateStatus("Could not set, enter a valid parameter and value.");
                return;
            }

            // 0 value OK
            UInt32.TryParse(txtParamSet.Text, out value);
            SetParameter(param, value);
        }

        private void SetParameter(byte subpage, UInt32 value)
        {
            GetSetMessage message = new GetSetMessage(subpage);
            message.SetPayLoad(value);
            var result = m_eMotionChannel.sendAcknowledgedData(message.AsBytes(), ACK_TIMEOUT);
            if (result != ANT_ReferenceLibrary.MessagingReturnCode.Pass)
            {
                UpdateStatus(string.Format("Unable to send set parameter, return result: {0}.", result));
            }
        }

        private byte GetParameter()
        {
            byte param = 0;
            byte.TryParse(txtParamGet.Text, out param);

            if (param == 0)
            {
                UpdateStatus("Please enter a parameter number.");
            }

            return param;
        }

        private void btnSettingsSet_Click(object sender, EventArgs e)
        {
            if (txtTotalWeight.Modified)
            {
                float weightKg = 0;
                float.TryParse(txtTotalWeight.Text, out weightKg);

                if (weightKg > 20.0f && weightKg < 140.0f)
                {
                    UpdateStatus("Setting total weight.");
                    SetWeight(weightKg);
                }
                else
                {
                    UpdateStatus("Invalid weight.");
                }
            }

            if (txtWheelSizeMm.Modified)
            {
                ushort wheelSizeMM = 0;
                ushort.TryParse(txtWheelSizeMm.Text, out wheelSizeMM);

                if (wheelSizeMM > 1500 && wheelSizeMM < 3000) // Valid range? 
                {
                    UpdateStatus("Setting wheel size.");
                    SetWheelSize(wheelSizeMM);
                }
                else
                {
                    UpdateStatus("Invalid wheel size.");
                }
            }
            
            /*if (chkLstSettings. chkLstSettings.CheckedItems.Count > 0)
            {
                // Todo: iterate through the settings.
                //UInt32 value = chkLstSettings.CheckedItems;
                // m_eMotion.SetParameter(SubPaegs.Settings, value);

            }*/
        }

        private void SetWeight(float weight)
        {
            ushort value = (ushort)(weight * 100);
            byte[] data = {
                (byte)value, // Weight LSB
                (byte)(value >> 8), // Weight MSB
            };

            SendCommand(Command.SetWeight, data);
        }

        // This uses the WAHOO method with a burst message.
        private bool SetWheelSize(ushort wheelSizeMM)
        {
            byte[] data = { 
                // Message 1
                0x48,
                0x48, // set wheel size mode
                0x00,
                0x00,
                0x00,
                0x00,
                0x00,
                0x00, 
                // Message 2
                0x00,
                0x00,
                0x00,
                0x00,
                0x00,
                0x00, 
                0x00,
                0x00, 
                // Message 3
                (byte)(wheelSizeMM*10),        // Wheel size LSB
                (byte)((wheelSizeMM*10) >> 8),   // Wheel size MSB          
                0x00,
                0x00,
                0x00,
                0x00, 
                0x00,
                0x00
                          };

            var result = m_eMotionChannel.sendBurstTransfer(data, 500);
            
            if (result != ANT_ReferenceLibrary.MessagingReturnCode.Pass)
            {
                UpdateStatus("Unable to set wheel size, result: " + result);
            }

            return (result == ANT_ReferenceLibrary.MessagingReturnCode.Pass);
        }

        bool SendCommand(Command command, byte[] value)
        {
            byte[] data = ResistanceMessage.GetCommand(
                (byte)command, m_sequence++, value);

            ANT_ReferenceLibrary.MessagingReturnCode ret = 
                m_eMotionChannel.sendAcknowledgedData(data, 500);

            return (ret == ANT_ReferenceLibrary.MessagingReturnCode.Pass);
        }

        bool SendCommand(Command command)
        {
            byte[] value = { 0, 0 };
            return SendCommand(command, value);
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
            ANT_ChannelStatus status = m_eMotionChannel.requestStatus(500);
            ANT_Managed_Library.ANT_ReferenceLibrary.BasicChannelStatusCode code = status.BasicStatus;

            // Channel is open, so lets close.
            if (code == ANT_ReferenceLibrary.BasicChannelStatusCode.TRACKING_0x3)
            {
                // Close the channel.
                UpdateStatus("Closing E-Motion channel.");
                CloseEmotion();
            }
            else
            {
                // Channel isn't open, go ahead and search for E-Motion.
                ushort deviceId = 0;
                ushort.TryParse(txtEmrDeviceId.Text, out deviceId);

                m_eMotion.ChannelParameters.DeviceNumber = deviceId;
                m_eMotion.TurnOn();
            }
        }

        private void btnRefPwrSearch_Click(object sender, EventArgs e)
        {
            ANT_ChannelStatus status = m_refChannel.requestStatus(500);
            ANT_Managed_Library.ANT_ReferenceLibrary.BasicChannelStatusCode code = status.BasicStatus;

            // Channel is open, so lets close.
            if (code == ANT_ReferenceLibrary.BasicChannelStatusCode.TRACKING_0x3)
            {
                // Close the channel.
                //m_refChannel.closeChannel(500);
                CloseRefPower();
            }
            else
            {
                // Channel isn't open, go ahead and search for Power Meter.
                ushort deviceId = 0;
                ushort.TryParse(txtRefPwrDeviceId.Text, out deviceId);

                m_refPower.ChannelParameters.DeviceNumber = deviceId;

                m_refPower.TurnOn();
            }
        }

        private void CloseRefPower()
        {
            // Turn off the display
            m_refPower.TurnOff();

            // Clear all the values.
            UpdateText(txtRefPwrDeviceId, "0");
            UpdateText(lblRefPwrManuf, "...");
            UpdateText(lblRefPwrModel, "...");
            UpdateText(lblRefPwrType, "...");
            UpdateText(lblRefPwrWatts, "000");
            UpdateText(lblRefPwr30SecAvg, "000");
        }

        private void CloseEmotion()
        {
            // Turn off the display
            m_eMotion.TurnOff();

            UpdateText(txtEmrDeviceId, "0");
            UpdateText(lblEmrBattTime, "...");
            UpdateText(lblEmrBattVolt, "...");
            UpdateText(lblEmrSerialNo, "...");
            UpdateText(lblEmrFirmwareRev, "...");
            UpdateText(lblEmrHardwareRev, "...");
            UpdateText(lblEmrModel, "...");
            UpdateText(lblEmrMph, "00.0");
            UpdateText(lblEmrWatts, "000");
            UpdateText(lblEmrWatts30SecAvg, "000");

            UpdateText(txtWheelSizeMm, "");
            UpdateText(txtTotalWeight, "");
            UpdateText(txtSlope, "");
            UpdateText(txtOffset, "");
            UpdateText(txtServoOffset, 0);

        }

        private void btnSettingsGet_Click(object sender, EventArgs e)
        {
            RequestSettings();
        }
        
        /*private bool TextBoxRangeCheck<T>(TextBox txt, string name, int min, int max, out T value) where T : IComparable<T>
        {
            
           //TextBoxRangeCheck 
            //return false;
        }*/

        private void RequestSettings()
        {
            List<SubPages> parameters = new List<SubPages>();
            parameters.Add(SubPages.TotalWeight);
            parameters.Add(SubPages.WheelSize);
            parameters.Add(SubPages.Settings);
            parameters.Add(SubPages.ServoOffset);
            parameters.Add(SubPages.Charger);

            var t = new System.Threading.Thread(() =>
            {
                foreach (var p in parameters)
                {
                    RequestDeviceParameter(p);
                    System.Diagnostics.Debug.WriteLine("Requested param: " + p);
                    System.Threading.Thread.Sleep(1500);
                }

            });
            
            t.Start();
        }

        private void RequestDeviceParameter(SubPages subPage)
        {
            int retries = 0;

            ANT_ReferenceLibrary.MessagingReturnCode result = 0;
            RequestDataMessage message = new RequestDataMessage();
            //RequestDataPage request = new RequestDataPage();

            if (subPage == SubPages.Battery)
            {
                message.RequestedPage = (byte)SubPages.Battery;
                //request.RequestedPageNumber = (byte)SubPages.Battery;
            }
            /*else if (subPage == SubPages.Temp)
            {
                message = new RequestDataMessage();
                message.RequestedPage = 0x03;
                //request.RequestedPageNumber = (byte)SubPages.Temp;
            }*/
            else
            {
                //request.DescriptorByte1 = (byte)subPage;
                //request.RequestedPageNumber = (byte)GetSetMessage.Page;
                message.RequestedPage = (byte)GetSetMessage.Page;
                message.SubPage = subPage;
            }

            //request.CommandType = Common.CommandType.RequestDataPage;
            //request.RequestedNumberTx = 2;
            //request.UseAck = false;
            //m_eMotion.SendDataPageRequest(request);


            while (retries++ < REQUEST_RETRY)
            {
                result = m_eMotionChannel.sendAcknowledgedData(message.AsBytes(), ACK_TIMEOUT);
                if (result == ANT_ReferenceLibrary.MessagingReturnCode.Pass)
                    return;
            }

            UpdateStatus(String.Format("Unable to request parameter: {0}, return result: {1}.", subPage, result));
        }

        private void chkLstSettings_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void UpdateStatus(string text)
        {
            ExecuteOnUI(() =>
            {
                txtLog.AppendText(text + '\n');
                lblStatus.Text = text;
            });
        }

        private void btnCalibrationGet_Click(object sender, EventArgs e)
        {
            RequestDeviceParameter(SubPages.Crr);
        }

        private void btnCalibrationSet_Click(object sender, EventArgs e)
        {
            // Bounds check.
            ushort slope = 0, offset = 0;

            ushort.TryParse(txtSlope.Text, out slope);
            ushort.TryParse(txtOffset.Text, out offset);

            if (slope == 0 || offset == 0)
            {
                UpdateStatus("Failed to update calibration.  Slope/Offset must be > 0.");
                return;
            }

            UInt32 value = (UInt32)(slope | (offset << 16));
            SetParameter((byte)SubPages.Crr, value);
        }

    }
}
