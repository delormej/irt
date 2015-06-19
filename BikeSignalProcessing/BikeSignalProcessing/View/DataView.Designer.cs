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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DataView));
            this.dgvSegments = new System.Windows.Forms.DataGridView();
            this.dgvMagnetFit = new System.Windows.Forms.DataGridView();
            this.txtSpeedMph = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.txtWatts = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
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
            this.dgvMagnetFit.Location = new System.Drawing.Point(0, 305);
            this.dgvMagnetFit.Name = "dgvMagnetFit";
            this.dgvMagnetFit.Size = new System.Drawing.Size(698, 250);
            this.dgvMagnetFit.TabIndex = 1;
            // 
            // txtSpeedMph
            // 
            this.txtSpeedMph.Location = new System.Drawing.Point(78, 279);
            this.txtSpeedMph.Name = "txtSpeedMph";
            this.txtSpeedMph.Size = new System.Drawing.Size(54, 20);
            this.txtSpeedMph.TabIndex = 2;
            this.txtSpeedMph.Text = "15.0";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(5, 282);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(67, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Speed (mph)";
            // 
            // txtWatts
            // 
            this.txtWatts.Location = new System.Drawing.Point(191, 279);
            this.txtWatts.Name = "txtWatts";
            this.txtWatts.Size = new System.Drawing.Size(54, 20);
            this.txtWatts.TabIndex = 4;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(147, 282);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(35, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Watts";
            // 
            // DataView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(698, 555);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txtWatts);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtSpeedMph);
            this.Controls.Add(this.dgvMagnetFit);
            this.Controls.Add(this.dgvSegments);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "DataView";
            this.Text = "DataView";
            ((System.ComponentModel.ISupportInitialize)(this.dgvSegments)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgvMagnetFit)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.DataGridView dgvSegments;
        private System.Windows.Forms.DataGridView dgvMagnetFit;
        private System.Windows.Forms.TextBox txtSpeedMph;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtWatts;
        private System.Windows.Forms.Label label2;
    }
}