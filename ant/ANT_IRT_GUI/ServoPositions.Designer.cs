﻿namespace IRT_GUI
{
    partial class ServoPositions
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ServoPositions));
            this.dgResistancePositions = new System.Windows.Forms.DataGridView();
            this.Position = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.label1 = new System.Windows.Forms.Label();
            this.numResistancePositions = new System.Windows.Forms.NumericUpDown();
            this.btnSetServoPositions = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.lblInstructions = new System.Windows.Forms.Label();
            this.txtMinPosition = new System.Windows.Forms.TextBox();
            this.txtMaxPosition = new System.Windows.Forms.TextBox();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPositions = new System.Windows.Forms.TabPage();
            this.tabCalibration = new System.Windows.Forms.TabPage();
            this.label8 = new System.Windows.Forms.Label();
            this.lblRootPosition = new System.Windows.Forms.Label();
            this.chartPowerCurve = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.label7 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.txtPosition = new System.Windows.Forms.TextBox();
            this.lblCalculatedWatts = new System.Windows.Forms.Label();
            this.txtSpeed = new System.Windows.Forms.TextBox();
            this.btnLoadMagCalibration = new System.Windows.Forms.Button();
            this.btnMagnetCalibrationLoadDefaults = new System.Windows.Forms.Button();
            this.dgvPolyFactors = new System.Windows.Forms.DataGridView();
            this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.btnMagnetCalibrationSet = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.dgResistancePositions)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numResistancePositions)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.tabPositions.SuspendLayout();
            this.tabCalibration.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chartPowerCurve)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgvPolyFactors)).BeginInit();
            this.SuspendLayout();
            // 
            // dgResistancePositions
            // 
            this.dgResistancePositions.AllowUserToAddRows = false;
            this.dgResistancePositions.AllowUserToDeleteRows = false;
            this.dgResistancePositions.AllowUserToResizeColumns = false;
            this.dgResistancePositions.AllowUserToResizeRows = false;
            this.dgResistancePositions.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.dgResistancePositions.CellBorderStyle = System.Windows.Forms.DataGridViewCellBorderStyle.None;
            this.dgResistancePositions.ColumnHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.Single;
            this.dgResistancePositions.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgResistancePositions.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Position});
            this.dgResistancePositions.Location = new System.Drawing.Point(236, 20);
            this.dgResistancePositions.Margin = new System.Windows.Forms.Padding(2);
            this.dgResistancePositions.Name = "dgResistancePositions";
            this.dgResistancePositions.RowHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.None;
            this.dgResistancePositions.RowHeadersVisible = false;
            this.dgResistancePositions.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            this.dgResistancePositions.RowTemplate.Height = 18;
            this.dgResistancePositions.RowTemplate.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.dgResistancePositions.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.dgResistancePositions.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.CellSelect;
            this.dgResistancePositions.Size = new System.Drawing.Size(118, 207);
            this.dgResistancePositions.TabIndex = 1;
            // 
            // Position
            // 
            this.Position.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.Position.HeaderText = "Position";
            this.Position.MaxInputLength = 4;
            this.Position.Name = "Position";
            this.Position.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(29, 154);
            this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(49, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Positions";
            // 
            // numResistancePositions
            // 
            this.numResistancePositions.Location = new System.Drawing.Point(84, 152);
            this.numResistancePositions.Margin = new System.Windows.Forms.Padding(2);
            this.numResistancePositions.Maximum = new decimal(new int[] {
            9,
            0,
            0,
            0});
            this.numResistancePositions.Name = "numResistancePositions";
            this.numResistancePositions.Size = new System.Drawing.Size(42, 20);
            this.numResistancePositions.TabIndex = 0;
            this.numResistancePositions.Value = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.numResistancePositions.ValueChanged += new System.EventHandler(this.numResistancePositions_ValueChanged);
            // 
            // btnSetServoPositions
            // 
            this.btnSetServoPositions.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnSetServoPositions.Location = new System.Drawing.Point(111, 257);
            this.btnSetServoPositions.Margin = new System.Windows.Forms.Padding(2);
            this.btnSetServoPositions.Name = "btnSetServoPositions";
            this.btnSetServoPositions.Size = new System.Drawing.Size(61, 23);
            this.btnSetServoPositions.TabIndex = 2;
            this.btnSetServoPositions.Text = "OK";
            this.btnSetServoPositions.UseVisualStyleBackColor = true;
            this.btnSetServoPositions.Click += new System.EventHandler(this.btnSetServoPositions_Click);
            // 
            // button1
            // 
            this.button1.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button1.Location = new System.Drawing.Point(217, 257);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(61, 23);
            this.button1.TabIndex = 3;
            this.button1.Text = "Cancel";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(29, 40);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(69, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Magnet OFF:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(29, 66);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(72, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "Magnet MAX:";
            // 
            // lblInstructions
            // 
            this.lblInstructions.AutoSize = true;
            this.lblInstructions.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblInstructions.Location = new System.Drawing.Point(29, 94);
            this.lblInstructions.Name = "lblInstructions";
            this.lblInstructions.Size = new System.Drawing.Size(193, 39);
            this.lblInstructions.TabIndex = 9;
            this.lblInstructions.Text = "Choose up to 9 positions starting\r\nwith a value greater than the \r\nMagnet OFF ";
            // 
            // txtMinPosition
            // 
            this.txtMinPosition.Enabled = false;
            this.txtMinPosition.Location = new System.Drawing.Point(111, 40);
            this.txtMinPosition.Margin = new System.Windows.Forms.Padding(2);
            this.txtMinPosition.Name = "txtMinPosition";
            this.txtMinPosition.Size = new System.Drawing.Size(51, 20);
            this.txtMinPosition.TabIndex = 10;
            // 
            // txtMaxPosition
            // 
            this.txtMaxPosition.Enabled = false;
            this.txtMaxPosition.Location = new System.Drawing.Point(111, 66);
            this.txtMaxPosition.Margin = new System.Windows.Forms.Padding(2);
            this.txtMaxPosition.Name = "txtMaxPosition";
            this.txtMaxPosition.Size = new System.Drawing.Size(51, 20);
            this.txtMaxPosition.TabIndex = 11;
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPositions);
            this.tabControl1.Controls.Add(this.tabCalibration);
            this.tabControl1.Location = new System.Drawing.Point(8, 8);
            this.tabControl1.Margin = new System.Windows.Forms.Padding(2);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(405, 439);
            this.tabControl1.TabIndex = 12;
            // 
            // tabPositions
            // 
            this.tabPositions.Controls.Add(this.label2);
            this.tabPositions.Controls.Add(this.txtMaxPosition);
            this.tabPositions.Controls.Add(this.dgResistancePositions);
            this.tabPositions.Controls.Add(this.txtMinPosition);
            this.tabPositions.Controls.Add(this.label1);
            this.tabPositions.Controls.Add(this.lblInstructions);
            this.tabPositions.Controls.Add(this.numResistancePositions);
            this.tabPositions.Controls.Add(this.label3);
            this.tabPositions.Controls.Add(this.btnSetServoPositions);
            this.tabPositions.Controls.Add(this.button1);
            this.tabPositions.Location = new System.Drawing.Point(4, 22);
            this.tabPositions.Margin = new System.Windows.Forms.Padding(2);
            this.tabPositions.Name = "tabPositions";
            this.tabPositions.Padding = new System.Windows.Forms.Padding(2);
            this.tabPositions.Size = new System.Drawing.Size(397, 413);
            this.tabPositions.TabIndex = 0;
            this.tabPositions.Text = "Positions";
            this.tabPositions.UseVisualStyleBackColor = true;
            // 
            // tabCalibration
            // 
            this.tabCalibration.Controls.Add(this.label8);
            this.tabCalibration.Controls.Add(this.lblRootPosition);
            this.tabCalibration.Controls.Add(this.chartPowerCurve);
            this.tabCalibration.Controls.Add(this.label7);
            this.tabCalibration.Controls.Add(this.label5);
            this.tabCalibration.Controls.Add(this.label4);
            this.tabCalibration.Controls.Add(this.txtPosition);
            this.tabCalibration.Controls.Add(this.lblCalculatedWatts);
            this.tabCalibration.Controls.Add(this.txtSpeed);
            this.tabCalibration.Controls.Add(this.btnLoadMagCalibration);
            this.tabCalibration.Controls.Add(this.btnMagnetCalibrationLoadDefaults);
            this.tabCalibration.Controls.Add(this.dgvPolyFactors);
            this.tabCalibration.Controls.Add(this.btnMagnetCalibrationSet);
            this.tabCalibration.Location = new System.Drawing.Point(4, 22);
            this.tabCalibration.Margin = new System.Windows.Forms.Padding(2);
            this.tabCalibration.Name = "tabCalibration";
            this.tabCalibration.Padding = new System.Windows.Forms.Padding(2);
            this.tabCalibration.Size = new System.Drawing.Size(397, 413);
            this.tabCalibration.TabIndex = 1;
            this.tabCalibration.Text = "Calibration";
            this.tabCalibration.UseVisualStyleBackColor = true;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(195, 148);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(119, 13);
            this.label8.TabIndex = 20;
            this.label8.Text = "Minimum Curve Position";
            // 
            // lblRootPosition
            // 
            this.lblRootPosition.AutoSize = true;
            this.lblRootPosition.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblRootPosition.Location = new System.Drawing.Point(195, 167);
            this.lblRootPosition.Name = "lblRootPosition";
            this.lblRootPosition.Size = new System.Drawing.Size(14, 13);
            this.lblRootPosition.TabIndex = 19;
            this.lblRootPosition.Text = "0";
            // 
            // chartPowerCurve
            // 
            this.chartPowerCurve.Location = new System.Drawing.Point(0, 190);
            this.chartPowerCurve.Name = "chartPowerCurve";
            this.chartPowerCurve.Size = new System.Drawing.Size(394, 191);
            this.chartPowerCurve.TabIndex = 18;
            this.chartPowerCurve.Text = "chart1";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(150, 148);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(35, 13);
            this.label7.TabIndex = 17;
            this.label7.Text = "Watts";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(4, 148);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(68, 13);
            this.label5.TabIndex = 16;
            this.label5.Text = "Speed (Mph)";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(77, 148);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(44, 13);
            this.label4.TabIndex = 15;
            this.label4.Text = "Position";
            // 
            // txtPosition
            // 
            this.txtPosition.Location = new System.Drawing.Point(80, 164);
            this.txtPosition.Name = "txtPosition";
            this.txtPosition.Size = new System.Drawing.Size(59, 20);
            this.txtPosition.TabIndex = 14;
            // 
            // lblCalculatedWatts
            // 
            this.lblCalculatedWatts.AutoSize = true;
            this.lblCalculatedWatts.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblCalculatedWatts.Location = new System.Drawing.Point(150, 167);
            this.lblCalculatedWatts.Name = "lblCalculatedWatts";
            this.lblCalculatedWatts.Size = new System.Drawing.Size(14, 13);
            this.lblCalculatedWatts.TabIndex = 13;
            this.lblCalculatedWatts.Text = "0";
            // 
            // txtSpeed
            // 
            this.txtSpeed.Location = new System.Drawing.Point(7, 164);
            this.txtSpeed.Name = "txtSpeed";
            this.txtSpeed.Size = new System.Drawing.Size(59, 20);
            this.txtSpeed.TabIndex = 12;
            // 
            // btnLoadMagCalibration
            // 
            this.btnLoadMagCalibration.Location = new System.Drawing.Point(246, 386);
            this.btnLoadMagCalibration.Margin = new System.Windows.Forms.Padding(2);
            this.btnLoadMagCalibration.Name = "btnLoadMagCalibration";
            this.btnLoadMagCalibration.Size = new System.Drawing.Size(69, 23);
            this.btnLoadMagCalibration.TabIndex = 11;
            this.btnLoadMagCalibration.Text = "Load";
            this.btnLoadMagCalibration.UseVisualStyleBackColor = true;
            this.btnLoadMagCalibration.Click += new System.EventHandler(this.btnLoadMagCalibration_Click);
            // 
            // btnMagnetCalibrationLoadDefaults
            // 
            this.btnMagnetCalibrationLoadDefaults.Location = new System.Drawing.Point(173, 386);
            this.btnMagnetCalibrationLoadDefaults.Margin = new System.Windows.Forms.Padding(2);
            this.btnMagnetCalibrationLoadDefaults.Name = "btnMagnetCalibrationLoadDefaults";
            this.btnMagnetCalibrationLoadDefaults.Size = new System.Drawing.Size(69, 23);
            this.btnMagnetCalibrationLoadDefaults.TabIndex = 8;
            this.btnMagnetCalibrationLoadDefaults.Text = "Default";
            this.btnMagnetCalibrationLoadDefaults.UseVisualStyleBackColor = true;
            this.btnMagnetCalibrationLoadDefaults.Click += new System.EventHandler(this.btnMagnetCalibrationLoadDefaults_Click);
            // 
            // dgvPolyFactors
            // 
            this.dgvPolyFactors.AllowUserToAddRows = false;
            this.dgvPolyFactors.AllowUserToDeleteRows = false;
            this.dgvPolyFactors.AllowUserToResizeColumns = false;
            this.dgvPolyFactors.AllowUserToResizeRows = false;
            this.dgvPolyFactors.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.dgvPolyFactors.CellBorderStyle = System.Windows.Forms.DataGridViewCellBorderStyle.None;
            this.dgvPolyFactors.ColumnHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.Single;
            this.dgvPolyFactors.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgvPolyFactors.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewTextBoxColumn1});
            this.dgvPolyFactors.Location = new System.Drawing.Point(4, 4);
            this.dgvPolyFactors.Margin = new System.Windows.Forms.Padding(2);
            this.dgvPolyFactors.Name = "dgvPolyFactors";
            this.dgvPolyFactors.RowHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.None;
            this.dgvPolyFactors.RowHeadersVisible = false;
            this.dgvPolyFactors.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            this.dgvPolyFactors.RowTemplate.Height = 18;
            this.dgvPolyFactors.RowTemplate.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.dgvPolyFactors.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.dgvPolyFactors.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.CellSelect;
            this.dgvPolyFactors.Size = new System.Drawing.Size(389, 130);
            this.dgvPolyFactors.TabIndex = 7;
            // 
            // dataGridViewTextBoxColumn1
            // 
            this.dataGridViewTextBoxColumn1.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.dataGridViewTextBoxColumn1.HeaderText = "Factor";
            this.dataGridViewTextBoxColumn1.MaxInputLength = 10;
            this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
            this.dataGridViewTextBoxColumn1.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            // 
            // btnMagnetCalibrationSet
            // 
            this.btnMagnetCalibrationSet.Location = new System.Drawing.Point(100, 386);
            this.btnMagnetCalibrationSet.Margin = new System.Windows.Forms.Padding(2);
            this.btnMagnetCalibrationSet.Name = "btnMagnetCalibrationSet";
            this.btnMagnetCalibrationSet.Size = new System.Drawing.Size(69, 23);
            this.btnMagnetCalibrationSet.TabIndex = 4;
            this.btnMagnetCalibrationSet.Text = "Set";
            this.btnMagnetCalibrationSet.UseVisualStyleBackColor = true;
            this.btnMagnetCalibrationSet.Click += new System.EventHandler(this.btnMagnetCalibrationSet_Click);
            // 
            // ServoPositions
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.CancelButton = this.button1;
            this.ClientSize = new System.Drawing.Size(419, 458);
            this.Controls.Add(this.tabControl1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(2);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ServoPositions";
            this.Text = "Magnet";
            this.Load += new System.EventHandler(this.ServoPositions_Load);
            ((System.ComponentModel.ISupportInitialize)(this.dgResistancePositions)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numResistancePositions)).EndInit();
            this.tabControl1.ResumeLayout(false);
            this.tabPositions.ResumeLayout(false);
            this.tabPositions.PerformLayout();
            this.tabCalibration.ResumeLayout(false);
            this.tabCalibration.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chartPowerCurve)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgvPolyFactors)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataGridView dgResistancePositions;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown numResistancePositions;
        private System.Windows.Forms.DataGridViewTextBoxColumn Position;
        private System.Windows.Forms.Button btnSetServoPositions;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label lblInstructions;
        private System.Windows.Forms.TextBox txtMinPosition;
        private System.Windows.Forms.TextBox txtMaxPosition;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPositions;
        private System.Windows.Forms.TabPage tabCalibration;
        private System.Windows.Forms.Button btnMagnetCalibrationSet;
        private System.Windows.Forms.DataGridView dgvPolyFactors;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private System.Windows.Forms.Button btnMagnetCalibrationLoadDefaults;
        private System.Windows.Forms.Button btnLoadMagCalibration;
        private System.Windows.Forms.TextBox txtPosition;
        private System.Windows.Forms.Label lblCalculatedWatts;
        private System.Windows.Forms.TextBox txtSpeed;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.DataVisualization.Charting.Chart chartPowerCurve;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label lblRootPosition;
    }
}