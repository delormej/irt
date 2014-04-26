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

        // Helper method to convert two bytes to ushort.
        protected ushort BigEndian(byte lsb, byte msb)
        {
            return (ushort)(lsb | msb << 8);
        }

        protected short BigEndianSigned(byte lsb, byte msb)
        {
            return (short)(lsb | msb << 8);
        }
    }

    public abstract class UpdateEventMessage : Message
    {
        const byte UPDATE_EVENT_COUNT = 1;

        internal UpdateEventMessage(ANT_Response response) : base(response) { }

        public byte EventCount
        {
            get { return m_payload[UPDATE_EVENT_COUNT]; }
        }
    }

    public class SpeedMessage : Message
    {
        internal SpeedMessage(ANT_Response response) : base(response) { }
    }

    public class TorqueMessage : UpdateEventMessage
    {
        public const byte Page = 0x11;

        const byte WHEEL_TICKS_INDEX = 2;
        const byte INSTANT_CADENCE_INDEX = 3;
        const byte WHEEL_PERIOD_LSB_INDEX = 4;
        const byte WHEEL_PERIOD_MSB_INDEX = 5;
        const byte ACCUM_TORQUE_LSB_INDEX = 6;
        const byte ACCUM_TORQUE_MSB_INDEX = 7;

        internal TorqueMessage(ANT_Response response) : base(response) { }

        public byte WheelTicks { get { return m_payload[WHEEL_TICKS_INDEX]; } }

        public byte Cadence { get { return m_payload[INSTANT_CADENCE_INDEX];  } }

        public ushort WheelPeriod
        {
            get
            {
                return BigEndian(m_payload[WHEEL_PERIOD_LSB_INDEX], m_payload[WHEEL_PERIOD_MSB_INDEX]);
            }
        }

        public ushort AccumTorque
        {
            get
            {
                return BigEndian(m_payload[ACCUM_TORQUE_LSB_INDEX], m_payload[ACCUM_TORQUE_MSB_INDEX]);
            }
        }

        public short CalculatedPower;
    }

    public class StandardPowerMessage : UpdateEventMessage
    {
        public const byte Page = 0x10;

        const byte PEDAL_POWER_INDEX = 2;
        const byte INSTANT_CADENCE_INDEX = 3;
        const byte ACCUM_POWER_LSB_INDEX = 4;
        const byte ACCUM_POWER_MSB_INDEX = 5;        
        const byte INSTANT_POWER_LSB_INDEX = 6;
        const byte INSTANT_POWER_MSB_INDEX = 7;

        internal StandardPowerMessage(ANT_Response response) : base(response) { }

        public byte Cadence
        {
            get { return m_payload[INSTANT_CADENCE_INDEX]; }
        }

        public ushort AccumWatts
        {
            get
            {
                return BigEndian(m_payload[ACCUM_POWER_LSB_INDEX],
                    m_payload[ACCUM_POWER_MSB_INDEX]);
            }
        }

        public short Watts
        {
            // Combine two bytes to make the watts.
            get
            {
                return BigEndianSigned(m_payload[INSTANT_POWER_LSB_INDEX],
                    m_payload[INSTANT_POWER_MSB_INDEX]);
            }
        }
    }

    public class ResistanceMessage : Message
    {
        public const byte Page = 0xF0;

        public static byte[] GetCommand(byte command, byte sequence, byte[] value)
        {
            byte[] data = {
                Page, 
                command,
                0x00, // TBD
                value[0],
                value[1],
                sequence, // increment sequence
                0x00, // TBD
                0x00  // TBD
            };

            return data;
        }

        internal ResistanceMessage(ANT_Response response) : base(response) { }
    }

    public class ExtraInfoMessage : Message
    {
        public const byte Page = 0x24;

        internal ExtraInfoMessage(ANT_Response response) : base(response) { }
        
        const byte EXTRA_INFO_FLYWHEEL_REVS = 1;
        const byte EXTRA_INFO_SERVO_POS_LSB	= 2;
        const byte EXTRA_INFO_SERVO_POS_MSB	= 3;
        const byte EXTRA_INFO_ACCEL_LSB	= 4;
        const byte EXTRA_INFO_ACCEL_MSB = 5;
        const byte EXTRA_INFO_TEMP = 6;

        public byte FlyweelRevs
        {
            get { return m_payload[EXTRA_INFO_FLYWHEEL_REVS];  }
        }

        public ushort ServoPosition
        {
            get
            {
                return BigEndian(m_payload[EXTRA_INFO_SERVO_POS_LSB], 
                    m_payload[EXTRA_INFO_SERVO_POS_MSB]);
            }
        }
        public short Accelerometer_y
        {
            get
            {
                return BigEndianSigned(m_payload[EXTRA_INFO_ACCEL_LSB],
                    m_payload[EXTRA_INFO_ACCEL_MSB]);
            }
        }

        public byte Temperature
        {
            get { return m_payload[EXTRA_INFO_TEMP]; }
        }
    }
}