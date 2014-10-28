using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IRT_GUI
{
    public class Position
    {
        public ushort Value { get; set; }

        public Position(ushort value)
        {
            this.Value = value;
        }

        public static implicit operator Position(int value)
        {
            return new Position((ushort)value);
        }
    }
}
