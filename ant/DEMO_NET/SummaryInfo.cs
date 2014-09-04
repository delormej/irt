using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;

namespace ANT_Console
{
    public class SummaryInfo
    {
        public string IrtConsoleVersion
        {
            get { return this.GetType().Assembly.GetName().Version.ToString(3); }
        }
        public int EmotionDeviceId { get; set; }
        public uint EmotionSerialNo { get; set; }
        public string EmotionFirmwareVersion { get; set; }
        public int EmotionModel { get; set; }
        public int EmotionHWRev { get; set; }
        public int RefPowerModel {get; set; } 
        public int RefPowerManfId {get; set; }
        public int TotalWeight { get; set; }
        public uint Crr { get; set; }
        public int Settings { get; set; }

        public SummaryInfo()
        { }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine(); // blank
            sb.AppendLine("|| SUMMARY_INFO ||");

            var properties = this.GetType().GetProperties(
                BindingFlags.Public | BindingFlags.Instance);

            foreach (var prop in properties)
            {
                sb.AppendFormat("{0},{1}\r\n", prop.Name, prop.GetValue(this, null));
            }

            return sb.ToString();
        }

        // Methods to parse from messages
    }
}
