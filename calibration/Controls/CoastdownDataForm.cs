using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace IRT.Calibration.Controls
{
    public partial class CoastdownDataForm : Form
    {
        public CoastdownDataForm(CoastdownData data)
        {
            InitializeComponent();

            Bind(data);
        }

        private void Bind(CoastdownData data)
        {
            dgvCoastdownData.Columns.Add("Seconds", "Seconds");
            dgvCoastdownData.Columns.Add("Speed", "Speed (mps)");
            dgvCoastdownData.Columns.Add("Distance", "Meters");
            dgvCoastdownData.Columns.Add("Acceleration", "Acceleration mps^2");

            int records = data.SpeedMps.Length;

            for (int i = 0; i < records; i++)
            {
                dgvCoastdownData.Rows.Add(
                    data.CoastdownSeconds[i],
                    data.SpeedMps[i],
                    data.Distance[i],
                    data.Acceleration[i]);
            }
        }
    }
}
