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

            byte[] command = ResistanceMessage.GetCommand(
                (byte)Command.SetWeight, m_sequence++, data);

            ANT_ReferenceLibrary.MessagingReturnCode ret = m_channel.sendAcknowledgedData(command, 500);

            return (ret == ANT_ReferenceLibrary.MessagingReturnCode.Pass);
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
                // Accumuldated Torque
                int accum_torque_delta = 0;
                if (message.AccumTorque < m_lastTorqueMessage.AccumTorque)
                    // If we had a rollover.
                    accum_torque_delta = (ushort)((m_lastTorqueMessage.AccumTorque ^ 0xFF) +
                        message.AccumTorque);
                else
                    accum_torque_delta = (ushort)(message.AccumTorque -
                        m_lastTorqueMessage.AccumTorque);

                // Wheel period / event time.
                ushort wheel_period_delta = 0;
                if (message.WheelPeriod < m_lastTorqueMessage.WheelPeriod)
                    // If we had a rollover.
                    wheel_period_delta = (ushort)((m_lastTorqueMessage.WheelPeriod ^ 0xFFFF) +
                        message.WheelPeriod);
                else
                    wheel_period_delta = (ushort)(message.WheelPeriod - m_lastTorqueMessage.WheelPeriod);

                if (wheel_period_delta > 0)
                {
                    float value = (float)accum_torque_delta / (float)wheel_period_delta;
                    message.CalculatedPower = (short)((128 * Math.PI) * value);
                }
            }

            // Save state for next round.
            m_lastTorqueMessage = message;

            if (TorqueEvent != null)
                TorqueEvent(message);
        }
    }
}
