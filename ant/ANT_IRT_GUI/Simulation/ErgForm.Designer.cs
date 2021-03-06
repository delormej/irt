﻿namespace IRT_GUI.Simulation
{
    partial class ErgForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.btnExport = new System.Windows.Forms.Button();
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.colTargetType = new System.Windows.Forms.DataGridViewComboBoxColumn();
            this.colDuration = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colTargetValue = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.colInstructions = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataSet1 = new System.Data.DataSet();
            this.chart1 = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.txtFtp = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataSet1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).BeginInit();
            this.SuspendLayout();
            // 
            // btnExport
            // 
            this.btnExport.Location = new System.Drawing.Point(237, 527);
            this.btnExport.Name = "btnExport";
            this.btnExport.Size = new System.Drawing.Size(75, 23);
            this.btnExport.TabIndex = 1;
            this.btnExport.Text = "Export";
            this.btnExport.UseVisualStyleBackColor = true;
            this.btnExport.Click += new System.EventHandler(this.button2_Click);
            // 
            // dataGridView1
            // 
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.colTargetType,
            this.colDuration,
            this.colTargetValue,
            this.colInstructions});
            this.dataGridView1.Location = new System.Drawing.Point(2, 211);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.Size = new System.Drawing.Size(544, 310);
            this.dataGridView1.TabIndex = 2;
            // 
            // colTargetType
            // 
            this.colTargetType.HeaderText = "Type";
            this.colTargetType.Items.AddRange(new object[] {
            "Erg",
            "Level",
            "Servo Position"});
            this.colTargetType.MaxDropDownItems = 3;
            this.colTargetType.Name = "colTargetType";
            this.colTargetType.Width = 50;
            // 
            // colDuration
            // 
            this.colDuration.HeaderText = "Duration (Mins)";
            this.colDuration.Name = "colDuration";
            this.colDuration.Width = 50;
            // 
            // colTargetValue
            // 
            this.colTargetValue.HeaderText = "Target";
            this.colTargetValue.Name = "colTargetValue";
            this.colTargetValue.Width = 50;
            // 
            // colInstructions
            // 
            this.colInstructions.HeaderText = "Instructions";
            this.colInstructions.Name = "colInstructions";
            this.colInstructions.Width = 350;
            // 
            // dataSet1
            // 
            this.dataSet1.DataSetName = "NewDataSet";
            // 
            // chart1
            // 
            chartArea1.Name = "ChartArea1";
            this.chart1.ChartAreas.Add(chartArea1);
            legend1.Name = "Legend1";
            this.chart1.Legends.Add(legend1);
            this.chart1.Location = new System.Drawing.Point(2, 0);
            this.chart1.Name = "chart1";
            series1.ChartArea = "ChartArea1";
            series1.Legend = "Legend1";
            series1.Name = "Series1";
            this.chart1.Series.Add(series1);
            this.chart1.Size = new System.Drawing.Size(544, 205);
            this.chart1.TabIndex = 3;
            this.chart1.Text = "chart1";
            // 
            // txtFtp
            // 
            this.txtFtp.Location = new System.Drawing.Point(122, 530);
            this.txtFtp.Name = "txtFtp";
            this.txtFtp.Size = new System.Drawing.Size(34, 20);
            this.txtFtp.TabIndex = 4;
            this.txtFtp.Text = "295";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(81, 537);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(27, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "FTP";
            // 
            // ErgForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(549, 562);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtFtp);
            this.Controls.Add(this.chart1);
            this.Controls.Add(this.dataGridView1);
            this.Controls.Add(this.btnExport);
            this.Name = "ErgForm";
            this.Text = "Erg Mode";
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataSet1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnExport;
        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Data.DataSet dataSet1;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart1;
        private System.Windows.Forms.DataGridViewComboBoxColumn colTargetType;
        private System.Windows.Forms.DataGridViewTextBoxColumn colDuration;
        private System.Windows.Forms.DataGridViewTextBoxColumn colTargetValue;
        private System.Windows.Forms.DataGridViewTextBoxColumn colInstructions;
        private System.Windows.Forms.TextBox txtFtp;
        private System.Windows.Forms.Label label1;
    }
}

