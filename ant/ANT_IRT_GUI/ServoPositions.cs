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
    public partial class ServoPositions : Form
    {
        ushort min = 0, max = 0;
        List<Position> m_positions;
        BindingSource m_source = null;
        const string INVALID_POSITION = "Position must be a valid number between {0} and {1}";
        const string INVALID_POSISTION_FIRST = INVALID_POSITION + ".\nFirst position must be {0}.";

        public event EventHandler SetPositions;

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

            this.Load += ServoPositions_Load;

            txtMinPosition.Text = min.ToString();
            txtMaxPosition.Text = max.ToString();
            //lblInstructions.Text += min.ToString();

            if (admin)
            {
                txtMinPosition.Enabled = true;
                txtMaxPosition.Enabled = true;
                txtMinPosition.DataBindings.Add("Text", this, "Min");
                txtMaxPosition.DataBindings.Add("Text", this, "Max");
            }

            m_positions = new List<Position>();
            m_positions.Add(new Position(min));
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
            numResistancePositions.Value = m_positions.Count() - 1;
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
    }
}
