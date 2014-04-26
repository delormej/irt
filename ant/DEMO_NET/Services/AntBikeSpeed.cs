using System;
using ANT_Managed_Library;

namespace ANT_Console.Services
{
    class AntBikeSpeed : AntService
    {
        public AntBikeSpeed(int channelId, ushort deviceId = 0)
        {
            // Configure the channel.
            AntConfig config = new AntConfig()
            {
                ChannelId = channelId,
                DeviceId = deviceId,
                TransmissionType = 0x01,
                ChannelType = ANT_ReferenceLibrary.ChannelType.BASE_Slave_Receive_0x00,
                DeviceType = 0x79,
                AntFreq = 0x39,
                ChannelPeriod = 8086
            };
            Configure(config);
        }

        protected override void ProcessResponse(ANT_Response response)
        { }
    }
}
