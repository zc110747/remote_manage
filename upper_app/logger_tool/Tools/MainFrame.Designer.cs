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
            this.ProtocolComboBox = new System.Windows.Forms.ComboBox();
            this.ConnectBtn = new System.Windows.Forms.Button();
            this.Port_label = new System.Windows.Forms.Label();
            this.Ipaddr_label = new System.Windows.Forms.Label();
            this.Protocol_label = new System.Windows.Forms.Label();
            this.PortTextBox = new System.Windows.Forms.TextBox();
            this.IpAddrTextBox = new System.Windows.Forms.TextBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.ClearLinkLabel = new System.Windows.Forms.LinkLabel();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.Clear = new System.Windows.Forms.Button();
            this.RxLable = new System.Windows.Forms.Label();
            this.TxLabel = new System.Windows.Forms.Label();
            this.WriteBox = new System.Windows.Forms.RichTextBox();
            this.SendBtn = new System.Windows.Forms.Button();
            this.ShowBox = new System.Windows.Forms.RichTextBox();
            this.SaveLinkLabel = new System.Windows.Forms.LinkLabel();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.ProtocolComboBox);
            this.groupBox1.Controls.Add(this.ConnectBtn);
            this.groupBox1.Controls.Add(this.Port_label);
            this.groupBox1.Controls.Add(this.Ipaddr_label);
            this.groupBox1.Controls.Add(this.Protocol_label);
            this.groupBox1.Controls.Add(this.PortTextBox);
            this.groupBox1.Controls.Add(this.IpAddrTextBox);
            this.groupBox1.Location = new System.Drawing.Point(14, 18);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox1.Size = new System.Drawing.Size(214, 300);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "网络设置";
            // 
            // ProtocolComboBox
            // 
            this.ProtocolComboBox.FormattingEnabled = true;
            this.ProtocolComboBox.Location = new System.Drawing.Point(14, 53);
            this.ProtocolComboBox.Margin = new System.Windows.Forms.Padding(4);
            this.ProtocolComboBox.Name = "ProtocolComboBox";
            this.ProtocolComboBox.Size = new System.Drawing.Size(183, 33);
            this.ProtocolComboBox.TabIndex = 7;
            // 
            // ConnectBtn
            // 
            this.ConnectBtn.Font = new System.Drawing.Font("Segoe UI", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.ConnectBtn.Location = new System.Drawing.Point(14, 240);
            this.ConnectBtn.Margin = new System.Windows.Forms.Padding(4);
            this.ConnectBtn.Name = "ConnectBtn";
            this.ConnectBtn.Size = new System.Drawing.Size(184, 51);
            this.ConnectBtn.TabIndex = 6;
            this.ConnectBtn.Text = "Open";
            this.ConnectBtn.UseVisualStyleBackColor = true;
            this.ConnectBtn.Click += new System.EventHandler(this.ConnectBtn_Click);
            // 
            // Port_label
            // 
            this.Port_label.AutoSize = true;
            this.Port_label.Location = new System.Drawing.Point(14, 168);
            this.Port_label.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.Port_label.Name = "Port_label";
            this.Port_label.Size = new System.Drawing.Size(66, 25);
            this.Port_label.TabIndex = 5;
            this.Port_label.Text = "端口号";
            // 
            // Ipaddr_label
            // 
            this.Ipaddr_label.AutoSize = true;
            this.Ipaddr_label.Location = new System.Drawing.Point(16, 94);
            this.Ipaddr_label.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.Ipaddr_label.Name = "Ipaddr_label";
            this.Ipaddr_label.Size = new System.Drawing.Size(64, 25);
            this.Ipaddr_label.TabIndex = 4;
            this.Ipaddr_label.Text = "Ip地址";
            // 
            // Protocol_label
            // 
            this.Protocol_label.AutoSize = true;
            this.Protocol_label.Location = new System.Drawing.Point(14, 24);
            this.Protocol_label.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.Protocol_label.Name = "Protocol_label";
            this.Protocol_label.Size = new System.Drawing.Size(87, 25);
            this.Protocol_label.TabIndex = 3;
            this.Protocol_label.Text = "协议类型";
            // 
            // PortTextBox
            // 
            this.PortTextBox.Location = new System.Drawing.Point(14, 197);
            this.PortTextBox.Margin = new System.Windows.Forms.Padding(4);
            this.PortTextBox.Name = "PortTextBox";
            this.PortTextBox.Size = new System.Drawing.Size(183, 31);
            this.PortTextBox.TabIndex = 2;
            // 
            // IpAddrTextBox
            // 
            this.IpAddrTextBox.Location = new System.Drawing.Point(14, 122);
            this.IpAddrTextBox.Margin = new System.Windows.Forms.Padding(4);
            this.IpAddrTextBox.Name = "IpAddrTextBox";
            this.IpAddrTextBox.Size = new System.Drawing.Size(183, 31);
            this.IpAddrTextBox.TabIndex = 0;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.SaveLinkLabel);
            this.groupBox2.Controls.Add(this.ClearLinkLabel);
            this.groupBox2.Location = new System.Drawing.Point(16, 326);
            this.groupBox2.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox2.Size = new System.Drawing.Size(212, 92);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Recv Process";
            // 
            // ClearLinkLabel
            // 
            this.ClearLinkLabel.AutoSize = true;
            this.ClearLinkLabel.Font = new System.Drawing.Font("Segoe UI", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.ClearLinkLabel.Location = new System.Drawing.Point(136, 43);
            this.ClearLinkLabel.Name = "ClearLinkLabel";
            this.ClearLinkLabel.Size = new System.Drawing.Size(59, 30);
            this.ClearLinkLabel.TabIndex = 0;
            this.ClearLinkLabel.TabStop = true;
            this.ClearLinkLabel.Text = "clear";
            this.ClearLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.ClearLinkLabel_LinkClicked);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.Clear);
            this.groupBox3.Controls.Add(this.RxLable);
            this.groupBox3.Controls.Add(this.TxLabel);
            this.groupBox3.Controls.Add(this.WriteBox);
            this.groupBox3.Controls.Add(this.SendBtn);
            this.groupBox3.Controls.Add(this.ShowBox);
            this.groupBox3.Location = new System.Drawing.Point(236, 18);
            this.groupBox3.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox3.Size = new System.Drawing.Size(805, 746);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "用户数据处理";
            // 
            // Clear
            // 
            this.Clear.Font = new System.Drawing.Font("Microsoft YaHei", 5.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point);
            this.Clear.ForeColor = System.Drawing.SystemColors.WindowText;
            this.Clear.Location = new System.Drawing.Point(663, 707);
            this.Clear.Margin = new System.Windows.Forms.Padding(4);
            this.Clear.Name = "Clear";
            this.Clear.Size = new System.Drawing.Size(120, 25);
            this.Clear.TabIndex = 4;
            this.Clear.Text = "清除计数";
            this.Clear.UseVisualStyleBackColor = true;
            this.Clear.Click += new System.EventHandler(this.Clear_Click);
            // 
            // RxLable
            // 
            this.RxLable.AutoSize = true;
            this.RxLable.Location = new System.Drawing.Point(589, 707);
            this.RxLable.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.RxLable.Name = "RxLable";
            this.RxLable.Size = new System.Drawing.Size(50, 25);
            this.RxLable.TabIndex = 3;
            this.RxLable.Text = "Rx: 0";
            // 
            // TxLabel
            // 
            this.TxLabel.AutoSize = true;
            this.TxLabel.Location = new System.Drawing.Point(535, 707);
            this.TxLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.TxLabel.Name = "TxLabel";
            this.TxLabel.Size = new System.Drawing.Size(46, 25);
            this.TxLabel.TabIndex = 2;
            this.TxLabel.Text = "Tx: 0";
            // 
            // WriteBox
            // 
            this.WriteBox.Location = new System.Drawing.Point(20, 581);
            this.WriteBox.Margin = new System.Windows.Forms.Padding(4);
            this.WriteBox.Name = "WriteBox";
            this.WriteBox.Size = new System.Drawing.Size(635, 119);
            this.WriteBox.TabIndex = 1;
            this.WriteBox.Text = "";
            // 
            // SendBtn
            // 
            this.SendBtn.Location = new System.Drawing.Point(663, 577);
            this.SendBtn.Margin = new System.Windows.Forms.Padding(4);
            this.SendBtn.Name = "SendBtn";
            this.SendBtn.Size = new System.Drawing.Size(134, 122);
            this.SendBtn.TabIndex = 0;
            this.SendBtn.Text = "发送";
            this.SendBtn.UseVisualStyleBackColor = true;
            this.SendBtn.Click += new System.EventHandler(this.SendBtn_Click);
            // 
            // ShowBox
            // 
            this.ShowBox.Location = new System.Drawing.Point(17, 32);
            this.ShowBox.Margin = new System.Windows.Forms.Padding(4);
            this.ShowBox.Name = "ShowBox";
            this.ShowBox.Size = new System.Drawing.Size(780, 537);
            this.ShowBox.TabIndex = 0;
            this.ShowBox.Text = "";
            this.ShowBox.TextChanged += new System.EventHandler(this.ShowBox_TextChanged);
            // 
            // SaveLinkLabel
            // 
            this.SaveLinkLabel.AutoSize = true;
            this.SaveLinkLabel.Location = new System.Drawing.Point(14, 47);
            this.SaveLinkLabel.Name = "SaveLinkLabel";
            this.SaveLinkLabel.Size = new System.Drawing.Size(86, 25);
            this.SaveLinkLabel.TabIndex = 1;
            this.SaveLinkLabel.TabStop = true;
            this.SaveLinkLabel.Text = "Save As...";
            this.SaveLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.SaveLinkLabel_LinkClicked);
            // 
            // MainFrame
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 25F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1057, 774);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MaximizeBox = false;
            this.Name = "MainFrame";
            this.Text = "网络Logger工具";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
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
        private Label TxLabel;
        private ComboBox ProtocolComboBox;
        private Button ConnectBtn;
        private Label Port_label;
        private Label Ipaddr_label;
        private Label Protocol_label;
        private TextBox PortTextBox;
        private TextBox IpAddrTextBox;
        private LinkLabel ClearLinkLabel;
        private LinkLabel SaveLinkLabel;
    }
}