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
            this.btnMagnetCalibrationLoadDefaults = new System.Windows.Forms.Button();
            this.dgvHighSpeed = new System.Windows.Forms.DataGridView();
            this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dgvLowSpeed = new System.Windows.Forms.DataGridView();
            this.dataGridViewTextBoxColumn2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.label6 = new System.Windows.Forms.Label();
            this.btnMagnetCalibrationSet = new System.Windows.Forms.Button();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.txtLowSpeedMph = new System.Windows.Forms.TextBox();
            this.txtHighSpeedMph = new System.Windows.Forms.TextBox();
            ((System.ComponentModel.ISupportInitialize)(this.dgResistancePositions)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numResistancePositions)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.tabPositions.SuspendLayout();
            this.tabCalibration.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dgvHighSpeed)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgvLowSpeed)).BeginInit();
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
            this.tabControl1.Size = new System.Drawing.Size(405, 318);
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
            this.tabPositions.Size = new System.Drawing.Size(397, 292);
            this.tabPositions.TabIndex = 0;
            this.tabPositions.Text = "Positions";
            this.tabPositions.UseVisualStyleBackColor = true;
            // 
            // tabCalibration
            // 
            this.tabCalibration.Controls.Add(this.txtHighSpeedMph);
            this.tabCalibration.Controls.Add(this.txtLowSpeedMph);
            this.tabCalibration.Controls.Add(this.btnMagnetCalibrationLoadDefaults);
            this.tabCalibration.Controls.Add(this.dgvHighSpeed);
            this.tabCalibration.Controls.Add(this.dgvLowSpeed);
            this.tabCalibration.Controls.Add(this.label6);
            this.tabCalibration.Controls.Add(this.btnMagnetCalibrationSet);
            this.tabCalibration.Controls.Add(this.label5);
            this.tabCalibration.Controls.Add(this.label4);
            this.tabCalibration.Location = new System.Drawing.Point(4, 22);
            this.tabCalibration.Margin = new System.Windows.Forms.Padding(2);
            this.tabCalibration.Name = "tabCalibration";
            this.tabCalibration.Padding = new System.Windows.Forms.Padding(2);
            this.tabCalibration.Size = new System.Drawing.Size(397, 292);
            this.tabCalibration.TabIndex = 1;
            this.tabCalibration.Text = "Calibration";
            this.tabCalibration.UseVisualStyleBackColor = true;
            // 
            // btnMagnetCalibrationLoadDefaults
            // 
            this.btnMagnetCalibrationLoadDefaults.Location = new System.Drawing.Point(33, 222);
            this.btnMagnetCalibrationLoadDefaults.Margin = new System.Windows.Forms.Padding(2);
            this.btnMagnetCalibrationLoadDefaults.Name = "btnMagnetCalibrationLoadDefaults";
            this.btnMagnetCalibrationLoadDefaults.Size = new System.Drawing.Size(69, 23);
            this.btnMagnetCalibrationLoadDefaults.TabIndex = 8;
            this.btnMagnetCalibrationLoadDefaults.Text = "Default";
            this.btnMagnetCalibrationLoadDefaults.UseVisualStyleBackColor = true;
            this.btnMagnetCalibrationLoadDefaults.Click += new System.EventHandler(this.btnMagnetCalibrationLoadDefaults_Click);
            // 
            // dgvHighSpeed
            // 
            this.dgvHighSpeed.AllowUserToAddRows = false;
            this.dgvHighSpeed.AllowUserToDeleteRows = false;
            this.dgvHighSpeed.AllowUserToResizeColumns = false;
            this.dgvHighSpeed.AllowUserToResizeRows = false;
            this.dgvHighSpeed.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.dgvHighSpeed.CellBorderStyle = System.Windows.Forms.DataGridViewCellBorderStyle.None;
            this.dgvHighSpeed.ColumnHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.Single;
            this.dgvHighSpeed.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgvHighSpeed.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewTextBoxColumn1});
            this.dgvHighSpeed.Location = new System.Drawing.Point(181, 91);
            this.dgvHighSpeed.Margin = new System.Windows.Forms.Padding(2);
            this.dgvHighSpeed.Name = "dgvHighSpeed";
            this.dgvHighSpeed.RowHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.None;
            this.dgvHighSpeed.RowHeadersVisible = false;
            this.dgvHighSpeed.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            this.dgvHighSpeed.RowTemplate.Height = 18;
            this.dgvHighSpeed.RowTemplate.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.dgvHighSpeed.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.dgvHighSpeed.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.CellSelect;
            this.dgvHighSpeed.Size = new System.Drawing.Size(118, 108);
            this.dgvHighSpeed.TabIndex = 7;
            // 
            // dataGridViewTextBoxColumn1
            // 
            this.dataGridViewTextBoxColumn1.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.dataGridViewTextBoxColumn1.HeaderText = "Factor";
            this.dataGridViewTextBoxColumn1.MaxInputLength = 10;
            this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
            this.dataGridViewTextBoxColumn1.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            // 
            // dgvLowSpeed
            // 
            this.dgvLowSpeed.AllowUserToAddRows = false;
            this.dgvLowSpeed.AllowUserToDeleteRows = false;
            this.dgvLowSpeed.AllowUserToResizeColumns = false;
            this.dgvLowSpeed.AllowUserToResizeRows = false;
            this.dgvLowSpeed.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.dgvLowSpeed.CellBorderStyle = System.Windows.Forms.DataGridViewCellBorderStyle.None;
            this.dgvLowSpeed.ColumnHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.Single;
            this.dgvLowSpeed.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgvLowSpeed.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewTextBoxColumn2});
            this.dgvLowSpeed.Location = new System.Drawing.Point(33, 91);
            this.dgvLowSpeed.Margin = new System.Windows.Forms.Padding(2);
            this.dgvLowSpeed.Name = "dgvLowSpeed";
            this.dgvLowSpeed.RowHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.None;
            this.dgvLowSpeed.RowHeadersVisible = false;
            this.dgvLowSpeed.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            this.dgvLowSpeed.RowTemplate.Height = 18;
            this.dgvLowSpeed.RowTemplate.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.dgvLowSpeed.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.dgvLowSpeed.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.CellSelect;
            this.dgvLowSpeed.Size = new System.Drawing.Size(118, 108);
            this.dgvLowSpeed.TabIndex = 0;
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
            this.label6.Location = new System.Drawing.Point(30, 18);
            this.label6.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(142, 13);
            this.label6.TabIndex = 6;
            this.label6.Text = "3rd Order Polynomial Factors";
            // 
            // btnMagnetCalibrationSet
            // 
            this.btnMagnetCalibrationSet.Location = new System.Drawing.Point(33, 259);
            this.btnMagnetCalibrationSet.Margin = new System.Windows.Forms.Padding(2);
            this.btnMagnetCalibrationSet.Name = "btnMagnetCalibrationSet";
            this.btnMagnetCalibrationSet.Size = new System.Drawing.Size(69, 23);
            this.btnMagnetCalibrationSet.TabIndex = 4;
            this.btnMagnetCalibrationSet.Text = "Set";
            this.btnMagnetCalibrationSet.UseVisualStyleBackColor = true;
            this.btnMagnetCalibrationSet.Click += new System.EventHandler(this.btnMagnetCalibrationSet_Click);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(179, 47);
            this.label5.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(73, 13);
            this.label5.TabIndex = 3;
            this.label5.Text = "High Speed";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(30, 47);
            this.label4.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(70, 13);
            this.label4.TabIndex = 2;
            this.label4.Text = "Low Speed";
            // 
            // txtLowSpeedMph
            // 
            this.txtLowSpeedMph.Location = new System.Drawing.Point(33, 64);
            this.txtLowSpeedMph.Name = "txtLowSpeedMph";
            this.txtLowSpeedMph.Size = new System.Drawing.Size(100, 20);
            this.txtLowSpeedMph.TabIndex = 9;
            // 
            // txtHighSpeedMph
            // 
            this.txtHighSpeedMph.Location = new System.Drawing.Point(181, 64);
            this.txtHighSpeedMph.Name = "txtHighSpeedMph";
            this.txtHighSpeedMph.Size = new System.Drawing.Size(100, 20);
            this.txtHighSpeedMph.TabIndex = 10;
            // 
            // ServoPositions
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.button1;
            this.ClientSize = new System.Drawing.Size(419, 352);
            this.ControlBox = false;
            this.Controls.Add(this.tabControl1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
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
            ((System.ComponentModel.ISupportInitialize)(this.dgvHighSpeed)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgvLowSpeed)).EndInit();
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
        private System.Windows.Forms.DataGridView dgvLowSpeed;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
        private System.Windows.Forms.DataGridView dgvHighSpeed;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private System.Windows.Forms.Button btnMagnetCalibrationLoadDefaults;
        private System.Windows.Forms.TextBox txtHighSpeedMph;
        private System.Windows.Forms.TextBox txtLowSpeedMph;
    }
}