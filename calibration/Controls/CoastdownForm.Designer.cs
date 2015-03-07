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
            this.label2 = new System.Windows.Forms.Label();
            this.lblDrag = new System.Windows.Forms.Label();
            this.lblRR = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.btnApply = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.chartCoastdown = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.lblStableWatts = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.lblStableSeconds = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.lblStableSpeed = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.chartCoastdown)).BeginInit();
            this.SuspendLayout();
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(273, 10);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(34, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Drag";
            // 
            // lblDrag
            // 
            this.lblDrag.AutoSize = true;
            this.lblDrag.Location = new System.Drawing.Point(422, 10);
            this.lblDrag.Name = "lblDrag";
            this.lblDrag.Size = new System.Drawing.Size(13, 13);
            this.lblDrag.TabIndex = 2;
            this.lblDrag.Text = "0";
            // 
            // lblRR
            // 
            this.lblRR.AutoSize = true;
            this.lblRR.Location = new System.Drawing.Point(422, 34);
            this.lblRR.Name = "lblRR";
            this.lblRR.Size = new System.Drawing.Size(13, 13);
            this.lblRR.TabIndex = 4;
            this.lblRR.Text = "0";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(273, 34);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(113, 13);
            this.label4.TabIndex = 3;
            this.label4.Text = "Rolling Resistance";
            // 
            // btnApply
            // 
            this.btnApply.Location = new System.Drawing.Point(276, 68);
            this.btnApply.Name = "btnApply";
            this.btnApply.Size = new System.Drawing.Size(75, 23);
            this.btnApply.TabIndex = 5;
            this.btnApply.Text = "Apply";
            this.btnApply.UseVisualStyleBackColor = true;
            this.btnApply.Click += new System.EventHandler(this.btnApply_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(357, 68);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 6;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // chartCoastdown
            // 
            chartArea1.Name = "ChartArea1";
            this.chartCoastdown.ChartAreas.Add(chartArea1);
            this.chartCoastdown.Dock = System.Windows.Forms.DockStyle.Bottom;
            legend1.Name = "Legend1";
            this.chartCoastdown.Legends.Add(legend1);
            this.chartCoastdown.Location = new System.Drawing.Point(0, 120);
            this.chartCoastdown.Name = "chartCoastdown";
            series1.ChartArea = "ChartArea1";
            series1.Legend = "Legend1";
            series1.Name = "Series1";
            this.chartCoastdown.Series.Add(series1);
            this.chartCoastdown.Size = new System.Drawing.Size(516, 429);
            this.chartCoastdown.TabIndex = 7;
            this.chartCoastdown.Text = "chart1";
            // 
            // lblStableWatts
            // 
            this.lblStableWatts.AutoSize = true;
            this.lblStableWatts.Location = new System.Drawing.Point(162, 34);
            this.lblStableWatts.Name = "lblStableWatts";
            this.lblStableWatts.Size = new System.Drawing.Size(13, 13);
            this.lblStableWatts.TabIndex = 11;
            this.lblStableWatts.Text = "0";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(13, 34);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(80, 13);
            this.label3.TabIndex = 10;
            this.label3.Text = "Stable Watts";
            // 
            // lblStableSeconds
            // 
            this.lblStableSeconds.AutoSize = true;
            this.lblStableSeconds.Location = new System.Drawing.Point(162, 56);
            this.lblStableSeconds.Name = "lblStableSeconds";
            this.lblStableSeconds.Size = new System.Drawing.Size(13, 13);
            this.lblStableSeconds.TabIndex = 9;
            this.lblStableSeconds.Text = "0";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(13, 56);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(96, 13);
            this.label6.TabIndex = 8;
            this.label6.Text = "Stable Seconds";
            // 
            // lblStableSpeed
            // 
            this.lblStableSpeed.AutoSize = true;
            this.lblStableSpeed.Location = new System.Drawing.Point(162, 9);
            this.lblStableSpeed.Name = "lblStableSpeed";
            this.lblStableSpeed.Size = new System.Drawing.Size(13, 13);
            this.lblStableSpeed.TabIndex = 13;
            this.lblStableSpeed.Text = "0";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(13, 9);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(83, 13);
            this.label5.TabIndex = 12;
            this.label5.Text = "Stable Speed";
            // 
            // CoastdownForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(516, 549);
            this.ControlBox = false;
            this.Controls.Add(this.lblStableSpeed);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.lblStableWatts);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.lblStableSeconds);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.chartCoastdown);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnApply);
            this.Controls.Add(this.lblRR);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.lblDrag);
            this.Controls.Add(this.label2);
            this.MaximizeBox = false;
            this.Name = "CoastdownForm";
            this.Text = "Coastdown";
            ((System.ComponentModel.ISupportInitialize)(this.chartCoastdown)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label lblDrag;
        private System.Windows.Forms.Label lblRR;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button btnApply;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.DataVisualization.Charting.Chart chartCoastdown;
        private System.Windows.Forms.Label lblStableWatts;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label lblStableSeconds;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label lblStableSpeed;
        private System.Windows.Forms.Label label5;
    }
}