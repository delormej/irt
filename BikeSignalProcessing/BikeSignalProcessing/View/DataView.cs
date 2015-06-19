using BikeSignalProcessing.Model;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace BikeSignalProcessing.View
{
    public partial class DataView : Form
    {
        public DataView(Data data)
        {
            InitializeComponent();

            dgvSegments.DataSource = data.StableSegments;
            dgvMagnetFit.DataSource = data.MagnetFits;

            dgvMagnetFit.RowEnter += DgvMagnetFit_RowEnter;
        }

        private void DgvMagnetFit_RowEnter(object sender, DataGridViewCellEventArgs e)
        {
            Collection<MagnetFit> data = dgvMagnetFit.DataSource as
                Collection<MagnetFit>;

            if (data != null)
            {
                MagnetFit fit = data[e.RowIndex];
                double speedMph;
                double.TryParse(txtSpeedMph.Text, out speedMph);

                if (speedMph > 0)
                {
                    double power = speedMph * fit.Slope + fit.Intercept;
                    txtWatts.Text = power.ToString("N1");
                }
            }
        }
    }
}
