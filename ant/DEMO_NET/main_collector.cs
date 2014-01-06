using ANT_Managed_Library;
using System;
using System.Collections;
using System.Collections.Generic;

namespace ANT_Console_Demo
{
    public struct CollectorEventData
    {
        public float bike_speed;
        public float emotion_speed;
        public ushort emotion_power;
        public ushort quarq_power;
        public ushort resistance_level;
    }

    public struct SpeedEvent
    {
        public ushort EventTime;
        public ushort CumulativeWheelRevs;
    }

    public class Collector
    {
        static readonly byte[] USER_NETWORK_KEY = { 0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45 };
        const byte USER_NETWORK_NUM = 0;         // The network key is assigned to this network number

        const byte QUARQ_ANT_CHANNEL = 0;
        const byte EMOTION_ANT_CHANNEL = 1;
        const byte BIKE_SPEED_ANT_CHANNEL = 2;

        const byte UPDATE_EVENT_COUNT_INDEX = 1;

        const byte SPEED_EVENT_TIME_LSB_INDEX = 4;
        const byte SPEED_EVENT_TIME_MSB_INDEX = 5;
        const byte SPEED_CUM_REV_LSB_INDEX = 6;
        const byte SPEED_CUM_REV_MSB_INDEX = 7;

        // Standard power page
        const byte STANDARD_POWER_ONLY_PAGE = 0x10;
        // Torque page
        const byte WHEEL_TORQUE_MAIN_PAGE = 0x11;
        const byte WHEEL_TICKS_INDEX = 2;
        const byte WHEEL_PERIOD_LSB_INDEX = 4;
        const byte WHEEL_PERIOD_MSB_INDEX = 5;

        const byte SET_RESISTANCE_PAGE = 0x60;
        const byte RESISTANCE_LEVEL_LSB_INDEX = 4;
        //const byte RESISTANCE_LEVEL_MSB_INDEX = 5;

        byte m_last_quarq_instant_power_update = 0;
        byte m_last_emotion_instant_power_update = 0;
        byte m_last_emotion_torque_update = 0;

        SynchronizedCollection<SpeedEvent> m_bikeSpeedEvents;
        SynchronizedCollection<SpeedEvent> m_torqueSpeedEvents; 

        CollectorEventData m_last_event = new CollectorEventData();
        Calculator m_calculator = new Calculator();

        public Collector()
        {
            m_bikeSpeedEvents = new SynchronizedCollection<SpeedEvent>();
            m_torqueSpeedEvents = new SynchronizedCollection<SpeedEvent>();
        }

        public CollectorEventData EventData
        {
            get
            {
                return m_last_event;
            }
        }

        // Pops bike speed events.  Element[0] is the oldest event, Element[n]is the newest.
        public SpeedEvent[] PopBikeSpeedEvents()
        {
            return PopSpeedEvents(m_bikeSpeedEvents);
        }

        public SpeedEvent[] PopTorqueSpeedEvents()
        {
            return PopSpeedEvents(m_torqueSpeedEvents);
        }

        private SpeedEvent[] PopSpeedEvents(SynchronizedCollection<SpeedEvent> collection)
        {
            SpeedEvent[] events = null;

            lock (collection.SyncRoot)
            {
                // Copy the events to return array.
                int count = collection.Count;
                if (count < 2)
                    return null;

                events = new SpeedEvent[count];
                collection.CopyTo(events, 0);

                // Clear the collection, but add back the last event.
                collection.Clear();
                collection.Add(events[count - 1]);
            }

            return events;
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

        private void ConfigureBikeSpeedChannel(ANT_Channel channel, ushort deviceNumber, byte transmissionType, dChannelResponseHandler ChannelResponse)
        {
            const ANT_ReferenceLibrary.ChannelType channelType = ANT_ReferenceLibrary.ChannelType.BASE_Slave_Receive_0x00;
            const byte BikeSpeedDeviceType = 0x79;
            const byte AntFreq = 0x39;
            const ushort BikeSpeedChannelPeriod = 8086;

            channel.channelResponse += ChannelResponse;  // Add channel response function to receive channel event messages

            if (!channel.assignChannel(channelType, USER_NETWORK_NUM, 500))
                throw new Exception("Error assigning channel");

            if (!channel.setChannelID(deviceNumber, false, BikeSpeedDeviceType, transmissionType, 500))  // Not using pairing bit
                throw new Exception("Error configuring Channel ID");

            if (!channel.setChannelFreq(AntFreq, 500))
                throw new Exception("Error configuring Radio Frequency");

            if (!channel.setChannelPeriod(BikeSpeedChannelPeriod, 500))
                throw new Exception("Error configuring Channel Period");

            if (channel.openChannel(500))
                Console.WriteLine("Channel opened");
            else
                throw new Exception("Error opening channel");
        }

        private byte[] GetBroadcastMessagePayload(ANT_Response response)
        {
            switch ((ANT_ReferenceLibrary.ANTMessageID)response.responseID)
            {
                case ANT_ReferenceLibrary.ANTMessageID.BROADCAST_DATA_0x4E:
                    return response.getDataPayload();
                default:
                    return null;
            }
        }

        private void QuarqChannelResponse(ANT_Response response)
        {
            ProcessBikePowerResponse(response, QUARQ_ANT_CHANNEL);
        }

        private void EmotionChannelResponse(ANT_Response response)
        {
            switch ((ANT_ReferenceLibrary.ANTMessageID)response.responseID)
            {
                case ANT_ReferenceLibrary.ANTMessageID.BURST_DATA_0x50:
                case ANT_ReferenceLibrary.ANTMessageID.BROADCAST_DATA_0x4E:
                    ProcessBikePowerResponse(response, EMOTION_ANT_CHANNEL);
                    break;
                default:
                    break;
            }
        }

        void SpeedChannelResponse(ANT_Response response)
        {
            ProcessBikeSpeedResponse(response, BIKE_SPEED_ANT_CHANNEL);
        }

        private void ProcessBikeSpeedResponse(ANT_Response response, byte channelId)
        {
            byte[] payload = GetBroadcastMessagePayload(response);
            if (payload == null)
                return;

            ushort event_time = (ushort)(payload[SPEED_EVENT_TIME_LSB_INDEX] |
                payload[SPEED_EVENT_TIME_MSB_INDEX] << 8);

            ushort cumulative_revs = (ushort)(payload[SPEED_CUM_REV_LSB_INDEX] |
                payload[SPEED_CUM_REV_MSB_INDEX] << 8);

            m_bikeSpeedEvents.Add(new SpeedEvent
            {
                CumulativeWheelRevs = cumulative_revs,
                EventTime = event_time
            });
        }

        private void ProcessBikePowerResponse(ANT_Response response, byte channelId)
        {
            // Determine if we have a power message to process.
            byte[] payload = GetBroadcastMessagePayload(response);
            if (payload == null)
                return;

            byte event_count = payload[UPDATE_EVENT_COUNT_INDEX];

            switch (payload[0])
            {
                case STANDARD_POWER_ONLY_PAGE:
                    ushort watts = m_calculator.GetInstantPower(payload);

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
                    break;

                case WHEEL_TORQUE_MAIN_PAGE:
                    // Only process message if it's an update.
                    if (m_last_emotion_torque_update != event_count)
                    {
                        m_last_emotion_torque_update = event_count;

                        byte wheel_ticks = payload[WHEEL_TICKS_INDEX];
                        ushort wheel_period = (ushort)(payload[WHEEL_PERIOD_LSB_INDEX] |
                            payload[WHEEL_PERIOD_MSB_INDEX] << 8);

                        m_torqueSpeedEvents.Add(new SpeedEvent
                        {
                            CumulativeWheelRevs = wheel_ticks,
                            EventTime = wheel_period
                        });
                    }
                    break;

                case SET_RESISTANCE_PAGE:
                    ProcessEmotionResistanceMessage(payload);
                    break;
            }
        }

        private void ProcessEmotionResistanceMessage(byte[] payload)
        {
            m_last_event.resistance_level = payload[RESISTANCE_LEVEL_LSB_INDEX];
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

            byte speed_transmission_type = 0x01;
            byte speed_device_id = 0;

            ANT_Channel quarq_channel = usb_ant_device.getChannel(QUARQ_ANT_CHANNEL);    // Get channel from ANT device
            ConfigureBikePowerChannel(quarq_channel, quarq_device_id, quarq_tranmission_type, QuarqChannelResponse);

            ANT_Channel emotion_channel = usb_ant_device.getChannel(EMOTION_ANT_CHANNEL);    // Get channel from ANT device
            ConfigureBikePowerChannel(emotion_channel, emotion_device_id, emotion_tranmission_type, EmotionChannelResponse);

            ANT_Channel bike_speed_channel = usb_ant_device.getChannel(BIKE_SPEED_ANT_CHANNEL);
            ConfigureBikeSpeedChannel(bike_speed_channel, speed_device_id, speed_transmission_type, SpeedChannelResponse);

            Console.WriteLine("Initialization was successful!");
        }

        static void Main(string[] args)
        {
            try
            {
                Collector collector = new Collector();
                collector.Start();

                using (Reporter reporter = new Reporter(collector))
                {
                    reporter.Start();

                    Console.ReadKey();  // Any key to exit
                    Console.WriteLine("Exiting...");
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }
    }
}
