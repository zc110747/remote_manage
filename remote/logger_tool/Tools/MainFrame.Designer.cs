namespace Tools
{
    partial class MainFrame
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.SaveLabel = new System.Windows.Forms.LinkLabel();
            this.ProtocolComboBox = new System.Windows.Forms.ComboBox();
            this.ConnectBtn = new System.Windows.Forms.Button();
            this.Port_label = new System.Windows.Forms.Label();
            this.Ipaddr_label = new System.Windows.Forms.Label();
            this.Protocol_label = new System.Windows.Forms.Label();
            this.PortTextBox = new System.Windows.Forms.TextBox();
            this.IpAddrTextBox = new System.Windows.Forms.TextBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.HexProcess = new System.Windows.Forms.CheckBox();
            this.SaveLinkLabel = new System.Windows.Forms.LinkLabel();
            this.ClearLinkLabel = new System.Windows.Forms.LinkLabel();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.Clear = new System.Windows.Forms.Button();
            this.RxLable = new System.Windows.Forms.Label();
            this.TxLable = new System.Windows.Forms.Label();
            this.WriteBox = new System.Windows.Forms.RichTextBox();
            this.SendBtn = new System.Windows.Forms.Button();
            this.ShowBox = new System.Windows.Forms.RichTextBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.label2 = new System.Windows.Forms.Label();
            this.ClearSendLabel = new System.Windows.Forms.LinkLabel();
            this.TimeLoop = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.LoopCheck = new System.Windows.Forms.CheckBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.SaveLabel);
            this.groupBox1.Controls.Add(this.ProtocolComboBox);
            this.groupBox1.Controls.Add(this.ConnectBtn);
            this.groupBox1.Controls.Add(this.Port_label);
            this.groupBox1.Controls.Add(this.Ipaddr_label);
            this.groupBox1.Controls.Add(this.Protocol_label);
            this.groupBox1.Controls.Add(this.PortTextBox);
            this.groupBox1.Controls.Add(this.IpAddrTextBox);
            this.groupBox1.Location = new System.Drawing.Point(13, 14);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.groupBox1.Size = new System.Drawing.Size(193, 240);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "网络设置";
            // 
            // SaveLabel
            // 
            this.SaveLabel.AutoSize = true;
            this.SaveLabel.Font = new System.Drawing.Font("Segoe UI", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.SaveLabel.Location = new System.Drawing.Point(95, 15);
            this.SaveLabel.Name = "SaveLabel";
            this.SaveLabel.Size = new System.Drawing.Size(92, 25);
            this.SaveLabel.TabIndex = 2;
            this.SaveLabel.TabStop = true;
            this.SaveLabel.Text = "保存配置";
            this.SaveLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.SaveLabel_LinkClicked);
            // 
            // ProtocolComboBox
            // 
            this.ProtocolComboBox.FormattingEnabled = true;
            this.ProtocolComboBox.Location = new System.Drawing.Point(13, 42);
            this.ProtocolComboBox.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.ProtocolComboBox.Name = "ProtocolComboBox";
            this.ProtocolComboBox.Size = new System.Drawing.Size(165, 28);
            this.ProtocolComboBox.TabIndex = 7;
            // 
            // ConnectBtn
            // 
            this.ConnectBtn.Font = new System.Drawing.Font("Segoe UI", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.ConnectBtn.Location = new System.Drawing.Point(14, 189);
            this.ConnectBtn.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.ConnectBtn.Name = "ConnectBtn";
            this.ConnectBtn.Size = new System.Drawing.Size(164, 45);
            this.ConnectBtn.TabIndex = 6;
            this.ConnectBtn.Text = "打开";
            this.ConnectBtn.UseVisualStyleBackColor = true;
            this.ConnectBtn.Click += new System.EventHandler(this.ConnectBtn_Click);
            // 
            // Port_label
            // 
            this.Port_label.AutoSize = true;
            this.Port_label.Location = new System.Drawing.Point(13, 134);
            this.Port_label.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.Port_label.Name = "Port_label";
            this.Port_label.Size = new System.Drawing.Size(54, 20);
            this.Port_label.TabIndex = 5;
            this.Port_label.Text = "端口号";
            // 
            // Ipaddr_label
            // 
            this.Ipaddr_label.AutoSize = true;
            this.Ipaddr_label.Location = new System.Drawing.Point(13, 74);
            this.Ipaddr_label.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.Ipaddr_label.Name = "Ipaddr_label";
            this.Ipaddr_label.Size = new System.Drawing.Size(52, 20);
            this.Ipaddr_label.TabIndex = 4;
            this.Ipaddr_label.Text = "IP地址";
            // 
            // Protocol_label
            // 
            this.Protocol_label.AutoSize = true;
            this.Protocol_label.Location = new System.Drawing.Point(13, 19);
            this.Protocol_label.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.Protocol_label.Name = "Protocol_label";
            this.Protocol_label.Size = new System.Drawing.Size(69, 20);
            this.Protocol_label.TabIndex = 3;
            this.Protocol_label.Text = "协议类型";
            // 
            // PortTextBox
            // 
            this.PortTextBox.Location = new System.Drawing.Point(13, 158);
            this.PortTextBox.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.PortTextBox.Name = "PortTextBox";
            this.PortTextBox.Size = new System.Drawing.Size(165, 27);
            this.PortTextBox.TabIndex = 2;
            // 
            // IpAddrTextBox
            // 
            this.IpAddrTextBox.Location = new System.Drawing.Point(13, 98);
            this.IpAddrTextBox.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.IpAddrTextBox.Name = "IpAddrTextBox";
            this.IpAddrTextBox.Size = new System.Drawing.Size(165, 27);
            this.IpAddrTextBox.TabIndex = 0;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.HexProcess);
            this.groupBox2.Controls.Add(this.SaveLinkLabel);
            this.groupBox2.Controls.Add(this.ClearLinkLabel);
            this.groupBox2.Location = new System.Drawing.Point(14, 261);
            this.groupBox2.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Padding = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.groupBox2.Size = new System.Drawing.Size(190, 146);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "接收处理";
            // 
            // HexProcess
            // 
            this.HexProcess.AutoSize = true;
            this.HexProcess.Location = new System.Drawing.Point(13, 97);
            this.HexProcess.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.HexProcess.Name = "HexProcess";
            this.HexProcess.Size = new System.Drawing.Size(91, 24);
            this.HexProcess.TabIndex = 4;
            this.HexProcess.Text = "HEX处理";
            this.HexProcess.UseVisualStyleBackColor = true;
            this.HexProcess.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // SaveLinkLabel
            // 
            this.SaveLinkLabel.AutoSize = true;
            this.SaveLinkLabel.Font = new System.Drawing.Font("Segoe UI", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.SaveLinkLabel.Location = new System.Drawing.Point(11, 22);
            this.SaveLinkLabel.Name = "SaveLinkLabel";
            this.SaveLinkLabel.Size = new System.Drawing.Size(112, 23);
            this.SaveLinkLabel.TabIndex = 1;
            this.SaveLinkLabel.TabStop = true;
            this.SaveLinkLabel.Text = "保存接收数据";
            this.SaveLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.SaveLinkLabel_LinkClicked);
            // 
            // ClearLinkLabel
            // 
            this.ClearLinkLabel.AutoSize = true;
            this.ClearLinkLabel.Font = new System.Drawing.Font("Segoe UI", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.ClearLinkLabel.Location = new System.Drawing.Point(11, 62);
            this.ClearLinkLabel.Name = "ClearLinkLabel";
            this.ClearLinkLabel.Size = new System.Drawing.Size(112, 23);
            this.ClearLinkLabel.TabIndex = 0;
            this.ClearLinkLabel.TabStop = true;
            this.ClearLinkLabel.Text = "清空接收数据";
            this.ClearLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.ClearLinkLabel_LinkClicked);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.Clear);
            this.groupBox3.Controls.Add(this.RxLable);
            this.groupBox3.Controls.Add(this.TxLable);
            this.groupBox3.Controls.Add(this.WriteBox);
            this.groupBox3.Controls.Add(this.SendBtn);
            this.groupBox3.Controls.Add(this.ShowBox);
            this.groupBox3.Location = new System.Drawing.Point(212, 14);
            this.groupBox3.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Padding = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.groupBox3.Size = new System.Drawing.Size(725, 588);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "用户数据处理";
            // 
            // Clear
            // 
            this.Clear.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.Clear.Font = new System.Drawing.Font("Courier New", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.Clear.ForeColor = System.Drawing.SystemColors.WindowText;
            this.Clear.Location = new System.Drawing.Point(617, 550);
            this.Clear.Margin = new System.Windows.Forms.Padding(1);
            this.Clear.Name = "Clear";
            this.Clear.Size = new System.Drawing.Size(81, 28);
            this.Clear.TabIndex = 4;
            this.Clear.Text = "Reset";
            this.Clear.UseVisualStyleBackColor = true;
            this.Clear.Click += new System.EventHandler(this.Clear_Click);
            // 
            // RxLable
            // 
            this.RxLable.AutoSize = true;
            this.RxLable.Location = new System.Drawing.Point(518, 558);
            this.RxLable.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.RxLable.Name = "RxLable";
            this.RxLable.Size = new System.Drawing.Size(56, 20);
            this.RxLable.TabIndex = 3;
            this.RxLable.Text = "接收: 0";
            // 
            // TxLable
            // 
            this.TxLable.AutoSize = true;
            this.TxLable.Location = new System.Drawing.Point(419, 558);
            this.TxLable.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.TxLable.Name = "TxLable";
            this.TxLable.Size = new System.Drawing.Size(52, 20);
            this.TxLable.TabIndex = 2;
            this.TxLable.Text = "发送:0";
            // 
            // WriteBox
            // 
            this.WriteBox.Location = new System.Drawing.Point(18, 450);
            this.WriteBox.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.WriteBox.Name = "WriteBox";
            this.WriteBox.Size = new System.Drawing.Size(572, 96);
            this.WriteBox.TabIndex = 1;
            this.WriteBox.Text = "";
            // 
            // SendBtn
            // 
            this.SendBtn.Location = new System.Drawing.Point(597, 449);
            this.SendBtn.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.SendBtn.Name = "SendBtn";
            this.SendBtn.Size = new System.Drawing.Size(121, 97);
            this.SendBtn.TabIndex = 0;
            this.SendBtn.Text = "发送";
            this.SendBtn.UseVisualStyleBackColor = true;
            this.SendBtn.Click += new System.EventHandler(this.SendBtn_Click);
            // 
            // ShowBox
            // 
            this.ShowBox.Location = new System.Drawing.Point(18, 26);
            this.ShowBox.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.ShowBox.Name = "ShowBox";
            this.ShowBox.ReadOnly = true;
            this.ShowBox.Size = new System.Drawing.Size(692, 419);
            this.ShowBox.TabIndex = 0;
            this.ShowBox.Text = "";
            this.ShowBox.TextChanged += new System.EventHandler(this.ShowBox_TextChanged);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.label2);
            this.groupBox4.Controls.Add(this.ClearSendLabel);
            this.groupBox4.Controls.Add(this.TimeLoop);
            this.groupBox4.Controls.Add(this.label1);
            this.groupBox4.Controls.Add(this.LoopCheck);
            this.groupBox4.Location = new System.Drawing.Point(17, 413);
            this.groupBox4.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox4.Size = new System.Drawing.Size(187, 190);
            this.groupBox4.TabIndex = 3;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "发送处理";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(140, 90);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(30, 20);
            this.label2.TabIndex = 3;
            this.label2.Text = "ms";
            // 
            // ClearSendLabel
            // 
            this.ClearSendLabel.AutoSize = true;
            this.ClearSendLabel.Font = new System.Drawing.Font("Segoe UI", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.ClearSendLabel.Location = new System.Drawing.Point(8, 125);
            this.ClearSendLabel.Name = "ClearSendLabel";
            this.ClearSendLabel.Size = new System.Drawing.Size(112, 23);
            this.ClearSendLabel.TabIndex = 2;
            this.ClearSendLabel.TabStop = true;
            this.ClearSendLabel.Text = "清空发送数据";
            this.ClearSendLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.ClearSendLabel_LinkClicked);
            // 
            // TimeLoop
            // 
            this.TimeLoop.Location = new System.Drawing.Point(6, 85);
            this.TimeLoop.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.TimeLoop.Name = "TimeLoop";
            this.TimeLoop.Size = new System.Drawing.Size(125, 27);
            this.TimeLoop.TabIndex = 2;
            this.TimeLoop.Text = "1000";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(5, 62);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(39, 20);
            this.label1.TabIndex = 1;
            this.label1.Text = "计时";
            // 
            // LoopCheck
            // 
            this.LoopCheck.AutoSize = true;
            this.LoopCheck.Location = new System.Drawing.Point(8, 33);
            this.LoopCheck.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.LoopCheck.Name = "LoopCheck";
            this.LoopCheck.Size = new System.Drawing.Size(91, 24);
            this.LoopCheck.TabIndex = 0;
            this.LoopCheck.Text = "循环发送";
            this.LoopCheck.UseVisualStyleBackColor = true;
            // 
            // MainFrame
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(951, 610);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Margin = new System.Windows.Forms.Padding(4, 3, 4, 3);
            this.MaximizeBox = false;
            this.Name = "MainFrame";
            this.Text = "网络Logger工具";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private GroupBox groupBox1;
        private GroupBox groupBox2;
        private GroupBox groupBox3;
        private RichTextBox ShowBox;
        private Button SendBtn;
        private RichTextBox WriteBox;
        private Button Clear;
        private Label RxLable;
        private Label TxLable;
        private ComboBox ProtocolComboBox;
        private Button ConnectBtn;
        private Label Port_label;
        private Label Ipaddr_label;
        private Label Protocol_label;
        private TextBox PortTextBox;
        private TextBox IpAddrTextBox;
        private LinkLabel ClearLinkLabel;
        private LinkLabel SaveLinkLabel;
        private LinkLabel SaveLabel;
        private GroupBox groupBox4;
        private LinkLabel ClearSendLabel;
        private TextBox TimeLoop;
        private Label label1;
        private CheckBox LoopCheck;
        private Label label2;
        private CheckBox HexProcess;
    }
}