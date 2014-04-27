using System;
using ANT_Managed_Library;

namespace ANT_Console.Messages
{
    // Represents the datapoints being collected from various sensors.
    public class DataPoint
    {
        const string FORMAT = "{0:H:mm:ss.fff}, {1:N1}, {2:N1}, {3:g}, {4:g}, {5:g}, {6:g}, {7:g}";

        public DateTime Timestamp;
        public float SpeedReference;
        public float SpeedEMotion;
        public short PowerReference;
        public short PowerEMotion;
        public ushort ServoPosition;
        public short Accelerometer_y;
        public byte Temperature;
        public byte ResistanceMode;
        public ushort TargetLevel;

        public override string ToString()
        {
            return string.Format(FORMAT,
                Timestamp,
                SpeedReference,
                SpeedEMotion,
                PowerReference,
                PowerEMotion,
                ServoPosition,
                Accelerometer_y,
                Temperature);
        }
    }

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

        /* NOTE */
        // The key difference between speed message and the calculated speed
        // from the torque message is the wheel period unit.
        // Speed messages use 1024, vs. torque is 2048:

        //float speed = (cumulative_revs_delta * m_wheel_size_m) / (event_time_delta / 1024f);
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
        public float SpeedMps;
        public float SpeedMph;
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

        const byte MODE_INDEX = 1;
        const byte SEQUENCE_INDEX = 3;
        const byte LEVEL_LSB = 4;
        const byte LEVEL_MSB = 5;

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

        public byte Mode { get { return m_payload[MODE_INDEX]; } }

        public byte Sequence { get { return m_payload[SEQUENCE_INDEX]; } }

        public ushort Level
        {
            get
            {
                return BigEndian(m_payload[LEVEL_LSB], m_payload[LEVEL_MSB]);
            }
        }
    }

    public class ManufacturerPage : Message
    {
        public const byte Page = 0x50;

        const byte HW_REV_INDEX = 3;
        const byte MANUFACTUR_LSB_INDEX = 4;
        const byte MANUFACTUR_MSB_INDEX = 5;
        const byte MODEL_LSB_INDEX = 6;
        const byte MODEL_MSB_INDEX = 7;

        internal ManufacturerPage(ANT_Response response) : base(response) { }

        public byte HardwareRevision { get { return m_payload[HW_REV_INDEX];  } }

        public ushort Manufacturer
        {
            get 
            { 
                return BigEndian(m_payload[MANUFACTUR_LSB_INDEX], m_payload[MANUFACTUR_MSB_INDEX]); 
            }
        }

        public ushort Model { get { return BigEndian(m_payload[MODEL_LSB_INDEX], m_payload[MODEL_MSB_INDEX]); } }
    }

    public class ProductPage : Message
    {
        public const byte Page = 0x51;

        const byte SW_REV_SUP_INDEX = 2;
        const byte SW_REV_MAIN_INDEX = 3;
        const byte SERIAL1_INDEX = 4;
        const byte SERIAL2_INDEX = 5;
        const byte SERIAL3_INDEX = 6;
        const byte SERIAL4_INDEX = 7;

        internal ProductPage(ANT_Response response) : base(response) { }

        public byte SoftwareRevMajor { get { return m_payload[SW_REV_MAIN_INDEX]; } }

        public byte SoftwareRevMinor { get { return m_payload[SW_REV_SUP_INDEX]; } }
    }

    public class BatteryStatus : Message
    {
        public const byte Page = 0x52;

        const byte BATT_ID_INDEX = 2;
        const byte OP_TIME1_INDEX = 3;
        const byte OP_TIME2_INDEX = 4;
        const byte OP_TIME3_INDEX = 5;
        const byte BATT_VOLT_INDEX = 6;
        const byte DESC_BIT_INDEX = 7;

        internal BatteryStatus(ANT_Response response) : base(response) { }
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

        public ushort Level
        {
            get
            {
                // Mask out the 2 first bits as they contain mode.
                byte msb = m_payload[EXTRA_INFO_ACCEL_MSB];
                return BigEndian(m_payload[EXTRA_INFO_ACCEL_LSB], (byte)(msb & 0x3F));
            }
        }

        public byte Mode
        {
            get
            {
                byte mode = m_payload[EXTRA_INFO_ACCEL_MSB];
                return (byte)((mode >> 6) + 0x40);
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