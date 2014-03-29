using ANT_Managed_Library;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading;

namespace ANT_Console_Demo
{
    public struct CollectorEventData
    {
        public float bike_speed;
        public float emotion_speed;
        public ushort emotion_power;
        public ushort quarq_power;
        public ushort resistance_level;
        public ushort servo_position;
        public short accelerometer_y;
        public byte temperature;
    }

    public struct SpeedEvent
    {
        public ushort EventTime;
        public ushort CumulativeWheelRevs;
        public ushort CumulativeTorque;
    }

    public class Collector
    {
        static readonly byte[] USER_NETWORK_KEY = { 0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45 };
        const byte USER_NETWORK_NUM = 0;         // The network key is assigned to this network number

        const byte QUARQ_ANT_CHANNEL = 0;
        const byte EMOTION_ANT_CHANNEL = 1;
        const byte BIKE_SPEED_ANT_CHANNEL = 2;
        const byte CTRL_ANT_CHANNEL = 3;

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
        const byte ACCUM_TORQUE_LSB_INDEX = 6;
        const byte ACCUM_TORQUE_MSB_INDEX = 7;

        const byte SET_RESISTANCE_PAGE = 0xF0;
        const byte RESISTANCE_LEVEL_LSB_INDEX = 4;
        const byte RESISTANCE_LEVEL_MSB_INDEX = 5;

        //TODO: Set the actual weight command ID.
        const byte SET_WEIGHT_COMMAND = 0x00;

        byte m_last_quarq_instant_power_update = 0;
        byte m_last_emotion_instant_power_update = 0;
        byte m_last_emotion_torque_update = 0;

        SynchronizedCollection<SpeedEvent> m_bikeSpeedEvents;
        SynchronizedCollection<SpeedEvent> m_torqueSpeedEvents; 

        CollectorEventData m_last_event = new CollectorEventData();
        Calculator m_calculator = new Calculator();

        ANT_Channel m_emotion_channel = null;
        ANT_Channel m_ctrl_channel = null;
        byte m_ctrl_sequence = 0;

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

        private void ConfigureRemoteControlChannel(ANT_Channel channel, ushort deviceNumber, byte transmissionType, dChannelResponseHandler ChannelResponse)
        {
            const ANT_ReferenceLibrary.ChannelType channelType = ANT_ReferenceLibrary.ChannelType.BASE_Slave_Receive_0x00;
            const byte CtrlDeviceType = 0x10;
            const byte AntFreq = 0x39;
            const ushort CtrlChannelPeriod = 8192;

            channel.channelResponse += ChannelResponse;  // Add channel response function to receive channel event messages

            if (!channel.assignChannel(channelType, USER_NETWORK_NUM, 500))
                throw new Exception("Error assigning channel");

            if (!channel.setChannelID(deviceNumber, false, CtrlDeviceType, transmissionType, 500))  // Not using pairing bit
                throw new Exception("Error configuring Channel ID");

            if (!channel.setChannelFreq(AntFreq, 500))
                throw new Exception("Error configuring Radio Frequency");

            if (!channel.setChannelPeriod(CtrlChannelPeriod, 500))
                throw new Exception("Error configuring Channel Period");

            if (channel.openChannel(500))
                Console.WriteLine("Remote Control Channel opened");
            else
                throw new Exception("Error opening channel");
        }


        private byte[] GetMessagePayload(ANT_Response response)
        {
            switch ((ANT_ReferenceLibrary.ANTMessageID)response.responseID)
            {
                case ANT_ReferenceLibrary.ANTMessageID.BROADCAST_DATA_0x4E:
                case ANT_ReferenceLibrary.ANTMessageID.ACKNOWLEDGED_DATA_0x4F:
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

        void RemoteControl(byte command)
        {
            if (m_ctrl_channel == null)
                return;

            byte[] data = {
                              0x49, // Page 73
                              0xA1, // Random slave serial number
                              0xFE, // (con't)
                              0xFB, // Random manufacturer ID
                              0xFB, // (con't)
                              m_ctrl_sequence++, // increment sequence
                              command, // actual command
                              0x00 // Unused (extra byte for command)
                          };

            ANT_ReferenceLibrary.MessagingReturnCode code = m_ctrl_channel.sendAcknowledgedData(data, 500);

            if (code == ANT_ReferenceLibrary.MessagingReturnCode.Pass)
            {
                return;
            }
        }

        void SetWeight(float weight)
        {

            if (m_emotion_channel == null)
                return;

            int value = (int)(weight * 100);
            /*
            uint8_t dataPage;   // SET_RESISTANCE_PAGE
            uint8_t commandId;      // 0x43
            uint8_t responseStatus; 
            uint8_t // data starts here
            uint8_t responseData0;
            uint8_t responseData1;
            uint8_t responseData2;
            uint8_t responseData3;

            // pBuffer[0] | pBuffer[1] << 8u) / 100.0f;
            */
            byte[] data = {
                SET_RESISTANCE_PAGE, 
                SET_WEIGHT_COMMAND,
                0x00, // TBD
                (byte)(value << 8), // Weight LSB
                (byte)(value), // Weight MSB
                m_ctrl_sequence++, // increment sequence
                0x00, // TBD
                0x00  // TBD
            };

            m_emotion_channel.sendAcknowledgedData(data); 
        }

        private void ProcessBikeSpeedResponse(ANT_Response response, byte channelId)
        {
            byte[] payload = GetMessagePayload(response);
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
            byte[] payload = GetMessagePayload(response);
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

                        ushort accum_torque = (ushort)(payload[ACCUM_TORQUE_MSB_INDEX] |
                            payload[ACCUM_TORQUE_MSB_INDEX] << 8);

                        m_torqueSpeedEvents.Add(new SpeedEvent
                        {
                            CumulativeWheelRevs = wheel_ticks,
                            EventTime = wheel_period,
                            CumulativeTorque = accum_torque
                        });
                    }
                    break;

                case SET_RESISTANCE_PAGE:
                    ProcessEmotionResistanceMessage(payload);
                    break;

                case 0x24:
                    m_last_event.servo_position = 
                        (ushort)(payload[2] | (payload[3] << 8));
                    m_last_event.accelerometer_y =
                        (short)(payload[4] | (payload[5] << 8));
                    m_last_event.temperature = payload[6];
                    break;
            }
        }

        private void ProcessEmotionResistanceMessage(byte[] payload)
        {
            // Only process set resistance standard acknowledgement.
            switch (payload[1])
            {
                case 0x41:
                    m_last_event.resistance_level = (ushort)(payload[RESISTANCE_LEVEL_LSB_INDEX] |
                                payload[RESISTANCE_LEVEL_MSB_INDEX] << 8);
                    break;

                case 0x40:
                case 0x42:
                case 0x43:
                case 0x44:
                case 0x45:
                case 0x46:
                case 0x47:
                    // Intentional fall through for any set resistance command other than standard.
                    m_last_event.resistance_level = 0;
                    break;

                default:
                    // All other messages should not impact resistance level.
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

            ushort emotion_device_id = 0;
            byte emotion_tranmission_type = 0xA5;

            byte speed_transmission_type = 0x01;
            byte speed_device_id = 0;

            byte ctrl_transmission_type = 0x05;
            byte ctrl_device_id = 0;
            
            ANT_Channel quarq_channel = usb_ant_device.getChannel(QUARQ_ANT_CHANNEL);    // Get channel from ANT device
            ConfigureBikePowerChannel(quarq_channel, quarq_device_id, quarq_tranmission_type, QuarqChannelResponse);
            
            m_emotion_channel = usb_ant_device.getChannel(EMOTION_ANT_CHANNEL);    // Get channel from ANT device
            ConfigureBikePowerChannel(m_emotion_channel, emotion_device_id, emotion_tranmission_type, EmotionChannelResponse);
            
            ANT_Channel bike_speed_channel = usb_ant_device.getChannel(BIKE_SPEED_ANT_CHANNEL);
            ConfigureBikeSpeedChannel(bike_speed_channel, speed_device_id, speed_transmission_type, SpeedChannelResponse);
            
            m_ctrl_channel = usb_ant_device.getChannel(CTRL_ANT_CHANNEL);
            ConfigureRemoteControlChannel(m_ctrl_channel, ctrl_device_id, ctrl_transmission_type, RemoteControlResponse);
            
            Console.WriteLine("Initialization was successful!");
        }

        static void SetWeightCommand(Collector collector)
        {
            m_inCommand = true;

            // Propmt user for weight in lbs
            
            string prompt = "<enter weight in lbs:>";
            Console.SetCursorPosition(Console.WindowLeft, Console.WindowTop + Console.WindowHeight - 1);
            ConsoleColor lastColor = Console.ForegroundColor;
            Console.ForegroundColor = ConsoleColor.Green;
            Console.Write(prompt);
            Console.ForegroundColor = lastColor;
            Console.SetCursorPosition(prompt.Length + 2, Console.CursorTop);
            Console.CursorVisible = true;

            float weight;

            if (float.TryParse(Console.ReadLine(), out weight))
            {
                if (weight < 100 || weight > 300)
                {
                    WriteCommand("Weight in lbs should be > 100 and < 300.");
                }
                else
                {
                    // Convert lb to kg.
                    float weightKg = weight / 2.2f;

                    collector.SetWeight(weightKg);
                    WriteCommand(string.Format("Set Weight to {0:N1}kg.", weightKg));
                }
            }
            else
            {
                WriteCommand("Invalid weight value.");
            }

            Console.CursorVisible = false;
            m_inCommand = false;
        }

        static void WriteCommand(string message)
        {
            m_inCommand = true;

            ConsoleColor last = Console.ForegroundColor;
            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.WriteLine(message);
            Console.ForegroundColor = last;

            m_inCommand = false;
        }

        static void OnReport(object o, string data)
        {
            if (m_inCommand)
                return;

            // Leave 2 rows at the bottom for command.
            int lastLine = Console.CursorTop;

            // Do we need to scroll?
            if (lastLine > Console.WindowHeight - 2)
            {
                // Buffer space to scroll? 
                if ((Console.WindowTop + Console.WindowHeight) >= Console.BufferHeight - 1)
                {
                    // We're out of buffer space, so flush.
                    Console.MoveBufferArea(Console.WindowLeft,
                        Console.WindowTop,
                        Console.WindowWidth,
                        Console.WindowHeight,
                        Console.WindowLeft,
                        0);

                    Console.SetWindowPosition(Console.WindowLeft, 0);
                    lastLine = Console.WindowHeight - 1;
                }
                else
                {
                    // Scroll
                    Console.SetWindowPosition(Console.WindowLeft, Console.WindowTop + 1);
                }
            }

            // Position the cursor at the bottom of the screen to write the command line.
            Console.SetCursorPosition(Console.WindowLeft, Console.WindowTop + Console.WindowHeight - 1);
            Console.Write("<enter cmd>");
            Console.SetCursorPosition(Console.WindowLeft, lastLine);
            Console.WriteLine(data);
        }

        static void ShowHelp()
        {
            m_inCommand = true;

            ConsoleColor color = Console.ForegroundColor;
            Console.ForegroundColor = ConsoleColor.Green;

            Console.WriteLine("{KEY} [Command]\n" +
                "H [Help]\n" +
                "W [Set Weight]\n" +
                "U [Send Up Command]\n" +
                "D [Send Down Command]\n" +
                "S [Send Select Command]\n" +
                "X [Exit]");

            Console.ForegroundColor = color;

            m_inCommand = false;
        }
        
        static void InteractiveConsole(Collector collector)
        {
            ConsoleKeyInfo cki;

            Console.CursorVisible = false;
            Console.WriteLine("Starting....");

            do
            {
                cki = Console.ReadKey(true);

                switch (cki.Key)
                {
                    case ConsoleKey.U:
                        collector.RemoteControl(0x00);
                        WriteCommand("Sent UP command.");
                        break;

                    case ConsoleKey.D:
                        collector.RemoteControl(0x01);
                        WriteCommand("Sent DOWN command.");
                        break;

                    case ConsoleKey.S:
                        collector.RemoteControl(0x02);
                        WriteCommand("Sent SELECT command.");
                        break;

                    case ConsoleKey.W:
                        SetWeightCommand(collector);
                        break;

                    case ConsoleKey.H:
                        ShowHelp();
                        break;

                    case ConsoleKey.X:
                        WriteCommand("Exiting...");
                        break;

                    default:
                        WriteCommand("Unrecognized command.");
                        ShowHelp();
                        break;
                }
            } while (cki.Key != ConsoleKey.X);
        }

        static bool m_inCommand = false;

        static void Main(string[] args)
        {
            Thread t = null;

            try
            {
                Collector collector = new Collector();

                t = new Thread(new ThreadStart(collector.Start));
                t.Start();
                
                using (Reporter reporter = new Reporter(collector))
                {
                    reporter.OnReport += OnReport;
                    reporter.Start();
                    InteractiveConsole(collector);
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                t.Abort();
            }

            t.Join(500);
        }
    }
}
