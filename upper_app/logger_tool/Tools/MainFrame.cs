using System.Net;
using System.Net.Sockets;
using System.Net.WebSockets;
using System.Text;
using System.Windows;
using System.Xml;

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
            Control.CheckForIllegalCrossThreadCalls = false;
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


            if (text.Length > 0) 
            {
                byte[] sendBytes = new byte[text.Length];
                sendBytes = Encoding.UTF8.GetBytes(text);

                if (ProtocolComboBox.SelectedIndex == 0)
                {
                    if (globalSocketManage.TcpSocket != null)
                    {
                        globalSocketManage.tx_count += (ulong)sendBytes.Length;
                        TxLable.Text = $"tx: {globalSocketManage.tx_count.ToString()}";
                        globalSocketManage.TcpSocket.Send(sendBytes, sendBytes.Length, 0);
                    }
                }
                else
                {
                    if (globalSocketManage.TcpClientSocket != null)
                    {
                        globalSocketManage.tx_count += (ulong)sendBytes.Length;
                        TxLable.Text = $"tx: {globalSocketManage.tx_count.ToString()}";
                        
                        foreach(var socket in globalSocketManage.TcpClientSocket)
                        {
                            socket.Send(sendBytes, sendBytes.Length, 0);
                        }
                            
                    }
                }
            }
        }

        private void Clear_Click(object sender, EventArgs e)
        {
            globalSocketManage.rx_count = 0;
            globalSocketManage.tx_count = 0;

            TxLable.Text = $"tx: {globalSocketManage.tx_count.ToString()}";
            RxLable.Text = $"rx: {globalSocketManage.rx_count.ToString()}";
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

                //if server
                if (ProtocolComboBox.SelectedIndex != 0)
                {
                    if (globalSocketManage.TcpClientSocket != null)
                    {
                        foreach (var socket in globalSocketManage.TcpClientSocket)
                        {
                            socket.Shutdown(SocketShutdown.Both);
                            socket.Close();
                        }    
                    }
                }

                if (globalSocketManage.TcpSocket != null)
                {
                    globalSocketManage.TcpSocket.Close();
                }
            }
            catch(Exception ex)
            {
                ShowBox.Invoke(AppendString, ex.Message+"\n");
            }
        }

        private void ConnectBtn_Click(object sender, EventArgs e)
        {
            if (!globalSocketManage.IsIPv4Address(IpAddrTextBox.Text))
            {
                ShowBox.Invoke(AppendString, $"Invalid IPAddress:{IpAddrTextBox.Text}!\n");
                return;
            }

            if (!globalSocketManage.IsPort(PortTextBox.Text))
            {
                ShowBox.Invoke(AppendString, $"Invalid Port:{PortTextBox.Text}!\n");
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

            //if accept, enter network
            enter_network();
            ShowBox.Invoke(AppendString, $"IPAddress:{globalSocketManage.socket_ip}:{globalSocketManage.port} Connect Success!\n");
            string recvStr;
            byte[] recvBytes = new byte[1024];
            int length;

            while (true)
            {
                length = globalSocketManage.TcpSocket.Receive(recvBytes, recvBytes.Length, 0);
                if(length > 0)
                {
                    globalSocketManage.rx_count += (ulong)length;
                    RxLable.Text = String.Format($"rx: {globalSocketManage.rx_count.ToString()}");

                    recvStr = Encoding.UTF8.GetString(recvBytes, 0, length);
                    ShowBox.Invoke(AppendString, recvStr);
                }
                else
                {
                    ShowBox.Invoke(AppendString, "Remote Close, Socket break!\n");
                    break;
                }
            }
        }

        private void thread_accpet(object? sender)
        {
            if (sender != null)
            {

                Socket AcceptSocket = (Socket)sender;

                try
                {
                    string recvStr;
                    byte[] recvBytes = new byte[1024];
                    int length;

                    while (true)
                    {
                        length = AcceptSocket.Receive(recvBytes, recvBytes.Length, 0);
                        if (length > 0)
                        {
                            globalSocketManage.rx_count += (ulong)length;
                            RxLable.Text = $"rx: {globalSocketManage.rx_count.ToString()}";

                            recvStr = Encoding.UTF8.GetString(recvBytes, 0, length);
                            ShowBox.Invoke(AppendString, recvStr);
                        }
                        else
                        {
                            break;
                        }
                    }

                    globalSocketManage.TcpClientSocket.Remove(AcceptSocket);
                    AcceptSocket.Close();
                }
                catch
                {
                    globalSocketManage.TcpClientSocket.Remove(AcceptSocket);
                    AcceptSocket.Close();
                    ShowBox.Invoke(AppendString, String.Format("TCP Accpet Connect Close Now!\n"));
                }
            }
            else
            {
                ShowBox.Invoke(AppendString, "Accept Socket is error!\n");
            }
        }

        private void tcp_server_loop()
        {
           
            IPEndPoint IpGlobal = new IPEndPoint(IPAddress.Parse(globalSocketManage.socket_ip), globalSocketManage.port);
            globalSocketManage.TcpSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            globalSocketManage.TcpSocket.ExclusiveAddressUse = false;
            globalSocketManage.TcpSocket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
            globalSocketManage.TcpSocket.Bind(IpGlobal);
            globalSocketManage.TcpSocket.Listen(32);

            ShowBox.Invoke(AppendString, $"Server Start Success, Now IPAddress is {globalSocketManage.socket_ip}:{globalSocketManage.port}!\n");
            //if listen success, enter network
            enter_network();

            while (true)
            {
                var accept_socket = globalSocketManage.TcpSocket.Accept();
                globalSocketManage.TcpClientSocket.Add(accept_socket);

                Thread t = new Thread(thread_accpet);
                t.Start(accept_socket);
            }
        }

        private void ConnetStartThread()
        {
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