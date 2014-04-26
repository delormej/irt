using System;
using ANT_Managed_Library;
using ANT_Console.Messages;

namespace ANT_Console.Services
{
    //
    // Encapsulates all ANT interactions and enriches the messages returned by ANT with
    // service context.
    //
    class AntBikePower : AntService
    {  
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
        short m_wheelSizeMM = 2107;
        TorqueMessage m_lastTorqueMessage;

        /*
         * Events
         * 
         */
        public event MessageHandler<StandardPowerMessage> StandardPowerEvent;
        public event MessageHandler<TorqueMessage> TorqueEvent;
        public event MessageHandler<ResistanceMessage> ResistanceEvent;
        public event MessageHandler<ExtraInfoMessage> ExtraInfoEvent;

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
                (byte)Command.SetWeight, m_sequence++, value);

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
                default:
                    break;
            }
        }

        protected virtual void ProcessMessage(TorqueMessage message)
        {
            //TODO: there is a lot of reuse opportunity here
            if (m_lastTorqueMessage != null)
            {
                Calculate(message);
            }

            // Save state for next round.
            m_lastTorqueMessage = message;

            if (TorqueEvent != null)
                TorqueEvent(message);
        }

        // Calculate speed & power.
        protected void Calculate(TorqueMessage message)
        {
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
                accum_torque_delta = (ushort)((m_lastTorqueMessage.AccumTorque ^ 0xFF) +
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
                if (wheel_ticks_delta > 0)
                {
                    // Calculate speed in meters per second.
                    float speed = (wheel_ticks_delta * (m_wheelSizeMM/1000.0f)) / (wheel_period_delta / 2048f);
                    message.SpeedMps = speed;
                    message.SpeedMph = speed * 2.23694f; // Convert to MPH
                }
            }
        }
    }
}
