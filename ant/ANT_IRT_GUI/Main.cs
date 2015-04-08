using ANT_Console;
using ANT_Managed_Library;
using AntPlus.Profiles.BikePower;
using AntPlus.Profiles.Common;
using AntPlus.Profiles.Components;
using AntPlus.Types;
using IntervalParser;
using IRT.Calibration;
using IRT_GUI.IrtMessages;
using IRT_GUI.Simulation;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
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
        const int ANT_RETRY_REQUESTS = 3;
        const int ANT_RETRY_DELAY = 250;
        byte m_sequence;

        ushort m_settings = 0;
        ushort m_EmrDeviceId = 0;
        List<IRTDevice> m_EmrDevices;

        bool m_PauseServoUpdate = false;
        bool m_requestingSettings = false;
        bool m_SystemUiUpdate = false;

        bool m_usingCtfRef = false; // using an SRM power meter or Crank Torque Frequency.

        // Logging stuff.
        private Timer m_reportTimer;
        private List<IReporter> m_reporters;
        private DataPoint m_dataPoint;
        private Controller m_calibration;

        // Used for calculating moving average.
        private int m_movingAvgPosition = 0;
        private List<ushort> m_RefPowerList;
        private List<ushort> m_eMotionPowerList;
        private List<double> m_SpeedList;

        private SimulateRefPower m_simRefPower = null;

        private Simulation.SimulationMode m_simulator;

        private bool m_enteredDFU = false;  // Tracks whether we went into DFU or not.


        private EmotionRollerDisplay m_eMotion;
        private BikePowerDisplay m_refPower;
        private ANT_Device m_ANT_Device;
        private Network m_ANT_Network;
        private ANT_Channel m_eMotionChannel;
        private ANT_Channel m_refChannel;

        // Wrapper for executing on the UI thread.
        private void ExecuteOnUI(Action a)
        {
            // Flag that we're updating UI elements.
            m_SystemUiUpdate = true;

            if (this.InvokeRequired)
            {
                this.BeginInvoke(a);
            }
            else
            {
                a.Invoke();
            }

            m_SystemUiUpdate = false;
        }

        /// <summary>
        /// Binds elements to the EmotionRollerDisplay object.
        /// </summary>
        private void BindEmotionDisplay(Control ctl, string property)
        {
            Binding binding = null;

            if (ctl is TextBox || ctl is Label)
            {
                binding = new Binding("Text", m_eMotion, property);
            }
            else if (ctl is CheckBox)
            {
                binding = new Binding("Checked", m_eMotion, property);
            }
            else if (ctl is NumericUpDown || ctl is TrackBar)
            {
                binding = new Binding("Value", m_eMotion, property);
            }


            if (binding != null)
            {
                ctl.DataBindings.Add(binding);
            }
            else
            {
                throw new ArgumentNullException(string.Format(
                    "Cannot bind {0}, to property: {1}.", ctl.Name, property));
            }
        }

        /// <summary>
        /// Binds all UI elements to the respective property of the Emotion display.
        /// </summary>
        private void BindEmotionControls()
        {
            BindEmotionDisplay(txtDrag, "Drag");
            BindEmotionDisplay(txtRR, "RollingResistance");
            BindEmotionDisplay(chkLstSettings, "Settings"); // TODO: this needs a type converter or binding adapter something...
            BindEmotionDisplay(txtTotalWeight, "WeightKg");
            BindEmotionDisplay(txtWheelSizeMm, "WheelSizeMm");
            BindEmotionDisplay(txtEmrDeviceId, "DeviceId");
            BindEmotionDisplay(txtServoOffset, "ServoOffset");
            BindEmotionDisplay(chkCharge, "Charging");
            BindEmotionDisplay(txtServoPos, "ServoPosition");
            BindEmotionDisplay(lblEmrSerialNo, "SerialNumber");
            BindEmotionDisplay(lblEmrFirmwareRev, "FirmwareRevision");
            BindEmotionDisplay(lblEmrHardwareRev, "HardwareRevision");
            BindEmotionDisplay(lblEmrModel, "Model");
            BindEmotionDisplay(lblEmrBattVolt, "BatteryVolts");
            // TODO: This deserves a special type converter instead of this.
            lblEmrBattVolt.DataBindings.Add(new Binding("ForeColor", m_eMotion, "BatteryStatusColor"));

            BindEmotionDisplay(lblEmrBattTime, "BatteryTime");
            BindEmotionDisplay(lblFlywheel, "FlywheelRevolutions");
            BindEmotionDisplay(lblFeatures, "Features");
            // TODO: This needs a converter as well to mph as it's in KM/H.
            BindEmotionDisplay(lblEmrMph, "AverageSpeedWheelTorque"); 
            BindEmotionDisplay(lblEmrWatts, "AveragePowerStandardPowerOnly");
            BindEmotionDisplay(lblSpeedAvg, "AverageSpeedMph");
            BindEmotionDisplay(lblEmrWattsAvg, "AveragePower");
            BindEmotionDisplay(lblDistance, "TotalDistanceWheelTorque");

            BindEmotionDisplay(numMovingAvgSec, "MovingAverageSeconds");

            // TODO: figure out how to dynamically bind the various resistance mode values.
            //BindEmotionDisplay(txtSimC, "Simulation.SimC");

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
                m_eMotion = (EmotionRollerDisplay)simulator;
            }
        }

        void frmIrtGui_Load(object sender, EventArgs e)
        {
            this.Text += " " +
                this.GetType().Assembly.GetName().Version.ToString(3);

            // Admin only features.  
            txtServoOffset.Enabled = true;
            btnServoOffset.Visible = true; //  AdminEnabled;
            btnServoOffset.Enabled = true; //  AdminEnabled;
            btnParamSet.Enabled = AdminEnabled;

            // Setup the settings checklist box.
            chkLstSettings.Items.Clear();
            chkLstSettings.Items.AddRange(Enum.GetNames(typeof(Settings)));

            // Setup the resistance modes.
            cmbResistanceMode.Items.Clear();
            cmbResistanceMode.Items.Add(Enum.GetName(typeof(ResistanceMode),
                ResistanceMode.Standard));
            cmbResistanceMode.Items.Add(Enum.GetName(typeof(ResistanceMode),
                ResistanceMode.Percent));
            cmbResistanceMode.Items.Add(Enum.GetName(typeof(ResistanceMode),
                ResistanceMode.Erg));
            cmbResistanceMode.Items.Add(Enum.GetName(typeof(ResistanceMode),
                ResistanceMode.Sim));

            cmbParamGet.Items.Clear();
            cmbParamGet.Items.AddRange(Enum.GetNames(typeof(SubPages)));

            m_eMotion.MovingAverageSeconds = 30;

            this.FormClosed += frmIrtGui_FormClosed;

            // Configure and start listening on ANT+.
            StartANT();

            // Create calibration object, which will listen for calibration events 
            // and react accordingly.
            m_calibration = new Controller(m_eMotion, m_refPower);
            m_calibration.StageChanged += m_calibration_StageChanged;

            StartReporting();
        }

        void m_calibration_StageChanged(IRT.Calibration.Globals.Stage stage)
        {
            UpdateStatus(stage.ToString());

            if (stage == IRT.Calibration.Globals.Stage.CoastdownComplete)
            {
                WriteCalibrationLog();
            }

            ExecuteOnUI(() =>
            {
                switch (stage)
                {
                    case IRT.Calibration.Globals.Stage.Started:
                        m_calibration.DisplayCalibrationProgress();
                        break;

                    case IRT.Calibration.Globals.Stage.Finished:
                        m_calibration.DisplayCalibrationResults(m_calibration_CoastdownCalibrationApply);
                        m_calibration.Reset();
                        break;

                    default:
                        break;
                }
            });
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
            items.Add("Weight", txtTotalWeight);
            items.Add("WheelSize", txtWheelSizeMm);
            items.Add("RefPwrManuf", lblRefPwrManuf);
            items.Add("Drag", txtDrag);
            items.Add("RR", txtRR);

            // items.Add("Settings", lblFeatures);
            string message = "\"{0}\",\"{1}\"\n";

            foreach (var item in items)
            {
                reporter.Report(string.Format(message,
                    item.Key, item.Value.Text));
            }

            // Report Settings
            reporter.Report(string.Format(message, "Settings", m_settings));

            // Report all E-Motion rollers that were connected.
            if (m_EmrDevices != null)
            {
                foreach(var device in m_EmrDevices)
                {
                    reporter.Report(string.Format(message, device.DeviceId, device.SerialNumber));
                }
            }
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
                
                m_eMotionChannel.DeviceNotification += channel_DeviceNotification;
                m_eMotionChannel.rawChannelResponse += channel_rawChannelResponse;

                m_ANT_Device.serialError += m_ANT_Device_serialError;
                m_ANT_Device.deviceResponse += m_ANT_Device_deviceResponse;

                m_eMotion = new EmotionRollerDisplay(m_eMotionChannel, m_ANT_Network);
                m_eMotion.SensorFound += m_eMotion_SensorFound;
                m_eMotion.ChannelStatusChanged += m_eMotion_ChannelStatusChanged;
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
                m_refPower.CrankTorqueFrequencyPageReceived += m_refPower_CrankTorqueFrequencyPageReceived;
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

        void m_refPower_CrankTorqueFrequencyPageReceived(CrankTorqueFrequencyPage arg1, uint arg2)
        {
            if (m_usingCtfRef == false)
                m_usingCtfRef = true;

            UpdateText(lblRefPwrWatts, m_refPower.PowerCtf.ToString("0"));
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
            m_dataPoint.Timestamp = DateTime.UtcNow;

            foreach (IReporter r in m_reporters)
                r.Report(m_dataPoint);

            UpdateMovingAverage();
            UpdateWatchClock();

            // Advance the simulator if in use.
            if (m_simulator != null)
            {
                m_simulator.DistanceEvent(m_eMotion.TotalDistanceWheelTorque);
            }

            this.lblDistance.Text =
                string.Format("{0:0.0}", m_eMotion.TotalDistanceWheelTorque / 1000);

            // Enable calibration start button when over 5 mph.
            if (m_dataPoint.SpeedEMotionMph > 5.0)
            {
                btnStartCalibration.Enabled = true;
            }
            else
            {
                btnStartCalibration.Enabled = false;
            }
        }

        private int m_watchClockms = 0;

        // Increments the watch clock if the bicycle is moving.
        private void UpdateWatchClock()
        {
            const string clockFormat = @"h\:mm\:ss";

            if (m_dataPoint.SpeedEmotionAvg > 0.0f)
            {
                m_watchClockms += m_reportTimer.Interval;
            }
            else
            {
                ResetWatchClock();
            }

            TimeSpan ts = TimeSpan.FromMilliseconds(m_watchClockms);
            UpdateText(lblWatchClock, ts.ToString(clockFormat));
        }

        private void ResetWatchClock()
        {
            m_watchClockms = 0;
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
                else if (m_simRefPower != null && m_simRefPower.State == SimulateState.Running)
                {
                    m_eMotionPowerList[m_movingAvgPosition] = (ushort)m_dataPoint.PowerReference;
                }
                else
                {
                    m_eMotionPowerList[m_movingAvgPosition] = 0;
                }
            }

            if (m_usingCtfRef)
            {
                m_RefPowerList[m_movingAvgPosition] = (ushort)m_refPower.PowerCtf;
            }
            else if (m_refPower != null && m_refPower.StandardPowerOnly != null &&
                m_refPower.StandardPowerOnly.InstantaneousPower != ushort.MaxValue)
            {
                m_RefPowerList[m_movingAvgPosition] = (ushort)m_dataPoint.PowerReference; // m_refPower.StandardPowerOnly.InstantaneousPower;
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

                        if (m_enteredDFU)
                        {
                            // Keep attempting to re-open the channel and restore parameters
                            m_eMotion.ChannelParameters.DeviceNumber = m_EmrDeviceId;
                            m_eMotion.TurnOn();
                        }
                        else
                        {
                            btnEmrSearch.Enabled = true;
                            txtEmrDeviceId.Enabled = true;
                            UpdateText(txtEmrDeviceId, 0);
                            btnEmrSearch.Text = "Search";
                        }
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

                        if (m_enteredDFU)
                        {
                            // Try to grab the new firmware rev right away.
                            RequestDeviceParameter(SubPages.Product);
                            RestoreUserProfile();
                        }

                        break;

                    default:
                        break;
                }
            });
        }

        void m_refPower_StandardPowerOnlyPageReceived(StandardPowerOnlyPage arg1, uint arg2)
        {
            // No longer using CTF, reset the flag.  TODO: this should be done on connection instead.
            if (m_usingCtfRef)
                m_usingCtfRef = false;

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


        void m_eMotion_SensorFound(ushort arg1, byte arg2)
        {
            UpdateText(txtEmrDeviceId, arg1);
            // Go grab the latest settings from the device.

            // Save last connected device id, don't request params again.
            if (arg1 != m_EmrDeviceId)
            {
                m_EmrDeviceId = arg1;
                RequestSettings();

                if (m_EmrDevices == null)
                {
                    m_EmrDevices = new List<IRTDevice>();
                }

                m_EmrDevices.Add(new IRTDevice() { DeviceId = m_EmrDeviceId });
            }
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

        private byte lastTorqueEventCount = 0;

        void m_eMotion_StandardWheelTorquePageReceived(StandardWheelTorquePage arg1, uint arg2)
        {
            // if we start getting a torque page, we're back out of calibration mode.
            if (m_calibration != null &&
                m_calibration.Stage != IRT.Calibration.Globals.Stage.Finished &&
                m_calibration.Stage != IRT.Calibration.Globals.Stage.Ready)
            {
                m_calibration.Cancel();
            }

            // No longer using this, it was a workaround to fix a bug that has been addressed in the device.
            //if (lastTorqueEventCount != arg1.WheelTorqueEventCount &&
            if (!double.IsInfinity(m_eMotion.AverageSpeedWheelTorque))
            {
                // Convert to mph from km/h.
                double mph = m_eMotion.AverageSpeedWheelTorque * 0.621371;
                m_dataPoint.SpeedEMotionMph = (float)mph;
                UpdateText(lblEmrMph, mph.ToString("00.0"));
            }
            else
            {
                m_dataPoint.SpeedEMotionMph = 0;
                UpdateText(lblEmrMph, "00.0");
            }

            lastTorqueEventCount = arg1.WheelTorqueEventCount;
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
                byte descriptor2 = 0xFF;

                if ((SubPages)param == SubPages.ServoPositions)
                {
                    byte startIdx = 0;
                    byte.TryParse(txtParamSet.Text, out startIdx);
                    descriptor2 = startIdx;
                }

                RequestDeviceParameter((SubPages)param, descriptor2);
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

                m_eMotion.WheelCircumference = wheelSizeMM;
                txtWheelSizeMm.Enabled = true;
            }
        }


        private void btnResistanceSet_Click(object sender, EventArgs e)
        {
            ushort position = 0;
            ushort.TryParse(txtServoPos.Text, out position);

            // do a check for valid range if not in admin mode
            if (!AdminEnabled)
            {
                if (position < 500 && position > 2500)
                {
                    m_PauseServoUpdate = false;
                    UpdateStatus("Invalid servo position.");
                    return;
                }
            }

            MoveServo(position);
            // Resume updating the servo position textbox.
            m_PauseServoUpdate = false;
        }


        private void btnResistanceInc_Click(object sender, EventArgs e)
        {
            ushort value = 0;
            ushort.TryParse(lblResistanceStdLevel.Text, out value);

            if (value < m_max_resistance_levels - 1)
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

            // Wait for status of channel to change, otherwise send the command again.

            m_enteredDFU = true;
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
            if (SetParameter((byte)(SubPages.ServoOffset), value))
            {
                UpdateStatus("Sent servo offset command.");
            }
            else
            {
                UpdateStatus("Failed to send servo offset command.");
            }
        }

        private ResistanceMode GetResistanceMode()
        {
            ResistanceMode mode = ResistanceMode.Standard;
            Enum.TryParse<ResistanceMode>(cmbResistanceMode.SelectedItem.ToString(), out mode);

            return mode;
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

            ResistanceMode mode = GetResistanceMode();

            switch (mode)
            {
                case ResistanceMode.Percent:
                    pnlResistancePercent.BringToFront();
                    UpdateStatus("Percentage selected.");
                    ExecuteOnUI(() => { btnResistanceLoad.Enabled = false; });
                    break;

                case ResistanceMode.Erg:
                    pnlErg.BringToFront();
                    UpdateStatus("Erg selected.");
                    ExecuteOnUI(() =>
                    {
                        txtResistanceErgWatts.Text = "";
                        if (!m_SystemUiUpdate)
                        {
                            txtResistanceErgWatts.Focus();
                        }
                        btnResistanceLoad.Enabled = true;
                    });
                    break;

                case ResistanceMode.Sim:
                    pnlResistanceSim.BringToFront();
                    UpdateStatus("Sim selected.");
                    ExecuteOnUI(() => { btnResistanceLoad.Enabled = true; });
                    break;

                case ResistanceMode.Standard:
                    // set the level to 0
                    SetResistanceStandard(0);
                    pnlResistanceStd.BringToFront();
                    UpdateStatus("Standard selected.");
                    ExecuteOnUI(() => { btnResistanceLoad.Enabled = false; });

                    break;

                default:
                    UpdateStatus("Unknown resistance mode.");
                    ExecuteOnUI(() => { btnResistanceLoad.Enabled = false; });
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

                if (value > 0 && value < 1500) // bounds checking for watt value.
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
            m_PauseServoUpdate = true;
        }

        private void cmbResistanceMode_Leave(object sender, EventArgs e)
        {
            // Don't do anything right now.
            m_PauseServoUpdate = false;
        }

        private void txtResistancePercent_Enter(object sender, EventArgs e)
        {
            m_PauseServoUpdate = true;
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

        private void txtSimC_Leave(object sender, EventArgs e)
        {
            if (txtSimC.Modified)
            {
                float c = 0.0f;
                if (float.TryParse(txtSimC.Text, out c))
                {
                    ushort value = (ushort)(c * 1000);

                    SendBurst(RESISTANCE_SET_C, value);
                }
                else
                {
                    UpdateStatus("Invalid bike type (C).");
                    return;
                }
            }
        }

        private void txtSimSlope_Leave(object sender, EventArgs e)
        {
            if (txtSimSlope.Modified)
            {
                float grade = 0.0f;

                if (float.TryParse(txtSimSlope.Text, out grade))
                {
                    SetSlope(grade);
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

                    value = (ushort)((wind * 1000) + 32768);

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
            try
            {
                ServoPositions pos = new ServoPositions(m_min_servo_pos, m_max_servo_pos, AdminEnabled);
                pos.SetMagnetCalibration += OnSetMagnetCalibration;

                if (m_positions != null)
                {
                    pos.Positions.Clear();
                    for (int i = 0; i < m_positions.Length; i++)
                    {
                        if (m_positions[i] != null && m_positions[i].Value > 0)
                        {
                            pos.Positions.Add(m_positions[i]);
                        }
                        else
                        {
                            // Exit out.
                            break;
                        }
                    }
                }
                else
                {
                    pos.Positions.Clear();
                    pos.Positions.Add(new Position(m_min_servo_pos));
                }

                pos.SetPositions += OnSetPositions;
                pos.ShowDialog();
            }
            catch (Exception ex)
            {
                UpdateStatus("Error occurred in setting servo positions: " + ex.Message);
            }
        }

        // Handles setting servo magnet calibration polynomial for both the Force to Position
        // calculation and the Position to Force calculation.
        void OnSetMagnetCalibration(object sender, MagnetCalibrationEventArgs e)
        {
            // 6 float elements.
            const byte FACTOR_COUNT = 6;
            const byte BUFFER_SIZE = 8 * 4; // 4 messages @ 8 bytes per mesage 

            byte index = 0;

            // Byte0 = 0x60 // MessageId
            // Byte1 = CalibrationType (Force2Pos or Pos2Force)
            // Byte2

            byte[] buffer = new byte[BUFFER_SIZE];
            buffer[index++] = ANT_BURST_MSG_ID_SET_MAGNET_CA;
            buffer[index++] = (byte)e.CalibrationType;

            // First factor
            Array.Copy(BitConverter.GetBytes(e.Factors[0]), 0,
                buffer, index, sizeof(float));

            // offset into the next 8 byte packet.
            index = 7;

            // Process factors 2,3,4,5,6
            for (int i = 1; i < FACTOR_COUNT; i++)
            {
                // Subsequence factors
                Array.Copy(BitConverter.GetBytes(e.Factors[i]), 0,
                    buffer, index, sizeof(float));
                index += sizeof(float);
            }

            // Dispatch burst over ANT.
            if (SendBurstData(buffer))
            {
                UpdateStatus("Sent servo calibration.");
            }
            else
            {
                UpdateStatus("Failed to send servo calibration.");
            }

            /* Test method, works.
            float firstVal = BitConverter.ToSingle(buffer, 2);
            UpdateStatus(firstVal.ToString());
            for (int j = 0; j < 5; j++)
            {
                float val = BitConverter.ToSingle(buffer, 7+(j*4));
                UpdateStatus(val.ToString());
            }*/

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

        private void btnSimRefPower_Click(object sender, EventArgs e)
        {
            if (m_simRefPower == null)
            {
                m_simRefPower = new SimulateRefPower();

                m_simRefPower.StateChanged += m_simRefPower_StateChanged;
                m_simRefPower.SimluatedPowerMessage += m_refPower_StandardPowerOnlyPageReceived;
                m_simRefPower.Progress += m_simRefPower_Progress;
            }

            switch (m_simRefPower.State)
            {
                case SimulateState.Invalid:
                    m_simRefPower.Load();
                    break;
                case SimulateState.Loaded:
                    m_simRefPower.Run();
                    break;
                case SimulateState.Running:
                    m_simRefPower.Stop();
                    break;
                case SimulateState.Cancelling:
                    m_simRefPower.Stop();
                    break;
                case SimulateState.Paused:
                    m_simRefPower.Stop();
                    break;
                case SimulateState.Stopped:
                    progressSimRefPower.Value = 0;
                    progressSimRefPower.Visible = false;
                    m_simRefPower.Load();
                    break;
                default:
                    break;
            }
        }

        void m_simRefPower_Progress(int current, int total)
        {
            if (progressSimRefPower.Maximum != total)
            {
                progressSimRefPower.Maximum = total;
            }

            progressSimRefPower.Value = current;
        }

        void m_simRefPower_StateChanged(SimulateState state)
        {
            switch (state)
            {
                case SimulateState.Loaded:
                    progressSimRefPower.Visible = true;
                    btnSimRefPower.Text = "Run";
                    break;
                case SimulateState.Cancelling:
                    btnSimRefPower.Enabled = false;
                    break;
                case SimulateState.Running:
                    btnSimRefPower.Text = "Stop";
                    btnPauseSimRefPower.Text = "Pause";
                    btnPauseSimRefPower.Visible = true;
                    break;
                case SimulateState.Stopped:
                    progressSimRefPower.Visible = false;
                    btnPauseSimRefPower.Visible = false;
                    btnSimRefPower.Text = "Load";
                    break;
                case SimulateState.Paused:
                    btnPauseSimRefPower.Text = "Resume";
                    break;
                default:
                    break;
            }
        }

        private void btnPauseSimRefPower_Click(object sender, EventArgs e)
        {
            if (m_simRefPower.State == SimulateState.Paused)
            {
                m_simRefPower.Run();
            }
            else if (m_simRefPower.State == SimulateState.Running)
            {
                m_simRefPower.Stop(true);
            }
        }

        private void btnCalibration2Set_Click(object sender, EventArgs e)
        {
            if (m_eMotion != null)
            {
                float drag = float.Parse(txtDrag.Text);
                float rr = float.Parse(txtRR.Text);
                m_eMotion.SetCalibration(drag, rr);
            }

        }

        private void btnCalibration2Get_Click(object sender, EventArgs e)
        {
            if (m_eMotion != null)
            {
                m_eMotion.RequestCalibration();
            }
        }

        private void btnStartCalibration_Click(object sender, EventArgs e)
        {
            m_eMotion.StartCalibration();
        }

        private void btnLoadCalibration_Click(object sender, EventArgs e)
        {

            OpenFileDialog dlg = new OpenFileDialog();
            //dlg.InitialDirectory = m_lastPath;
            dlg.Filter = "Ride Logs (*.csv)|*.csv|All files (*.*)|*.*";
            dlg.FilterIndex = 1;
            dlg.RestoreDirectory = false;
            dlg.CheckFileExists = true;
            dlg.Multiselect = false;

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    Model model;
                    Coastdown coastdown = new Coastdown();

                    if (dlg.FileNames.Length > 1)
                    {
                        List<Model> models = new List<Model>();

                        foreach (string file in dlg.FileNames)
                        {
                            models.Add(Model.FromFile(file));
                        }

                        // Calculate based on multiple files.
                        model = coastdown.Calculate(models.ToArray());

                        // For display purposes, use the lsat stable speed and watts.
                        model.StableSeconds = models.Last().StableSeconds;
                        model.StableSpeedMps = models.Last().StableSpeedMps;
                        model.StableWatts = models.Last().StableWatts;
                    }
                    else
                    {
                        model = Model.FromFile(dlg.FileName);
                        coastdown.Calculate(model);
                    }

                    CoastdownForm form = new CoastdownForm(coastdown, model);
                    form.Apply += m_calibration_CoastdownCalibrationApply;
                    form.Show();
                }
                catch (Exception ex)
                {
                    UpdateStatus("Error attempting to parse calibration file.\r\n" +
                        ex.Message);
                }
            }
        }

        void m_calibration_CoastdownCalibrationApply(IRT.Calibration.Coastdown coastdown)
        {
            m_eMotion.SetCalibration((float)coastdown.Drag,
                (float)coastdown.RollingResistance);

            ExecuteOnUI(() =>
            {
                this.txtDrag.Text = coastdown.Drag.ToString();
                this.txtRR.Text = coastdown.RollingResistance.ToString();
            });
        }

        private void WriteCalibrationLog()
        {
            StreamWriter log;

            // open up a stream to start logging
            string filename = string.Format("calib_{0}_{1:yyyyMMdd-HHmmss-F}.csv",
                System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString(3),
                DateTime.Now);

            using (log = new StreamWriter(filename))
            {
                log.WriteLine("timestamp_ms, count, ticks, watts, pwr_events, accum_pwr");

                foreach (var tick in m_calibration.Model.Events)
                {
                    log.WriteLine(tick);
                }

                log.Flush();
                log.Close();
            }
        }

        private void LaunchErgMode(string filename)
        {
            ErgMode erg = ErgMode.FromFile(filename);
            ErgForm form = new ErgForm(erg);
            form.Show();
        }

        private void LaunchSimMode(string filename)
        {
            m_simulator = new Simulation.SimulationMode();
            m_simulator.Load(filename);
            m_simulator.SlopeChanged += simulator_SlopeChanged;
            m_simulator.SimulationEnded += m_simulator_SimulationEnded;

            ElevationProfileForm form = new ElevationProfileForm(m_simulator);
            form.StartPosition = FormStartPosition.Manual;
            form.Location = new Point(this.Location.X, this.Location.Y + this.Height);
            form.Width = this.Width;
            form.Show();
        }

        private void btnResistanceLoad_Click(object sender, EventArgs e)
        {
            ResistanceMode mode = GetResistanceMode();

            OpenFileDialog dlg = new OpenFileDialog();

            if (mode == ResistanceMode.Sim)
            {
                dlg.Filter = "GPS Files (*.csv)|*.csv|All files (*.*)|*.*";
            }
            else if (mode == ResistanceMode.Erg)
            {
                dlg.Filter = "Erg Files (*.csv)|*.csv|All files (*.*)|*.*";
            }

            dlg.FilterIndex = 1;
            dlg.RestoreDirectory = false;
            dlg.CheckFileExists = true;
            dlg.Multiselect = false;

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    Cursor.Current = Cursors.WaitCursor;

                    switch (mode)
                    {
                        case ResistanceMode.Sim:
                            LaunchSimMode(dlg.FileName);
                            break;

                        case ResistanceMode.Erg:
                            LaunchErgMode(dlg.FileName);
                            break;

                        default:
                            break;
                    }

                }
                catch (Exception ex)
                {
                    UpdateStatus("Error loading simulator: \r\n" + ex.Message);
                }
                finally
                {
                    Cursor.Current = Cursors.Default;
                }
            }
        }

        void m_simulator_SimulationEnded()
        {
            m_simulator = null;
            UpdateStatus("Simulation ended.");
            SetResistanceStandard(0);
        }

        void simulator_SlopeChanged(double slope)
        {
            SetSlope(slope);
            ExecuteOnUI(() => { UpdateText(txtSimSlope, Math.Round(slope, 1)); });
        }
    }
}
