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
            this.SaveLinkLabel = new System.Windows.Forms.LinkLabel();
            this.ClearLinkLabel = new System.Windows.Forms.LinkLabel();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.Clear = new System.Windows.Forms.Button();
            this.RxLable = new System.Windows.Forms.Label();
            this.TxLable = new System.Windows.Forms.Label();
            this.WriteBox = new System.Windows.Forms.RichTextBox();
            this.SendBtn = new System.Windows.Forms.Button();
            this.ShowBox = new System.Windows.Forms.RichTextBox();
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
            this.groupBox1.Location = new System.Drawing.Point(10, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(150, 204);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "网络设置";
            // 
            // ProtocolComboBox
            // 
            this.ProtocolComboBox.FormattingEnabled = true;
            this.ProtocolComboBox.Location = new System.Drawing.Point(10, 36);
            this.ProtocolComboBox.Name = "ProtocolComboBox";
            this.ProtocolComboBox.Size = new System.Drawing.Size(129, 25);
            this.ProtocolComboBox.TabIndex = 7;
            // 
            // ConnectBtn
            // 
            this.ConnectBtn.Font = new System.Drawing.Font("Segoe UI", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.ConnectBtn.Location = new System.Drawing.Point(10, 163);
            this.ConnectBtn.Name = "ConnectBtn";
            this.ConnectBtn.Size = new System.Drawing.Size(129, 35);
            this.ConnectBtn.TabIndex = 6;
            this.ConnectBtn.Text = "Open";
            this.ConnectBtn.UseVisualStyleBackColor = true;
            this.ConnectBtn.Click += new System.EventHandler(this.ConnectBtn_Click);
            // 
            // Port_label
            // 
            this.Port_label.AutoSize = true;
            this.Port_label.Location = new System.Drawing.Point(10, 114);
            this.Port_label.Name = "Port_label";
            this.Port_label.Size = new System.Drawing.Size(44, 17);
            this.Port_label.TabIndex = 5;
            this.Port_label.Text = "端口号";
            // 
            // Ipaddr_label
            // 
            this.Ipaddr_label.AutoSize = true;
            this.Ipaddr_label.Location = new System.Drawing.Point(11, 64);
            this.Ipaddr_label.Name = "Ipaddr_label";
            this.Ipaddr_label.Size = new System.Drawing.Size(44, 17);
            this.Ipaddr_label.TabIndex = 4;
            this.Ipaddr_label.Text = "Ip地址";
            // 
            // Protocol_label
            // 
            this.Protocol_label.AutoSize = true;
            this.Protocol_label.Location = new System.Drawing.Point(10, 16);
            this.Protocol_label.Name = "Protocol_label";
            this.Protocol_label.Size = new System.Drawing.Size(56, 17);
            this.Protocol_label.TabIndex = 3;
            this.Protocol_label.Text = "协议类型";
            // 
            // PortTextBox
            // 
            this.PortTextBox.Location = new System.Drawing.Point(10, 134);
            this.PortTextBox.Name = "PortTextBox";
            this.PortTextBox.Size = new System.Drawing.Size(129, 23);
            this.PortTextBox.TabIndex = 2;
            // 
            // IpAddrTextBox
            // 
            this.IpAddrTextBox.Location = new System.Drawing.Point(10, 83);
            this.IpAddrTextBox.Name = "IpAddrTextBox";
            this.IpAddrTextBox.Size = new System.Drawing.Size(129, 23);
            this.IpAddrTextBox.TabIndex = 0;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.SaveLinkLabel);
            this.groupBox2.Controls.Add(this.ClearLinkLabel);
            this.groupBox2.Location = new System.Drawing.Point(11, 222);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(148, 63);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Recv Process";
            // 
            // SaveLinkLabel
            // 
            this.SaveLinkLabel.AutoSize = true;
            this.SaveLinkLabel.Location = new System.Drawing.Point(10, 32);
            this.SaveLinkLabel.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.SaveLinkLabel.Name = "SaveLinkLabel";
            this.SaveLinkLabel.Size = new System.Drawing.Size(62, 17);
            this.SaveLinkLabel.TabIndex = 1;
            this.SaveLinkLabel.TabStop = true;
            this.SaveLinkLabel.Text = "Save As...";
            this.SaveLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.SaveLinkLabel_LinkClicked);
            // 
            // ClearLinkLabel
            // 
            this.ClearLinkLabel.AutoSize = true;
            this.ClearLinkLabel.Font = new System.Drawing.Font("Segoe UI", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.ClearLinkLabel.Location = new System.Drawing.Point(87, 29);
            this.ClearLinkLabel.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.ClearLinkLabel.Name = "ClearLinkLabel";
            this.ClearLinkLabel.Size = new System.Drawing.Size(41, 20);
            this.ClearLinkLabel.TabIndex = 0;
            this.ClearLinkLabel.TabStop = true;
            this.ClearLinkLabel.Text = "clear";
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
            this.groupBox3.Location = new System.Drawing.Point(165, 12);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(564, 503);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "用户数据处理";
            // 
            // Clear
            // 
            this.Clear.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.Clear.Font = new System.Drawing.Font("Arial", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.Clear.ForeColor = System.Drawing.SystemColors.WindowText;
            this.Clear.Location = new System.Drawing.Point(464, 479);
            this.Clear.Margin = new System.Windows.Forms.Padding(1);
            this.Clear.Name = "Clear";
            this.Clear.Size = new System.Drawing.Size(94, 24);
            this.Clear.TabIndex = 4;
            this.Clear.Text = "reset";
            this.Clear.UseVisualStyleBackColor = true;
            this.Clear.Click += new System.EventHandler(this.Clear_Click);
            // 
            // RxLable
            // 
            this.RxLable.AutoSize = true;
            this.RxLable.Location = new System.Drawing.Point(395, 482);
            this.RxLable.Name = "RxLable";
            this.RxLable.Size = new System.Drawing.Size(33, 17);
            this.RxLable.TabIndex = 3;
            this.RxLable.Text = "rx: 0";
            // 
            // TxLable
            // 
            this.TxLable.AutoSize = true;
            this.TxLable.Location = new System.Drawing.Point(338, 483);
            this.TxLable.Name = "TxLable";
            this.TxLable.Size = new System.Drawing.Size(32, 17);
            this.TxLable.TabIndex = 2;
            this.TxLable.Text = "tx: 0";
            // 
            // WriteBox
            // 
            this.WriteBox.Location = new System.Drawing.Point(14, 395);
            this.WriteBox.Name = "WriteBox";
            this.WriteBox.Size = new System.Drawing.Size(446, 82);
            this.WriteBox.TabIndex = 1;
            this.WriteBox.Text = "";
            // 
            // SendBtn
            // 
            this.SendBtn.Location = new System.Drawing.Point(464, 392);
            this.SendBtn.Name = "SendBtn";
            this.SendBtn.Size = new System.Drawing.Size(94, 83);
            this.SendBtn.TabIndex = 0;
            this.SendBtn.Text = "发送";
            this.SendBtn.UseVisualStyleBackColor = true;
            this.SendBtn.Click += new System.EventHandler(this.SendBtn_Click);
            // 
            // ShowBox
            // 
            this.ShowBox.Location = new System.Drawing.Point(12, 22);
            this.ShowBox.Name = "ShowBox";
            this.ShowBox.ReadOnly = true;
            this.ShowBox.Size = new System.Drawing.Size(547, 366);
            this.ShowBox.TabIndex = 0;
            this.ShowBox.Text = "";
            this.ShowBox.TextChanged += new System.EventHandler(this.ShowBox_TextChanged);
            // 
            // MainFrame
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 17F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(740, 526);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
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
    }
}