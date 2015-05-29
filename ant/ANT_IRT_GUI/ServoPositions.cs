using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Message = IRT_GUI.IrtMessages.Message;
using IRT.Calibration;

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

            // Create 4 rows.
            dgvLowSpeed.Rows.Add(4);
            dgvHighSpeed.Rows.Add(4);
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

        private void MagnetCalibrationSet()
        {
            try
            {
                var lowSpeedValues = dgvLowSpeed.Rows.Cast<DataGridViewRow>()
                    .Select(row => float.Parse(row.Cells[0].Value.ToString()));

                var highSpeedValues = dgvHighSpeed.Rows.Cast<DataGridViewRow>()
                    .Select(row => float.Parse(row.Cells[0].Value.ToString()));

                if (SetMagnetCalibration != null)
                {
                    MagnetCalibration calibration = new MagnetCalibration();
                    calibration.LowSpeedMps = 15 * 0.44704f;
                    calibration.HighSpeedMps = 25 * 0.44704f;

                    Array.Copy(lowSpeedValues.ToArray(), 0, calibration.LowSpeedFactors, 
                        0, calibration.LowSpeedFactors.Length);

                    Array.Copy(highSpeedValues.ToArray(), 0, calibration.HighSpeedFactors,
                        0, calibration.HighSpeedFactors.Length);

                    MagnetCalibrationEventArgs e = new MagnetCalibrationEventArgs(calibration);
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
            // Validate that all the values are set properly before calling.

            MagnetCalibrationSet();
        }

        private void btnMagnetCalibrationLoadDefaults_Click(object sender, EventArgs e)
        {
            float[] lowSpeedFactors = {
                1.27516039631e-06f,
                -0.00401345920329f,
                3.58655403892f,
                -645.523540742f };

            float[] highSpeedFactors = {
                2.19872670294e-06f,
                -0.00686992504214f,
                6.03431060782f,
                -998.115074474f };

            txtLowSpeedMph.Text = "15.0";
            txtHighSpeedMph.Text = "25.0";

            for (int i = 0; i < highSpeedFactors.Length; i++)
            {
                dgvLowSpeed.Rows[i].Cells[0].Value = lowSpeedFactors[i];
                dgvHighSpeed.Rows[i].Cells[0].Value = highSpeedFactors[i];
            }
        }
    }
}

