using System;
using ANT_Console.Messages;
using ANT_Managed_Library;

namespace ANT_Console.Services
{
    public struct AntConfig
    {
        public int ChannelId;
        public ushort DeviceId;
        public byte TransmissionType;
        public ANT_ReferenceLibrary.ChannelType ChannelType;  // = ANT_ReferenceLibrary.ChannelType.BASE_Slave_Receive_0x00;
        public byte DeviceType; //  = 0x0B;
        public byte AntFreq; // = 0x39;
        public ushort ChannelPeriod; //  = 0x1FF6;
    }

    public delegate void MessageHandler<T>(T message) where T : Message;

    abstract class AntService
    {
        static byte[] USER_NETWORK_KEY = { 0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45 };
        static byte USER_NETWORK_NUM = 0;         // The network key is assigned to this network number

        static ANT_Device m_device;
        protected ANT_Channel m_channel;

        public event EventHandler<EventArgs> Connected;
        public event EventHandler<EventArgs> Closing;

        protected abstract void ProcessResponse(ANT_Response response);

        // Generic function to get moved out of here and work for any ANT channel.
        protected virtual void Configure(AntConfig config)
        {
            lock(this)
            {
                if (m_device == null)
                    m_device = GetDevice();
            }

            m_channel = m_device.getChannel(config.ChannelId);
            m_channel.channelResponse += OnResponse;
            
            if (!m_channel.assignChannel(config.ChannelType, USER_NETWORK_NUM, 500))
                throw new Exception("Error assigning channel");

            if (!m_channel.setChannelID(config.DeviceId, false, config.DeviceType, config.TransmissionType, 500))  // Not using pairing bit
                throw new Exception("Error configuring Channel ID");

            if (!m_channel.setChannelFreq(config.AntFreq, 500))
                throw new Exception("Error configuring Radio Frequency");

            if (!m_channel.setChannelPeriod(config.ChannelPeriod, 500))
                throw new Exception("Error configuring Channel Period");

            if (m_channel.openChannel(500))
                Console.WriteLine("Channel opened");
            else
                throw new Exception("Error opening channel");
        }

        protected virtual void OnResponse(ANT_Response response)
        {
            switch ((ANT_ReferenceLibrary.ANTMessageID)response.responseID)
            {
                case ANT_ReferenceLibrary.ANTMessageID.BROADCAST_DATA_0x4E:
                case ANT_ReferenceLibrary.ANTMessageID.ACKNOWLEDGED_DATA_0x4F:
                case ANT_ReferenceLibrary.ANTMessageID.ADV_BURST_DATA_0x72:
                case ANT_ReferenceLibrary.ANTMessageID.BURST_DATA_0x50:
                    // Process response.
                    ProcessResponse(response);
                    break;
                case ANT_ReferenceLibrary.ANTMessageID.OPEN_CHANNEL_0x4B:
                    if (Connected != null)
                        Connected(this, null);
                    break;
                case ANT_ReferenceLibrary.ANTMessageID.CLOSE_CHANNEL_0x4C:
                    if (Closing !=null)
                        Closing(this, null);
                    break;
                default:
                    // Nothing to process.
                    break;
            }
        }

        private ANT_Device GetDevice()
        {
            ANT_Device device = new ANT_Device();   // Create a device instance using the automatic constructor (automatic detection of USB device number and baud rate

            device.ResetSystem();     // Soft reset
            System.Threading.Thread.Sleep(500);    // Delay 500ms after a reset

            // If you call the setup functions specifying a wait time, you can check the return value for success or failure of the command
            // This function is blocking - the thread will be blocked while waiting for a response.
            // 500ms is usually a safe value to ensure you wait long enough for any response
            // If you do not specify a wait time, the command is simply sent, and you have to monitor the protocol events for the response,
            if (!device.setNetworkKey(USER_NETWORK_NUM, USER_NETWORK_KEY, 500))
                throw new Exception("Error configuring network key");

            return device;
        }
    }
}
