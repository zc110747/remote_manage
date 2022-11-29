using System.Net;
using System.Net.Sockets;
using System.Net.WebSockets;
using System.Text;

namespace Tools
{
    public partial class MainFrame : Form
    {
        private SocketManage globalSocketManage = new SocketManage();

        public delegate void AppendMethod(string str);
        public AppendMethod? AppendString;

        public MainFrame()
        {
            InitializeComponent();
            InitUserView();
        }

        public void InitUserView()
        {
            globalSocketManage.initilize();
            IpAddrTextBox.Text = globalSocketManage.socket_ip;
            PortTextBox.Text = globalSocketManage.port.ToString();

            foreach(var value in globalSocketManage.protocolList)
            {
                ProtocolComboBox.Items.Add(value);
            }
            ProtocolComboBox.SelectedIndex = ProtocolComboBox.Items.IndexOf(globalSocketManage.protocolList.First());

            AppendString = new AppendMethod(str =>
            {
                ShowBox.AppendText(str);
            });
        }

        private void SendBtn_Click(object sender, EventArgs e)
        {
            string text = WriteBox.Text;
            byte[] sendBytes = new byte[text.Length];
            sendBytes = Encoding.UTF8.GetBytes(text);

            if (ProtocolComboBox.SelectedIndex == 0)
            {
                if (globalSocketManage.TcpSocket != null)
                {
                    globalSocketManage.TcpSocket.Send(sendBytes, sendBytes.Length, 0);
                }
            }
            else
            {
                if(globalSocketManage.TcpClientSocket != null)
                {
                    globalSocketManage.TcpClientSocket.Send(sendBytes, sendBytes.Length, 0);
                }
            }

        }

        private void Clear_Click(object sender, EventArgs e)
        {

        }

        private bool is_thread_start = false;

        private void enter_network()
        {
            ConnectBtn.Text = "Close";
            is_thread_start = true;
            ProtocolComboBox.Enabled = false;
            IpAddrTextBox.ReadOnly = true;
            PortTextBox.ReadOnly = true;
        }

        private void exit_network()
        {
            try
            {
                ConnectBtn.Text = "Open";
                is_thread_start = false;
                ProtocolComboBox.Enabled = true;
                IpAddrTextBox.ReadOnly = false;
                PortTextBox.ReadOnly = false;

                if (ProtocolComboBox.SelectedIndex != 0)
                {
                    if (globalSocketManage.TcpClientSocket != null
                    && globalSocketManage.TcpClientSocket.Connected)
                    {
                        ShowBox.Invoke(AppendString, "Client Socket Close!\n");
                        globalSocketManage.TcpClientSocket.Shutdown(SocketShutdown.Both);
                        globalSocketManage.TcpClientSocket.Close();
                    }
                }

                if (globalSocketManage.TcpSocket != null)
                {
                    globalSocketManage.TcpSocket.Close();
                }
            }
            catch(Exception ex)
            {
                ShowBox.Invoke(AppendString, ex.Message);
            }
        }

        private void ConnectBtn_Click(object sender, EventArgs e)
        {
            if (!globalSocketManage.IsIPv4Address(IpAddrTextBox.Text))
            {
                ShowBox.Invoke(AppendString, "Invalid IPAddress!\n");
                return;
            }

            if (!globalSocketManage.IsPort(PortTextBox.Text))
            {
                ShowBox.Invoke(AppendString, "Invalid Port!\n");
                return;
            }
            globalSocketManage.port = Int32.Parse(PortTextBox.Text);
            globalSocketManage.socket_ip = IpAddrTextBox.Text;

            if(!is_thread_start)
            {
                Thread process_thread = new Thread(ConnetStartThread);
                process_thread.IsBackground = true;
                process_thread.Start();
            }
            else
            {
                exit_network();
            }
        }

        private void tcp_client_loop()
        {
            IPEndPoint IpGlobal = new IPEndPoint(IPAddress.Parse(globalSocketManage.socket_ip), globalSocketManage.port);
            globalSocketManage.TcpSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            globalSocketManage.TcpSocket.ExclusiveAddressUse = false;
            globalSocketManage.TcpSocket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
            globalSocketManage.TcpSocket.Connect(IpGlobal);

            ShowBox.Invoke(AppendString, String.Format("IPAddress:{0}:{1} Connect Success!\n", globalSocketManage.socket_ip, globalSocketManage.port));
            string recvStr;
            byte[] recvBytes = new byte[1024];
            int length;

            while (true)
            {
                length = globalSocketManage.TcpSocket.Receive(recvBytes, recvBytes.Length, 0);
                recvStr = Encoding.UTF8.GetString(recvBytes, 0, length);
                ShowBox.Invoke(AppendString, recvStr);
            }
        }

        private void tcp_server_loop()
        {
            ShowBox.Invoke(AppendString, String.Format("IPAddress:{0}:{1} Server Start!\n", globalSocketManage.socket_ip, globalSocketManage.port));
            IPEndPoint IpGlobal = new IPEndPoint(IPAddress.Parse(globalSocketManage.socket_ip), globalSocketManage.port);
            globalSocketManage.TcpSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            globalSocketManage.TcpSocket.ExclusiveAddressUse = false;
            globalSocketManage.TcpSocket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
            globalSocketManage.TcpSocket.Bind(IpGlobal);
            globalSocketManage.TcpSocket.Listen(1);
            globalSocketManage.TcpClientSocket = null;
            globalSocketManage.TcpClientSocket = globalSocketManage.TcpSocket.Accept();

            ShowBox.Invoke(AppendString, "Client IPAddress Connect Success!\n");
            string recvStr;
            byte[] recvBytes = new byte[1024];
            int length;

            while (true)
            {
                length = globalSocketManage.TcpClientSocket.Receive(recvBytes, recvBytes.Length, 0);
                recvStr = Encoding.UTF8.GetString(recvBytes, 0, length);
                ShowBox.Invoke(AppendString, recvStr);
            }
        }

        private void ConnetStartThread()
        {
            enter_network();

            try
            {
                if (ProtocolComboBox.SelectedIndex == 0)
                {
                    tcp_client_loop();
                }
                else
                {
                    tcp_server_loop();
                }
            }
            catch
            {
                ShowBox.Invoke(AppendString, String.Format("TCP Connect Close Now!\n"));
            }

            exit_network();
        }

        private void ShowBox_TextChanged(object sender, EventArgs e)
        {
            ShowBox.SelectionStart = ShowBox.Text.Length;
            ShowBox.ScrollToCaret();
        }

        private void ClearLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            ShowBox.Clear();
        }

        private void SaveLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            
        }
    }
}