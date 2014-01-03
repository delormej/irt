using ANT_Managed_Library;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ANT_Console_Demo
{
    public struct CollectorEventData
    {
        public float emotion_speed;
        public ushort emotion_power;
        public ushort quarq_power; 
    }

    public class Collector
    {

        static readonly byte[] USER_NETWORK_KEY = { 0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45 };
        const byte USER_NETWORK_NUM = 0;         // The network key is assigned to this network number

        const byte QUARQ_ANT_CHANNEL = 0;
        const byte EMOTION_ANT_CHANNEL = 1;

        const byte STANDARD_POWER_ONLY_PAGE = 0x10;
        const byte UPDATE_EVENT_COUNT_INDEX = 1;
        const byte INSTANT_POWER_LSB_INDEX = 6;
        const byte INSTANT_POWER_MSB_INDEX = 7;

        const byte WHEEL_TORQUE_MAIN_PAGE = 0x11;
        const byte WHEEL_TICKS_INDEX = 2;
        const byte WHEEL_PERIOD_LSB_INDEX = 4;
        const byte WHEEL_PERIOD_MSB_INDEX = 5;

        byte m_last_quarq_instant_power_update = 0;
        byte m_last_emotion_instant_power_update = 0;
        byte m_last_emotion_torque_update = 0;
        byte m_last_wheel_ticks = 0;
        ushort m_last_wheel_period = 0;

        CollectorEventData m_last_event = new CollectorEventData();

        public CollectorEventData EventData
        {
            get
            {
                return m_last_event;
            }
        }

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

        private byte[] GetPowerMessagePayload(ANT_Response response)
        {
            if ((ANT_ReferenceLibrary.ANTMessageID)response.responseID ==
                    ANT_ReferenceLibrary.ANTMessageID.BROADCAST_DATA_0x4E)
            {
                //Console.Console.Write("Rx:(" + response.antChannel.ToString() + "): ");
                //Console.Write(BitConverter.ToString(response.getDataPayload()) + Environment.NewLine);  // Display data payload

                return response.getDataPayload();
            }
            else
            {
                return null;
            }
        }

        private void QuarqChannelResponse(ANT_Response response)
        {
            ProcessBikePowerResponse(response, QUARQ_ANT_CHANNEL);
        }

        private void EmotionChannelResponse(ANT_Response response)
        {
            ProcessBikePowerResponse(response, EMOTION_ANT_CHANNEL);
        }

        private void ProcessBikePowerResponse(ANT_Response response, byte channelId)
        {
            // Determine if we have a power message to process.
            byte[] payload = GetPowerMessagePayload(response);
            if (payload == null)
                return;

            byte event_count = payload[UPDATE_EVENT_COUNT_INDEX];

            if (payload[0] == STANDARD_POWER_ONLY_PAGE)
            {
                ushort watts = GetInstantPower(payload);

                switch (channelId)
                {
                    case QUARQ_ANT_CHANNEL:
                        if (m_last_quarq_instant_power_update != event_count)
                        {
                            m_last_event.quarq_power = watts;
                            m_last_quarq_instant_power_update = event_count;
                        }
                        break;
                    case EMOTION_ANT_CHANNEL:
                        if (m_last_emotion_instant_power_update != event_count)
                        {
                            m_last_event.emotion_power = watts;
                            m_last_emotion_instant_power_update = event_count;
                            /*
                            Console.WriteLine("[{0:H:mm:ss.fff}] E-Motion Power: {1:N0}",
                                response.timeReceived, watts);*/
                        }
                        break;
                }
            }
            else if (payload[0] == WHEEL_TORQUE_MAIN_PAGE)
            {
                if (m_last_emotion_torque_update != event_count)
                {
                    float speed_mps = GetSpeed(payload);
                    m_last_event.emotion_speed = speed_mps;
                    m_last_emotion_torque_update = event_count;
                    /*
                    Console.WriteLine("[{0:H:mm:ss.fff}] E-Motion Speed: {1:N1}",
                        response.timeReceived, speed_mps * 2.23693629f);*/
                }
            }
        }

        private ushort GetInstantPower(byte[] payload)
        {
            // Combine two bytes to make the watts.
            ushort watts = (ushort)(payload[INSTANT_POWER_LSB_INDEX] |
                payload[INSTANT_POWER_MSB_INDEX] << 8);

            return watts;
        }

        private float GetSpeed(byte[] payload)
        {
            byte wheel_ticks = payload[WHEEL_TICKS_INDEX];
            ushort wheel_period = (ushort)(payload[WHEEL_PERIOD_LSB_INDEX] |
                payload[WHEEL_PERIOD_MSB_INDEX] << 8);

            if (wheel_period == 0 || wheel_ticks == 0)
                return 0.0f;

            byte wheel_ticks_delta = 0;
            if (wheel_ticks < m_last_wheel_ticks)
                // If we had a rollover.
                wheel_ticks_delta = (byte)((255 - m_last_wheel_ticks) + wheel_ticks);
            else
                wheel_ticks_delta = (byte)(wheel_ticks - m_last_wheel_ticks);

            ushort wheel_period_delta = 0;
            if (wheel_period < m_last_wheel_period)
                // If we had a rollover.
                wheel_period_delta = (ushort)((32768 - m_last_wheel_period) + wheel_period);
            else
                wheel_period_delta = (ushort)(wheel_period - m_last_wheel_period);

            if (wheel_ticks_delta == 0 || wheel_period_delta == 0)
                return 0.0f;

            // Calculate speed
            float speed = wheel_ticks_delta / (wheel_period_delta / 2048f);

            m_last_wheel_ticks = wheel_ticks;
            m_last_wheel_period = wheel_period;

            return speed;
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
            ConfigureBikePowerChannel(quarq_channel, quarq_device_id, quarq_tranmission_type, QuarqChannelResponse);

            ANT_Channel emotion_channel = usb_ant_device.getChannel(EMOTION_ANT_CHANNEL);    // Get channel from ANT device
            ConfigureBikePowerChannel(emotion_channel, emotion_device_id, emotion_tranmission_type, EmotionChannelResponse);
            
            Console.WriteLine("Initialization was successful!");
        }

        static void Main(string[] args)
        {
            try
            {
                Collector collector = new Collector();
                collector.Start();

                Reporter reporter = new Reporter(collector);
                reporter.Start();


                while (true) ;  // Don't exit unless an exception forces.
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }
    }


    public class Reporter
    {
        Collector m_collector;
        const string report_format = "{0:H:mm:ss.fff}, {1:N1}, {2:N1}, {3:N0}, {4:N0}";


        public Reporter(Collector collector)
        {
            m_collector = collector;
        }

        public void Start()
        {
            Console.WriteLine("Starting....");
            Console.WriteLine("event_time, emotion_speed_mps, emotion_speed_mph, emotion_power, quarq_power");
            System.Timers.Timer timer = new System.Timers.Timer(1000);
            timer.Elapsed += Reporter_Elapsed;
            timer.Start();
        }

        void Reporter_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            Report(m_collector.EventData);
        }

        public void Report(CollectorEventData eventData)
        {

            Console.WriteLine(report_format,
                DateTime.Now,
                eventData.emotion_speed * 2.23693629f,
                eventData.emotion_speed,
                eventData.emotion_power,
                eventData.quarq_power);
        }

    }

}
