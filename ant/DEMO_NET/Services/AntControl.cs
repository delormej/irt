using System;
using ANT_Managed_Library;

namespace ANT_Console.Services
{
    class AntControl : AntService
    {
        byte m_ctrl_sequence;
        byte[] serial_no = { 0xFF, 0xFF }; // No serial number - so device doesn't bond.

        public AntControl(int channelId, ushort deviceId = 0)
        {
            // Configure the channel.
            AntConfig config = new AntConfig()
            {
                ChannelId = channelId,
                DeviceId = deviceId,
                TransmissionType = 0x05,
                ChannelType = ANT_ReferenceLibrary.ChannelType.BASE_Slave_Receive_0x00,
                DeviceType = 0x10,
                AntFreq = 0x39,
                ChannelPeriod = 8192
            };
            Configure(config);
        }

        public void RemoteControl(byte command)
        {

            byte[] data = {
                              0x49, // Page 73
                              serial_no[0], // Slave serial number
                              serial_no[1], // (con't)
                              0xFB, // Random manufacturer ID
                              0xFB, // (con't)
                              m_ctrl_sequence++, // increment sequence
                              command, // actual command
                              0x00 // Unused (extra byte for command)
                          };

            ANT_ReferenceLibrary.MessagingReturnCode code = m_channel.sendAcknowledgedData(data, 500);

            if (code == ANT_ReferenceLibrary.MessagingReturnCode.Pass)
            {
                return;
            }
        }

        protected override void ProcessResponse(ANT_Response response)
        {
            /*
            bool m_remoteAvailable = false;

            void RemoteControlResponse(ANT_Response response)
            {
                byte[] payload = GetMessagePayload(response);
                if (payload == null)
                    return;

                // Just determines if the remote control is available.
                byte page_number = payload[0];

                switch (page_number)
                {
                    case 0x02:
                        if (payload[1] == 0)
                            m_remoteAvailable = true;
                        break;
                    default:
                        break;
                }
            }
            */
        }
    }
}
