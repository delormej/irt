namespace BikeSignalProcessing.View
{
    partial class DataView
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
            this.dgvSegments = new System.Windows.Forms.DataGridView();
            this.dgvMagnetFit = new System.Windows.Forms.DataGridView();
            ((System.ComponentModel.ISupportInitialize)(this.dgvSegments)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgvMagnetFit)).BeginInit();
            this.SuspendLayout();
            // 
            // dgvSegments
            // 
            this.dgvSegments.AllowUserToAddRows = false;
            this.dgvSegments.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgvSegments.Dock = System.Windows.Forms.DockStyle.Top;
            this.dgvSegments.Location = new System.Drawing.Point(0, 0);
            this.dgvSegments.Name = "dgvSegments";
            this.dgvSegments.Size = new System.Drawing.Size(698, 260);
            this.dgvSegments.TabIndex = 0;
            // 
            // dgvMagnetFit
            // 
            this.dgvMagnetFit.AllowUserToAddRows = false;
            this.dgvMagnetFit.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgvMagnetFit.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dgvMagnetFit.Location = new System.Drawing.Point(0, 292);
            this.dgvMagnetFit.Name = "dgvMagnetFit";
            this.dgvMagnetFit.Size = new System.Drawing.Size(698, 263);
            this.dgvMagnetFit.TabIndex = 1;
            // 
            // DataView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(698, 555);
            this.Controls.Add(this.dgvMagnetFit);
            this.Controls.Add(this.dgvSegments);
            this.Name = "DataView";
            this.Text = "DataView";
            ((System.ComponentModel.ISupportInitialize)(this.dgvSegments)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgvMagnetFit)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataGridView dgvSegments;
        private System.Windows.Forms.DataGridView dgvMagnetFit;
    }
}