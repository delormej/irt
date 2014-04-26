using System;
using ANT_Managed_Library;
using ANT_Console.Messages;

namespace ANT_Console.Services
{
    // Encapsulates all ANT interactions.
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
                    if (TorqueEvent != null)
                        TorqueEvent(new TorqueMessage(response));
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
    }
}
