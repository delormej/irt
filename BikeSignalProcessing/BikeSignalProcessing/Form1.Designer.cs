namespace BikeSignalProcessing
{
    partial class Form1
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
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend2 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.button1 = new System.Windows.Forms.Button();
            this.chart1 = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.button3 = new System.Windows.Forms.Button();
            this.upDownThreshold = new System.Windows.Forms.NumericUpDown();
            this.btnBest = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.upDownMinWindow = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).BeginInit();
            this.contextMenuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.upDownThreshold)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.upDownMinWindow)).BeginInit();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(16, 19);
            this.button1.Margin = new System.Windows.Forms.Padding(2);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(69, 20);
            this.button1.TabIndex = 0;
            this.button1.Text = "Load";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // chart1
            // 
            chartArea2.Name = "ChartArea1";
            this.chart1.ChartAreas.Add(chartArea2);
            this.chart1.Dock = System.Windows.Forms.DockStyle.Fill;
            legend2.Name = "Legend1";
            this.chart1.Legends.Add(legend2);
            this.chart1.Location = new System.Drawing.Point(0, 0);
            this.chart1.Margin = new System.Windows.Forms.Padding(2);
            this.chart1.Name = "chart1";
            series2.ChartArea = "ChartArea1";
            series2.Legend = "Legend1";
            series2.Name = "Series1";
            this.chart1.Series.Add(series2);
            this.chart1.Size = new System.Drawing.Size(661, 428);
            this.chart1.TabIndex = 1;
            this.chart1.Text = "chart1";
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.ImageScalingSize = new System.Drawing.Size(24, 24);
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(93, 26);
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(92, 22);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(103, 22);
            this.openToolStripMenuItem.Text = "Open";
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(16, 68);
            this.button3.Margin = new System.Windows.Forms.Padding(2);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(69, 20);
            this.button3.TabIndex = 3;
            this.button3.Text = "Reset";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // upDownThreshold
            // 
            this.upDownThreshold.DecimalPlaces = 1;
            this.upDownThreshold.Increment = new decimal(new int[] {
            5,
            0,
            0,
            65536});
            this.upDownThreshold.Location = new System.Drawing.Point(16, 90);
            this.upDownThreshold.Margin = new System.Windows.Forms.Padding(2);
            this.upDownThreshold.Maximum = new decimal(new int[] {
            50,
            0,
            0,
            0});
            this.upDownThreshold.Name = "upDownThreshold";
            this.upDownThreshold.Size = new System.Drawing.Size(46, 20);
            this.upDownThreshold.TabIndex = 4;
            this.upDownThreshold.ValueChanged += new System.EventHandler(this.upDownThreshold_ValueChanged);
            // 
            // btnBest
            // 
            this.btnBest.Location = new System.Drawing.Point(16, 43);
            this.btnBest.Margin = new System.Windows.Forms.Padding(2);
            this.btnBest.Name = "btnBest";
            this.btnBest.Size = new System.Drawing.Size(69, 20);
            this.btnBest.TabIndex = 5;
            this.btnBest.Text = "Best";
            this.btnBest.UseVisualStyleBackColor = true;
            this.btnBest.Click += new System.EventHandler(this.btnBest_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.SystemColors.Window;
            this.label1.Location = new System.Drawing.Point(66, 92);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(43, 13);
            this.label1.TabIndex = 6;
            this.label1.Text = "StdDev";
            // 
            // upDownMinWindow
            // 
            this.upDownMinWindow.Location = new System.Drawing.Point(16, 114);
            this.upDownMinWindow.Margin = new System.Windows.Forms.Padding(2);
            this.upDownMinWindow.Maximum = new decimal(new int[] {
            500,
            0,
            0,
            0});
            this.upDownMinWindow.Minimum = new decimal(new int[] {
            9,
            0,
            0,
            0});
            this.upDownMinWindow.Name = "upDownMinWindow";
            this.upDownMinWindow.Size = new System.Drawing.Size(46, 20);
            this.upDownMinWindow.TabIndex = 7;
            this.upDownMinWindow.Value = new decimal(new int[] {
            9,
            0,
            0,
            0});
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.BackColor = System.Drawing.SystemColors.Window;
            this.label2.Location = new System.Drawing.Point(66, 116);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(46, 13);
            this.label2.TabIndex = 8;
            this.label2.Text = "Window";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(661, 428);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.upDownMinWindow);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnBest);
            this.Controls.Add(this.upDownThreshold);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.chart1);
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "Form1";
            this.Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).EndInit();
            this.contextMenuStrip1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.upDownThreshold)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.upDownMinWindow)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart1;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.NumericUpDown upDownThreshold;
        private System.Windows.Forms.Button btnBest;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown upDownMinWindow;
        private System.Windows.Forms.Label label2;
    }
}

