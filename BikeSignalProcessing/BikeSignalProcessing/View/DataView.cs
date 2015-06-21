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
        Data mData;

        public DataView(Data data)
        {
            mData = data;

            InitializeComponent();

            dgvSegments.DataSource = mData.StableSegments;
            dgvMagnetFit.DataSource = mData.MagnetFits;

            dgvMagnetFit.RowEnter += DgvMagnetFit_RowEnter;
        }

        private void DgvMagnetFit_RowEnter(object sender, DataGridViewCellEventArgs e)
        {
            var data = mData.MagnetFits;

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

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            
        }
    }
}
