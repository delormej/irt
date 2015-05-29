using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Message = IRT_GUI.IrtMessages.Message;

namespace IRT_GUI
{
    public partial class ServoPositions : Form
    {
        ushort min = 0, max = 0;
        List<Position> m_positions;
        BindingSource m_source = null;
        const string INVALID_POSITION = "Position must be a valid number between {0} and {1}";
        const string INVALID_POSISTION_FIRST = INVALID_POSITION + ".\nFirst position must be {0}.";

        public event EventHandler SetPositions;

        public event MagnetCalibrationEventHandler SetMagnetCalibration;


        public ushort Min { get { return this.min; } set { this.min = value; } }
        public ushort Max { get { return this.max; } set { this.max = value; } }

        public ServoPositions()
        {
            InitializeComponent();
        }

        public ServoPositions(ushort min, ushort max, bool admin) : this()
        {
            this.Min = min;
            this.Max = max;

            txtMinPosition.Text = min.ToString();
            txtMaxPosition.Text = max.ToString();
            //lblInstructions.Text += min.ToString();

            m_positions = new List<Position>();
            m_positions.Add(new Position(min));

            if (admin)
            {
                txtMinPosition.Enabled = true;
                txtMaxPosition.Enabled = true;
                txtMinPosition.DataBindings.Add("Text", this, "Min");
                txtMaxPosition.DataBindings.Add("Text", this, "Max");
            }
        }
        
        public List<Position> Positions
        {
            get { return m_positions; }
        }

        private void ShowError()
        {
            MessageBox.Show(string.Format(INVALID_POSISTION_FIRST, min, max),
                "Invalid Position",
                MessageBoxButtons.OK,
                MessageBoxIcon.Warning);
        }

        private void ServoPositions_Load(object sender, EventArgs e)
        {
            m_source = new BindingSource();
            m_source.DataSource = m_positions;

            dgResistancePositions.DataSource = m_source;
            dgResistancePositions.AutoGenerateColumns = false;
            dgResistancePositions.Columns.Clear();
            
            var col = new DataGridViewTextBoxColumn();
            col.Name = "Position";
            col.DataPropertyName = "Value";
            col.HeaderText = "Position";

            dgResistancePositions.ShowRowErrors = true;

            dgResistancePositions.Columns.Add(col);
            dgResistancePositions.AllowUserToAddRows = false;

            int count = m_positions.Count();
            if (count > 0 && count >= numResistancePositions.Minimum)
            {
                numResistancePositions.Value = count;
            }
            dgResistancePositions.CellValidating += dgResistancePositions_CellValidating;

            // Create 6 rows.
            dgvForce2Pos.Rows.Add(6);
            dgvPos2Force.Rows.Add(6);
        }

        private void btnSetServoPositions_Click(object sender, EventArgs e)
        {
            if (m_source.Count < 1)
            {
                ShowError();
                return;
            }

            foreach(Position p in m_source)
            {
                if (p.Value < max || p.Value > min)
                {
                    ShowError();
                    return;
                }
            }

            // First position set?
            if (Positions[0].Value != min)
            {
                // Insert the HOME position which user cannot change.
                Positions.Insert(0, new Position(min));
            }

            if (SetPositions != null)
            {
                SetPositions(this, EventArgs.Empty);
            }
        }

        private void dgResistancePositions_CellValidating(object sender, DataGridViewCellValidatingEventArgs e)
        {
            if (dgResistancePositions.Columns[e.ColumnIndex].Name == "Position")
            {
                int value = 0;

                if (!int.TryParse(e.FormattedValue.ToString(), out value) ||
                    value < max || value > min)
                {
                    dgResistancePositions.Rows[e.RowIndex].ErrorText = string.Format(INVALID_POSITION, min, max);
                    //ShowError();

                    e.Cancel = true;
                }
            }
        }

        private void numResistancePositions_ValueChanged(object sender, EventArgs e)
        {
            while (numResistancePositions.Value > m_positions.Count())
            {
                m_source.Add(new Position(max));
            }

            while (numResistancePositions.Value < m_positions.Count())
            {
                m_source.RemoveAt(m_positions.Count() - 1);
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void MagnetCalibrationSet(MagnetCalibrationType type)
        {
            DataGridView dgv = null;

            switch (type)
            {
                case MagnetCalibrationType.Force2Position:
                    dgv = dgvForce2Pos;
                    break;
                case MagnetCalibrationType.Position2Force:
                    dgv = dgvPos2Force;
                    break;
            }

            try
            {
                var values = dgv.Rows.Cast<DataGridViewRow>()
                    .Select(row => float.Parse(row.Cells[0].Value.ToString()));

                if (SetMagnetCalibration != null)
                {
                    MagnetCalibration calibration = new MagnetCalibration();
                    calibration.LowSpeedMps = 15 * 0.44704f;
                    calibration.HighSpeedMps = 25 * 0.44704f;

                    Array.Copy(values.ToArray(), 2, calibration.HighSpeedFactors, 
                        0, calibration.HighSpeedFactors.Length);

                    Array.Copy(values.ToArray(), 2, calibration.LowSpeedFactors,
                        0, calibration.LowSpeedFactors.Length);

                    MagnetCalibrationEventArgs e = new MagnetCalibrationEventArgs(type, values.ToArray());
                    e.Calibration = calibration;

                    SetMagnetCalibration(this, e);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Invalid value:" + ex.Message, "Error",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void btnMagnetCalibrationSet_Click(object sender, EventArgs e)
        {
            // Set both.
            //MagnetCalibrationSet(MagnetCalibrationType.Force2Position);
            MagnetCalibrationSet(MagnetCalibrationType.Position2Force);
        }

        private void btnMagnetCalibrationLoadDefaults_Click(object sender, EventArgs e)
        {
            float[] force2Servo = {
                -0.00000000000033469583f,
				+0.00000000202071048200f,
				-0.00000466916875500000f,
				+0.00513145135800000000f,
				-2.691480529f,
				+562.4577135f };

            float[] servo2Force = {
					-0.0000000000012401f,
					+0.0000000067486647f,
					-0.0000141629606351f,
					+0.0142639827784839f,
					-6.92836459712442f,
					+1351.463567618f };

            for (int i = 0; i < 6; i++)
            {
                dgvPos2Force.Rows[i].Cells[0].Value = servo2Force[i];
                dgvForce2Pos.Rows[i].Cells[0].Value = force2Servo[i];
            }
        }
    }

    public delegate void MagnetCalibrationEventHandler(object sender, MagnetCalibrationEventArgs e);

    public class MagnetCalibration
    {
        public MagnetCalibration()
        {
            LowSpeedFactors = new float[4];
            HighSpeedFactors = new float[4];
        }

        public float LowSpeedMps;
        public float HighSpeedMps;

        public float[] LowSpeedFactors;
        public float[] HighSpeedFactors;

        public byte[] GetBytes()
        {
            byte[] buffer = new byte[8 * 5];
            int index = 0;

            buffer[index++] = Message.ANT_BURST_MSG_ID_SET_MAGNET_CA;

            // Convert speeds to uint16_t.
            ushort lowSpeed = (ushort)(LowSpeedMps * 1000);
            ushort highSpeed = (ushort)(HighSpeedMps * 1000);

            Message.LittleEndian(lowSpeed, out buffer[index++], out buffer[index++]);
            Message.LittleEndian(highSpeed, out buffer[index++], out buffer[index++]);

            int factorIdx = 0;

            // Advance 3 places, 4 blank bytes to advance to 2nd message.
            index = 8;

            while (index < buffer.Length)
            {
                if (index >= 24)
                {
                    // We're into page 4 & 5
                    Array.Copy(BitConverter.GetBytes(HighSpeedFactors[factorIdx % 4]), 0,
                    buffer, index, sizeof(float));
                }
                else
                {
                    Array.Copy(BitConverter.GetBytes(LowSpeedFactors[factorIdx % 4]), 0,
                    buffer, index, sizeof(float));
                }

                factorIdx++;
                index += sizeof(float);
            }

            return buffer;
        }
    }

    public enum MagnetCalibrationType : byte
    {
        Force2Position = 0,
        Position2Force = 1
    }

    public class MagnetCalibrationEventArgs : EventArgs
    {
        public float[] Factors;
        public MagnetCalibrationType CalibrationType;
        public MagnetCalibration Calibration;

        public MagnetCalibrationEventArgs(MagnetCalibrationType calibrationType, float[] factors)
        {
            this.CalibrationType = calibrationType;
            this.Factors = factors;
        }
    }

}
