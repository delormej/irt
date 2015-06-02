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

            dgvPolyFactors.Columns.Clear();
            dgvPolyFactors.Columns.Add("lowSpeed", "Low");
            dgvPolyFactors.Columns.Add("highSpeed", "High");
            dgvPolyFactors.RowHeadersVisible = true;

            // Add rows to the grid.
            for (int i = 0; i < 5; i++)
            {
                var row = new DataGridViewRow();
                string header = string.Empty;

                if (i == 0)
                {
                    header = "Speed (Mph)";
                }
                else
                {
                    header = i.ToString();
                }

                row.HeaderCell.Value = header;
                dgvPolyFactors.Rows.Add(row);
            }

            dgvPolyFactors.RowHeadersWidth = 110;

            txtPosition.LostFocus += TxtPosition_LostFocus;

        }

        private void TxtPosition_LostFocus(object sender, EventArgs e)
        {
            int position;
            float speedMph;
            if (int.TryParse(txtPosition.Text, out position) && 
                float.TryParse(txtSpeed.Text, out speedMph))
            {
                MagnetCalibration calibration = GetMagnetCalibration();
                float watts = calibration.MagnetWatts(speedMph * 0.44704f, position);
                lblCalculatedWatts.Text = watts.ToString();
            }
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

        private MagnetCalibration GetMagnetCalibration()
        {
            var lowSpeedValues = GetPolyFactors(Factor.Low);
            var highSpeedValues = GetPolyFactors(Factor.High);

            MagnetCalibration calibration = new MagnetCalibration();
            calibration.LowSpeedMps = lowSpeedValues[0] * 0.44704f;
            calibration.HighSpeedMps = highSpeedValues[0] * 0.44704f;

            Array.Copy(lowSpeedValues, 1, calibration.LowSpeedFactors,
                0, calibration.LowSpeedFactors.Length);

            Array.Copy(highSpeedValues, 1, calibration.HighSpeedFactors,
                0, calibration.HighSpeedFactors.Length);

            return calibration;
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

        private enum Factor { Low, High };

        private float[] GetPolyFactors(Factor factor)
        {
            var values = dgvPolyFactors.Rows.Cast<DataGridViewRow>()
                .Select(row => float.Parse(row.Cells[(int)factor].Value.ToString()));

            return values.ToArray<float>();
        }

        private void MagnetCalibrationSet()
        {
            try
            {
                if (SetMagnetCalibration != null)
                {
                    MagnetCalibration calibration = GetMagnetCalibration();
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

        private void btnLoadMagCalibration_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            //dlg.InitialDirectory = m_lastPath;
            dlg.Filter = "Mag Calibration (*.csv)|*.csv|All files (*.*)|*.*";
            dlg.FilterIndex = 1;
            dlg.RestoreDirectory = false;
            dlg.CheckFileExists = true;
            dlg.Multiselect = false;

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                List<MagnetPosition> positions = MagnetPosition.GetMagnetPositions(dlg.FileName);
                if (positions != null && positions.Count > 0)
                {
                    ShowMagnetCalibration(positions);
                }
            }
        }

        private void btnMagnetCalibrationLoadDefaults_Click(object sender, EventArgs e)
        {
            List<MagnetPosition> positions = MagnetPosition.GetMagnetPositions();
            ShowMagnetCalibration(positions);
        }

        private void ShowMagnetCalibration(List<MagnetPosition> positions)
        {
            float lowSpeedMph = 15;
            float highSpeedMph = 25;

            
            MagnetCalibration magCalibration = new MagnetCalibration();
            float[] lowSpeedFactors = magCalibration.Fit(lowSpeedMph * 0.44704f, positions);
            float[] highSpeedFactors = magCalibration.Fit(highSpeedMph * 0.44704f, positions);

            dgvPolyFactors.Rows[0].Cells[0].Value = lowSpeedMph.ToString("0.0");
            dgvPolyFactors.Rows[0].Cells[1].Value = highSpeedMph.ToString("0.0");

            for (int i = 0; i < highSpeedFactors.Length; i++)
            {
                dgvPolyFactors.Rows[i+1].Cells[0].Value = lowSpeedFactors[i];
                dgvPolyFactors.Rows[i+1].Cells[1].Value = highSpeedFactors[i];
            }
        }
    }
}

