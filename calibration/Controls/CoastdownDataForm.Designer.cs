namespace IRT.Calibration.Controls
{
    partial class CoastdownDataForm
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
            this.dgvCoastdownData = new System.Windows.Forms.DataGridView();
            ((System.ComponentModel.ISupportInitialize)(this.dgvCoastdownData)).BeginInit();
            this.SuspendLayout();
            // 
            // dgvCoastdownData
            // 
            this.dgvCoastdownData.AllowUserToAddRows = false;
            this.dgvCoastdownData.AllowUserToDeleteRows = false;
            this.dgvCoastdownData.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgvCoastdownData.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dgvCoastdownData.Location = new System.Drawing.Point(0, 0);
            this.dgvCoastdownData.Name = "dgvCoastdownData";
            this.dgvCoastdownData.Size = new System.Drawing.Size(525, 261);
            this.dgvCoastdownData.TabIndex = 0;
            // 
            // CoastdownData
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(525, 261);
            this.Controls.Add(this.dgvCoastdownData);
            this.Name = "CoastdownData";
            this.Text = "CoastdownData";
            ((System.ComponentModel.ISupportInitialize)(this.dgvCoastdownData)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataGridView dgvCoastdownData;
    }
}