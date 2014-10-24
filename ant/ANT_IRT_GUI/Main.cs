using ANT_Console;
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
        const byte ANT_BURST_MSG_ID_SET_POSITIONS   = 0x59;
        const byte ANT_BURST_MSG_ID_SET_RESISTANCE  = 0x48;
        const byte RESISTANCE_SET_SLOPE		        = 0x46;
	    const byte RESISTANCE_SET_WIND			    = 0x47;
        const byte RESISTANCE_SET_WHEEL_CR          = 0x48;
        const byte RESISTANCE_SET_BIKE_TYPE	        = 0x44; // Co-efficient of rolling resistance
        const byte RESISTANCE_SET_C                 = 0x45; // Wind resistance offset.
        const byte MAX_RESISTANCE_LEVELS = 9;
        const ushort MIN_SERVO_POS = 2000;
        const ushort MAX_SERVO_POS = 700;

        const int EMR_CHANNEL_ID = 0;
        const int REF_PWR_CHANNEL_ID = 1;
        const byte ANT_FREQUENCY = 0x39;     // 2457 Mhz
        readonly byte[] USER_NETWORK_KEY = { 0xb9, 0xa5, 0x21, 0xfb, 0xbd, 0x72, 0xc3, 0x45 };

        const uint ACK_TIMEOUT = 1000;
        const int ANT_RETRY_REQUESTS = 3;
        const int ANT_RETRY_DELAY = 250;
        byte m_sequence;

        ushort m_settings = 0;
        ushort m_EmrDeviceId = 0;

        bool m_PauseServoUpdate = false;
        bool m_requestingSettings = false;

        // Logging stuff.
        private Timer m_reportTimer;
        private List<IReporter> m_reporters;
        private DataPoint m_dataPoint;

        // Used for calculating moving average.
        private int m_movingAvgPosition = 0;
        private List<ushort> m_RefPowerList;
        private List<ushort> m_eMotionPowerList;
        private List<double> m_SpeedList;

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
            m_dataPoint = new DataPoint(); // Last valid data.
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
            txtServoOffset.Enabled = AdminEnabled;
            btnServoOffset.Visible = AdminEnabled;
            btnServoOffset.Enabled = AdminEnabled;
            btnParamSet.Enabled = AdminEnabled;

            // Setup the settings checklist box.
            chkLstSettings.Items.Clear();
            chkLstSettings.Items.AddRange(Enum.GetNames(typeof(Settings)));

            cmbResistanceMode.Items.Clear();
            cmbResistanceMode.Items.AddRange(Enum.GetNames(typeof(ResistanceMode)));

            cmbParamGet.Items.Clear();
            cmbParamGet.Items.AddRange(Enum.GetNames(typeof(SubPages)));

            numMovingAvgSec.Value = 30;
            InitMovingAverage((int)numMovingAvgSec.Value);

            this.FormClosed += frmIrtGui_FormClosed;

            // Configure and start listening on ANT+.
            StartANT();
            StartReporting();
        }

        void frmIrtGui_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (m_reportTimer != null)
                m_reportTimer.Stop();

            if (m_reporters != null)
            {
                // Dispose of all reporters properly.
                foreach (IReporter r in m_reporters)
                {
                    if (r is LogReporter)
                    {
                        ReportSummary(r);
                    }

                    var id = r as IDisposable;
                    if (id != null)
                        id.Dispose();
                }
            }
        }

        void ReportSummary(IReporter reporter)
        {
            // List of settings and the control their value lives in.
            Dictionary<string, Control> items = new Dictionary<string, Control>();
            items.Add("DeviceID", txtEmrDeviceId);
            items.Add("SerialNo", lblEmrSerialNo);
            items.Add("FirmwareRev", lblEmrFirmwareRev);
            items.Add("HardwareRev", lblEmrHardwareRev);
            items.Add("Model", lblEmrModel);
            items.Add("BattVolt", lblEmrBattVolt);
            items.Add("BattOpTime", lblEmrBattTime);
            items.Add("ServoOffset", txtServoOffset);
            items.Add("Features", lblFeatures);
            items.Add("CrrSlope", txtSlope);
            items.Add("CrrOffset", txtOffset);
            items.Add("Weight", txtTotalWeight);
            items.Add("WheelSize", txtWheelSizeMm);
            items.Add("RefPwrManuf", lblRefPwrManuf);

            // items.Add("Settings", lblFeatures);
            string message = "\"{0}\",\"{1}\"\n";

            foreach(var item in items)
            {
                reporter.Report(string.Format(message, 
                    item.Key, item.Value.Text));
            }

            // Report Settings
            reporter.Report(string.Format(message, "Settings", m_settings));
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

            m_dataPoint.ServoPosition = message.ServoPosition;
            m_dataPoint.TargetLevel = message.Level;
            m_dataPoint.Temperature = message.Temperature;
            m_dataPoint.ResistanceMode = message.Mode;
            m_dataPoint.FlywheelRevs = message.FlyweelRevs;

            UpdateText(lblFlywheel, message.FlyweelRevs);

            if (!m_PauseServoUpdate)
            {
                UpdateText(txtServoPos, message.ServoPosition);

                ExecuteOnUI(() =>
                    {
                        switch ((ResistanceMode)message.Mode)
                        {
                            case ResistanceMode.Percent:
                                cmbResistanceMode.SelectedIndex = 0;
                                break;
                            case ResistanceMode.Standard:
                                cmbResistanceMode.SelectedIndex = 1;
                                UpdateText(lblResistanceStdLevel, message.Level);
                                break;
                            case ResistanceMode.Erg:
                                cmbResistanceMode.SelectedIndex = 2;
                                UpdateText(txtResistanceErgWatts, message.Level);
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

                    UpdateStatus("Received CRR parameter.");
                    UpdateText(txtSlope, slope);
                    UpdateText(txtOffset, intercept);
                    
                    break;

                case SubPages.TotalWeight:
                    int grams = Message.BigEndian(buffer[2], buffer[3]);
                    double kg = grams / 100.0;
                    UpdateStatus("Received weight parameter.");
                    UpdateText(txtTotalWeight, kg.ToString("N2"));

                    break;

                case SubPages.ServoOffset:
                    ushort servoOffset = Message.BigEndian(buffer[2], buffer[3]);
                    UpdateStatus("Received ServoOffset parameter.");
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
                    UpdateStatus("Received wheelsize parameter.");
                    UpdateText(txtWheelSizeMm, wheelSize);
                    break;

                case SubPages.Settings:
                    m_settings = Message.BigEndian(buffer[2], buffer[3]);
                    UpdateStatus("Received settings parameter.");
                    UpdateSettings(m_settings);
                    break;

                case SubPages.Charger:
                    UpdateStatus("Received charger parameter.");
                    bool check = (buffer[2] == 0x02); // Charging
                    UpdateCheckbox(chkCharge, check);
                    break;

                case SubPages.Features:
                    UpdateStatus("Received features parameter.");
                    ushort features = Message.BigEndian(buffer[2], buffer[3]);
                    UpdateText(lblFeatures, features);
                    break;

                default:
                    UpdateStatus(string.Format("Received Parameters page: {0} - [{1:x2}][{2:x2}][{3:x2}][{4:x2}][{5:x2}][{6:x2}]", m.SubPage,
                        buffer[2],
                        buffer[3],
                        buffer[4],
                        buffer[5],
                        buffer[6],
                        buffer[7]));
                    break;
            }
        }

        void UpdateSettings(ushort settings)
        {
            ExecuteOnUI(() =>
            {
                for (int i = 0; i < chkLstSettings.Items.Count; i++)
                {
                    Settings setting = GetSetting(i);

                    bool check = ((settings & (ushort)setting) == (ushort)setting);
                    chkLstSettings.SetItemChecked(i, check);
                }
            });
        }

        private Settings GetSetting(int index)
        {
            // Get the setting
            Settings setting;
            Enum.TryParse<Settings>(chkLstSettings.Items[index].ToString(), out setting);
            return setting;
        }

        void StartANT()
        {
            try
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
            catch (ANT_Managed_Library.ANT_Exception e)
            {
                MessageBox.Show(
                    "Please ensure that your ANT+ USB key is \n" +
                    "inserted and no other application is using\n" +
                    "(i.e. TrainerRoad/Garmin Device Sync)\n\n" +
                    "Restart the application once you have verified.",
                    "Unable to connect to ANT+ USB Key",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error);

                Application.Exit();
            }
        }

        private void StartReporting()
        {
            // Start logging.
            m_reporters = new List<IReporter>();
            m_reporters.Add(new LogReporter());

            m_reportTimer = new Timer();
            m_reportTimer.Interval = 1000; // Every second.
            m_reportTimer.Tick += m_reportTimer_Tick;
            m_reportTimer.Start();
        }

        void m_reportTimer_Tick(object sender, EventArgs e)
        {
            foreach (IReporter r in m_reporters)
                r.Report(m_dataPoint);
            
            UpdateMovingAverage();
        }

        private void UpdateMovingAverage()
        {
            // length
            int len = (int)numMovingAvgSec.Value;

            if (++m_movingAvgPosition == len)
            {
                m_movingAvgPosition = 0;
            }

            if (m_eMotion != null)
            {
                if (!double.IsInfinity(m_eMotion.AverageSpeedWheelTorque) &&
                    m_eMotion.AverageSpeedWheelTorque > 0 && 
                    m_eMotion.AverageSpeedWheelTorque != ushort.MaxValue)
                {
                    m_SpeedList[m_movingAvgPosition] = m_eMotion.AverageSpeedWheelTorque * 0.621371;
                }
                else
                {
                    m_SpeedList[m_movingAvgPosition] = 0;
                }

                if (m_eMotion.StandardPowerOnly != null &&
                    m_eMotion.StandardPowerOnly.InstantaneousPower < 3000) // sometimes we get just huge values here.
                    //m_eMotion.StandardPowerOnly.InstantaneousPower != ushort.MaxValue)
                {
                    m_eMotionPowerList[m_movingAvgPosition] = m_eMotion.StandardPowerOnly.InstantaneousPower;
                }
                else
                {
                    m_eMotionPowerList[m_movingAvgPosition] = 0;
                }
            }

            if (m_refPower != null && m_refPower.StandardPowerOnly != null &&
                m_refPower.StandardPowerOnly.InstantaneousPower != ushort.MaxValue)
            {
                m_RefPowerList[m_movingAvgPosition] = m_refPower.StandardPowerOnly.InstantaneousPower;
            }
            else
            {
                m_RefPowerList[m_movingAvgPosition] = 0;
            }

            var eMotionAvg = CalcAverage(m_eMotionPowerList);
            var refPowerAvg = CalcAverage(m_RefPowerList);
            var speedAvg = CalcAverage(m_SpeedList);

            m_dataPoint.PowerEMotionAvg = (short)eMotionAvg;
            m_dataPoint.PowerReferenceAvg = (short)refPowerAvg;
            m_dataPoint.SpeedEmotionAvg = (float)speedAvg;

            UpdateText(lblEmrWattsAvg, (int)eMotionAvg);
            UpdateText(lblRefPwrWattsAvg, (int)refPowerAvg);
            UpdateText(lblSpeedAvg, string.Format("{0:0.0}", speedAvg));
        }

        private double CalcAverage(List<ushort> list)
        {
            // Non-zeros
            var nonZero = list.Where(x => x > 0);
            double avg = 0.0; 

            if (nonZero.Count() > 0)
            {
                avg = (double)nonZero.Average(x => x);
            }

            return avg;
        }

        private double CalcAverage(List<double> list)
        {
            // Non-zeros
            var nonZero = list.Where(x => x > 0.0);
            double avg = 0.0;

            if (nonZero.Count() > 0)
            {
                avg = (double)nonZero.Average(x => x);
            }

            return avg;
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
                        UpdateText(lblEmrMph, "00.0");
                        UpdateText(lblEmrWatts, "000");
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
            if (arg1.InstantaneousPower != ushort.MaxValue)
            {
                m_dataPoint.PowerReference = (short)arg1.InstantaneousPower;

                UpdateText(lblRefPwrWatts, arg1.InstantaneousPower);
                //UpdateText(lblRefPwrWattsAvg, m_refPower.AveragePowerStandardPowerOnly);
            }
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
            // Go grab the latest settings from the device.

            // Save last connected device id, don't request params again.
            if (arg1 != m_EmrDeviceId)
            {
                m_EmrDeviceId = arg1;
                RequestSettings();
            }
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

            UpdateStatus("Received battery reading.");

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
            if (arg1.InstantaneousPower < 3000) // Sometimes we just get huge #s here...
            {
                m_dataPoint.PowerEMotion = (short)arg1.InstantaneousPower;

                UpdateText(lblEmrWatts, arg1.InstantaneousPower);
                //UpdateText(lblEmrWattsAvg, m_eMotion.AveragePowerStandardPowerOnly);
            }
        }


        void m_eMotion_StandardWheelTorquePageReceived(StandardWheelTorquePage arg1, uint arg2)
        {
            if (!double.IsInfinity(m_eMotion.AverageSpeedWheelTorque))
            {
                // Convert to mph from km/h.
                double mph = m_eMotion.AverageSpeedWheelTorque * 0.621371;
                m_dataPoint.SpeedEMotion = (float)mph;
                UpdateText(lblEmrMph, mph.ToString("00.0"));
            }
            else
            {
                m_dataPoint.SpeedEMotion = 0;
                UpdateText(lblEmrMph, "00.0");
            }
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

        private bool SetParameter(byte subpage, UInt32 value)
        {
            UpdateStatus(string.Format("Sending {0} parameter.", (SubPages)subpage));

            GetSetMessage message = new GetSetMessage(subpage);
            message.SetPayLoad(value);
            var result = m_eMotionChannel.sendAcknowledgedData(message.AsBytes(), ACK_TIMEOUT);
            return (result == ANT_ReferenceLibrary.MessagingReturnCode.Pass);
        }

        private byte GetParameter()
        {
            SubPages param = 0;

            if (!Enum.TryParse<SubPages>(cmbParamGet.Text, out param))
            {
                UpdateStatus("Please enter a parameter number.");
                return 0;   
            }
            else
            {
                return (byte)param;
            }
        }

        private void btnSettingsSet_Click(object sender, EventArgs e)
        {
            if (txtTotalWeight.Modified)
            {
                txtTotalWeight.Enabled = false;

                float weightKg = 0;
                float.TryParse(txtTotalWeight.Text, out weightKg);

                if (weightKg > 20.0f && weightKg < 140.0f)
                {
                    UpdateStatus("Setting total weight.");
                    if (SetWeight(weightKg))
                    {
                        UpdateStatus("Updated weight.");
                    }
                    else
                    {
                        UpdateStatus("Failed to update weight.");
                    }
                }
                else
                {
                    UpdateStatus("Invalid weight.");
                }

                txtTotalWeight.Enabled = true;
            }

            if (txtWheelSizeMm.Modified)
            {
                txtWheelSizeMm.Enabled = false;

                ushort wheelSizeMM = 0;
                ushort.TryParse(txtWheelSizeMm.Text, out wheelSizeMM);

                if (wheelSizeMM > 1500 && wheelSizeMM < 3000) // Valid range? 
                {
                    UpdateStatus("Setting wheel size.");
                    if (SetWheelSize(wheelSizeMM))
                    {
                        UpdateStatus("Updated wheel size.");
                    }
                    else
                    {
                        UpdateStatus("Failed to update wheel size.");
                    }
                }
                else
                {
                    UpdateStatus("Invalid wheel size.");
                }

                txtWheelSizeMm.Enabled = true;
            }
        }

        private void MoveServo(int position)
        {
            byte[] data = {
                (byte)(position), // Position LSB
                (byte)(position >> 8), // Position MSB
            };
            SendCommand(Command.MoveServo, data);
        }


        private bool SetWeight(float weight)
        {
            ushort value = (ushort)(weight * 100);
            byte[] data = {
                (byte)value, // Weight LSB
                (byte)(value >> 8), // Weight MSB
            };

            return SendCommand(Command.SetWeight, data);
        }

        private bool SetResistanceStandard(ushort level)
        {
            return SendBurst((byte)ResistanceMode.Standard, level);
        }

        private bool SetResistancePercent(float value)
        {
            bool success = false;

            // Calculate percentage.
            ushort wireValue = 0;
            wireValue = (ushort)(16383 - (16383 * (value / 100.0f)));

            success = SendBurst((byte)ResistanceMode.Percent, wireValue);

            if (success)
            {
                UpdateStatus(String.Format("Set resistance to {0} percent.", value));
            }
            else
            {
                UpdateStatus("Failed to set resistance percent.");
            }

            return success;
        }

        // This uses the WAHOO method with a burst message.
        private bool SetWheelSize(ushort wheelSizeMM)
        {
            // set wheel size mode
            return SendBurst(0x48, (ushort)(wheelSizeMM * 10));
        }

        bool SendBurstData(byte[] data)
        {
            bool result = RetryCommand(ANT_RETRY_REQUESTS, ANT_RETRY_DELAY, () =>
                { return m_eMotionChannel.sendBurstTransfer(data, 500); });

            if (!result)
            {
                UpdateStatus("Unable to send burst command, result");
            }

            return result;
        }

        bool SendBurst(byte command, ushort value)
        {
            byte[] data = { 
                // Message 1
                ANT_BURST_MSG_ID_SET_RESISTANCE,
                command, 
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
                (byte)(value),        // LSB
                (byte)((value) >> 8),   // MSB          
                0x00,
                0x00,
                0x00,
                0x00, 
                0x00,
                0x00
                          };
            
            return SendBurstData(data);
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
            ushort position = 0;
            ushort.TryParse(txtServoPos.Text, out position);

            if (position > 500 && position < 2500)
            {
                MoveServo(position);
            }
            else
            {
                UpdateStatus("Invalid servo position.");
            }

            // Resume updating the servo position textbox.
            m_PauseServoUpdate = false;
        }


        private void btnResistanceInc_Click(object sender, EventArgs e)
        {
            ushort value = 0;
            ushort.TryParse(lblResistanceStdLevel.Text, out value);

            if (value < MAX_RESISTANCE_LEVELS)
            {
                if (SetResistanceStandard(++value))
                {
                    UpdateStatus("Set standard level resistance.");
                }
                else
                {
                    UpdateStatus("Unable to set standard level.");
                }
            }
            else
            {
                UpdateStatus("Max level reached.");
            }

            m_PauseServoUpdate = false;
        }

        private void btnResistanceDec_Click(object sender, EventArgs e)
        {
            ushort value = 0;
            ushort.TryParse(lblResistanceStdLevel.Text, out value);

            if (value > 0)
            {
                if (SetResistanceStandard(--value))
                {
                    UpdateStatus("Set standard level resistance.");
                }
                else
                {
                    UpdateStatus("Unable to set standard level.");
                }

            }
            else
            {
                UpdateStatus("Min level reached.");
            }

            m_PauseServoUpdate = false;
        }

        private void btnDfuEnable_Click(object sender, EventArgs e)
        {
            UpdateStatus("Sending DFU command, channel should disconnect.");
            // Todo: set some other kind of state / notification here.
            SendCommand(Command.SetDFUMode);
        }

        private void chkCharge_CheckedChanged(object sender, EventArgs e)
        {
            // Toggles charger... need to then also wait for response back and update display.
            // BUG: When the box is checked via code this handler is invoked and issues a toggle command
            // on the charge flag... disabled for now.  
            // WORKAROUND: use a flag to suppress when set via code and check that.
            //SetParameter((byte)(SubPages.Charger), 1);
        }

        private void btnServoOffset_Click(object sender, EventArgs e)
        {
            uint value = 0;

            try
            {
                value = uint.Parse(txtServoOffset.Text);
            }
            catch
            {
                UpdateStatus("Invalid servo offset.");
                return;
            }

            // TODO: this is broken, because servo offset can be negative.
            SetParameter((byte)(SubPages.ServoOffset), value);
        }

        private void cmbResistanceMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (cmbResistanceMode.SelectedItem == null)
                return;

            // Iterate through each panel in group box.
            foreach (var ctl in groupBoxResistance.Controls)
            {
                if (ctl is Panel)
                {
                    System.Diagnostics.Debug.WriteLine(((Panel)ctl).Name);
                }
            }

            ResistanceMode mode = 0;
            Enum.TryParse<ResistanceMode>(cmbResistanceMode.SelectedItem.ToString(), out mode);

            switch (mode)
            {
                case ResistanceMode.Percent:
                    pnlResistancePercent.BringToFront();
                    UpdateStatus("Percentage selected.");
                    break;

                case ResistanceMode.Erg:
                    pnlErg.BringToFront();
                    UpdateStatus("Erg selected.");
                    ExecuteOnUI(() =>
                        {
                            txtResistanceErgWatts.Text = "";
                            txtResistanceErgWatts.Focus();
                        });
                    break;

                case ResistanceMode.Sim:
                    pnlResistanceSim.BringToFront();    
                    UpdateStatus("Sim selected.");
                    break;

                case ResistanceMode.Standard:
                    // set the level to 0
                    SetResistanceStandard(0);
                    pnlResistanceStd.BringToFront();
                    UpdateStatus("Standard selected.");
                    break;

                default:
                    UpdateStatus("Unknown resistance mode.");
                    break;
            }
        }

        private void btnEmrSearch_Click(object sender, EventArgs e)
        {
            try
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
            catch (Exception ex)
            {
                UpdateStatus("ANT Exception connecting to E-Motion Rollers: " + ex.Message);
            }
        }

        private void btnRefPwrSearch_Click(object sender, EventArgs e)
        {
            try
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
            catch (Exception ex)
            {
                UpdateStatus("ANT Exception connecting to power meter: " + ex.Message);
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
            UpdateText(lblRefPwrWattsAvg, "000");
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
            UpdateText(lblEmrWattsAvg, "000");

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
            if (!m_requestingSettings)
            {
                lock (this)
                {
                    m_requestingSettings = true;

                    List<SubPages> parameters = new List<SubPages>();
                    parameters.Add(SubPages.TotalWeight);
                    parameters.Add(SubPages.WheelSize);
                    parameters.Add(SubPages.Settings);
                    parameters.Add(SubPages.ServoOffset);
                    parameters.Add(SubPages.Charger);
                    parameters.Add(SubPages.Features);
                    parameters.Add(SubPages.Crr);

                    var t = new System.Threading.Thread(() =>
                    {
                        foreach (var p in parameters)
                        {
                            RequestDeviceParameter(p);
                            System.Diagnostics.Debug.WriteLine("Requested param: " + p);
                            System.Threading.Thread.Sleep(1500);
                        }

                        m_requestingSettings = false;
                    });

                    t.Start();
                }
            }
            else
            {
            }
        }

        private bool RequestDeviceParameter(SubPages subPage)
        {
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

            UpdateStatus(String.Format("Requesting parameter: {0}.", subPage));
            bool result = RetryCommand(ANT_RETRY_REQUESTS, ANT_RETRY_DELAY, () =>
                { return m_eMotionChannel.sendAcknowledgedData(message.AsBytes(), ACK_TIMEOUT); });

            if (!result)
            {
                UpdateStatus(String.Format("Unable to request parameter: {0}, return result: {1}.", subPage, result));
            }

            return result;
        }
        
        private bool RetryCommand(int times, int delay_ms, Func<ANT_ReferenceLibrary.MessagingReturnCode> command)
        {
            int retries = 0;

            while (retries++ < times)
            {
                var result = command();
                if (result == ANT_ReferenceLibrary.MessagingReturnCode.Pass)
                    return true;
                else
                    System.Threading.Thread.Sleep(delay_ms); // pause for a 1/4 second.
            }

            return false;
        }

        private void UpdateCheckbox(CheckBox ctl, bool check)
        {
            if (check != ctl.Checked)
            {
                ExecuteOnUI(() => 
                {
                    ctl.Checked = check;
                });
            }
        }

        private void UpdateStatus(string text)
        {
            ExecuteOnUI(() =>
            {
                if (txtLog == null || txtLog.IsDisposed)
                    return;
                
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
                UpdateStatus("ERROR: Slope/offset must be > 0.");
                return;
            }

            UInt32 value = (UInt32)(slope | (offset << 16));
            if (SetParameter((byte)SubPages.Crr, value))
            {
                UpdateStatus("Updated slope/offset.");
            }
            else
            {
                UpdateStatus("Failed to update slope/offset.");
            }
        }

        private void chkLstSettings_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            ushort value = 0;

            // Get checked status of each to update value.
            // This is the value BEFORE the checkbox is invoked.
            foreach (int i in chkLstSettings.CheckedIndices)
            {
                value |= (ushort)GetSetting(i);
            }

            // A new item was checked so also account for that.
            if (e.NewValue == CheckState.Checked)
            {
                value |= (ushort)GetSetting(e.Index);
            }
            else if (e.NewValue == CheckState.Unchecked)
            {
                // Account for item being unchecked.
                value ^= (ushort)GetSetting(e.Index);

            }

            // Update text boxes for get/set params
            UpdateText(cmbParamGet, (byte)SubPages.Settings);
            UpdateText(txtParamSet, value);
        }

        private void txtServoPos_Enter(object sender, EventArgs e)
        {
            // Once the user has entered this box, don't update.
            m_PauseServoUpdate = true;
        }

        private void txtServoPos_Leave(object sender, EventArgs e)
        {
            // If the user did not modify the servo position, return to updating value.
            if (!txtServoPos.Modified)
            {
                m_PauseServoUpdate = false;
            }
        }

        private void txtResistanceErgWatts_Enter(object sender, EventArgs e)
        {
            m_PauseServoUpdate = true;
        }

        private void txtResistanceErgWatts_Leave(object sender, EventArgs e)
        {
            if (txtResistanceErgWatts.Modified)
            {
                ushort value = 0;
                ushort.TryParse(txtResistanceErgWatts.Text, out value);

                if (value > 0 && value < 1500)
                {
                    // Send erg target.
                    SendBurst((byte)ResistanceMode.Erg, value);
                }
                else
                {
                    UpdateStatus("Erg target out of range.");
                }
            }

            // Reset state.
            m_PauseServoUpdate = false;
        }

        private void cmbResistanceMode_Enter(object sender, EventArgs e)
        {
            //m_PauseServoUpdate = true;
        }

        private void cmbResistanceMode_Leave(object sender, EventArgs e)
        {
            // Don't do anything right now.
        }

        private void txtResistancePercent_Enter(object sender, EventArgs e)
        {

        }

        private void txtResistancePercent_Leave(object sender, EventArgs e)
        {
            float value = 0.0f;

            if (txtResistancePercent.Modified)
            {
                float.TryParse(txtResistancePercent.Text, out value);

                if (value >= 0.0f && value <= 100.0f)
                {
                    ExecuteOnUI(() =>
                    {
                        trackBarResistancePct.Value = (int)value;
                    });
                    SetResistancePercent(value);
                }
            }

            m_PauseServoUpdate = false;
        }

        private void txtSimSlope_Leave(object sender, EventArgs e)
        {
            if (txtSimSlope.Modified)
            {
                float grade = 0.0f;
                if (float.TryParse(txtSimSlope.Text, out grade))
                {
                    /*
                                     * 	value ^= 1 << 15u;
                                       grade = value / 32768.0f;
                                     */

                    ushort value = 0;

                    if (grade < 0.0f)
                    {
                        // Make the grade positive.
                        grade *= -1;
                    }
                    else
                    {
                        // Set the high order bit.
                        value = 32768;
                    }

                    value |= (ushort)(32768 * (grade / 100.0f));

                    SendBurst(RESISTANCE_SET_SLOPE, value);
                }
                else
                {
                    UpdateStatus("Invalid grade.");
                    return;
                }
            }
        }

        private void txtSimCrr_Leave(object sender, EventArgs e)
        {
            if (txtSimCrr.Modified)
            {
                float crr = 0.0f;
                if (float.TryParse(txtSimCrr.Text, out crr))
                {
                    ushort value = (ushort)(crr * 10000);

                    SendBurst(RESISTANCE_SET_BIKE_TYPE, value);
                }
                else
                {
                    UpdateStatus("Invalid crr.");
                }
            }
        }

        private void txtSimWind_Leave(object sender, EventArgs e)
        {
            if (txtSimWind.Modified)
            {
                float wind = 0.0f;
                if (float.TryParse(txtSimWind.Text, out wind))
                {
                    ushort value = 0;

                    if (wind < -30.0f || wind > 30.0f)
                    {
                        UpdateStatus("Invalid Wind speed.");
                        return;
                    }

                    if (wind < 0.0f)
                    {
                        // Make the wind speed positive.
                        wind *= -1;
                    }
                    else
                    {
                        // Set the high order bit to indicate positive.
                        value = 32768;
                    }

                    value |= (ushort)(wind * 1000);

                    SendBurst(RESISTANCE_SET_WIND, value);
                }
                else
                {
                    UpdateStatus("Invalid Wind.");
                    return;
                }
            }

        }

        private void trackBarResistancePct_Scroll(object sender, EventArgs e)
        {
            UpdateText(txtResistancePercent, trackBarResistancePct.Value);
            
            SetResistancePercent((float)trackBarResistancePct.Value);
        }

        private void numMovingAvgSec_ValueChanged(object sender, EventArgs e)
        {
            InitMovingAverage((int)numMovingAvgSec.Value);
        }

        private void InitMovingAverage(int count)
        {
            m_movingAvgPosition = 0;
            InitializeArray(count, ref m_eMotionPowerList);
            InitializeArray(count, ref m_RefPowerList);
            InitializeArray(count, ref m_SpeedList);
        }

        private void InitializeArray<T>(int count, ref List<T> list)
        {
            list = new List<T>(count);
            for (int i = 0; i < count; i++)
            {
                list.Add(default(T));
            }
        }

        private void btnSetResistancePositions_Click(object sender, EventArgs e)
        {
            /*
            Do NOT use this offset logic BECAUSE the device will translate max servo pos (700)
            into a valid position + the offset.  In order to actually get the modeled 700 position
            we need to send the modeled positions, not the new positions.  See servo offset logic.

            ushort offset = 0;
            ushort.TryParse(txtServoOffset.Text, out offset);

            ushort max = (ushort)(MAX_SERVO_POS + offset);
            // Max position resistance should always be able to be 1,000.
            if (max > 1000)
                max = 1000;
            */
            ServoPositions pos = new ServoPositions(MIN_SERVO_POS, MAX_SERVO_POS);
                
            pos.SetPositions += OnSetPositions;
            pos.ShowDialog();
        }

        void OnSetPositions(object sender, EventArgs e)
        {
            ServoPositions dialog = sender as ServoPositions;
            if (dialog == null)
                return;

            // Insert the HOME position which user cannot change.
            dialog.Positions.Insert(0, new Position(MIN_SERVO_POS));

            // 3 messages * 8 bytes each
            byte[] data = new byte[24];
            data[0] = ANT_BURST_MSG_ID_SET_POSITIONS;
            data[1] = (byte)(dialog.Positions.Count());

            // Start index for positions.
            int index = 2;

            foreach (Position p in dialog.Positions)
            {
                Message.LittleEndian(p.Value, out data[index++], out data[index++]);
            }

            while (index < data.Length)
            {
                // Fill in blanks.
                data[index++] = 0xFF;
            }

            if (SendBurstData(data))
            {
                UpdateStatus("Set new servo positions.");
                dialog.Close();
            }
            else
            {
                UpdateStatus("Failed to set servo positions.");
            }

        }
        
        private void btnChartOpen_Click(object sender, EventArgs e)
        {
            GraphForm graph = new GraphForm();
            m_reporters.Add(graph);
            graph.FormClosing += (o, v) =>
            {
                m_reporters.Remove(graph);
            };

            Screen screen = Screen.FromControl(this);
            int width = screen.WorkingArea.Width;
            graph.Width = width;
            graph.Height = (int)(screen.WorkingArea.Height * 0.4);
            graph.SetDesktopLocation(0, 0);
            
            graph.Show();
        }
    }
}
