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

        List<Position> m_positions = new List<Position>();
        BindingSource m_source = null;
        const string INVALID_POSITION = "Position must be a valid number between 700 and 2000";

        public ServoPositions()
        {
            InitializeComponent();

            m_positions.Add(2000);
            m_positions.Add(1400);
            m_positions.Add(1300);
            m_positions.Add(1200);
            m_positions.Add(1100);
            m_positions.Add(1000);
            m_positions.Add(900);
            //m_positions.Add(800);
            //m_positions.Add(850);
            //m_positions.Add(700);

            m_source = new BindingSource();
            m_source.DataSource = m_positions;
            dgResistancePositions.DataSource = m_source;
            dgResistancePositions.AllowUserToAddRows = false;
            numResistancePositions.Value = m_positions.Count();
        }

        private void ShowError()
        {
            MessageBox.Show(INVALID_POSITION,
                "Invalid Position",
                MessageBoxButtons.OK,
                MessageBoxIcon.Warning);
        }

        private void ServoPositions_Load(object sender, EventArgs e)
        {

        }

        private void btnSetServoPositions_Click(object sender, EventArgs e)
        {
            var x = dgResistancePositions.DataSource;
            System.Diagnostics.Debug.WriteLine(x);

            foreach(Position p in m_source)
            {
                if (p.Value < 700 || p.Value > 2000)
                {
                    ShowError();
                    return;
                }
            }
        }

        private void dgResistancePositions_CellValidating(object sender, DataGridViewCellValidatingEventArgs e)
        {
            if (dgResistancePositions.Columns[e.ColumnIndex].Name == "Value")
            {
                int value = 0;

                if (!int.TryParse(e.FormattedValue.ToString(), out value) ||
                    value < 700 || value > 2000)
                {
                    dgResistancePositions.Rows[e.RowIndex].ErrorText = INVALID_POSITION;
                    ShowError();
                        
                    e.Cancel = true;
                }
            }
        }

        private void numResistancePositions_ValueChanged(object sender, EventArgs e)
        {
            if (numResistancePositions.Value > m_positions.Count())
            {
                while (numResistancePositions.Value > m_positions.Count())
                {
                    //m_positions.Add(0);
                    m_source.Add(new Position(0));
                }
            }
            else if (numResistancePositions.Value < m_positions.Count())
            {
                while (numResistancePositions.Value < m_positions.Count())
                {
                    //m_positions.RemoveAt(m_positions.Count() - 1);
                    m_source.RemoveAt(m_positions.Count() - 1);
                }
            }
        }
    }
}
