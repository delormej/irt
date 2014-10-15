using System;
using ANT_Managed_Library;
using ANT_Console.Messages;

namespace ANT_Console.Services
{
    //
    // Encapsulates all ANT interactions and enriches the messages returned by ANT with
    // service context.
    //
    public class AntBikePower : AntService
    {
        const uint ACK_TIMEOUT = 1000;
        const uint REQUEST_RETRY = 3;

        // Commands
        enum Command : byte
        {
            SetWeight = 0x60,
            MoveServo = 0x61,
            SetButtonStops = 0x62,
            SetDFUMode = 0x64
        };

        ushort m_deviceId;
        byte m_sequence;
        short m_wheelSizeMM = 2096;
        TorqueMessage m_lastTorqueMessage;

        /*
         * Events
         * 
         */
        public event MessageHandler<StandardPowerMessage> StandardPowerEvent;
        public event MessageHandler<TorqueMessage> TorqueEvent;
        public event MessageHandler<ResistanceMessage> ResistanceEvent;
        public event MessageHandler<ExtraInfoMessage> ExtraInfoEvent;
        public event MessageHandler<GetSetMessage> GetSetParameterEvent;
        public event MessageHandler<BatteryStatusMessage> BatteryStatusEvent;
        public event MessageHandler<MeasureOutputMessage> MeasureOutputEvent;

        public AntBikePower(int channelId, ushort deviceId = 0, byte transmissionType = 0)
        {
            m_lastTorqueMessage = null;
            m_deviceId = deviceId;

            // Configure the channel.
            AntConfig config = new AntConfig() {
                ChannelId = channelId,
                DeviceId = deviceId,
                TransmissionType = transmissionType,
                ChannelType = ANT_ReferenceLibrary.ChannelType.BASE_Slave_Receive_0x00,
                DeviceType = 0x0B,
                AntFreq = 0x39,
                ChannelPeriod = 0x1FF6
            };
            Configure(config);
        }

        public short WheelSizeMM
        {
            get { return m_wheelSizeMM;  }
            set { m_wheelSizeMM = value;  }
        }

        public string FirmwareVersion
        {
            get
            {
                if (m_productPage != null)
                {
                    return string.Format("{0}.{1}.{2}",
                        this.m_productPage.SoftwareRevMajor,
                        this.m_productPage.SoftwareRevMinor,
                        this.m_productPage.SoftwareRevBuild);
                }
                else
                {
                    return "unavailable";
                }
            }
        }

        /*
         *  Commands
         *  
         */
        public bool SetWeight(float weight)
        {
            ushort value = (ushort)(weight * 100);
            byte[] data = {
                (byte)value, // Weight LSB
                (byte)(value >> 8), // Weight MSB
            };

            return SendCommand(Command.SetWeight, data);
        }

        // This uses the WAHOO method with a burst message.
        public void SetWheelSize(int wheelSizeMM)
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

            var result = m_channel.sendBurstTransfer(data, 300);

            // Save local parameter.
            m_wheelSizeMM = (short)wheelSizeMM;
        }

        public void SetParameter(SubPages subpage, UInt32 value)
        {
            GetSetMessage message = new GetSetMessage(subpage);
            message.SetPayLoad(value);
            var result = m_channel.sendAcknowledgedData(message.AsBytes(), ACK_TIMEOUT);
            if (result != ANT_ReferenceLibrary.MessagingReturnCode.Pass)
            {
                throw new ApplicationException(string.Format("Unable to send set parameter, return result: {0}.", result));
            }
        }

        public void SetFirmwareUpdateMode()
        {
            SendCommand(Command.SetDFUMode);
        }

        public void MoveServo(int position)
        {
            byte[] data = {
                (byte)(position), // Position LSB
                (byte)(position >> 8), // Position MSB
            };
            SendCommand(Command.MoveServo, data);
        }

        public void RequestDeviceParameter(SubPages subPage)
        {
            int retries = 0;

            ANT_ReferenceLibrary.MessagingReturnCode result = 0;
            RequestDataMessage message;
            
            if (subPage == SubPages.Battery)
            {
                message = new RequestDataMessage();
                message.RequestedPage = (byte)SubPages.Battery;
            }
            else if (subPage == SubPages.Temp)
            {
                message = new RequestDataMessage();
                message.RequestedPage = 0x03;
            }
            else
            {
                message = new RequestDataMessage(subPage);
            }

            while (retries < REQUEST_RETRY)
            {
                result = m_channel.sendAcknowledgedData(message.AsBytes(), ACK_TIMEOUT);
                if (result == ANT_ReferenceLibrary.MessagingReturnCode.Pass)
                    return;
            }

            throw new ApplicationException(string.Format("Unable to request parameter, return result: {0}.", result));
        }

        public void SetButtonStops(ushort[] positionStops, ushort[] wattStops)
        {
            /*
            byte sequence = 1;
            byte stopCount = (byte)positionStops.Length;

            // Message format.
            // Message 1 - command, # of stops
            byte[] message = {
                    0x0,
                    0x0,
                    sequence++,
                    SET_BUTTON_STOPS,
                    stopCount,
                    0x0,
                    0x0,
                    0x0
            };

            var result = m_emotion_channel.sendBurstTransfer(message, ANT_BURST_WAIT);
            if (result != ANT_ReferenceLibrary.MessagingReturnCode.Pass)
            {
                return;
            }

            // Message n - sequence+, standard stop, erg stop
            for (byte i = 0; i < stopCount; i++)
            {
                byte[] stopMessage = {
                    0x0,
                    0x0,
                    sequence++,
                    SET_BUTTON_STOPS,
                    (byte)positionStops[i],         // LSB
                    (byte)(positionStops[i] >> 8),  // MSB
                    (byte)wattStops[i],             // LSB
                    (byte)(wattStops[i] >> 8)      // MSB
                };

                result = m_emotion_channel.sendBurstTransfer(stopMessage, ANT_BURST_WAIT);
                if (result != ANT_ReferenceLibrary.MessagingReturnCode.Pass)
                {
                    break;
                }
            }
             */
        }
        bool SendCommand(Command command, byte[] value)
        {
            byte[] data = ResistanceMessage.GetCommand(
                (byte)command, m_sequence++, value);

            ANT_ReferenceLibrary.MessagingReturnCode ret = m_channel.sendAcknowledgedData(data, 500);

            return (ret == ANT_ReferenceLibrary.MessagingReturnCode.Pass);
        }

        bool SendCommand(Command command)
        {
            byte[] value = { 0, 0 };
            return SendCommand(command, value);
        }

        /*
         * Handle messages.
         * 
         */
        protected override void ProcessResponse(ANT_Response response)
        {
            // switch case based on the message ID.
            // Each message is responsible for representing itself as a strongly typed object.

            switch (Message.GetPage(response))
            {
                case StandardPowerMessage.Page:
                    if (StandardPowerEvent != null)
                        StandardPowerEvent(new StandardPowerMessage(response));
                    break;
                case TorqueMessage.Page:
                    ProcessMessage(new TorqueMessage(response));
                    break;
                case ResistanceMessage.Page:
                    if (ResistanceEvent != null)
                        ResistanceEvent(new ResistanceMessage(response));
                    break;
                case ExtraInfoMessage.Page:
                    if (ExtraInfoEvent != null)
                        ExtraInfoEvent(new ExtraInfoMessage(response));
                    break;
                case ProductPage.Page:
                    ProcessMessage(new ProductPage(response));
                    break;
                case ManufacturerPage.Page:
                    ProcessMessage(new ManufacturerPage(response));
                    break;
                case GetSetMessage.Page:
                    
                    if (GetSetParameterEvent != null)
                        GetSetParameterEvent(new GetSetMessage(response));
                    break;
                case BatteryStatusMessage.Page:
                    if (BatteryStatusEvent != null)
                        BatteryStatusEvent(new BatteryStatusMessage(response));
                        break;

                case MeasureOutputMessage.Page:
                    if (MeasureOutputEvent != null)
                        MeasureOutputEvent(new MeasureOutputMessage(response));
                    break;

                default:
                    break;
            }
        }

        protected virtual void ProcessMessage(TorqueMessage message)
        {
            if (m_lastTorqueMessage != null)
            {
                Calculate(message);

                if (TorqueEvent != null)
                    TorqueEvent(message);
            }
            // Save state for next round.
            m_lastTorqueMessage = message;
        }

        // Calculate speed & power.
        protected bool Calculate(TorqueMessage message)
        {
            int event_delta = 0;
            
            // Handle rollover of the event count.
            if (message.EventCount < m_lastTorqueMessage.EventCount)
                // If we had a rollover.
                event_delta = (byte)((m_lastTorqueMessage.EventCount ^ 0xFF) +
                    message.EventCount);
            else
                event_delta = (byte)(message.EventCount -
                    m_lastTorqueMessage.EventCount);

            if (event_delta <= 0)
            {
                return false;
            }

            // Wheel period / event time
            ushort wheel_period_delta = 0;
            if (message.WheelPeriod < m_lastTorqueMessage.WheelPeriod)
                // If we had a rollover.
                wheel_period_delta = (ushort)((m_lastTorqueMessage.WheelPeriod ^ 0xFFFF) +
                    message.WheelPeriod);
            else
                wheel_period_delta = (ushort)(message.WheelPeriod - m_lastTorqueMessage.WheelPeriod);

            // Accumuldated Torque
            int accum_torque_delta = 0;
            if (message.AccumTorque < m_lastTorqueMessage.AccumTorque)
                // If we had a rollover.
                accum_torque_delta = (ushort)((m_lastTorqueMessage.AccumTorque ^ 0xFFFF) +
                    message.AccumTorque);
            else
                accum_torque_delta = (ushort)(message.AccumTorque -
                    m_lastTorqueMessage.AccumTorque);

            // Wheel ticks (revolutions)
            byte wheel_ticks_delta = 0;
            if (message.WheelTicks < m_lastTorqueMessage.WheelTicks)
                // If we had a rollover.
                wheel_ticks_delta = (byte)((m_lastTorqueMessage.WheelTicks ^ 0xFF) +
                    message.WheelTicks);
            else
                wheel_ticks_delta = (byte)(message.WheelTicks -
                    m_lastTorqueMessage.WheelTicks);

            if (wheel_period_delta > 0)
            {
                if (accum_torque_delta > 0)
                {
                    // Calculate power in watts.
                    float value = (float)accum_torque_delta / (float)wheel_period_delta;
                    message.CalculatedPower = (short)((128 * Math.PI) * value);
                }
                
                // Speed is calculated from the wheel period.
                message.SpeedMps = (2048 * (m_wheelSizeMM / 1000.0f) * event_delta) / wheel_period_delta;
                message.SpeedMph = message.SpeedMps * 2.23694f;

                //System.Diagnostics.Debug.Assert((message.SpeedMps > 0.5), 
                //    string.Format("Events:{0} Period:{1}: Meters/Hr:{2}",
                //        event_delta,
                //        wheel_period_delta,
                //        message.SpeedMps)); 
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}
