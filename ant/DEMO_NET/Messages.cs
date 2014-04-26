using System;
using ANT_Managed_Library;

namespace ANT_Console.Messages
{
    public abstract class Message
    {
        // Message format defines.
        const byte PAGE_INDEX = 0;

        protected ANT_Response m_response;
        protected byte[] m_payload;

        internal Message() { }

        internal Message(ANT_Response response)
        {
            m_response = response;
            m_payload = m_response.getDataPayload();
        }

        public ANT_Response Source { get { return m_response; } }

        public static int GetPage(ANT_Response response)
        {
            byte[] payload = response.getDataPayload();
            return payload[PAGE_INDEX];
        }
    }

    public class SpeedMessage : Message
    {
        internal SpeedMessage(ANT_Response response) : base(response) { }
    }

    public class TorqueMessage : Message
    {
        public const byte Page = 0x11;

        const byte WHEEL_TICKS_INDEX = 2;
        const byte WHEEL_PERIOD_LSB_INDEX = 4;
        const byte WHEEL_PERIOD_MSB_INDEX = 5;
        const byte ACCUM_TORQUE_LSB_INDEX = 6;
        const byte ACCUM_TORQUE_MSB_INDEX = 7;

        internal TorqueMessage(ANT_Response response) : base(response) { }

        public byte WheelTicks { get { return m_payload[WHEEL_TICKS_INDEX]; } }
    }

    public class ResistanceMessage : Message 
    {
        public const byte Page = 0xF0;

        internal ResistanceMessage(ANT_Response response) : base(response) { }
    }

    public class InstantPowerMessage : Message
    {
        public const byte Page = 0x10;
        const byte INSTANT_POWER_LSB_INDEX = 6;
        const byte INSTANT_POWER_MSB_INDEX = 7;

        internal InstantPowerMessage(ANT_Response response) : base(response) { }

        public ushort Watts
        {
            // Combine two bytes to make the watts.
            get
            {
                return (ushort)(m_payload[INSTANT_POWER_LSB_INDEX] |
                    m_payload[INSTANT_POWER_MSB_INDEX] << 8);
            }
        }
    }

    // case 0x24:
    public class ExtraInfoMessage : Message
    {
        internal ExtraInfoMessage(ANT_Response response) : base(response) { }
        
        // Debugging information.
        /*
#define EXTRA_INFO_FLYWHEEL_REVS		1u
#define EXTRA_INFO_SERVO_POS_LSB		2u
#define EXTRA_INFO_SERVO_POS_MSB		3u
#define EXTRA_INFO_ACCEL_LSB			4u
#define EXTRA_INFO_ACCEL_MSB			5u
#define EXTRA_INFO_TEMP					6u
        */

        
        public byte FlyweelRevs
        {
            get { return m_response.messageContents[1];  }
        }

        public ushort ServoPosition
        {
            get
            {
                return (ushort)(m_response.messageContents[2] |
                    (m_response.messageContents[3] << 8));
            }
        }
        public short Accelerometer_y
        {
            get
            {
                return (short)(m_response.messageContents[4] |
                    (m_response.messageContents[5] << 8));
            }
        }

        public byte Temperature
        {
            get {return m_response.messageContents[6];}
        }
    }
}