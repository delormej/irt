using ANT_Managed_Library;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ANT_Console_Demo
{
    class main_collector
    {

        static readonly byte[] USER_NETWORK_KEY = { 0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45 };
        static readonly byte USER_NETWORK_NUM = 0;         // The network key is assigned to this network number

        static readonly byte QUARQ_ANT_CHANNEL = 0;             
        static readonly byte EMOTION_ANT_CHANNEL = 1;

        private void ConfigureDevice(ANT_Device device, ANT_Device.dDeviceResponseHandler DeviceResponse)
        {
            device.ResetSystem();     // Soft reset
            System.Threading.Thread.Sleep(500);    // Delay 500ms after a reset

            // If you call the setup functions specifying a wait time, you can check the return value for success or failure of the command
            // This function is blocking - the thread will be blocked while waiting for a response.
            // 500ms is usually a safe value to ensure you wait long enough for any response
            // If you do not specify a wait time, the command is simply sent, and you have to monitor the protocol events for the response,
            if (!device.setNetworkKey(USER_NETWORK_NUM, USER_NETWORK_KEY, 500))
                throw new Exception("Error configuring network key");
        }

        private void ConfigureBikePowerChannel(ANT_Channel channel, ushort deviceNumber, byte transmissionType, dChannelResponseHandler ChannelResponse)
        {
            const ANT_ReferenceLibrary.ChannelType channelType = ANT_ReferenceLibrary.ChannelType.BASE_Slave_Receive_0x00;
            const byte BikePowerDeviceType = 0x0B;
            const byte AntFreq = 0x39;
            const ushort BikePowerChannelPeriod = 0x1FF6;

            channel.channelResponse += ChannelResponse;  // Add channel response function to receive channel event messages

            if (!channel.assignChannel(channelType, USER_NETWORK_NUM, 500))
                throw new Exception("Error assigning channel");

            if (!channel.setChannelID(deviceNumber, false, BikePowerDeviceType, transmissionType, 500))  // Not using pairing bit
                throw new Exception("Error configuring Channel ID");

            if (!channel.setChannelFreq(AntFreq, 500))
                throw new Exception("Error configuring Radio Frequency");

            if (!channel.setChannelPeriod(BikePowerChannelPeriod, 500))
                throw new Exception("Error configuring Channel Period");

            if (channel.openChannel(500))
                Console.WriteLine("Channel opened");
            else
                throw new Exception("Error opening channel");
        }

        private void QuarqChannelResponse(ANT_Response response)
        {
        }

        private void EmotionChannelResponse(ANT_Response response)
        {
        }

        private void ChannelResponse(ANT_Response response)
        {
            switch ((ANT_ReferenceLibrary.ANTMessageID)response.responseID)
            {
                case ANT_ReferenceLibrary.ANTMessageID.BROADCAST_DATA_0x4E:
                case ANT_ReferenceLibrary.ANTMessageID.ACKNOWLEDGED_DATA_0x4F:
                case ANT_ReferenceLibrary.ANTMessageID.BURST_DATA_0x50:
                case ANT_ReferenceLibrary.ANTMessageID.EXT_BROADCAST_DATA_0x5D:
                case ANT_ReferenceLibrary.ANTMessageID.EXT_ACKNOWLEDGED_DATA_0x5E:
                case ANT_ReferenceLibrary.ANTMessageID.EXT_BURST_DATA_0x5F:
                    Console.Write("Rx:(" + response.antChannel.ToString() + "): ");
                    Console.Write(BitConverter.ToString(response.getDataPayload()) + Environment.NewLine);  // Display data payload
                    break;
                default:
                    break;
            }
        }

        private void DeviceResponse(ANT_Response response)
        {
            // Going to just ignore for now.
        }

        public void Start()
        {
            Console.WriteLine("Attempting to connect to an ANT USB device...");
            ANT_Device usb_ant_device = new ANT_Device();   // Create a device instance using the automatic constructor (automatic detection of USB device number and baud rate)
            ConfigureDevice(usb_ant_device, DeviceResponse);

            ushort quarq_device_id = 52652;
            byte quarq_tranmission_type = 0x5;

            ushort emotion_device_id = 1;
            byte emotion_tranmission_type = 0xA5;

            ANT_Channel quarq_channel = usb_ant_device.getChannel(QUARQ_ANT_CHANNEL);    // Get channel from ANT device
            ConfigureBikePowerChannel(quarq_channel, quarq_device_id, quarq_tranmission_type, ChannelResponse);

            ANT_Channel emotion_channel = usb_ant_device.getChannel(EMOTION_ANT_CHANNEL);    // Get channel from ANT device
            ConfigureBikePowerChannel(emotion_channel, emotion_device_id, emotion_tranmission_type, ChannelResponse);
            
            Console.WriteLine("Initialization was successful!");
        }

        static void Main(string[] args)
        {
            try
            {
                main_collector collector = new main_collector();
                collector.Start();

                while (true) ;  // Don't exit unless an exception forces.
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }
    }
}
