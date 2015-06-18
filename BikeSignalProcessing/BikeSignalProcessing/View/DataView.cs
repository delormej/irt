using BikeSignalProcessing.Model;
using System;
using System.Collections.Generic;
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
        }
    }
}
