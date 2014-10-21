using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace IRT_GUI
{
    public class Position
    {
        public ushort Value { get; private set; }

        public Position(ushort value)
        {
            this.Value = value;
        }

        public static implicit operator Position(int value)
        {
            return new Position(value);
        }
    }

    public partial class ServoPositions : Form
    {
        public ServoPositions()
        {
            InitializeComponent();

            var positions = new List<Position>();
            positions.Add(2000);
            positions.Add(1300);
            positions.Add(1200);
            positions.Add(1100);
            positions.Add(1000);
            positions.Add(900);
            positions.Add(800);

            dgResistancePositions.DataSource = positions;
            dgResistancePositions.DataMember = "Value";

            numResistancePositions.Value = positions.Count();

        }

        private void ServoPositions_Load(object sender, EventArgs e)
        {

        }

        private void btnSetServoPositions_Click(object sender, EventArgs e)
        {
            var x = dgResistancePositions.DataSource;
            System.Diagnostics.Debug.WriteLine(x);
        }
    }
}
