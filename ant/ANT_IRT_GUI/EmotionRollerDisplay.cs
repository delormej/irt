using ANT_Managed_Library;
using AntPlus.Profiles.BikePower;
using AntPlus.Profiles.Components;
using IRT_GUI.IrtMessages;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IRT_GUI
{
    /// <summary>
    /// IRT Emotion Roller display and control device.  Use this object to get/set 
    /// data on the emotion rollers.  This object implements INotifyPropertyChanged 
    /// You can use it to bind to display items.
    /// </summary>
    public class EmotionRollerDisplay : BikePowerDisplay
    {
        const byte ANT_BURST_MSG_ID_SET_MAGNET_CA = 0x60;
        const byte ANT_BURST_MSG_ID_SET_POSITIONS = 0x59;
        const byte ANT_BURST_MSG_ID_SET_RESISTANCE = 0x48;

        const byte RESISTANCE_SET_SLOPE = 0x46;
        const byte RESISTANCE_SET_WIND = 0x47;
        const byte RESISTANCE_SET_WHEEL_CR = 0x48;
        const byte RESISTANCE_SET_BIKE_TYPE = 0x44; // Co-efficient of rolling resistance
        const byte RESISTANCE_SET_C = 0x45; // Wind resistance offset.

        FirmwareRev m_firmwareRev;

        byte m_max_resistance_levels = 9;
        ushort m_min_servo_pos = 2000;
        ushort m_max_servo_pos = 700;

        Position[] m_positions;

        const ushort DEFAULT_WHEEL_SIZE = 2096;


        public EmotionRollerDisplay(ANT_Channel channel, AntPlus.Types.Network network)
            : base(channel, network)
        {
            // TODO: This is only temporary, can be removed.
            channel.channelResponse += channel_channelResponse;
            this.ChannelParameters.TransmissionType = 0xA5;

            this.SensorFound += m_eMotion_SensorFound;
            this.ChannelStatusChanged += m_eMotion_ChannelStatusChanged;

            this.ManufacturerIdentificationPageReceived += m_eMotion_ManufacturerIdentificationPageReceived;
            this.ProductInformationPageReceived += m_eMotion_ProductInformationPageReceived;

            this.BatteryStatusPageReceived += m_eMotion_BatteryStatusPageReceived;
            this.DataPageReceived += m_eMotion_DataPageReceived;
            this.ManufacturerSpecificPageReceived += m_eMotion_ManufacturerSpecificPageReceived;
            this.GetSetParametersPageReceived += m_eMotion_GetSetParametersPageReceived;
            this.TemperatureSubPageReceived += m_eMotion_TemperatureSubPageReceived;
            this.MeasurementOutputPageReceived += m_eMotion_MeasurementOutputPageReceived;

            this.StandardWheelTorquePageReceived += m_eMotion_StandardWheelTorquePageReceived;
            this.StandardPowerOnlyPageReceived += m_eMotion_StandardPowerOnlyPageReceived;

            // Set the default to the defaulto f the device (2096)
            this.WheelCircumference = DEFAULT_WHEEL_SIZE;


        }

        /// <summary>
        /// TODO: This will require an event handler that takes a resistance object with 
        /// type and value.
        /// </summary>
        public event Action ResistanceChanged;

        public event Action StatusChanged;

        public DeviceStatus Status;

        public List<ushort> ServoPositions;

        public ushort DeviceId { get; set; }

        public int ServoPosition { get; set; }

        /// <summary>
        /// Target Watt / Position, etc... level.. TOOD: refactor into different types (see: Step?)
        /// </summary>
        public int TargetLevel { get; }

        public int ResistanceMode { get; }

        public float Temperature { get; }

        public int FlywheelRevs { get; }

        public float BatteryVolts { get; }

        public System.Drawing.Color BatteryStatusColor { get; }

        public string BatteryTime { get; }

        public int MovingAverageSeconds { get; set; }

        public float AverageSpeedMph { get; }

        public float AveragePower { get; }

        public FirmwareRev FirmwareRevision { get; }

        public string HardwareRevision { get; }

        public string Model { get; set; }

        public string SerialNumber { get; }

        public float WeightKg { get; set; }

        public int WheelSizeMm { get; set; }

        public float Drag { get; }

        public float RollingResistance { get; }

        public int FlywheelRevolutions { get; set; }

        public int ServoOffset { get; set; }

        public int Settings { get; set; }

        public int Features { get; }

        public bool Charging { get; set; }

        /// <summary>
        /// Sets the period for the moving average.
        /// </summary>
        /// <param name="seconds"></param>
        private void SetMovingAverage(int seconds) { }

        public void SetCalibration(float drag, float rr) 
        {
            try
            {
                byte[] drag_buffer = BitConverter.GetBytes(drag);
                byte[] rr_buffer = BitConverter.GetBytes(rr);

                GetSetMessage dragMessage = new GetSetMessage(SubPages.Drag);
                dragMessage.SetPayLoad(drag_buffer);

                if (!SetParameter(dragMessage))
                {
                    throw new Exception("Failed to set drag, please retry.");
                }

                GetSetMessage rrMessage = new GetSetMessage(SubPages.RR);
                rrMessage.SetPayLoad(rr_buffer);

                if (!SetParameter(rrMessage))
                {
                    throw new Exception("Failed to set rr, please retry.");
                }
            }
            catch (Exception ex)
            {
                UpdateStatus("Error trying to set calibration: " + ex.Message);
            }
 
        }

        public void RequestCalibration()
        {
            RequestDeviceParameter(SubPages.Drag);
            RequestDeviceParameter(SubPages.RR);
        }

        public void StartCalibration()
        {
            RequestCalibrationMessage message = new RequestCalibrationMessage();

            UpdateStatus("Requesting calibration.");
            bool result = RetryCommand(ANT_RETRY_REQUESTS, ANT_RETRY_DELAY, () =>
            { return m_eMotionChannel.sendAcknowledgedData(message.AsBytes(), ACK_TIMEOUT); });

            if (!result)
            {
                UpdateStatus(String.Format("Unable to request calibraiton, return result: {0}.", result));
            }
        }

        void SetSlope(double slope)
        {
            ushort value = 0;

            value = (ushort)(32768 * (slope / 100.0f));
            value ^= 1 << 15; // Flip the sign.

            SendBurst(RESISTANCE_SET_SLOPE, value);
        }

        // Commands
        public enum Command : byte
        {
            SetWeight = 0x60,
            MoveServo = 0x61,
            SetButtonStops = 0x62,
            SetDFUMode = 0x64
        };

        public enum DeviceStatus
        {
            Closed,
            Searching,
            Timeout,
            SentDFU,
            Cablibration,
            Connected
        }

        // TODO: I don't think this is requried, should be able to get this through the standard events.
        void channel_channelResponse(ANT_Response response)
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

            if (m_dataPoint.ResistanceMode != message.Mode ||
                m_dataPoint.TargetLevel != message.Level)
            {
                ResetWatchClock();
            }

            m_dataPoint.ServoPosition = message.ServoPosition;
            m_dataPoint.TargetLevel = message.Level;
            m_dataPoint.Temperature = message.Temperature;
            m_dataPoint.ResistanceMode = message.Mode;
            m_dataPoint.FlywheelRevs = message.FlyweelRevs;

            UpdateText(lblFlywheel, message.FlyweelRevs);

            UpdateResistanceDisplay((ResistanceMode)message.Mode, message.Level,
                message.ServoPosition);
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
                    m_eMotion.WheelCircumference = wheelSize;
                    UpdateStatus("Received wheelsize parameter.");
                    UpdateText(txtWheelSizeMm, wheelSize);
                    break;

                case SubPages.Settings:
                    m_settings = Message.BigEndian(buffer[2], buffer[3]);
                    UpdateStatus("Received settings parameter.");
                    UpdateSettings(m_settings);
                    break;

                case SubPages.Charger:
                    UpdateStatus(string.Format("Received charger parameter {0}.", buffer[2]));
                    bool check = (buffer[2] == 0x02); // Charging
                    UpdateCheckbox(chkCharge, check);
                    break;

                case SubPages.Features:
                    UpdateStatus("Received features parameter.");
                    ushort features = Message.BigEndian(buffer[2], buffer[3]);
                    UpdateText(lblFeatures, features);
                    break;

                case SubPages.Drag:
                    UpdateStatus("Received Drag parameter.");
                    UpdateText(txtDrag, BitConverter.ToSingle(buffer, 2));
                    break;

                case SubPages.RR:
                    UpdateStatus("Received RR parameter.");
                    UpdateText(txtRR, BitConverter.ToSingle(buffer, 2));
                    break;

                case SubPages.ServoPositions:
                    UpdateStatus("Received servo positions message. Max positions: " +
                        buffer[2]);
                    ProcessServoPositions(buffer);

                    goto default;
                default:
                    UpdateStatus(string.Format("Received Parameters page: {0} - " +
                        "[{1:x2}][{2:x2}][{3:x2}][{4:x2}][{5:x2}][{6:x2}]",
                        m.SubPage,
                        buffer[2],
                        buffer[3],
                        buffer[4],
                        buffer[5],
                        buffer[6],
                        buffer[7]));
                    break;
            }
        }


        private void RestoreUserProfile()
        {
            // Restore parameters
            UpdateStatus("Reconnected after DFU.");
            m_enteredDFU = false;

            var result = MessageBox.Show(
                    "User profile settings are reset during firmware update.\r\n" +
                    "Would you like to restore current user profile settings?",
                    "Resuming from DFU",
                    MessageBoxButtons.YesNo,
                    MessageBoxIcon.Question,
                    MessageBoxDefaultButton.Button1);

            if (result == System.Windows.Forms.DialogResult.Yes)
            {
                // Flag these as dirty
                txtTotalWeight.Modified = true;
                txtWheelSizeMm.Modified = true;
                txtDrag.Modified = true;
                txtRR.Modified = true;

                // Wrap this in a seperate thread.
                //var t = new System.Threading.Thread(() =>
                //{
                // Use last saved values to reprogram Crr, Weight, WheelSize & ServoOffset.
                UpdateStatus("Restoring user profile.");

                // Simulate pressing both of these buttons.
                btnServoOffset_Click(this, null);

                // Wait 1/2 second before pushing the next one.
                System.Threading.Thread.Sleep(500);

                btnCalibration2Set_Click(this, null);
                System.Threading.Thread.Sleep(500);

                btnSettingsSet_Click(this, null);
                //});

                //t.Start();
            }
        }

        void m_eMotion_ManufacturerSpecificPageReceived(ManufacturerSpecificPage arg1, uint arg2)
        {
            // wahoo resistance ack pages come through here.
            if (arg1.RawContent[0] == ResistanceMessage.Page)
            {
                ResistanceMode mode = (ResistanceMode)arg1.RawContent[1];
                ushort level = Message.BigEndian(arg1.RawContent[4], arg1.RawContent[5]);

                System.Diagnostics.Debug.Print("Mode: 0x{0:X}, Value: {1}", mode, level);

                UpdateResistanceDisplay(mode, level);
            }
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

            Color changeColor = SystemColors.ControlText;

            switch (arg1.BatteryStatus)
            {
                case Common.BatteryStatus.Critical:
                    changeColor = Color.Red;
                    break;
                case Common.BatteryStatus.New:
                case Common.BatteryStatus.Good:
                    changeColor = Color.Green;
                    break;
                case Common.BatteryStatus.Low:
                    changeColor = Color.Goldenrod;
                    break;
                default:
                    changeColor = SystemColors.ControlText;
                    break;
            }

            if (lblEmrBattVolt.ForeColor != changeColor)
            {
                ExecuteOnUI(() => { lblEmrBattVolt.ForeColor = changeColor; });
            }

            double hours = 0.0;
            int minutes = 0;

            if (arg1.CumulativeOperatingTimeResolution == Common.Resolution.TwoSeconds)
            {
                // CumulativeOperatingTime represents a tick every 2 seconds.
                hours = arg1.CumulativeOperatingTime * 2 / 3600.0d;
            }
            else
            {
                //minutes = arg1.CumulativeOperatingTime * 16 / 60;
                //hours = minutes / 60;

                // CumulativeOperatingTime represents a tick every 16 seconds.
                hours = arg1.CumulativeOperatingTime * 16 / 3600.0d;
            }

            // Convert to minutes.
            //minutes = (int)((hours - Math.Floor(hours)) * 60);
            minutes = (int)((hours - Math.Floor(hours)) * 60);

            UpdateText(lblEmrBattTime, string.Format("{0:N0}:{1:D2}", hours, minutes));
        }

        void m_eMotion_ProductInformationPageReceived(AntPlus.Profiles.Common.ProductInformationPage arg1, uint arg2)
        {
            UpdateText(lblEmrSerialNo, arg1.SerialNumber.ToString("X"));
            if (m_EmrDevices != null && m_EmrDevices.Count > 0)
            {
                var device = m_EmrDevices.Last();
                device.SerialNumber = arg1.SerialNumber;
            }

            m_firmwareRev = new FirmwareRev(arg1.SoftwareRevision, arg1.SupplementalSoftwareRevision);

            UpdateText(lblEmrFirmwareRev, m_firmwareRev);
        }

        void m_eMotion_ManufacturerIdentificationPageReceived(AntPlus.Profiles.Common.ManufacturerIdentificationPage arg1, uint arg2)
        {
            UpdateText(lblEmrHardwareRev, arg1.HardwareRevision);
            UpdateText(lblEmrModel, arg1.ModelNumber);
        }

        private bool SetParameter(byte subpage, UInt32 value)
        {
            GetSetMessage message = new GetSetMessage(subpage);
            message.SetPayLoad(value);
            return SetParameter(message);
        }

        private bool SetParameter(GetSetMessage message)
        {
            UpdateStatus(string.Format("Sending {0} parameter.", message.SubPage));
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

        private void MoveServo(int position)
        {
            byte[] data = {
                (byte)(position), // Position LSB
                (byte)(position >> 8), // Position MSB
            };
            if (SendCommand(Command.MoveServo, data))
            {
                UpdateStatus(string.Format("Moving servo to position: {0}", position));
            }
            else
            {
                UpdateStatus("Failed to send servo move command.");
            }
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

        private float DecodeResistancePercent(int value)
        {
            float percent = ((16383.0f - value) / 16383.0f) * 100.0f;

            // Round to a single decimal.
            return (float)Math.Round(percent, 1);
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


        private void RequestSettings()
        {
            if (!m_requestingSettings)
            {
                lock (this)
                {
                    m_requestingSettings = true;

                    List<SubPages> parameters = new List<SubPages>();
                    parameters.Add(SubPages.Product);
                    parameters.Add(SubPages.Drag);
                    parameters.Add(SubPages.RR);
                    parameters.Add(SubPages.TotalWeight);
                    parameters.Add(SubPages.WheelSize);
                    parameters.Add(SubPages.Settings);
                    parameters.Add(SubPages.ServoOffset);
                    parameters.Add(SubPages.Charger);
                    parameters.Add(SubPages.Features);
                    parameters.Add(SubPages.Crr);
                    parameters.Add(SubPages.ServoPositions);

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

        private bool RequestDeviceParameter(SubPages subPage, byte descriptor2 = 0xFF)
        {
            RequestDataMessage message = new RequestDataMessage();

            if (subPage == SubPages.Battery || subPage == SubPages.Product || subPage == SubPages.Manufacturer)
            {
                message.RequestedPage = (byte)subPage;
            }
            else if (subPage == SubPages.Temp)
            {
                message.RequestedPage = (byte)SubPages.MeasurementOuput;

            }
            else
            {
                message.RequestedPage = (byte)GetSetMessage.Page;
                message.SubPage = subPage;
                message.Descriptor2 = descriptor2;
            }

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

        private void InitMovingAverage(int count)
        {
            m_movingAvgPosition = 0;
            InitializeArray(count, ref m_eMotionPowerList);
            InitializeArray(count, ref m_RefPowerList);
            InitializeArray(count, ref m_SpeedList);
        }

        void OnSetPositions(object sender, EventArgs e)
        {
            ServoPositions dialog = sender as ServoPositions;
            if (dialog == null)
                return;

            // 3 messages * 8 bytes each
            byte[] data = new byte[24];
            data[0] = ANT_BURST_MSG_ID_SET_POSITIONS;
            data[1] = (byte)(dialog.Positions.Count());

            // Start index for positions.
            int index = 2;

            m_positions = dialog.Positions.ToArray<Position>();

            foreach (Position p in m_positions)
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

                // Initiate a request to verify.
                var t = new System.Threading.Timer(o =>
                {
                    RequestDeviceParameter(SubPages.ServoPositions, 0);
                }
                    , null, 500, System.Threading.Timeout.Infinite);
            }
            else
            {
                UpdateStatus("Failed to set servo positions.");
            }

            //RequestDeviceParameter(SubPages.ServoPositions /*, start index */);
        }

        // Safety against too many recursive calls to get the next sequence.
        private int m_lastPositionRequestIndex = 0;

        private void ProcessServoPositions(byte[] buffer)
        {
            m_max_resistance_levels = (byte)buffer[2];      // Count of resistance levels, 0 == no resistance level.
            byte startIndex = buffer[3];                    // Start index for these 2 levels.

            if (m_positions == null || m_positions.Length != m_max_resistance_levels)
            {
                m_positions = new Position[m_max_resistance_levels];
                m_lastPositionRequestIndex = 0;
            }

            for (byte i = 0; startIndex < m_max_resistance_levels && i < 3; ) // can only read 2 positions per buffer.
            {
                Position p = new Position(Message.BigEndian(buffer[4 + i++], buffer[4 + i++]));
                m_positions[startIndex++] = p;
            }

            // Set minimum servo position value.
            if (m_min_servo_pos != m_positions[0].Value)
            {
                m_min_servo_pos = m_positions[0].Value;
            }

            // Request next series of servo positions if there are more.
            if (startIndex < m_max_resistance_levels && m_lastPositionRequestIndex != startIndex)
            {
                if (RequestDeviceParameter(SubPages.ServoPositions, startIndex))
                {
                    UpdateStatus(String.Format("Requested servo positions from position: {0}", startIndex));
                    m_lastPositionRequestIndex = startIndex;
                }
                else
                {
                    UpdateStatus(String.Format("Failed to request servo positions from: {0}.", startIndex));
                }
            }
        }

        /* Functions to decode simulation responses from WAHOO */
        private void UpdateCrr(ushort value)
        {
            float crr = value / 10000.0f;
            UpdateText(txtSimCrr, crr);
        }

        private void UpdateC(ushort value)
        {
            float c = value / 1000.0f;
            UpdateText(txtSimC, c);
        }

        private void UpdateWind(ushort value)
        {
            float wind = (value - 32768) / 1000.0f;

            UpdateText(txtSimWind, Math.Round(wind, 1));
            System.Diagnostics.Debug.WriteLine("Set wind speed to: {0}", wind);
        }

        private void UpdateSlope(ushort value)
        {
            short svalue;
            // Flip the sign bit, since it's in reverse.
            svalue = (short)(value ^ 1 << 15);
            float grade = (svalue / 32768.0f) * 100.0f;

            UpdateText(txtSimSlope, Math.Round(grade, 1));
        }

        private void UpdateResistancePercentage(ushort level)
        {
            if (level > 0)
            {
                float percent = DecodeResistancePercent(level);

                UpdateText(txtResistancePercent, percent);
                trackBarResistancePct.Value = (int)percent;
            }
        }

        private void UpdateResistanceDisplay(ResistanceMode mode, ushort level, ushort servoPosition = 0)
        {
            if (!m_PauseServoUpdate)
            {
                if (servoPosition != 0)
                {
                    UpdateText(txtServoPos, servoPosition);
                }

                ExecuteOnUI(() =>
                {
                    switch (mode)
                    {
                        case ResistanceMode.Standard:
                            cmbResistanceMode.SelectedIndex = 0;
                            UpdateText(lblResistanceStdLevel, level);
                            break;
                        case ResistanceMode.Percent:
                            cmbResistanceMode.SelectedIndex = 1;
                            UpdateResistancePercentage(level);
                            break;
                        case ResistanceMode.Erg:
                            cmbResistanceMode.SelectedIndex = 2;
                            UpdateText(txtResistanceErgWatts, level);
                            break;
                        case ResistanceMode.Sim:
                            cmbResistanceMode.SelectedIndex = 3;
                            break;
                        case ResistanceMode.SimSetCrr:
                            UpdateCrr(level);
                            break;
                        case ResistanceMode.SimSetSlope:
                            UpdateSlope(level);
                            break;
                        case ResistanceMode.SimSetWind:
                            UpdateWind(level);
                            break;
                        case ResistanceMode.SimSetC:
                            UpdateC(level);
                            break;
                        default:
                            System.Diagnostics.Debug.WriteLine("Unrecognized mode.");
                            break;
                    }
                });
            }
        }

        void m_eMotion_MeasurementOutputPageReceived(MeasurementOutputPage arg1, uint arg2)
        {
            if (arg1.DataType == BikePower.MeasurementDataType.Temperature)
            {
                double temp = arg1.MeasurementValue / 1024.0;

                UpdateStatus(string.Format("Recieved temperature: {0}", temp));
            }
        }

        /// <summary>
        /// Propogates the PropertyChanged event on the UI thread.
        /// </summary>
        /// <param name="property">Grabs the caller's property name, so it is not
        /// necessary to pass this parameter from a setter.</param>
        protected virtual void OnPropertyChanged(
            [System.Runtime.CompilerServices.CallerMemberName] string property = "")
        {
            base.OnPropertyChanged(property);

            /*
            Action a = () =>
            {
                if (PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs(property));
                }
            };

            if (SyncContext != null && SyncContext.InvokeRequired)
            {
                SyncContext.BeginInvoke(a, null);
            }
            else
            {
                a.Invoke();
            }*/
        }

    }
}
