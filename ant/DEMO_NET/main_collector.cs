﻿using ANT_Managed_Library;
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

        static readonly byte STANDARD_POWER_ONLY_PAGE = 0x10;
        static readonly byte UPDATE_EVENT_COUNT_INDEX = 1;
        static readonly byte INSTANT_POWER_LSB_INDEX = 6;
        static readonly byte INSTANT_POWER_MSB_INDEX = 7;

        static readonly byte WHEEL_TORQUE_MAIN_PAGE = 0x11;
        static readonly byte WHEEL_TICKS_INDEX = 2;
        static readonly byte WHEEL_PERIOD_LSB_INDEX = 4;
        static readonly byte WHEEL_PERIOD_MSB_INDEX = 5;


        byte m_last_wheel_ticks = 0;
        ushort m_last_wheel_period = 0;

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
            byte[] payload = GetPowerMessagePayload(response);
            if (payload == null || payload[0] != STANDARD_POWER_ONLY_PAGE)
                return;

            ushort watts = GetInstantPower(payload);

            Console.WriteLine("[{0:H:mm:ss.fff}] Quarq Power: {1:N0}", 
                response.timeReceived, watts);
        }

        private void EmotionChannelResponse(ANT_Response response)
        {
            byte[] payload = GetPowerMessagePayload(response);
            if (payload == null)
                return;

            if (payload[0] == STANDARD_POWER_ONLY_PAGE)
            {
                ushort watts = GetInstantPower(payload);
                Console.WriteLine("[{0:H:mm:ss.fff}] E-Motion Power: {1:N0}",
                    response.timeReceived, watts);
            }
            else if (payload[0] == WHEEL_TORQUE_MAIN_PAGE)
            {
                float speed_mps = GetSpeed(payload);
                Console.WriteLine("[{0:H:mm:ss.fff}] E-Motion Speed: {1:N1}",
                    response.timeReceived, speed_mps * 2.23693629f);
            }
        }

        byte m_last_quarq_instant_power_update = 0;

        private ushort GetInstantPower(byte[] payload)
        {
            if (payload[UPDATE_EVENT_COUNT_INDEX] == m_last_quarq_instant_power_update)
                // No updates.
                return 0;
            else
                m_last_quarq_instant_power_update = payload[UPDATE_EVENT_COUNT_INDEX];

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

            // Calculate speed
            float speed = (wheel_ticks - m_last_wheel_ticks) /
                ((wheel_period - m_last_wheel_period) / 2048f);

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
