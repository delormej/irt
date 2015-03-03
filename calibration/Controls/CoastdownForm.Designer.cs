namespace IRTCalibration
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
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea3 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend3 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series3 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.lblInstructions = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.lblDrag = new System.Windows.Forms.Label();
            this.lblRR = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.btnApply = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.chartCoastdown = new System.Windows.Forms.DataVisualization.Charting.Chart();
            ((System.ComponentModel.ISupportInitialize)(this.chartCoastdown)).BeginInit();
            this.SuspendLayout();
            // 
            // lblInstructions
            // 
            this.lblInstructions.Font = new System.Drawing.Font("Microsoft Sans Serif", 14F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblInstructions.Location = new System.Drawing.Point(12, 9);
            this.lblInstructions.Name = "lblInstructions";
            this.lblInstructions.Size = new System.Drawing.Size(492, 55);
            this.lblInstructions.TabIndex = 0;
            this.lblInstructions.Text = "In {0} second(s) calibration parameters will be applied.  \r\nClose this dialog or " +
    "begin another coastdown to interrupt.";
            this.lblInstructions.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(13, 78);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(34, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Drag";
            // 
            // lblDrag
            // 
            this.lblDrag.AutoSize = true;
            this.lblDrag.Location = new System.Drawing.Point(162, 78);
            this.lblDrag.Name = "lblDrag";
            this.lblDrag.Size = new System.Drawing.Size(13, 13);
            this.lblDrag.TabIndex = 2;
            this.lblDrag.Text = "0";
            // 
            // lblRR
            // 
            this.lblRR.AutoSize = true;
            this.lblRR.Location = new System.Drawing.Point(162, 102);
            this.lblRR.Name = "lblRR";
            this.lblRR.Size = new System.Drawing.Size(13, 13);
            this.lblRR.TabIndex = 4;
            this.lblRR.Text = "0";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(13, 102);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(113, 13);
            this.label4.TabIndex = 3;
            this.label4.Text = "Rolling Resistance";
            // 
            // btnApply
            // 
            this.btnApply.Location = new System.Drawing.Point(180, 135);
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
            this.btnCancel.Location = new System.Drawing.Point(261, 135);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 6;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // chartCoastdown
            // 
            chartArea3.Name = "ChartArea1";
            this.chartCoastdown.ChartAreas.Add(chartArea3);
            this.chartCoastdown.Dock = System.Windows.Forms.DockStyle.Bottom;
            legend3.Name = "Legend1";
            this.chartCoastdown.Legends.Add(legend3);
            this.chartCoastdown.Location = new System.Drawing.Point(0, 164);
            this.chartCoastdown.Name = "chartCoastdown";
            series3.ChartArea = "ChartArea1";
            series3.Legend = "Legend1";
            series3.Name = "Series1";
            this.chartCoastdown.Series.Add(series3);
            this.chartCoastdown.Size = new System.Drawing.Size(516, 385);
            this.chartCoastdown.TabIndex = 7;
            this.chartCoastdown.Text = "chart1";
            // 
            // CoastdownForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(516, 549);
            this.ControlBox = false;
            this.Controls.Add(this.chartCoastdown);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnApply);
            this.Controls.Add(this.lblRR);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.lblDrag);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.lblInstructions);
            this.MaximizeBox = false;
            this.Name = "CoastdownForm";
            this.Text = "Coastdown";
            ((System.ComponentModel.ISupportInitialize)(this.chartCoastdown)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lblInstructions;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label lblDrag;
        private System.Windows.Forms.Label lblRR;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button btnApply;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.DataVisualization.Charting.Chart chartCoastdown;
    }
}