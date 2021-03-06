﻿using ANT_Managed_Library;
using System;
using IRT.Calibration;

namespace IRT_GUI.IrtMessages
{

    // Represents the datapoints being collected from various sensors.
    public class DataPoint
    {
        const string FORMAT = "{0:H:mm:ss.fff}, {1:N1}, {2:N1}, {3:g}, {4:g}, {5:g}, {6:g}, {7:g}";

        public DateTime Timestamp;
        public float SpeedReference;
        public float SpeedEMotionMph;
        public float SpeedMPSEMotion;
        public short PowerReference;
        public short PowerEMotion;
        public ushort ServoPosition;
        public short Accelerometer_y;
        public byte Temperature;
        public byte ResistanceMode;
        public ushort TargetLevel;
        public uint FlywheelRevs;
        public ushort RefPowerWheelEvents;
        public ushort RefPowerAccumTorque;
        public double RefTorque;

        public short PowerReferenceAvg;
        public short PowerEMotionAvg;
        public float SpeedEmotionAvg;

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

        public byte HardwareRevision { get { return m_payload[HW_REV_INDEX]; } }

        public ushort Manufacturer
        {
            get
            {
                return BigEndian(m_payload[MANUFACTUR_LSB_INDEX], m_payload[MANUFACTUR_MSB_INDEX]);
            }
        }

        public ushort Model { get { return BigEndian(m_payload[MODEL_LSB_INDEX], m_payload[MODEL_MSB_INDEX]); } }
    }

    public class ExtraInfoMessage : Message
    {
        public const byte Page = 0xF1;

        internal ExtraInfoMessage(ANT_Response response) : base(response) { }

        const byte EXTRA_INFO_SERVO_POS_LSB = 1;
        const byte EXTRA_INFO_SERVO_POS_MSB = 2;
        const byte EXTRA_INFO_TARGET_LSB = 3;
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
        MeasurementOuput = 0x03,

        // Custom defined parameters - starting at 16u
        Crr = 16,
        Settings = 17,
        TotalWeight = 18,
        WheelSize = 19, // I'm sure this is defined in a standard message somewhere.
        ServoPositions = 20, // Ability to configure custom button stops on the servo.
        Charger = 21,       // Get/set charger status.
        GetLastError = 22,
        ServoOffset = 23,
        CalibrationSpeed = 24,
        AuxPwr = 25,                 // Get/Set auxilury 3V power on J7-6.

        // Outliers, these are not actually a subpages, it's an actual page - need to fix this.
        Temp = 26,           // Get the temperature from the device.  This isn't a
        Sleep = 27,          // Sets the device to go to sleep.
        SpeedDebug = 28,
        Features = 29,
        Drag = 30,          // Co-efficient of drag for calibration.
        RR = 31,            // Co-efficient of rolling resistance for calibration.
        GapOffset = 32,

        // These are not actually subpages, they are pages in of themselves.
        Manufacturer = 0x50,
        Product = 0x51,
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
        AntExtraInfoEnabled = 32,
        AntBikeSpeedEnabled = 64,
        Persist = 0x8000
    }

    public abstract class Message
    {
        public const byte ANT_BURST_MSG_ID_SET_MAGNET_CA = 0x60;


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

        // Returns lsb and msb of a ushort.
        public static void LittleEndian(ushort value, out byte lsb, out byte msb)
        {
            lsb = (byte)value;
            msb = (byte)((value & 0x0000FF00) >> 8);
        }
    }

    public enum ResistanceMode : byte
    {
        Percent = 0x40,
        Standard = 0x41,
        Erg = 0x42,
        Sim = 0x43,
        SimSetCrr = 0x44,
        SimSetC = 0x45,
        SimSetSlope = 0x46,
        SimSetWind = 0x47,
        SimSetWheel = 0x48,
        InitSpinDown = 0x49,
        ReadMode = 0x4A,
        SpindownResult = 0x5A,
        SetServo = 0x5B,	
        SetWeight = 0x5C,	
        StartStopTrainerRoad = 0x65	
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

        public RequestDataMessage(SubPages subPage)
            : this()
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

        public byte Descriptor2 = 0xFF;

        public byte[] AsBytes()
        {
            byte[] data = {
                              Page,
                              0xFF,
                              0xFF,
                              (byte)SubPage, // Descriptor byte 1
                              Descriptor2, // 0xFF,
                              RequestTransmissionResponse,
                              RequestedPage,
                              CommandType
                          };

            return data;
        }
    }

    // This is mostly used to Set values on the device.
    public class GetSetMessage : Message
    {
        public const byte Page = 0x02;

        private byte m_subpage;

        public GetSetMessage(byte subPage)
        {
            m_payload = new byte[8];
            SubPage = subPage;
        }

        public GetSetMessage(SubPages subPage) : this((byte)subPage)
        {
        }

        internal GetSetMessage(ANT_Response response)
            : base(response)
        { }

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

        public void SetPayLoad(byte[] payload)
        {
            m_payload[5] = payload[3];
            m_payload[4] = payload[2];
            m_payload[3] = payload[1];
            m_payload[2] = payload[0];
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

    public class RequestCalibrationMessage : CalibrationMessage
    {
        public RequestCalibrationMessage()
        {
            m_payload[1] = 0xAA; // ID of Request calibration.
        }
    }

    // This is mostly used to Set values on the device.
    public class CalibrationMessage : Message
    {
        public const byte Page = 0x01;

        private byte m_subpage;

        public CalibrationMessage ()
        {
            m_payload = new byte[8];
        }

        internal CalibrationMessage(ANT_Response response)
            : base(response)
        { }

        byte[] TickReadings { 
            get 
            {  
                byte[] ticks = new byte[5];
                for (int i = 0; i < 5; i++)
                {
                    ticks[i] = m_payload[i+3];
                }

                return ticks;
            } 
        }

        byte Sequence { get { return m_payload[2]; } }

        public byte[] AsBytes()
        {
            byte[] data = new byte[] {
                Page,
                m_payload[1],
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

    public class IRTDevice
    {
        public ushort DeviceId;
        public uint SerialNumber;
    }

    public class MagnetCalibrationMessage
    {
        public static byte[] GetBytes(MagnetCalibration magCalibration)
        {
            /*
             * 5 Messages in total.  Each speed has 4 factors, for a total of 8 factors.
             * 2 messages per speed, 2 factors per message.
             *
             * message 1:
             * 				byte 0: 	Message ID
             * 				byte 1-2: 	Low Speed as uint16_t, divide by 1000.
             * 				byte 3-4:	High Speed  as uint16_t, divide by 1000.
             * 				byte 5-6:	Minimum servo position supported by polynomial, 
             *							below which we use linear formula to solve.
             *
             * message 2-5:
             *				byte 0-3:	float factor
             *				byte 4-7:	float factor
             */
            byte[] buffer = new byte[8 * 5];
            int index = 0;

            buffer[index++] = Message.ANT_BURST_MSG_ID_SET_MAGNET_CA;

            // Convert speeds to uint16_t.
            ushort lowSpeed = (ushort)(magCalibration.LowSpeedMps * 1000);
            ushort highSpeed = (ushort)(magCalibration.HighSpeedMps * 1000);

            Message.LittleEndian(lowSpeed, out buffer[index++], out buffer[index++]);
            Message.LittleEndian(highSpeed, out buffer[index++], out buffer[index++]);

            ushort rootPosition = (ushort)magCalibration.GetRootPosition();
            Message.LittleEndian(rootPosition, out buffer[index++], out buffer[index++]);

            // Advance 1 place, 1 blank byte to 2nd message.
            index++;

            int factorIdx = 0;

            while (index < buffer.Length)
            {
                if (index >= 24)
                {
                    // We're into page 4 & 5
                    Array.Copy(BitConverter.GetBytes(magCalibration.HighSpeedFactors[factorIdx % 4]), 0,
                    buffer, index, sizeof(float));
                }
                else
                {
                    Array.Copy(BitConverter.GetBytes(magCalibration.LowSpeedFactors[factorIdx % 4]), 0,
                    buffer, index, sizeof(float));
                }

                factorIdx++;
                index += sizeof(float);
            }

            return buffer;
        }
    }
}
