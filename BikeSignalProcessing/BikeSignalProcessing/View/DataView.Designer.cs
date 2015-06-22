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
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.closeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            ((System.ComponentModel.ISupportInitialize)(this.dgvSegments)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgvMagnetFit)).BeginInit();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // dgvSegments
            // 
            this.dgvSegments.AllowUserToAddRows = false;
            this.dgvSegments.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgvSegments.Dock = System.Windows.Forms.DockStyle.Top;
            this.dgvSegments.Location = new System.Drawing.Point(0, 35);
            this.dgvSegments.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.dgvSegments.Name = "dgvSegments";
            this.dgvSegments.Size = new System.Drawing.Size(1047, 400);
            this.dgvSegments.TabIndex = 0;
            // 
            // dgvMagnetFit
            // 
            this.dgvMagnetFit.AllowUserToAddRows = false;
            this.dgvMagnetFit.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgvMagnetFit.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dgvMagnetFit.Location = new System.Drawing.Point(0, 495);
            this.dgvMagnetFit.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.dgvMagnetFit.Name = "dgvMagnetFit";
            this.dgvMagnetFit.Size = new System.Drawing.Size(1047, 359);
            this.dgvMagnetFit.TabIndex = 1;
            // 
            // txtSpeedMph
            // 
            this.txtSpeedMph.Location = new System.Drawing.Point(117, 445);
            this.txtSpeedMph.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.txtSpeedMph.Name = "txtSpeedMph";
            this.txtSpeedMph.Size = new System.Drawing.Size(79, 26);
            this.txtSpeedMph.TabIndex = 2;
            this.txtSpeedMph.Text = "15.0";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(8, 450);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(101, 20);
            this.label1.TabIndex = 3;
            this.label1.Text = "Speed (mph)";
            // 
            // txtWatts
            // 
            this.txtWatts.Location = new System.Drawing.Point(286, 445);
            this.txtWatts.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.txtWatts.Name = "txtWatts";
            this.txtWatts.Size = new System.Drawing.Size(79, 26);
            this.txtWatts.TabIndex = 4;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(220, 450);
            this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(51, 20);
            this.label2.TabIndex = 5;
            this.label2.Text = "Watts";
            // 
            // menuStrip1
            // 
            this.menuStrip1.ImageScalingSize = new System.Drawing.Size(24, 24);
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Padding = new System.Windows.Forms.Padding(9, 3, 0, 3);
            this.menuStrip1.Size = new System.Drawing.Size(1047, 35);
            this.menuStrip1.TabIndex = 6;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.saveToolStripMenuItem,
            this.closeToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(50, 29);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(140, 30);
            this.saveToolStripMenuItem.Text = "&Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // closeToolStripMenuItem
            // 
            this.closeToolStripMenuItem.Name = "closeToolStripMenuItem";
            this.closeToolStripMenuItem.Size = new System.Drawing.Size(140, 30);
            this.closeToolStripMenuItem.Text = "&Close";
            // 
            // DataView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1047, 854);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txtWatts);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.txtSpeedMph);
            this.Controls.Add(this.dgvMagnetFit);
            this.Controls.Add(this.dgvSegments);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.Name = "DataView";
            this.Text = "DataView";
            ((System.ComponentModel.ISupportInitialize)(this.dgvSegments)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dgvMagnetFit)).EndInit();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
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
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem closeToolStripMenuItem;
    }
}