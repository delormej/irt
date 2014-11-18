namespace IRT_GUI
{
    partial class frmIrtGui
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmIrtGui));
            this.txtLog = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label27 = new System.Windows.Forms.Label();
            this.lblSpeedAvg = new System.Windows.Forms.Label();
            this.lblFlywheel = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.lblEmrWattsAvg = new System.Windows.Forms.Label();
            this.label29 = new System.Windows.Forms.Label();
            this.btnDfuEnable = new System.Windows.Forms.Button();
            this.chkCharge = new System.Windows.Forms.CheckBox();
            this.btnServoOffset = new System.Windows.Forms.Button();
            this.txtServoOffset = new System.Windows.Forms.TextBox();
            this.label28 = new System.Windows.Forms.Label();
            this.label18 = new System.Windows.Forms.Label();
            this.lblEmrMph = new System.Windows.Forms.Label();
            this.lblEmrBattTime = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.lblEmrBattVolt = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.lblEmrWatts = new System.Windows.Forms.Label();
            this.btnEmrSearch = new System.Windows.Forms.Button();
            this.lblEmrModel = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.lblEmrHardwareRev = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.lblEmrFirmwareRev = new System.Windows.Forms.Label();
            this.lblEmrSerialNo = new System.Windows.Forms.Label();
            this.txtEmrDeviceId = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.lblRefPwrModel = new System.Windows.Forms.Label();
            this.lblRefPwrManuf = new System.Windows.Forms.Label();
            this.txtRefPwrDeviceId = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.progressSimRefPower = new System.Windows.Forms.ProgressBar();
            this.btnSimRefPower = new System.Windows.Forms.Button();
            this.btnChartOpen = new System.Windows.Forms.Button();
            this.label15 = new System.Windows.Forms.Label();
            this.numMovingAvgSec = new System.Windows.Forms.NumericUpDown();
            this.lblRefPwrWattsAvg = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.lblRefPwrWatts = new System.Windows.Forms.Label();
            this.label19 = new System.Windows.Forms.Label();
            this.btnRefPwrSearch = new System.Windows.Forms.Button();
            this.lblRefPwrType = new System.Windows.Forms.Label();
            this.label222 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.txtSlope = new System.Windows.Forms.TextBox();
            this.txtOffset = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.btnCalibrationSet = new System.Windows.Forms.Button();
            this.btnCalibrationGet = new System.Windows.Forms.Button();
            this.groupBoxResistance = new System.Windows.Forms.GroupBox();
            this.btnSetResistancePositions = new System.Windows.Forms.Button();
            this.pnlResistanceStd = new System.Windows.Forms.Panel();
            this.lblResistanceStdLevel = new System.Windows.Forms.Label();
            this.btnResistanceDec = new System.Windows.Forms.Button();
            this.btnResistanceInc = new System.Windows.Forms.Button();
            this.pnlResistancePercent = new System.Windows.Forms.Panel();
            this.trackBarResistancePct = new System.Windows.Forms.TrackBar();
            this.txtResistancePercent = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.pnlErg = new System.Windows.Forms.Panel();
            this.txtResistanceErgWatts = new System.Windows.Forms.TextBox();
            this.label20 = new System.Windows.Forms.Label();
            this.pnlResistanceSim = new System.Windows.Forms.Panel();
            this.txtSimWind = new System.Windows.Forms.TextBox();
            this.label25 = new System.Windows.Forms.Label();
            this.txtSimCrr = new System.Windows.Forms.TextBox();
            this.label24 = new System.Windows.Forms.Label();
            this.txtSimSlope = new System.Windows.Forms.TextBox();
            this.label23 = new System.Windows.Forms.Label();
            this.btnResistanceSet = new System.Windows.Forms.Button();
            this.txtServoPos = new System.Windows.Forms.TextBox();
            this.cmbResistanceMode = new System.Windows.Forms.ComboBox();
            this.label22 = new System.Windows.Forms.Label();
            this.label26 = new System.Windows.Forms.Label();
            this.txtTotalWeight = new System.Windows.Forms.TextBox();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.btnSettingsGet = new System.Windows.Forms.Button();
            this.btnSettingsSet = new System.Windows.Forms.Button();
            this.label21 = new System.Windows.Forms.Label();
            this.txtWheelSizeMm = new System.Windows.Forms.TextBox();
            this.chkLstSettings = new System.Windows.Forms.CheckedListBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.lblFeatures = new System.Windows.Forms.Label();
            this.cmbParamGet = new System.Windows.Forms.ComboBox();
            this.label31 = new System.Windows.Forms.Label();
            this.txtParamSet = new System.Windows.Forms.TextBox();
            this.btnParamGet = new System.Windows.Forms.Button();
            this.btnParamSet = new System.Windows.Forms.Button();
            this.statusStrip = new System.Windows.Forms.StatusStrip();
            this.lblStatus = new System.Windows.Forms.ToolStripStatusLabel();
            this.btnPauseSimRefPower = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numMovingAvgSec)).BeginInit();
            this.groupBox3.SuspendLayout();
            this.groupBoxResistance.SuspendLayout();
            this.pnlResistanceStd.SuspendLayout();
            this.pnlResistancePercent.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBarResistancePct)).BeginInit();
            this.pnlErg.SuspendLayout();
            this.pnlResistanceSim.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.statusStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // txtLog
            // 
            this.txtLog.Location = new System.Drawing.Point(13, 364);
            this.txtLog.Multiline = true;
            this.txtLog.Name = "txtLog";
            this.txtLog.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtLog.Size = new System.Drawing.Size(759, 159);
            this.txtLog.TabIndex = 0;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label27);
            this.groupBox1.Controls.Add(this.lblSpeedAvg);
            this.groupBox1.Controls.Add(this.lblFlywheel);
            this.groupBox1.Controls.Add(this.label13);
            this.groupBox1.Controls.Add(this.lblEmrWattsAvg);
            this.groupBox1.Controls.Add(this.label29);
            this.groupBox1.Controls.Add(this.btnDfuEnable);
            this.groupBox1.Controls.Add(this.chkCharge);
            this.groupBox1.Controls.Add(this.btnServoOffset);
            this.groupBox1.Controls.Add(this.txtServoOffset);
            this.groupBox1.Controls.Add(this.label28);
            this.groupBox1.Controls.Add(this.label18);
            this.groupBox1.Controls.Add(this.lblEmrMph);
            this.groupBox1.Controls.Add(this.lblEmrBattTime);
            this.groupBox1.Controls.Add(this.label10);
            this.groupBox1.Controls.Add(this.label11);
            this.groupBox1.Controls.Add(this.lblEmrBattVolt);
            this.groupBox1.Controls.Add(this.label16);
            this.groupBox1.Controls.Add(this.lblEmrWatts);
            this.groupBox1.Controls.Add(this.btnEmrSearch);
            this.groupBox1.Controls.Add(this.lblEmrModel);
            this.groupBox1.Controls.Add(this.label9);
            this.groupBox1.Controls.Add(this.lblEmrHardwareRev);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Controls.Add(this.lblEmrFirmwareRev);
            this.groupBox1.Controls.Add(this.lblEmrSerialNo);
            this.groupBox1.Controls.Add(this.txtEmrDeviceId);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(346, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(211, 342);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "E-Motion Rollers";
            // 
            // label27
            // 
            this.label27.AutoSize = true;
            this.label27.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label27.Location = new System.Drawing.Point(31, 314);
            this.label27.Name = "label27";
            this.label27.Size = new System.Drawing.Size(54, 13);
            this.label27.TabIndex = 36;
            this.label27.Text = "Average";
            // 
            // lblSpeedAvg
            // 
            this.lblSpeedAvg.AutoSize = true;
            this.lblSpeedAvg.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblSpeedAvg.Location = new System.Drawing.Point(20, 277);
            this.lblSpeedAvg.Name = "lblSpeedAvg";
            this.lblSpeedAvg.Size = new System.Drawing.Size(80, 37);
            this.lblSpeedAvg.TabIndex = 35;
            this.lblSpeedAvg.Text = "00.0";
            this.lblSpeedAvg.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lblFlywheel
            // 
            this.lblFlywheel.AutoSize = true;
            this.lblFlywheel.Location = new System.Drawing.Point(108, 178);
            this.lblFlywheel.Name = "lblFlywheel";
            this.lblFlywheel.Size = new System.Drawing.Size(13, 13);
            this.lblFlywheel.TabIndex = 34;
            this.lblFlywheel.Text = "..";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label13.Location = new System.Drawing.Point(129, 316);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(54, 13);
            this.label13.TabIndex = 32;
            this.label13.Text = "Average";
            // 
            // lblEmrWattsAvg
            // 
            this.lblEmrWattsAvg.AutoSize = true;
            this.lblEmrWattsAvg.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblEmrWattsAvg.Location = new System.Drawing.Point(121, 279);
            this.lblEmrWattsAvg.Name = "lblEmrWattsAvg";
            this.lblEmrWattsAvg.Size = new System.Drawing.Size(71, 37);
            this.lblEmrWattsAvg.TabIndex = 30;
            this.lblEmrWattsAvg.Text = "000";
            this.lblEmrWattsAvg.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label29
            // 
            this.label29.AutoSize = true;
            this.label29.Location = new System.Drawing.Point(7, 174);
            this.label29.Name = "label29";
            this.label29.Size = new System.Drawing.Size(48, 13);
            this.label29.TabIndex = 33;
            this.label29.Text = "Flywheel";
            // 
            // btnDfuEnable
            // 
            this.btnDfuEnable.Location = new System.Drawing.Point(154, 59);
            this.btnDfuEnable.Name = "btnDfuEnable";
            this.btnDfuEnable.Size = new System.Drawing.Size(51, 23);
            this.btnDfuEnable.TabIndex = 28;
            this.btnDfuEnable.Text = "DFU";
            this.btnDfuEnable.UseVisualStyleBackColor = true;
            this.btnDfuEnable.Click += new System.EventHandler(this.btnDfuEnable_Click);
            // 
            // chkCharge
            // 
            this.chkCharge.AutoSize = true;
            this.chkCharge.Enabled = false;
            this.chkCharge.Location = new System.Drawing.Point(135, 121);
            this.chkCharge.Name = "chkCharge";
            this.chkCharge.Size = new System.Drawing.Size(68, 17);
            this.chkCharge.TabIndex = 27;
            this.chkCharge.Text = "Charging";
            this.chkCharge.UseVisualStyleBackColor = true;
            this.chkCharge.CheckedChanged += new System.EventHandler(this.chkCharge_CheckedChanged);
            // 
            // btnServoOffset
            // 
            this.btnServoOffset.Location = new System.Drawing.Point(154, 153);
            this.btnServoOffset.Name = "btnServoOffset";
            this.btnServoOffset.Size = new System.Drawing.Size(51, 23);
            this.btnServoOffset.TabIndex = 26;
            this.btnServoOffset.Text = "Set";
            this.btnServoOffset.UseVisualStyleBackColor = true;
            this.btnServoOffset.Visible = false;
            this.btnServoOffset.Click += new System.EventHandler(this.btnServoOffset_Click);
            // 
            // txtServoOffset
            // 
            this.txtServoOffset.Enabled = false;
            this.txtServoOffset.Location = new System.Drawing.Point(111, 155);
            this.txtServoOffset.Name = "txtServoOffset";
            this.txtServoOffset.Size = new System.Drawing.Size(37, 20);
            this.txtServoOffset.TabIndex = 17;
            this.txtServoOffset.Text = "0";
            // 
            // label28
            // 
            this.label28.AutoSize = true;
            this.label28.Location = new System.Drawing.Point(7, 156);
            this.label28.Name = "label28";
            this.label28.Size = new System.Drawing.Size(66, 13);
            this.label28.TabIndex = 15;
            this.label28.Text = "Servo Offset";
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label18.Location = new System.Drawing.Point(39, 253);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(34, 13);
            this.label18.TabIndex = 13;
            this.label18.Text = "MPH";
            // 
            // lblEmrMph
            // 
            this.lblEmrMph.AutoSize = true;
            this.lblEmrMph.Font = new System.Drawing.Font("Microsoft Sans Serif", 32F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblEmrMph.Location = new System.Drawing.Point(4, 196);
            this.lblEmrMph.Name = "lblEmrMph";
            this.lblEmrMph.Size = new System.Drawing.Size(106, 51);
            this.lblEmrMph.TabIndex = 12;
            this.lblEmrMph.Text = "00.0";
            this.lblEmrMph.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lblEmrBattTime
            // 
            this.lblEmrBattTime.AutoSize = true;
            this.lblEmrBattTime.Location = new System.Drawing.Point(108, 139);
            this.lblEmrBattTime.Name = "lblEmrBattTime";
            this.lblEmrBattTime.Size = new System.Drawing.Size(13, 13);
            this.lblEmrBattTime.TabIndex = 13;
            this.lblEmrBattTime.Text = "..";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(7, 119);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(79, 13);
            this.label10.TabIndex = 10;
            this.label10.Text = "Battery Voltage";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(7, 137);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(79, 13);
            this.label11.TabIndex = 12;
            this.label11.Text = "Operating Time";
            // 
            // lblEmrBattVolt
            // 
            this.lblEmrBattVolt.AutoSize = true;
            this.lblEmrBattVolt.Location = new System.Drawing.Point(108, 121);
            this.lblEmrBattVolt.Name = "lblEmrBattVolt";
            this.lblEmrBattVolt.Size = new System.Drawing.Size(13, 13);
            this.lblEmrBattVolt.TabIndex = 11;
            this.lblEmrBattVolt.Text = "..";
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label16.Location = new System.Drawing.Point(134, 253);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(40, 13);
            this.label16.TabIndex = 11;
            this.label16.Text = "Watts";
            // 
            // lblEmrWatts
            // 
            this.lblEmrWatts.AutoSize = true;
            this.lblEmrWatts.Font = new System.Drawing.Font("Microsoft Sans Serif", 32F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblEmrWatts.Location = new System.Drawing.Point(109, 196);
            this.lblEmrWatts.Name = "lblEmrWatts";
            this.lblEmrWatts.Size = new System.Drawing.Size(94, 51);
            this.lblEmrWatts.TabIndex = 10;
            this.lblEmrWatts.Text = "000";
            this.lblEmrWatts.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // btnEmrSearch
            // 
            this.btnEmrSearch.Location = new System.Drawing.Point(154, 16);
            this.btnEmrSearch.Name = "btnEmrSearch";
            this.btnEmrSearch.Size = new System.Drawing.Size(51, 23);
            this.btnEmrSearch.TabIndex = 9;
            this.btnEmrSearch.Text = "Search";
            this.btnEmrSearch.UseVisualStyleBackColor = true;
            this.btnEmrSearch.Click += new System.EventHandler(this.btnEmrSearch_Click);
            // 
            // lblEmrModel
            // 
            this.lblEmrModel.AutoSize = true;
            this.lblEmrModel.Location = new System.Drawing.Point(108, 104);
            this.lblEmrModel.Name = "lblEmrModel";
            this.lblEmrModel.Size = new System.Drawing.Size(13, 13);
            this.lblEmrModel.TabIndex = 9;
            this.lblEmrModel.Text = "..";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(7, 102);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(36, 13);
            this.label9.TabIndex = 8;
            this.label9.Text = "Model";
            // 
            // lblEmrHardwareRev
            // 
            this.lblEmrHardwareRev.AutoSize = true;
            this.lblEmrHardwareRev.Location = new System.Drawing.Point(108, 83);
            this.lblEmrHardwareRev.Name = "lblEmrHardwareRev";
            this.lblEmrHardwareRev.Size = new System.Drawing.Size(16, 13);
            this.lblEmrHardwareRev.TabIndex = 7;
            this.lblEmrHardwareRev.Text = "...";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(7, 83);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(76, 13);
            this.label6.TabIndex = 6;
            this.label6.Text = "Hardware Rev";
            // 
            // lblEmrFirmwareRev
            // 
            this.lblEmrFirmwareRev.AutoSize = true;
            this.lblEmrFirmwareRev.Location = new System.Drawing.Point(108, 64);
            this.lblEmrFirmwareRev.Name = "lblEmrFirmwareRev";
            this.lblEmrFirmwareRev.Size = new System.Drawing.Size(16, 13);
            this.lblEmrFirmwareRev.TabIndex = 5;
            this.lblEmrFirmwareRev.Text = "...";
            // 
            // lblEmrSerialNo
            // 
            this.lblEmrSerialNo.AutoSize = true;
            this.lblEmrSerialNo.Location = new System.Drawing.Point(108, 45);
            this.lblEmrSerialNo.Name = "lblEmrSerialNo";
            this.lblEmrSerialNo.Size = new System.Drawing.Size(16, 13);
            this.lblEmrSerialNo.TabIndex = 4;
            this.lblEmrSerialNo.Text = "...";
            // 
            // txtEmrDeviceId
            // 
            this.txtEmrDeviceId.Location = new System.Drawing.Point(111, 19);
            this.txtEmrDeviceId.Name = "txtEmrDeviceId";
            this.txtEmrDeviceId.Size = new System.Drawing.Size(37, 20);
            this.txtEmrDeviceId.TabIndex = 3;
            this.txtEmrDeviceId.Text = "0";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 64);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(72, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "Firmware Rev";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 45);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(50, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Serial No";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 26);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(55, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Device ID";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(7, 83);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(31, 13);
            this.label4.TabIndex = 6;
            this.label4.Text = "Type";
            // 
            // lblRefPwrModel
            // 
            this.lblRefPwrModel.AutoSize = true;
            this.lblRefPwrModel.Location = new System.Drawing.Point(108, 64);
            this.lblRefPwrModel.Name = "lblRefPwrModel";
            this.lblRefPwrModel.Size = new System.Drawing.Size(16, 13);
            this.lblRefPwrModel.TabIndex = 5;
            this.lblRefPwrModel.Text = "...";
            // 
            // lblRefPwrManuf
            // 
            this.lblRefPwrManuf.AutoSize = true;
            this.lblRefPwrManuf.Location = new System.Drawing.Point(108, 45);
            this.lblRefPwrManuf.Name = "lblRefPwrManuf";
            this.lblRefPwrManuf.Size = new System.Drawing.Size(16, 13);
            this.lblRefPwrManuf.TabIndex = 4;
            this.lblRefPwrManuf.Text = "...";
            // 
            // txtRefPwrDeviceId
            // 
            this.txtRefPwrDeviceId.Location = new System.Drawing.Point(111, 19);
            this.txtRefPwrDeviceId.Name = "txtRefPwrDeviceId";
            this.txtRefPwrDeviceId.Size = new System.Drawing.Size(37, 20);
            this.txtRefPwrDeviceId.TabIndex = 3;
            this.txtRefPwrDeviceId.Text = "0";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(7, 26);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(55, 13);
            this.label8.TabIndex = 0;
            this.label8.Text = "Device ID";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.btnPauseSimRefPower);
            this.groupBox2.Controls.Add(this.progressSimRefPower);
            this.groupBox2.Controls.Add(this.btnSimRefPower);
            this.groupBox2.Controls.Add(this.btnChartOpen);
            this.groupBox2.Controls.Add(this.label15);
            this.groupBox2.Controls.Add(this.numMovingAvgSec);
            this.groupBox2.Controls.Add(this.lblRefPwrWattsAvg);
            this.groupBox2.Controls.Add(this.label17);
            this.groupBox2.Controls.Add(this.lblRefPwrWatts);
            this.groupBox2.Controls.Add(this.label19);
            this.groupBox2.Controls.Add(this.btnRefPwrSearch);
            this.groupBox2.Controls.Add(this.lblRefPwrType);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.lblRefPwrModel);
            this.groupBox2.Controls.Add(this.lblRefPwrManuf);
            this.groupBox2.Controls.Add(this.txtRefPwrDeviceId);
            this.groupBox2.Controls.Add(this.label222);
            this.groupBox2.Controls.Add(this.label14);
            this.groupBox2.Controls.Add(this.label8);
            this.groupBox2.Location = new System.Drawing.Point(563, 12);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(211, 342);
            this.groupBox2.TabIndex = 10;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Power Meter";
            // 
            // progressSimRefPower
            // 
            this.progressSimRefPower.Location = new System.Drawing.Point(10, 153);
            this.progressSimRefPower.Name = "progressSimRefPower";
            this.progressSimRefPower.Size = new System.Drawing.Size(195, 23);
            this.progressSimRefPower.Step = 1;
            this.progressSimRefPower.TabIndex = 36;
            this.progressSimRefPower.Visible = false;
            // 
            // btnSimRefPower
            // 
            this.btnSimRefPower.Location = new System.Drawing.Point(142, 127);
            this.btnSimRefPower.Name = "btnSimRefPower";
            this.btnSimRefPower.Size = new System.Drawing.Size(63, 23);
            this.btnSimRefPower.TabIndex = 35;
            this.btnSimRefPower.Text = "Load";
            this.btnSimRefPower.UseVisualStyleBackColor = true;
            this.btnSimRefPower.Click += new System.EventHandler(this.btnSimRefPower_Click);
            // 
            // btnChartOpen
            // 
            this.btnChartOpen.Location = new System.Drawing.Point(154, 253);
            this.btnChartOpen.Name = "btnChartOpen";
            this.btnChartOpen.Size = new System.Drawing.Size(51, 23);
            this.btnChartOpen.TabIndex = 34;
            this.btnChartOpen.Text = "Chart";
            this.btnChartOpen.UseVisualStyleBackColor = true;
            this.btnChartOpen.Click += new System.EventHandler(this.btnChartOpen_Click);
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(108, 316);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(90, 13);
            this.label15.TabIndex = 33;
            this.label15.Text = "Moving Avg (sec)";
            // 
            // numMovingAvgSec
            // 
            this.numMovingAvgSec.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.numMovingAvgSec.Location = new System.Drawing.Point(119, 291);
            this.numMovingAvgSec.Name = "numMovingAvgSec";
            this.numMovingAvgSec.Size = new System.Drawing.Size(75, 23);
            this.numMovingAvgSec.TabIndex = 32;
            this.numMovingAvgSec.Value = new decimal(new int[] {
            30,
            0,
            0,
            0});
            this.numMovingAvgSec.ValueChanged += new System.EventHandler(this.numMovingAvgSec_ValueChanged);
            // 
            // lblRefPwrWattsAvg
            // 
            this.lblRefPwrWattsAvg.AutoSize = true;
            this.lblRefPwrWattsAvg.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblRefPwrWattsAvg.Location = new System.Drawing.Point(20, 277);
            this.lblRefPwrWattsAvg.Name = "lblRefPwrWattsAvg";
            this.lblRefPwrWattsAvg.Size = new System.Drawing.Size(71, 37);
            this.lblRefPwrWattsAvg.TabIndex = 29;
            this.lblRefPwrWattsAvg.Text = "000";
            this.lblRefPwrWattsAvg.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label17.Location = new System.Drawing.Point(33, 252);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(40, 13);
            this.label17.TabIndex = 12;
            this.label17.Text = "Watts";
            // 
            // lblRefPwrWatts
            // 
            this.lblRefPwrWatts.AutoSize = true;
            this.lblRefPwrWatts.Font = new System.Drawing.Font("Microsoft Sans Serif", 32F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblRefPwrWatts.Location = new System.Drawing.Point(6, 196);
            this.lblRefPwrWatts.Name = "lblRefPwrWatts";
            this.lblRefPwrWatts.Size = new System.Drawing.Size(94, 51);
            this.lblRefPwrWatts.TabIndex = 11;
            this.lblRefPwrWatts.Text = "000";
            this.lblRefPwrWatts.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label19.Location = new System.Drawing.Point(28, 314);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(54, 13);
            this.label19.TabIndex = 31;
            this.label19.Text = "Average";
            // 
            // btnRefPwrSearch
            // 
            this.btnRefPwrSearch.Location = new System.Drawing.Point(154, 17);
            this.btnRefPwrSearch.Name = "btnRefPwrSearch";
            this.btnRefPwrSearch.Size = new System.Drawing.Size(51, 23);
            this.btnRefPwrSearch.TabIndex = 8;
            this.btnRefPwrSearch.Text = "Search";
            this.btnRefPwrSearch.UseVisualStyleBackColor = true;
            this.btnRefPwrSearch.Click += new System.EventHandler(this.btnRefPwrSearch_Click);
            // 
            // lblRefPwrType
            // 
            this.lblRefPwrType.AutoSize = true;
            this.lblRefPwrType.Location = new System.Drawing.Point(108, 83);
            this.lblRefPwrType.Name = "lblRefPwrType";
            this.lblRefPwrType.Size = new System.Drawing.Size(16, 13);
            this.lblRefPwrType.TabIndex = 7;
            this.lblRefPwrType.Text = "...";
            // 
            // label222
            // 
            this.label222.AutoSize = true;
            this.label222.Location = new System.Drawing.Point(7, 64);
            this.label222.Name = "label222";
            this.label222.Size = new System.Drawing.Size(36, 13);
            this.label222.TabIndex = 2;
            this.label222.Text = "Model";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(7, 45);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(70, 13);
            this.label14.TabIndex = 1;
            this.label14.Text = "Manufacturer";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(9, 22);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(34, 13);
            this.label12.TabIndex = 14;
            this.label12.Text = "Slope";
            // 
            // txtSlope
            // 
            this.txtSlope.Location = new System.Drawing.Point(61, 19);
            this.txtSlope.Name = "txtSlope";
            this.txtSlope.Size = new System.Drawing.Size(75, 20);
            this.txtSlope.TabIndex = 10;
            // 
            // txtOffset
            // 
            this.txtOffset.Location = new System.Drawing.Point(61, 42);
            this.txtOffset.Name = "txtOffset";
            this.txtOffset.Size = new System.Drawing.Size(75, 20);
            this.txtOffset.TabIndex = 15;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(9, 45);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(35, 13);
            this.label7.TabIndex = 16;
            this.label7.Text = "Offset";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.btnCalibrationSet);
            this.groupBox3.Controls.Add(this.btnCalibrationGet);
            this.groupBox3.Controls.Add(this.label12);
            this.groupBox3.Controls.Add(this.txtOffset);
            this.groupBox3.Controls.Add(this.txtSlope);
            this.groupBox3.Controls.Add(this.label7);
            this.groupBox3.Location = new System.Drawing.Point(13, 12);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(171, 103);
            this.groupBox3.TabIndex = 17;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Calibration";
            // 
            // btnCalibrationSet
            // 
            this.btnCalibrationSet.Location = new System.Drawing.Point(88, 73);
            this.btnCalibrationSet.Name = "btnCalibrationSet";
            this.btnCalibrationSet.Size = new System.Drawing.Size(47, 23);
            this.btnCalibrationSet.TabIndex = 26;
            this.btnCalibrationSet.Text = "Set";
            this.btnCalibrationSet.UseVisualStyleBackColor = true;
            this.btnCalibrationSet.Click += new System.EventHandler(this.btnCalibrationSet_Click);
            // 
            // btnCalibrationGet
            // 
            this.btnCalibrationGet.Location = new System.Drawing.Point(16, 73);
            this.btnCalibrationGet.Name = "btnCalibrationGet";
            this.btnCalibrationGet.Size = new System.Drawing.Size(47, 23);
            this.btnCalibrationGet.TabIndex = 26;
            this.btnCalibrationGet.Text = "Get";
            this.btnCalibrationGet.UseVisualStyleBackColor = true;
            this.btnCalibrationGet.Click += new System.EventHandler(this.btnCalibrationGet_Click);
            // 
            // groupBoxResistance
            // 
            this.groupBoxResistance.Controls.Add(this.btnSetResistancePositions);
            this.groupBoxResistance.Controls.Add(this.pnlResistanceStd);
            this.groupBoxResistance.Controls.Add(this.pnlResistancePercent);
            this.groupBoxResistance.Controls.Add(this.pnlErg);
            this.groupBoxResistance.Controls.Add(this.pnlResistanceSim);
            this.groupBoxResistance.Controls.Add(this.btnResistanceSet);
            this.groupBoxResistance.Controls.Add(this.txtServoPos);
            this.groupBoxResistance.Controls.Add(this.cmbResistanceMode);
            this.groupBoxResistance.Controls.Add(this.label22);
            this.groupBoxResistance.Location = new System.Drawing.Point(12, 237);
            this.groupBoxResistance.Name = "groupBoxResistance";
            this.groupBoxResistance.Size = new System.Drawing.Size(327, 117);
            this.groupBoxResistance.TabIndex = 19;
            this.groupBoxResistance.TabStop = false;
            this.groupBoxResistance.Text = "Resistance";
            // 
            // btnSetResistancePositions
            // 
            this.btnSetResistancePositions.Location = new System.Drawing.Point(25, 88);
            this.btnSetResistancePositions.Name = "btnSetResistancePositions";
            this.btnSetResistancePositions.Size = new System.Drawing.Size(111, 23);
            this.btnSetResistancePositions.TabIndex = 29;
            this.btnSetResistancePositions.Text = "Positions";
            this.btnSetResistancePositions.UseVisualStyleBackColor = true;
            this.btnSetResistancePositions.Click += new System.EventHandler(this.btnSetResistancePositions_Click);
            // 
            // pnlResistanceStd
            // 
            this.pnlResistanceStd.Controls.Add(this.lblResistanceStdLevel);
            this.pnlResistanceStd.Controls.Add(this.btnResistanceDec);
            this.pnlResistanceStd.Controls.Add(this.btnResistanceInc);
            this.pnlResistanceStd.Location = new System.Drawing.Point(147, 21);
            this.pnlResistanceStd.Name = "pnlResistanceStd";
            this.pnlResistanceStd.Size = new System.Drawing.Size(164, 91);
            this.pnlResistanceStd.TabIndex = 20;
            // 
            // lblResistanceStdLevel
            // 
            this.lblResistanceStdLevel.AutoSize = true;
            this.lblResistanceStdLevel.Font = new System.Drawing.Font("Microsoft Sans Serif", 32F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblResistanceStdLevel.Location = new System.Drawing.Point(60, 16);
            this.lblResistanceStdLevel.Name = "lblResistanceStdLevel";
            this.lblResistanceStdLevel.Size = new System.Drawing.Size(46, 51);
            this.lblResistanceStdLevel.TabIndex = 27;
            this.lblResistanceStdLevel.Text = "0";
            this.lblResistanceStdLevel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // btnResistanceDec
            // 
            this.btnResistanceDec.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnResistanceDec.Location = new System.Drawing.Point(133, 27);
            this.btnResistanceDec.Name = "btnResistanceDec";
            this.btnResistanceDec.Size = new System.Drawing.Size(23, 28);
            this.btnResistanceDec.TabIndex = 1;
            this.btnResistanceDec.Text = "&-";
            this.btnResistanceDec.UseVisualStyleBackColor = true;
            this.btnResistanceDec.Click += new System.EventHandler(this.btnResistanceDec_Click);
            // 
            // btnResistanceInc
            // 
            this.btnResistanceInc.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnResistanceInc.Location = new System.Drawing.Point(10, 27);
            this.btnResistanceInc.Name = "btnResistanceInc";
            this.btnResistanceInc.Size = new System.Drawing.Size(23, 28);
            this.btnResistanceInc.TabIndex = 0;
            this.btnResistanceInc.Text = "&+";
            this.btnResistanceInc.UseVisualStyleBackColor = true;
            this.btnResistanceInc.Click += new System.EventHandler(this.btnResistanceInc_Click);
            // 
            // pnlResistancePercent
            // 
            this.pnlResistancePercent.Controls.Add(this.trackBarResistancePct);
            this.pnlResistancePercent.Controls.Add(this.txtResistancePercent);
            this.pnlResistancePercent.Controls.Add(this.label5);
            this.pnlResistancePercent.Location = new System.Drawing.Point(147, 21);
            this.pnlResistancePercent.Name = "pnlResistancePercent";
            this.pnlResistancePercent.Size = new System.Drawing.Size(164, 91);
            this.pnlResistancePercent.TabIndex = 28;
            // 
            // trackBarResistancePct
            // 
            this.trackBarResistancePct.Location = new System.Drawing.Point(10, 5);
            this.trackBarResistancePct.Maximum = 100;
            this.trackBarResistancePct.Name = "trackBarResistancePct";
            this.trackBarResistancePct.Size = new System.Drawing.Size(147, 45);
            this.trackBarResistancePct.TabIndex = 2;
            this.trackBarResistancePct.Scroll += new System.EventHandler(this.trackBarResistancePct_Scroll);
            // 
            // txtResistancePercent
            // 
            this.txtResistancePercent.Location = new System.Drawing.Point(89, 56);
            this.txtResistancePercent.MaxLength = 6;
            this.txtResistancePercent.Name = "txtResistancePercent";
            this.txtResistancePercent.Size = new System.Drawing.Size(45, 20);
            this.txtResistancePercent.TabIndex = 1;
            this.txtResistancePercent.Text = "0";
            this.txtResistancePercent.Enter += new System.EventHandler(this.txtResistancePercent_Enter);
            this.txtResistancePercent.Leave += new System.EventHandler(this.txtResistancePercent_Leave);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(10, 58);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(61, 13);
            this.label5.TabIndex = 0;
            this.label5.Text = "Percent (%)";
            // 
            // pnlErg
            // 
            this.pnlErg.Controls.Add(this.txtResistanceErgWatts);
            this.pnlErg.Controls.Add(this.label20);
            this.pnlErg.Location = new System.Drawing.Point(147, 21);
            this.pnlErg.Name = "pnlErg";
            this.pnlErg.Size = new System.Drawing.Size(164, 91);
            this.pnlErg.TabIndex = 21;
            // 
            // txtResistanceErgWatts
            // 
            this.txtResistanceErgWatts.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtResistanceErgWatts.Location = new System.Drawing.Point(76, 22);
            this.txtResistanceErgWatts.Name = "txtResistanceErgWatts";
            this.txtResistanceErgWatts.Size = new System.Drawing.Size(78, 44);
            this.txtResistanceErgWatts.TabIndex = 22;
            this.txtResistanceErgWatts.TabStop = false;
            this.txtResistanceErgWatts.Enter += new System.EventHandler(this.txtResistanceErgWatts_Enter);
            this.txtResistanceErgWatts.Leave += new System.EventHandler(this.txtResistanceErgWatts_Leave);
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Location = new System.Drawing.Point(6, 36);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(69, 13);
            this.label20.TabIndex = 21;
            this.label20.Text = "Target Watts";
            // 
            // pnlResistanceSim
            // 
            this.pnlResistanceSim.Controls.Add(this.txtSimWind);
            this.pnlResistanceSim.Controls.Add(this.label25);
            this.pnlResistanceSim.Controls.Add(this.txtSimCrr);
            this.pnlResistanceSim.Controls.Add(this.label24);
            this.pnlResistanceSim.Controls.Add(this.txtSimSlope);
            this.pnlResistanceSim.Controls.Add(this.label23);
            this.pnlResistanceSim.Location = new System.Drawing.Point(147, 21);
            this.pnlResistanceSim.Name = "pnlResistanceSim";
            this.pnlResistanceSim.Size = new System.Drawing.Size(164, 91);
            this.pnlResistanceSim.TabIndex = 23;
            // 
            // txtSimWind
            // 
            this.txtSimWind.Location = new System.Drawing.Point(89, 55);
            this.txtSimWind.Name = "txtSimWind";
            this.txtSimWind.Size = new System.Drawing.Size(63, 20);
            this.txtSimWind.TabIndex = 26;
            this.txtSimWind.Leave += new System.EventHandler(this.txtSimWind_Leave);
            // 
            // label25
            // 
            this.label25.AutoSize = true;
            this.label25.Location = new System.Drawing.Point(6, 58);
            this.label25.Name = "label25";
            this.label25.Size = new System.Drawing.Size(60, 13);
            this.label25.TabIndex = 25;
            this.label25.Text = "Wind (mps)";
            // 
            // txtSimCrr
            // 
            this.txtSimCrr.Location = new System.Drawing.Point(89, 32);
            this.txtSimCrr.Name = "txtSimCrr";
            this.txtSimCrr.Size = new System.Drawing.Size(63, 20);
            this.txtSimCrr.TabIndex = 24;
            this.txtSimCrr.Leave += new System.EventHandler(this.txtSimCrr_Leave);
            // 
            // label24
            // 
            this.label24.AutoSize = true;
            this.label24.Location = new System.Drawing.Point(6, 35);
            this.label24.Name = "label24";
            this.label24.Size = new System.Drawing.Size(20, 13);
            this.label24.TabIndex = 23;
            this.label24.Text = "Crr";
            // 
            // txtSimSlope
            // 
            this.txtSimSlope.Location = new System.Drawing.Point(89, 9);
            this.txtSimSlope.Name = "txtSimSlope";
            this.txtSimSlope.Size = new System.Drawing.Size(63, 20);
            this.txtSimSlope.TabIndex = 22;
            this.txtSimSlope.Leave += new System.EventHandler(this.txtSimSlope_Leave);
            // 
            // label23
            // 
            this.label23.AutoSize = true;
            this.label23.Location = new System.Drawing.Point(6, 12);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(45, 13);
            this.label23.TabIndex = 21;
            this.label23.Text = "Slope %";
            // 
            // btnResistanceSet
            // 
            this.btnResistanceSet.Location = new System.Drawing.Point(89, 60);
            this.btnResistanceSet.Name = "btnResistanceSet";
            this.btnResistanceSet.Size = new System.Drawing.Size(47, 23);
            this.btnResistanceSet.TabIndex = 21;
            this.btnResistanceSet.Text = "Set";
            this.btnResistanceSet.UseVisualStyleBackColor = true;
            this.btnResistanceSet.Click += new System.EventHandler(this.btnResistanceSet_Click);
            // 
            // txtServoPos
            // 
            this.txtServoPos.Font = new System.Drawing.Font("Microsoft Sans Serif", 14F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtServoPos.Location = new System.Drawing.Point(25, 57);
            this.txtServoPos.Name = "txtServoPos";
            this.txtServoPos.Size = new System.Drawing.Size(63, 29);
            this.txtServoPos.TabIndex = 20;
            this.txtServoPos.Enter += new System.EventHandler(this.txtServoPos_Enter);
            this.txtServoPos.Leave += new System.EventHandler(this.txtServoPos_Leave);
            // 
            // cmbResistanceMode
            // 
            this.cmbResistanceMode.FormattingEnabled = true;
            this.cmbResistanceMode.Items.AddRange(new object[] {
            "Standard",
            "Percentage",
            "Erg",
            "Simulation"});
            this.cmbResistanceMode.Location = new System.Drawing.Point(25, 21);
            this.cmbResistanceMode.Name = "cmbResistanceMode";
            this.cmbResistanceMode.Size = new System.Drawing.Size(112, 21);
            this.cmbResistanceMode.TabIndex = 19;
            this.cmbResistanceMode.SelectedIndexChanged += new System.EventHandler(this.cmbResistanceMode_SelectedIndexChanged);
            this.cmbResistanceMode.Enter += new System.EventHandler(this.cmbResistanceMode_Enter);
            this.cmbResistanceMode.Leave += new System.EventHandler(this.cmbResistanceMode_Leave);
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Location = new System.Drawing.Point(22, 43);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(44, 13);
            this.label22.TabIndex = 19;
            this.label22.Text = "Position";
            // 
            // label26
            // 
            this.label26.AutoSize = true;
            this.label26.Location = new System.Drawing.Point(6, 24);
            this.label26.Name = "label26";
            this.label26.Size = new System.Drawing.Size(62, 13);
            this.label26.TabIndex = 19;
            this.label26.Text = "Weight (kg)";
            // 
            // txtTotalWeight
            // 
            this.txtTotalWeight.Location = new System.Drawing.Point(84, 21);
            this.txtTotalWeight.MaxLength = 6;
            this.txtTotalWeight.Name = "txtTotalWeight";
            this.txtTotalWeight.Size = new System.Drawing.Size(75, 20);
            this.txtTotalWeight.TabIndex = 18;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.btnSettingsGet);
            this.groupBox6.Controls.Add(this.btnSettingsSet);
            this.groupBox6.Controls.Add(this.label21);
            this.groupBox6.Controls.Add(this.txtWheelSizeMm);
            this.groupBox6.Controls.Add(this.label26);
            this.groupBox6.Controls.Add(this.txtTotalWeight);
            this.groupBox6.Location = new System.Drawing.Point(13, 121);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(171, 100);
            this.groupBox6.TabIndex = 19;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Profile";
            // 
            // btnSettingsGet
            // 
            this.btnSettingsGet.Location = new System.Drawing.Point(16, 73);
            this.btnSettingsGet.Name = "btnSettingsGet";
            this.btnSettingsGet.Size = new System.Drawing.Size(47, 23);
            this.btnSettingsGet.TabIndex = 27;
            this.btnSettingsGet.Text = "Get";
            this.btnSettingsGet.UseVisualStyleBackColor = true;
            this.btnSettingsGet.Click += new System.EventHandler(this.btnSettingsGet_Click);
            // 
            // btnSettingsSet
            // 
            this.btnSettingsSet.Location = new System.Drawing.Point(88, 73);
            this.btnSettingsSet.Name = "btnSettingsSet";
            this.btnSettingsSet.Size = new System.Drawing.Size(47, 23);
            this.btnSettingsSet.TabIndex = 26;
            this.btnSettingsSet.Text = "Set";
            this.btnSettingsSet.UseVisualStyleBackColor = true;
            this.btnSettingsSet.Click += new System.EventHandler(this.btnSettingsSet_Click);
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Location = new System.Drawing.Point(5, 50);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(63, 13);
            this.label21.TabIndex = 21;
            this.label21.Text = "Wheel (mm)";
            // 
            // txtWheelSizeMm
            // 
            this.txtWheelSizeMm.Location = new System.Drawing.Point(83, 47);
            this.txtWheelSizeMm.Name = "txtWheelSizeMm";
            this.txtWheelSizeMm.Size = new System.Drawing.Size(75, 20);
            this.txtWheelSizeMm.TabIndex = 20;
            // 
            // chkLstSettings
            // 
            this.chkLstSettings.FormattingEnabled = true;
            this.chkLstSettings.Items.AddRange(new object[] {
            "Accelerometer Sleeps",
            "Bluetooth Enabled",
            "ANT+ Control",
            "ANT+ Bike Power",
            "ANT+ FE-C",
            "ANT+ Extra Info"});
            this.chkLstSettings.Location = new System.Drawing.Point(6, 45);
            this.chkLstSettings.Name = "chkLstSettings";
            this.chkLstSettings.Size = new System.Drawing.Size(134, 109);
            this.chkLstSettings.TabIndex = 12;
            this.chkLstSettings.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.chkLstSettings_ItemCheck);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.lblFeatures);
            this.groupBox4.Controls.Add(this.cmbParamGet);
            this.groupBox4.Controls.Add(this.label31);
            this.groupBox4.Controls.Add(this.txtParamSet);
            this.groupBox4.Controls.Add(this.btnParamGet);
            this.groupBox4.Controls.Add(this.chkLstSettings);
            this.groupBox4.Controls.Add(this.btnParamSet);
            this.groupBox4.Location = new System.Drawing.Point(190, 12);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(146, 209);
            this.groupBox4.TabIndex = 18;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Get/Set Parameter";
            // 
            // lblFeatures
            // 
            this.lblFeatures.AutoSize = true;
            this.lblFeatures.Location = new System.Drawing.Point(87, 157);
            this.lblFeatures.Name = "lblFeatures";
            this.lblFeatures.Size = new System.Drawing.Size(13, 13);
            this.lblFeatures.TabIndex = 38;
            this.lblFeatures.Text = "..";
            // 
            // cmbParamGet
            // 
            this.cmbParamGet.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Suggest;
            this.cmbParamGet.CausesValidation = false;
            this.cmbParamGet.FormattingEnabled = true;
            this.cmbParamGet.Location = new System.Drawing.Point(5, 18);
            this.cmbParamGet.Margin = new System.Windows.Forms.Padding(2);
            this.cmbParamGet.Name = "cmbParamGet";
            this.cmbParamGet.Size = new System.Drawing.Size(82, 21);
            this.cmbParamGet.TabIndex = 10;
            // 
            // label31
            // 
            this.label31.AutoSize = true;
            this.label31.Location = new System.Drawing.Point(6, 157);
            this.label31.Name = "label31";
            this.label31.Size = new System.Drawing.Size(48, 13);
            this.label31.TabIndex = 37;
            this.label31.Text = "Features";
            // 
            // txtParamSet
            // 
            this.txtParamSet.Location = new System.Drawing.Point(90, 19);
            this.txtParamSet.Name = "txtParamSet";
            this.txtParamSet.Size = new System.Drawing.Size(50, 20);
            this.txtParamSet.TabIndex = 11;
            // 
            // btnParamGet
            // 
            this.btnParamGet.Location = new System.Drawing.Point(15, 182);
            this.btnParamGet.Name = "btnParamGet";
            this.btnParamGet.Size = new System.Drawing.Size(47, 23);
            this.btnParamGet.TabIndex = 13;
            this.btnParamGet.Text = "Get";
            this.btnParamGet.UseVisualStyleBackColor = true;
            this.btnParamGet.Click += new System.EventHandler(this.btnParamGet_Click);
            // 
            // btnParamSet
            // 
            this.btnParamSet.Location = new System.Drawing.Point(87, 182);
            this.btnParamSet.Name = "btnParamSet";
            this.btnParamSet.Size = new System.Drawing.Size(47, 23);
            this.btnParamSet.TabIndex = 14;
            this.btnParamSet.Text = "Set";
            this.btnParamSet.UseVisualStyleBackColor = true;
            this.btnParamSet.Click += new System.EventHandler(this.btnParamSet_Click);
            // 
            // statusStrip
            // 
            this.statusStrip.ImageScalingSize = new System.Drawing.Size(24, 24);
            this.statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lblStatus});
            this.statusStrip.Location = new System.Drawing.Point(0, 533);
            this.statusStrip.Name = "statusStrip";
            this.statusStrip.Size = new System.Drawing.Size(782, 22);
            this.statusStrip.TabIndex = 20;
            this.statusStrip.Text = "Running";
            // 
            // lblStatus
            // 
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(118, 17);
            this.lblStatus.Text = "toolStripStatusLabel1";
            // 
            // btnPauseSimRefPower
            // 
            this.btnPauseSimRefPower.Location = new System.Drawing.Point(76, 127);
            this.btnPauseSimRefPower.Name = "btnPauseSimRefPower";
            this.btnPauseSimRefPower.Size = new System.Drawing.Size(63, 23);
            this.btnPauseSimRefPower.TabIndex = 37;
            this.btnPauseSimRefPower.Text = "Pause";
            this.btnPauseSimRefPower.UseVisualStyleBackColor = true;
            this.btnPauseSimRefPower.Visible = false;
            this.btnPauseSimRefPower.Click += new System.EventHandler(this.btnPauseSimRefPower_Click);
            // 
            // frmIrtGui
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(782, 555);
            this.Controls.Add(this.statusStrip);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox6);
            this.Controls.Add(this.groupBoxResistance);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.txtLog);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(798, 594);
            this.MinimumSize = new System.Drawing.Size(798, 594);
            this.Name = "frmIrtGui";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "E-Motion Test Tool";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numMovingAvgSec)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBoxResistance.ResumeLayout(false);
            this.groupBoxResistance.PerformLayout();
            this.pnlResistanceStd.ResumeLayout(false);
            this.pnlResistanceStd.PerformLayout();
            this.pnlResistancePercent.ResumeLayout(false);
            this.pnlResistancePercent.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBarResistancePct)).EndInit();
            this.pnlErg.ResumeLayout(false);
            this.pnlErg.PerformLayout();
            this.pnlResistanceSim.ResumeLayout(false);
            this.pnlResistanceSim.PerformLayout();
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.statusStrip.ResumeLayout(false);
            this.statusStrip.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox txtLog;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label lblEmrFirmwareRev;
        private System.Windows.Forms.Label lblEmrSerialNo;
        private System.Windows.Forms.TextBox txtEmrDeviceId;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label lblEmrModel;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label lblEmrHardwareRev;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label lblRefPwrModel;
        private System.Windows.Forms.Label lblRefPwrManuf;
        private System.Windows.Forms.TextBox txtRefPwrDeviceId;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label lblRefPwrType;
        private System.Windows.Forms.Label label222;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label lblEmrWatts;
        private System.Windows.Forms.Button btnEmrSearch;
        private System.Windows.Forms.Label lblRefPwrWatts;
        private System.Windows.Forms.Button btnRefPwrSearch;
        private System.Windows.Forms.Label lblEmrBattVolt;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label lblEmrBattTime;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox txtSlope;
        private System.Windows.Forms.TextBox txtOffset;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label lblEmrMph;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.GroupBox groupBoxResistance;
        private System.Windows.Forms.Panel pnlResistanceStd;
        private System.Windows.Forms.Button btnResistanceSet;
        private System.Windows.Forms.ComboBox cmbResistanceMode;
        private System.Windows.Forms.Button btnResistanceDec;
        private System.Windows.Forms.Button btnResistanceInc;
        private System.Windows.Forms.TextBox txtServoPos;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.Panel pnlErg;
        private System.Windows.Forms.TextBox txtResistanceErgWatts;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Label label26;
        private System.Windows.Forms.TextBox txtTotalWeight;
        private System.Windows.Forms.Panel pnlResistanceSim;
        private System.Windows.Forms.TextBox txtSimWind;
        private System.Windows.Forms.Label label25;
        private System.Windows.Forms.TextBox txtSimCrr;
        private System.Windows.Forms.Label label24;
        private System.Windows.Forms.TextBox txtSimSlope;
        private System.Windows.Forms.Label label23;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.CheckedListBox chkLstSettings;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.TextBox txtWheelSizeMm;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.TextBox txtParamSet;
        private System.Windows.Forms.Button btnParamGet;
        private System.Windows.Forms.Button btnParamSet;
        private System.Windows.Forms.TextBox txtServoOffset;
        private System.Windows.Forms.Label label28;
        private System.Windows.Forms.Button btnServoOffset;
        private System.Windows.Forms.Label lblResistanceStdLevel;
        private System.Windows.Forms.CheckBox chkCharge;
        private System.Windows.Forms.Button btnDfuEnable;
        private System.Windows.Forms.StatusStrip statusStrip;
        private System.Windows.Forms.Button btnCalibrationSet;
        private System.Windows.Forms.Button btnCalibrationGet;
        private System.Windows.Forms.Button btnSettingsGet;
        private System.Windows.Forms.Button btnSettingsSet;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label lblEmrWattsAvg;
        private System.Windows.Forms.Label lblRefPwrWattsAvg;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Label lblFlywheel;
        private System.Windows.Forms.Label label29;
        private System.Windows.Forms.ToolStripStatusLabel lblStatus;
        private System.Windows.Forms.Panel pnlResistancePercent;
        private System.Windows.Forms.TextBox txtResistancePercent;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TrackBar trackBarResistancePct;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.NumericUpDown numMovingAvgSec;
        private System.Windows.Forms.Label label27;
        private System.Windows.Forms.Label lblSpeedAvg;
        private System.Windows.Forms.Button btnSetResistancePositions;
        private System.Windows.Forms.ComboBox cmbParamGet;
        private System.Windows.Forms.Button btnChartOpen;
        private System.Windows.Forms.Label lblFeatures;
        private System.Windows.Forms.Label label31;
        private System.Windows.Forms.Button btnSimRefPower;
        private System.Windows.Forms.ProgressBar progressSimRefPower;
        private System.Windows.Forms.Button btnPauseSimRefPower;
    }
}

