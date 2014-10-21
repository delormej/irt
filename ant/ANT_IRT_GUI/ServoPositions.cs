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

        private void dgResistancePositions_CellValidating(object sender, DataGridViewCellValidatingEventArgs e)
        {
            if (dgResistancePositions.Columns[e.ColumnIndex].Name == "Value")
            {
                int value = 0;

                if (!int.TryParse(e.FormattedValue.ToString(), out value) ||
                    value < 700 || value > 2000)
                {
                    string error = "Position must be a valid number between 700 and 2000";
                    dgResistancePositions.Rows[e.RowIndex].ErrorText = error;
                    MessageBox.Show(error,
                        "Invalid Position",
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Warning);
                        
                    e.Cancel = true;
                }
            }
        }
    }
}
