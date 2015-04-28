using IntervalParser;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace IRT_GUI.Simulation
{
    public partial class ErgForm : Form
    {
        ErgMode m_ergMode;
        public ErgForm(ErgMode erg)
        {
            InitializeComponent();
            AddContextMenu();

            m_ergMode = erg;
            BindGridColumns();
            LoadChart();

            m_ergMode.ResistanceStepChange += erg_ResistanceStepChange;
            m_ergMode.Finished += erg_Finished;
        }

        private void BindGridColumns()
        {
            dataGridView1.AutoGenerateColumns = false;
            colTargetType.DataPropertyName = "Type";
            colDuration.DataPropertyName = "Duration";
            colInstructions.DataPropertyName = "Comments";
            colTargetValue.DataPropertyName = "Watts";

            BindingSource bs = new BindingSource();

            if (m_ergMode.Steps != null)
            {
                foreach (var step in m_ergMode.Steps)
                {
                    bs.Add(step);
                }
            }
            else
            {
                bs.Add(new ResistanceStep() { Duration = 1, Comments = "Default", Type = ResistanceType.Erg, Watts = 100 });
            }

            dataGridView1.DataSource = bs;
        }

        private void button1_Click(object sender, EventArgs e)
        {
        }

        private void LoadChart()
        {
            chart1.Series.Clear();
            Series series = chart1.Series.Add("ErgTarget");
            series.ChartType = SeriesChartType.Line;

            chart1.DataSource = dataGridView1.DataSource;
            series.XValueMember ="ElapsedEnd";
            series.YValueMembers = "Watts";

            chart1.DataBind();
        }

        private void AddContextMenu()
        {
            ContextMenu dgvContextMenu = new ContextMenu();

            dgvContextMenu.MenuItems.Add("Repeat", new EventHandler(Repeat_Click));
            dataGridView1.ContextMenu = dgvContextMenu;
            dataGridView1.MouseDown += dataGridView1_MouseDown;
        }

        void dataGridView1_MouseDown(object sender, MouseEventArgs e)
        {
            switch (e.Button)
            {
                case MouseButtons.Right:
                    if (dataGridView1.SelectedRows.Count > 1)
                    {
                        dataGridView1.ContextMenu.Show(dataGridView1, e.Location);
                    }
                    break;

                default:
                    break;
            }
        }

        private void Repeat_Click(object sender, EventArgs e)
        {
            var bs = dataGridView1.DataSource as BindingSource;

            var steps = from row in dataGridView1.SelectedRows.Cast<DataGridViewRow>()
                    orderby row.Index
                    select row.DataBoundItem as ResistanceStep;

            foreach (var step in steps)
            {
                step.ElapsedStart = 0;
                bs.Add(step);
            }
        }

        void erg_Finished()
        {
            throw new NotImplementedException();
        }

        void erg_ResistanceStepChange(ResistanceStep obj)
        {
            throw new NotImplementedException();
        }


        private void button2_Click(object sender, EventArgs e)
        {
            var steps = new List<ResistanceStep>();

            var source = from row in dataGridView1.Rows.Cast<DataGridViewRow>()
                        orderby row.Index
                        select row.DataBoundItem as ResistanceStep;

            float lastEnd = 0.0f;

            foreach (ResistanceStep step in source)
            {
                if (step != null)
                {
                    step.ElapsedStart = lastEnd;
                    lastEnd = step.ElapsedEnd;
                    steps.Add(step);
                }
            }

            int ftp = int.Parse(txtFtp.Text);

            Parser.WriteOuput("output.txt", ftp, steps);
        }
    }
}
