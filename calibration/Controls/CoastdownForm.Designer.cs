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
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend2 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.label2 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.btnApply = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.chartCoastdown = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.label3 = new System.Windows.Forms.Label();
            this.lblStableSeconds = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.txtStableSpeed = new System.Windows.Forms.TextBox();
            this.txtStableWatts = new System.Windows.Forms.TextBox();
            this.btnRecalc = new System.Windows.Forms.Button();
            this.txtDrag = new System.Windows.Forms.TextBox();
            this.txtRR = new System.Windows.Forms.TextBox();
            this.btnCalcWatts = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.chartCoastdown)).BeginInit();
            this.SuspendLayout();
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(273, 34);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(34, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Drag";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(273, 57);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(113, 13);
            this.label4.TabIndex = 3;
            this.label4.Text = "Rolling Resistance";
            // 
            // btnApply
            // 
            this.btnApply.Location = new System.Drawing.Point(299, 80);
            this.btnApply.Name = "btnApply";
            this.btnApply.Size = new System.Drawing.Size(75, 23);
            this.btnApply.TabIndex = 17;
            this.btnApply.Text = "Apply";
            this.btnApply.UseVisualStyleBackColor = true;
            this.btnApply.Click += new System.EventHandler(this.btnApply_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(380, 80);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 1;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // chartCoastdown
            // 
            chartArea2.Name = "ChartArea1";
            this.chartCoastdown.ChartAreas.Add(chartArea2);
            this.chartCoastdown.Dock = System.Windows.Forms.DockStyle.Bottom;
            legend2.Name = "Legend1";
            this.chartCoastdown.Legends.Add(legend2);
            this.chartCoastdown.Location = new System.Drawing.Point(0, 120);
            this.chartCoastdown.Name = "chartCoastdown";
            series2.ChartArea = "ChartArea1";
            series2.Legend = "Legend1";
            series2.Name = "Series1";
            this.chartCoastdown.Series.Add(series2);
            this.chartCoastdown.Size = new System.Drawing.Size(516, 429);
            this.chartCoastdown.TabIndex = 7;
            this.chartCoastdown.Text = "chart1";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(12, 34);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(80, 13);
            this.label3.TabIndex = 10;
            this.label3.Text = "Stable Watts";
            // 
            // lblStableSeconds
            // 
            this.lblStableSeconds.AutoSize = true;
            this.lblStableSeconds.Location = new System.Drawing.Point(154, 10);
            this.lblStableSeconds.Name = "lblStableSeconds";
            this.lblStableSeconds.Size = new System.Drawing.Size(13, 13);
            this.lblStableSeconds.TabIndex = 9;
            this.lblStableSeconds.Text = "0";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(12, 10);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(96, 13);
            this.label6.TabIndex = 8;
            this.label6.Text = "Stable Seconds";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(12, 57);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(118, 13);
            this.label5.TabIndex = 12;
            this.label5.Text = "Stable Speed (mph)";
            // 
            // txtStableSpeed
            // 
            this.txtStableSpeed.Location = new System.Drawing.Point(157, 54);
            this.txtStableSpeed.Name = "txtStableSpeed";
            this.txtStableSpeed.Size = new System.Drawing.Size(75, 20);
            this.txtStableSpeed.TabIndex = 14;
            // 
            // txtStableWatts
            // 
            this.txtStableWatts.Location = new System.Drawing.Point(157, 31);
            this.txtStableWatts.Name = "txtStableWatts";
            this.txtStableWatts.Size = new System.Drawing.Size(75, 20);
            this.txtStableWatts.TabIndex = 13;
            // 
            // btnRecalc
            // 
            this.btnRecalc.Location = new System.Drawing.Point(134, 80);
            this.btnRecalc.Name = "btnRecalc";
            this.btnRecalc.Size = new System.Drawing.Size(75, 23);
            this.btnRecalc.TabIndex = 16;
            this.btnRecalc.Text = "Recalculate";
            this.btnRecalc.UseVisualStyleBackColor = true;
            this.btnRecalc.Click += new System.EventHandler(this.btnRecalc_Click);
            // 
            // txtDrag
            // 
            this.txtDrag.Location = new System.Drawing.Point(405, 31);
            this.txtDrag.Name = "txtDrag";
            this.txtDrag.ReadOnly = true;
            this.txtDrag.Size = new System.Drawing.Size(75, 20);
            this.txtDrag.TabIndex = 18;
            // 
            // txtRR
            // 
            this.txtRR.Location = new System.Drawing.Point(405, 54);
            this.txtRR.Name = "txtRR";
            this.txtRR.ReadOnly = true;
            this.txtRR.Size = new System.Drawing.Size(75, 20);
            this.txtRR.TabIndex = 19;
            // 
            // btnCalcWatts
            // 
            this.btnCalcWatts.Location = new System.Drawing.Point(53, 80);
            this.btnCalcWatts.Name = "btnCalcWatts";
            this.btnCalcWatts.Size = new System.Drawing.Size(75, 23);
            this.btnCalcWatts.TabIndex = 15;
            this.btnCalcWatts.Text = "Test";
            this.btnCalcWatts.UseVisualStyleBackColor = true;
            this.btnCalcWatts.Click += new System.EventHandler(this.btnCalcWatts_Click);
            // 
            // CoastdownForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(516, 549);
            this.ControlBox = false;
            this.Controls.Add(this.btnCalcWatts);
            this.Controls.Add(this.txtDrag);
            this.Controls.Add(this.txtRR);
            this.Controls.Add(this.btnRecalc);
            this.Controls.Add(this.txtStableWatts);
            this.Controls.Add(this.txtStableSpeed);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.lblStableSeconds);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.chartCoastdown);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnApply);
            this.Controls.Add(this.label4);
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
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button btnApply;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.DataVisualization.Charting.Chart chartCoastdown;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label lblStableSeconds;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox txtStableSpeed;
        private System.Windows.Forms.TextBox txtStableWatts;
        private System.Windows.Forms.Button btnRecalc;
        private System.Windows.Forms.TextBox txtDrag;
        private System.Windows.Forms.TextBox txtRR;
        private System.Windows.Forms.Button btnCalcWatts;
    }
}