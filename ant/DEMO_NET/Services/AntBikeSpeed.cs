using System;
using ANT_Managed_Library;
using ANT_Console.Messages;

namespace ANT_Console.Services
{
    class AntBikeSpeed : AntService
    {
        public event MessageHandler<SpeedMessage> SpeedEvent;

        private SpeedMessage m_lastSpeedMessage;
        private short m_wheelSizeMM = 2096;

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
        public short WheelSizeMM
        {
            get { return m_wheelSizeMM; }
            set { m_wheelSizeMM = value; }
        }

        protected override void ProcessResponse(ANT_Response response)
        {
            //switch (Message.GetPage(response))
            //{
            //    case SpeedMessage.Page:
                    ProcessMessage(new SpeedMessage(response));
            /*        break;

                default:
                    break;
            }*/
        }

        protected virtual void ProcessMessage(SpeedMessage message)
        {
            //TODO: there is a lot of reuse opportunity here
            if (m_lastSpeedMessage != null)
            {
                Calculate(message);
            }

            // Save state for next round.
            m_lastSpeedMessage = message;

            if (SpeedEvent != null)
                SpeedEvent(message);
        }

        // Calclates speed and enriches the message.
        protected void Calculate(SpeedMessage message)
        {
            // TODO: there is opportunity for reuse with Torque Message / calc here.  Need to refactor.
            // Wheel period / event time
            ushort wheel_period_delta = 0;
            if (message.EventTime < m_lastSpeedMessage.EventTime)
                // If we had a rollover.
                wheel_period_delta = (ushort)((m_lastSpeedMessage.EventTime ^ 0xFFFF) +
                    message.EventTime);
            else
                wheel_period_delta = (ushort)(message.EventTime - m_lastSpeedMessage.EventTime);

            // Wheel ticks (revolutions)
            byte wheel_ticks_delta = 0;
            if (message.WheelRevs < m_lastSpeedMessage.WheelRevs)
                // If we had a rollover.
                wheel_ticks_delta = (byte)((m_lastSpeedMessage.WheelRevs ^ 0xFF) +
                    message.WheelRevs);
            else
                wheel_ticks_delta = (byte)(message.WheelRevs -
                    m_lastSpeedMessage.WheelRevs);

            if (wheel_period_delta > 0)
            {
                if (wheel_ticks_delta > 0)
                {
                    // Calculate speed in meters per second.
                    float speed = (wheel_ticks_delta * (m_wheelSizeMM / 1000.0f)) / (wheel_period_delta / 1024f);  // NOTE: this is different from torque message.
                    message.SpeedMps = speed;
                    message.SpeedMph = speed * 2.23694f; // Convert to MPH
                }
            }
        }
    }
}
