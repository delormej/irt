namespace IRT_GUI
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
            this.dgvPos2Force = new System.Windows.Forms.DataGridView();
            this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dgvForce2Pos = new System.Windows.Forms.DataGridView();
            this.dataGridViewTextBoxColumn2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.label6 = new System.Windows.Forms.Label();
            this.btnMagnetCalibrationSet = new System.Windows.Forms.Button();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.dgResistancePositions)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numResistancePositions)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.tabPositions.SuspendLayout();
            this.tabCalibration.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dgvPos2Force)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgvForce2Pos)).BeginInit();
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
            this.dgResistancePositions.Location = new System.Drawing.Point(354, 31);
            this.dgResistancePositions.Name = "dgResistancePositions";
            this.dgResistancePositions.RowHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.None;
            this.dgResistancePositions.RowHeadersVisible = false;
            this.dgResistancePositions.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            this.dgResistancePositions.RowTemplate.Height = 18;
            this.dgResistancePositions.RowTemplate.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.dgResistancePositions.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.dgResistancePositions.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.CellSelect;
            this.dgResistancePositions.Size = new System.Drawing.Size(177, 318);
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
            this.label1.Location = new System.Drawing.Point(44, 237);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(73, 20);
            this.label1.TabIndex = 1;
            this.label1.Text = "Positions";
            // 
            // numResistancePositions
            // 
            this.numResistancePositions.Location = new System.Drawing.Point(126, 234);
            this.numResistancePositions.Maximum = new decimal(new int[] {
            9,
            0,
            0,
            0});
            this.numResistancePositions.Name = "numResistancePositions";
            this.numResistancePositions.Size = new System.Drawing.Size(63, 26);
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
            this.btnSetServoPositions.Location = new System.Drawing.Point(166, 395);
            this.btnSetServoPositions.Name = "btnSetServoPositions";
            this.btnSetServoPositions.Size = new System.Drawing.Size(92, 35);
            this.btnSetServoPositions.TabIndex = 2;
            this.btnSetServoPositions.Text = "OK";
            this.btnSetServoPositions.UseVisualStyleBackColor = true;
            this.btnSetServoPositions.Click += new System.EventHandler(this.btnSetServoPositions_Click);
            // 
            // button1
            // 
            this.button1.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button1.Location = new System.Drawing.Point(326, 395);
            this.button1.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(92, 35);
            this.button1.TabIndex = 3;
            this.button1.Text = "Cancel";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(44, 62);
            this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(103, 20);
            this.label2.TabIndex = 5;
            this.label2.Text = "Magnet OFF:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(44, 101);
            this.label3.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(106, 20);
            this.label3.TabIndex = 6;
            this.label3.Text = "Magnet MAX:";
            // 
            // lblInstructions
            // 
            this.lblInstructions.AutoSize = true;
            this.lblInstructions.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblInstructions.Location = new System.Drawing.Point(44, 145);
            this.lblInstructions.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.lblInstructions.Name = "lblInstructions";
            this.lblInstructions.Size = new System.Drawing.Size(288, 60);
            this.lblInstructions.TabIndex = 9;
            this.lblInstructions.Text = "Choose up to 9 positions starting\r\nwith a value greater than the \r\nMagnet OFF ";
            // 
            // txtMinPosition
            // 
            this.txtMinPosition.Enabled = false;
            this.txtMinPosition.Location = new System.Drawing.Point(166, 62);
            this.txtMinPosition.Name = "txtMinPosition";
            this.txtMinPosition.Size = new System.Drawing.Size(74, 26);
            this.txtMinPosition.TabIndex = 10;
            // 
            // txtMaxPosition
            // 
            this.txtMaxPosition.Enabled = false;
            this.txtMaxPosition.Location = new System.Drawing.Point(166, 101);
            this.txtMaxPosition.Name = "txtMaxPosition";
            this.txtMaxPosition.Size = new System.Drawing.Size(74, 26);
            this.txtMaxPosition.TabIndex = 11;
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPositions);
            this.tabControl1.Controls.Add(this.tabCalibration);
            this.tabControl1.Location = new System.Drawing.Point(12, 12);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(608, 490);
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
            this.tabPositions.Location = new System.Drawing.Point(4, 29);
            this.tabPositions.Name = "tabPositions";
            this.tabPositions.Padding = new System.Windows.Forms.Padding(3);
            this.tabPositions.Size = new System.Drawing.Size(600, 457);
            this.tabPositions.TabIndex = 0;
            this.tabPositions.Text = "Positions";
            this.tabPositions.UseVisualStyleBackColor = true;
            // 
            // tabCalibration
            // 
            this.tabCalibration.Controls.Add(this.dgvPos2Force);
            this.tabCalibration.Controls.Add(this.dgvForce2Pos);
            this.tabCalibration.Controls.Add(this.label6);
            this.tabCalibration.Controls.Add(this.btnMagnetCalibrationSet);
            this.tabCalibration.Controls.Add(this.label5);
            this.tabCalibration.Controls.Add(this.label4);
            this.tabCalibration.Location = new System.Drawing.Point(4, 29);
            this.tabCalibration.Name = "tabCalibration";
            this.tabCalibration.Padding = new System.Windows.Forms.Padding(3);
            this.tabCalibration.Size = new System.Drawing.Size(600, 457);
            this.tabCalibration.TabIndex = 1;
            this.tabCalibration.Text = "Calibration";
            this.tabCalibration.UseVisualStyleBackColor = true;
            // 
            // dgvPos2Force
            // 
            this.dgvPos2Force.AllowUserToAddRows = false;
            this.dgvPos2Force.AllowUserToDeleteRows = false;
            this.dgvPos2Force.AllowUserToResizeColumns = false;
            this.dgvPos2Force.AllowUserToResizeRows = false;
            this.dgvPos2Force.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.dgvPos2Force.CellBorderStyle = System.Windows.Forms.DataGridViewCellBorderStyle.None;
            this.dgvPos2Force.ColumnHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.Single;
            this.dgvPos2Force.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgvPos2Force.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewTextBoxColumn1});
            this.dgvPos2Force.Location = new System.Drawing.Point(272, 96);
            this.dgvPos2Force.Name = "dgvPos2Force";
            this.dgvPos2Force.RowHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.None;
            this.dgvPos2Force.RowHeadersVisible = false;
            this.dgvPos2Force.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            this.dgvPos2Force.RowTemplate.Height = 18;
            this.dgvPos2Force.RowTemplate.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.dgvPos2Force.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.dgvPos2Force.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.CellSelect;
            this.dgvPos2Force.Size = new System.Drawing.Size(177, 211);
            this.dgvPos2Force.TabIndex = 7;
            // 
            // dataGridViewTextBoxColumn1
            // 
            this.dataGridViewTextBoxColumn1.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.dataGridViewTextBoxColumn1.HeaderText = "Factor";
            this.dataGridViewTextBoxColumn1.MaxInputLength = 10;
            this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
            this.dataGridViewTextBoxColumn1.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            // 
            // dgvForce2Pos
            // 
            this.dgvForce2Pos.AllowUserToAddRows = false;
            this.dgvForce2Pos.AllowUserToDeleteRows = false;
            this.dgvForce2Pos.AllowUserToResizeColumns = false;
            this.dgvForce2Pos.AllowUserToResizeRows = false;
            this.dgvForce2Pos.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.dgvForce2Pos.CellBorderStyle = System.Windows.Forms.DataGridViewCellBorderStyle.None;
            this.dgvForce2Pos.ColumnHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.Single;
            this.dgvForce2Pos.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgvForce2Pos.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewTextBoxColumn2});
            this.dgvForce2Pos.Location = new System.Drawing.Point(49, 96);
            this.dgvForce2Pos.Name = "dgvForce2Pos";
            this.dgvForce2Pos.RowHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.None;
            this.dgvForce2Pos.RowHeadersVisible = false;
            this.dgvForce2Pos.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            this.dgvForce2Pos.RowTemplate.Height = 18;
            this.dgvForce2Pos.RowTemplate.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.dgvForce2Pos.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.dgvForce2Pos.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.CellSelect;
            this.dgvForce2Pos.Size = new System.Drawing.Size(177, 211);
            this.dgvForce2Pos.TabIndex = 0;
            // 
            // dataGridViewTextBoxColumn2
            // 
            this.dataGridViewTextBoxColumn2.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.dataGridViewTextBoxColumn2.HeaderText = "Factor";
            this.dataGridViewTextBoxColumn2.MaxInputLength = 10;
            this.dataGridViewTextBoxColumn2.Name = "dataGridViewTextBoxColumn2";
            this.dataGridViewTextBoxColumn2.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(45, 28);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(213, 20);
            this.label6.TabIndex = 6;
            this.label6.Text = "5th Order Polynomial Factors";
            // 
            // btnMagnetCalibrationSet
            // 
            this.btnMagnetCalibrationSet.Location = new System.Drawing.Point(49, 399);
            this.btnMagnetCalibrationSet.Name = "btnMagnetCalibrationSet";
            this.btnMagnetCalibrationSet.Size = new System.Drawing.Size(75, 35);
            this.btnMagnetCalibrationSet.TabIndex = 4;
            this.btnMagnetCalibrationSet.Text = "Set";
            this.btnMagnetCalibrationSet.UseVisualStyleBackColor = true;
            this.btnMagnetCalibrationSet.Click += new System.EventHandler(this.btnMagnetCalibrationSet_Click);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(268, 73);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(145, 20);
            this.label5.TabIndex = 3;
            this.label5.Text = "Position to Force";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(45, 73);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(145, 20);
            this.label4.TabIndex = 2;
            this.label4.Text = "Force to Position";
            // 
            // ServoPositions
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.button1;
            this.ClientSize = new System.Drawing.Size(629, 541);
            this.ControlBox = false;
            this.Controls.Add(this.tabControl1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
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
            ((System.ComponentModel.ISupportInitialize)(this.dgvPos2Force)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgvForce2Pos)).EndInit();
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
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.DataGridView dgvForce2Pos;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
        private System.Windows.Forms.DataGridView dgvPos2Force;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
    }
}