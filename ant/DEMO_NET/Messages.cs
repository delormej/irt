﻿using System;
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
        public float SpeedMPSEMotion;
        public short PowerReference;
        public short PowerEMotion;
        public ushort ServoPosition;
        public short Accelerometer_y;
        public byte Temperature;
        public byte ResistanceMode;
        public ushort TargetLevel;
        public uint FlywheelRevs;
        public ushort RefSpeedWheelRevs;
        public ushort RefPowerAccumTorque;

        public override string ToString()
        {
            return string.Format(FORMAT,
                Timestamp,
                SpeedReference,
                SpeedMPSEMotion,
                PowerReference,
                PowerEMotion,
                ServoPosition,
                Accelerometer_y,
                Temperature);
        }
    }

    public class CalibrationSpeed
    {
        byte[] m_buffer;

        public CalibrationSpeed(byte[] buffer)
        {
            m_buffer = buffer;
        }

        public UInt16 Seconds { get { return Message.BigEndian(m_buffer[2], m_buffer[3]); } }
        public UInt32 FlywheelRotations { 
            get 
            { 
                return 
                    (UInt32)(Message.BigEndian(m_buffer[6], m_buffer[7]) << 16) |
                    Message.BigEndian(m_buffer[4], m_buffer[5]);
            }
        }

        public override string ToString()
        {
            return String.Format("Calibration Time:{0}, Rotations:{1}",
                Seconds, FlywheelRotations);
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
        public static ushort BigEndian(byte lsb, byte msb)
        {
            return (ushort)(lsb | msb << 8);
        }

        // Returns last 4 bytes of an 8 byte buffer as UInt32.
        public static UInt32 BigEndian(byte[] buffer)
        {
            return (UInt32)(Message.BigEndian(buffer[6], buffer[7]) << 16) |
                                Message.BigEndian(buffer[4], buffer[5]);
        }

        public static short BigEndianSigned(byte lsb, byte msb)
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

        public const byte Page = 0x0; // note that it's the last 7 bits, first bit is page toggle.
        
        const byte EVENT_TIME_LSB_INDEX = 4;
        const byte EVENT_TIME_MSB_INDEX = 5;
        const byte WHEEL_REV_LSB_INDEX = 6;
        const byte WHEEL_REV_MSB_INDEX = 7;

        /* NOTE */
        // The key difference between speed message and the calculated speed
        // from the torque message is the wheel period unit.
        // Speed messages use 1024, vs. torque is 2048:

        //float speed = (cumulative_revs_delta * m_wheel_size_m) / (event_time_delta / 1024f);

        // 1/1024 second, rolls over at 64 seconds. 
        public ushort EventTime
        {
            get
            {
                return BigEndian(m_payload[EVENT_TIME_LSB_INDEX], m_payload[EVENT_TIME_MSB_INDEX]);
            }
        }

        public ushort WheelRevs
        {
            get
            {
                return BigEndian(m_payload[WHEEL_REV_LSB_INDEX], m_payload[WHEEL_REV_MSB_INDEX]);
            }
        }

        public float SpeedMps;
        public float SpeedMph;
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

        const byte REV_MAJ_MASK = 0xF0;
        const byte REV_MIN_MASK = 0x0F;

        internal ProductPage(ANT_Response response) : base(response) { }

        public byte SoftwareRevMajor
        {
            get { return (byte)(m_payload[SW_REV_MAIN_INDEX] & REV_MAJ_MASK); }
        } // 4 most sig bits

        public byte SoftwareRevMinor
        {
            get { return (byte)(m_payload[SW_REV_MAIN_INDEX] & REV_MIN_MASK); }
        } // 4 least sig bits

        public byte SoftwareRevBuild { get { return m_payload[SW_REV_SUP_INDEX]; } }

        public UInt32 SerialNumber { get { return Message.BigEndian(m_payload); } }
    }
    
    public class BatteryStatusMessage : Message
    {
        public const byte Page = 0x52;

        internal BatteryStatusMessage(ANT_Response response) : base(response) { }

        const byte ANT_BAT_ID_INDEX = 2;
        const byte ANT_BAT_TIME_LSB_INDEX = 3;
        const byte ANT_BAT_TIME_INDEX = 4;
        const byte ANT_BAT_TIME_MSB_INDEX = 5;
        const byte ANT_BAT_FRAC_VOLT_INDEX = 6;
        const byte ANT_BAT_DESC_INDEX = 7;

        public float Voltage
        {
            get
            {
                int volts;
                float fracVolts;

                // Get the coarse voltage.
                volts = this.m_payload[ANT_BAT_DESC_INDEX] & 0xF;
                fracVolts = this.m_payload[ANT_BAT_FRAC_VOLT_INDEX] / 256.0f;
                
                return volts + fracVolts;
            }
        }

        // Returns operating time in hours.  Comes accross the wire in ticks.
        public float OperatingTime
        {
            get 
            {
                int ticks;
                bool resolution2sec;
                float hours;

                resolution2sec = (this.m_payload[ANT_BAT_DESC_INDEX] & 0x80) == 1; // 7th bit.

                ticks = this.m_payload[ANT_BAT_TIME_LSB_INDEX] |
                    this.m_payload[ANT_BAT_TIME_INDEX] << 8 |
                    this.m_payload[ANT_BAT_TIME_MSB_INDEX] << 16;

                // If not 2 second resolution, then it's 16 second.
                hours = ticks * (resolution2sec ? 2 : 16) / 3600.0f;
                
                return hours; 
            
            }
        }
    }

    public class ExtraInfoMessage : Message
    {
        public const byte Page = 0x24;

        internal ExtraInfoMessage(ANT_Response response) : base(response) { }
        
        const byte EXTRA_INFO_SERVO_POS_LSB	= 1;
        const byte EXTRA_INFO_SERVO_POS_MSB	= 2;
        const byte EXTRA_INFO_TARGET_LSB	= 3;
        const byte EXTRA_INFO_TARGET_MSB = 4;
        const byte EXTRA_INFO_FLYWHEEL_REVS_LSB = 5;
        const byte EXTRA_INFO_FLYWHEEL_REVS_MSB = 6;
        const byte EXTRA_INFO_TEMP = 7;

        public ushort FlyweelRevs
        {
            get 
            { 
                return BigEndian(m_payload[EXTRA_INFO_FLYWHEEL_REVS_LSB], 
                    m_payload[EXTRA_INFO_FLYWHEEL_REVS_MSB]); 
            }
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
                byte msb = m_payload[EXTRA_INFO_TARGET_MSB];
                return BigEndian(m_payload[EXTRA_INFO_TARGET_LSB], (byte)(msb & 0x3F));
            }
        }

        public byte Mode
        {
            get
            {
                // First 2 bits of the MSB contain mode.
                byte mode = m_payload[EXTRA_INFO_TARGET_MSB];
                return (byte)((mode >> 6) + 0x40);
            }
        }

        public short Accelerometer_y
        {
            get
            {
                return 0;
                /*return BigEndianSigned(m_payload[EXTRA_INFO_TARGET_LSB],
                    m_payload[EXTRA_INFO_TARGET_MSB]);*/
            }
        }

        public byte Temperature
        {
            get { return m_payload[EXTRA_INFO_TEMP]; }
        }
    }

    public enum SubPages
    {
        CrankParameters = 0x01,
        // Custom defined parameters - starting at 16u
        Crr = 16,
        Settings = 17,
        TotalWeight = 18,
        WheelSize = 19, // I'm sure this is defined in a standard message somewhere.
        ButtonStops = 20, // Ability to configure custom button stops on the servo.
        Charger = 21,       // Get/set charger status.
        GetLastError = 22,
        ServoOffset = 23,
        CalibrationSpeed = 24,
        AuxPwr = 25,                 // Get/Set auxilury 3V power on J7-6.

        // Outliers, these are not actually a subpages, it's an actual page - need to fix this.
        Temp = 26,           // Get the temperature from the device.  This isn't a
        Sleep = 27,          // Sets the device to go to sleep.
        Battery = 0x52
        
        // Should we send commands this way, i.e.:
        // Move servo
        // Enable Device Firmware Update Mode 
        // Shutdown, etc...
    }

    public enum Settings
    {
        AccelerometerSleep = 1,
        BluetoothEnabled = 2,
        AntRemoteEnabled = 4,
        AntBikePowerEnabled = 8,
        AntFECEnabled = 16,
        AntExtraInfoEnabled = 32
    }

    // This is mostly used to Set values on the device.
    public class GetSetMessage : Message
    {
        public const byte Page = 0x02;

        private byte m_subpage;

        public GetSetMessage(SubPages subPage) 
        {
            m_payload = new byte[8];
            SubPage = (byte)subPage;
        }

        internal GetSetMessage(ANT_Response response) : base(response) 
        {  }

        public byte SubPage
        {
            private set { m_subpage = value; }
            get { return m_payload[1]; }
        }

        public void SetPayLoad(UInt32 payload)
        {
            // Adjust for endianness of the ARM Cortex M0 device.
            m_payload[5] = (byte)((payload & 0xFF000000) >> 24);
            m_payload[4] = (byte)((payload & 0x00FF0000) >> 16);
            m_payload[3] = (byte)((payload & 0x0000FF00) >> 8);
            m_payload[2] = (byte)payload;
        }

        public byte[] AsBytes()
        {
            byte[] data = new byte[] {
                Page,
                m_subpage,
                m_payload[2],
                m_payload[3],
                m_payload[4],
                m_payload[5],
                m_payload[6],
                m_payload[7]
            };

            return data;
        }
    }

    // Any time you want to Get a parameter from the device, you issue a Request Data page.
    // This should be sent as an acknowledged message to the device.
    public class RequestDataMessage : Message
    {
        public const byte Page = 0x46;
        
        public RequestDataMessage()
        {
            // Defaults.
            CommandType = 0x01;
            RequestTransmissionResponse = 2;
        }

        public RequestDataMessage(SubPages subPage) : this()
        {
            //CommandType = 0x01;
            RequestedPage = GetSetMessage.Page; // Always use the get/set parameter page.
            SubPage = subPage;
            //RequestTransmissionResponse = 0x80;
            //RequestTransmissionResponse = 2;    // Default to trying 2 times to send.
        }

        // Use this value to requesting a specific subpage.  First byte should be the value, last byte might be 0xFF?
        public SubPages SubPage;

        // Type of response the device should send back.
        /*
         * Bit 0-6: Number of times to transmit the page.
         * Bit 7:   Reply using acknowledged message, HOWEVER - ANT BP says these should always respond as broadcast.
         * 0x80:    Transmit until a succesful acknowledgement.
         * 0x00:    Invalid value.
         */
        public byte RequestTransmissionResponse;

        // The page requested.
        public byte RequestedPage;

        // The type of command, i.e. 0x01 for Requesting a Data Page, 0x02 for Setting parameters.
        public byte CommandType; 

        public byte[] AsBytes()
        {
            byte[] data = {
                              Page,
                              0xFF,
                              0xFF,
                              (byte)SubPage,
                              0xFF,
                              RequestTransmissionResponse,
                              RequestedPage,
                              CommandType
                          };

            return data;
        }
    }

    public class MeasureOutputMessage : Message
    {
        public const byte Page = 0x03;

        internal MeasureOutputMessage(ANT_Response response) : base(response) { }

        public short Value
        {
            get
            {
                return BigEndianSigned(m_payload[6],
                        m_payload[7]);
            }
        }
    }

}
