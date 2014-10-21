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
            this.label1 = new System.Windows.Forms.Label();
            this.numResistancePositions = new System.Windows.Forms.NumericUpDown();
            this.Position = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.btnSetServoPositions = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.dgResistancePositions)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numResistancePositions)).BeginInit();
            this.SuspendLayout();
            // 
            // dgResistancePositions
            // 
            this.dgResistancePositions.AllowUserToResizeColumns = false;
            this.dgResistancePositions.AllowUserToResizeRows = false;
            this.dgResistancePositions.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.dgResistancePositions.CellBorderStyle = System.Windows.Forms.DataGridViewCellBorderStyle.None;
            this.dgResistancePositions.ColumnHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.Single;
            this.dgResistancePositions.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgResistancePositions.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Position});
            this.dgResistancePositions.Location = new System.Drawing.Point(37, 53);
            this.dgResistancePositions.Name = "dgResistancePositions";
            this.dgResistancePositions.RowHeadersVisible = false;
            this.dgResistancePositions.RowTemplate.Height = 28;
            this.dgResistancePositions.Size = new System.Drawing.Size(142, 285);
            this.dgResistancePositions.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(33, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(73, 20);
            this.label1.TabIndex = 1;
            this.label1.Text = "Positions";
            // 
            // numResistancePositions
            // 
            this.numResistancePositions.Location = new System.Drawing.Point(116, 17);
            this.numResistancePositions.Maximum = new decimal(new int[] {
            9,
            0,
            0,
            0});
            this.numResistancePositions.Minimum = new decimal(new int[] {
            2,
            0,
            0,
            0});
            this.numResistancePositions.Name = "numResistancePositions";
            this.numResistancePositions.Size = new System.Drawing.Size(63, 26);
            this.numResistancePositions.TabIndex = 2;
            this.numResistancePositions.Value = new decimal(new int[] {
            2,
            0,
            0,
            0});
            // 
            // Position
            // 
            this.Position.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.Position.HeaderText = "Position";
            this.Position.MaxInputLength = 4;
            this.Position.Name = "Position";
            this.Position.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            // 
            // btnSetServoPositions
            // 
            this.btnSetServoPositions.Location = new System.Drawing.Point(248, 150);
            this.btnSetServoPositions.Name = "btnSetServoPositions";
            this.btnSetServoPositions.Size = new System.Drawing.Size(75, 36);
            this.btnSetServoPositions.TabIndex = 3;
            this.btnSetServoPositions.Text = "Set";
            this.btnSetServoPositions.UseVisualStyleBackColor = true;
            this.btnSetServoPositions.Click += new System.EventHandler(this.btnSetServoPositions_Click);
            // 
            // ServoPositions
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(455, 350);
            this.Controls.Add(this.btnSetServoPositions);
            this.Controls.Add(this.numResistancePositions);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.dgResistancePositions);
            this.Name = "ServoPositions";
            this.Text = "ServoPositions";
            this.Load += new System.EventHandler(this.ServoPositions_Load);
            ((System.ComponentModel.ISupportInitialize)(this.dgResistancePositions)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numResistancePositions)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.DataGridView dgResistancePositions;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown numResistancePositions;
        private System.Windows.Forms.DataGridViewTextBoxColumn Position;
        private System.Windows.Forms.Button btnSetServoPositions;
    }
}