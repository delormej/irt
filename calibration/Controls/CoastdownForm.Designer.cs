namespace IRT.Calibration
{
    partial class CoastdownForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CoastdownForm));
            this.label2 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.btnApply = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.lblStableSeconds = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.txtStableSpeed = new System.Windows.Forms.TextBox();
            this.txtStableWatts = new System.Windows.Forms.TextBox();
            this.txtDrag = new System.Windows.Forms.TextBox();
            this.txtRR = new System.Windows.Forms.TextBox();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.coastdownToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.evaluateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.recalculateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.resetToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.applyToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.chartCoastdown = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chartCoastdown)).BeginInit();
            this.SuspendLayout();
            // 
            // label2
            // 
            this.label2.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(410, 748);
            this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(50, 20);
            this.label2.TabIndex = 1;
            this.label2.Text = "Drag";
            // 
            // label4
            // 
            this.label4.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(410, 783);
            this.label4.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(167, 20);
            this.label4.TabIndex = 3;
            this.label4.Text = "Rolling Resistance";
            // 
            // btnApply
            // 
            this.btnApply.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.btnApply.Location = new System.Drawing.Point(270, 828);
            this.btnApply.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.btnApply.Name = "btnApply";
            this.btnApply.Size = new System.Drawing.Size(112, 35);
            this.btnApply.TabIndex = 20;
            this.btnApply.Text = "Apply";
            this.btnApply.UseVisualStyleBackColor = true;
            this.btnApply.Click += new System.EventHandler(this.btnApply_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(392, 828);
            this.btnCancel.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(112, 35);
            this.btnCancel.TabIndex = 1;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // label3
            // 
            this.label3.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(18, 748);
            this.label3.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(117, 20);
            this.label3.TabIndex = 10;
            this.label3.Text = "Stable Watts";
            // 
            // lblStableSeconds
            // 
            this.lblStableSeconds.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.lblStableSeconds.AutoSize = true;
            this.lblStableSeconds.Location = new System.Drawing.Point(231, 711);
            this.lblStableSeconds.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.lblStableSeconds.Name = "lblStableSeconds";
            this.lblStableSeconds.Size = new System.Drawing.Size(18, 20);
            this.lblStableSeconds.TabIndex = 9;
            this.lblStableSeconds.Text = "0";
            // 
            // label6
            // 
            this.label6.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(18, 711);
            this.label6.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(140, 20);
            this.label6.TabIndex = 8;
            this.label6.Text = "Stable Seconds";
            // 
            // label5
            // 
            this.label5.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(18, 783);
            this.label5.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(175, 20);
            this.label5.TabIndex = 12;
            this.label5.Text = "Stable Speed (mph)";
            // 
            // txtStableSpeed
            // 
            this.txtStableSpeed.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.txtStableSpeed.Location = new System.Drawing.Point(236, 778);
            this.txtStableSpeed.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.txtStableSpeed.Name = "txtStableSpeed";
            this.txtStableSpeed.Size = new System.Drawing.Size(110, 26);
            this.txtStableSpeed.TabIndex = 14;
            this.txtStableSpeed.Leave += new System.EventHandler(this.txtStableSpeed_Leave);
            // 
            // txtStableWatts
            // 
            this.txtStableWatts.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.txtStableWatts.Location = new System.Drawing.Point(236, 743);
            this.txtStableWatts.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.txtStableWatts.Name = "txtStableWatts";
            this.txtStableWatts.Size = new System.Drawing.Size(110, 26);
            this.txtStableWatts.TabIndex = 13;
            // 
            // txtDrag
            // 
            this.txtDrag.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.txtDrag.Location = new System.Drawing.Point(608, 743);
            this.txtDrag.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.txtDrag.Name = "txtDrag";
            this.txtDrag.ReadOnly = true;
            this.txtDrag.Size = new System.Drawing.Size(110, 26);
            this.txtDrag.TabIndex = 18;
            // 
            // txtRR
            // 
            this.txtRR.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.txtRR.Location = new System.Drawing.Point(608, 778);
            this.txtRR.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.txtRR.Name = "txtRR";
            this.txtRR.ReadOnly = true;
            this.txtRR.Size = new System.Drawing.Size(110, 26);
            this.txtRR.TabIndex = 19;
            // 
            // menuStrip1
            // 
            this.menuStrip1.GripStyle = System.Windows.Forms.ToolStripGripStyle.Visible;
            this.menuStrip1.ImageScalingSize = new System.Drawing.Size(24, 24);
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.coastdownToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Padding = new System.Windows.Forms.Padding(4, 3, 0, 3);
            this.menuStrip1.Size = new System.Drawing.Size(774, 35);
            this.menuStrip1.TabIndex = 22;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(50, 29);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(124, 30);
            this.exitToolStripMenuItem.Text = "E&xit";
            // 
            // coastdownToolStripMenuItem
            // 
            this.coastdownToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.evaluateToolStripMenuItem,
            this.recalculateToolStripMenuItem,
            this.resetToolStripMenuItem,
            this.applyToolStripMenuItem});
            this.coastdownToolStripMenuItem.Name = "coastdownToolStripMenuItem";
            this.coastdownToolStripMenuItem.Size = new System.Drawing.Size(114, 29);
            this.coastdownToolStripMenuItem.Text = "Coastdown";
            // 
            // evaluateToolStripMenuItem
            // 
            this.evaluateToolStripMenuItem.Name = "evaluateToolStripMenuItem";
            this.evaluateToolStripMenuItem.Size = new System.Drawing.Size(183, 30);
            this.evaluateToolStripMenuItem.Text = "&Evaluate";
            // 
            // recalculateToolStripMenuItem
            // 
            this.recalculateToolStripMenuItem.Name = "recalculateToolStripMenuItem";
            this.recalculateToolStripMenuItem.Size = new System.Drawing.Size(183, 30);
            this.recalculateToolStripMenuItem.Text = "&Recalculate";
            // 
            // resetToolStripMenuItem
            // 
            this.resetToolStripMenuItem.Name = "resetToolStripMenuItem";
            this.resetToolStripMenuItem.Size = new System.Drawing.Size(183, 30);
            this.resetToolStripMenuItem.Text = "Re&set";
            // 
            // applyToolStripMenuItem
            // 
            this.applyToolStripMenuItem.Name = "applyToolStripMenuItem";
            this.applyToolStripMenuItem.Size = new System.Drawing.Size(183, 30);
            this.applyToolStripMenuItem.Text = "&Apply";
            // 
            // chartCoastdown
            // 
            this.chartCoastdown.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            chartArea1.Name = "ChartArea1";
            this.chartCoastdown.ChartAreas.Add(chartArea1);
            legend1.Name = "Legend1";
            this.chartCoastdown.Legends.Add(legend1);
            this.chartCoastdown.Location = new System.Drawing.Point(0, 42);
            this.chartCoastdown.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.chartCoastdown.Name = "chartCoastdown";
            series1.ChartArea = "ChartArea1";
            series1.Legend = "Legend1";
            series1.Name = "Series1";
            this.chartCoastdown.Series.Add(series1);
            this.chartCoastdown.Size = new System.Drawing.Size(774, 645);
            this.chartCoastdown.TabIndex = 7;
            this.chartCoastdown.Text = "chart1";
            // 
            // CoastdownForm
            // 
            this.AcceptButton = this.btnApply;
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(774, 882);
            this.Controls.Add(this.chartCoastdown);
            this.Controls.Add(this.txtDrag);
            this.Controls.Add(this.txtRR);
            this.Controls.Add(this.txtStableWatts);
            this.Controls.Add(this.txtStableSpeed);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.lblStableSeconds);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnApply);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.MinimumSize = new System.Drawing.Size(787, 911);
            this.Name = "CoastdownForm";
            this.Text = "Coastdown";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chartCoastdown)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button btnApply;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label lblStableSeconds;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox txtStableSpeed;
        private System.Windows.Forms.TextBox txtStableWatts;
        private System.Windows.Forms.TextBox txtDrag;
        private System.Windows.Forms.TextBox txtRR;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem coastdownToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem evaluateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem recalculateToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem resetToolStripMenuItem;
        private System.Windows.Forms.DataVisualization.Charting.Chart chartCoastdown;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem applyToolStripMenuItem;
    }
}