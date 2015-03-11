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
            m_ergMode = erg;
            BindGridColumns();
            Load();

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
        }

        private void button1_Click(object sender, EventArgs e)
        {
        }

        private void Load()
        {
            chart1.Series.Clear();
            Series series = chart1.Series.Add("ErgTarget");
            series.ChartType = SeriesChartType.Line;

            BindingSource bs = new BindingSource();

            foreach (var step in m_ergMode.Steps)
            {
                series.Points.AddXY(step.ElapsedStart, step.Watts);
                series.Points.AddXY(step.ElapsedEnd, step.Watts);
                bs.Add(step);
            }

            dataGridView1.DataSource = bs;

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
            List<ResistanceStep> steps = new List<ResistanceStep>();

            foreach (DataGridViewRow row in this.dataGridView1.Rows)
            {
                ResistanceStep step = row.DataBoundItem as ResistanceStep;
                if (step != null)
                {
                    steps.Add(step);
                }
            }

            Parser.WriteOuput("output.txt", 300, steps);
        }
    }
}
