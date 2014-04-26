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

        // Keep track of state.
        TorqueMessage m_lastTorque; // or we could have a list of torque messages here.

        public event EventHandler<BikePowerEventArgs> PowerReported;

        public AntBikePower(int channelId, ushort deviceId = 0, byte transmissionType = 0)
        {
            m_deviceId = deviceId;

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

        public class BikePowerEventArgs : EventArgs
        {
            public DateTime TimeStamp;
            public ushort DeviceId;
            public ushort Watts;
        }

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

        protected override void ProcessResponse(ANT_Response response)
        {
            // switch case based on the message ID.
            // Each message is responsible for representing itself as a strongly typed object.

            switch (Message.GetPage(response))
            {
                case InstantPowerMessage.Page:
                    ProcessMessage(new InstantPowerMessage(response));
                    break;
                case TorqueMessage.Page:
                    ProcessMessage(new TorqueMessage(response));
                    break;
                case ResistanceMessage.Page:
                    ProcessMessage(new ResistanceMessage(response));
                    break;
                default:
                    break;
            }
        }

        protected override void ProcessBurst(ANT_Response response)
        {
            // Does the page show up in the same place for a burst message?
            if (Message.GetPage(response) != ResistanceMessage.Page)
                return;
            
            // Dependingon the sequence # behave differently?
            byte sequence = response.getBurstSequenceNumber();

            
        }

        protected virtual void ProcessMessage(TorqueMessage message)
        {
            // Calculate power based on torque.

        }

        protected virtual void ProcessMessage(InstantPowerMessage message)
        {
            // Report instant power.
            if (PowerReported != null)
                PowerReported(this, new BikePowerEventArgs()
                {
                    TimeStamp = message.Source.timeReceived,
                    Watts = message.Watts
                });
        }

        protected virtual void ProcessMessage(ResistanceMessage message)
        {

        }
    }
}
