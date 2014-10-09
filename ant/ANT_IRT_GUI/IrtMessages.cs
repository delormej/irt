using ANT_Managed_Library;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IRT_GUI.IrtMessages
{
    public class ExtraInfoMessage : Message
    {
        public const byte Page = 0x24;

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

    public enum ResistanceMode : byte
    {
        Percent = 0x40,
        Standard = 0x41,
        Erg = 0x42,
        Sim = 0x43             
    }
}
