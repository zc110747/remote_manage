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
                ShowBox.AppendText(str+"\n");
            });
        }

        private void SendBtn_Click(object sender, EventArgs e)
        {
            string text = WriteBox.Text;
            byte[] sendBytes = new byte[text.Length];
            sendBytes = Encoding.UTF8.GetBytes(text);
            if (globalSocketManage.TcpSocket != null)
            {
                globalSocketManage.TcpSocket.Send(sendBytes, sendBytes.Length, 0);
            }
        }

        private void Clear_Click(object sender, EventArgs e)
        {

        }

        private bool is_thread_start = false;

        private void ConnectBtn_Click(object sender, EventArgs e)
        {
            if (!globalSocketManage.IsIPv4Address(IpAddrTextBox.Text))
            {
                ShowBox.Invoke(AppendString, "Invalid IPAddress!");
                return;
            }

            if (!globalSocketManage.IsPort(PortTextBox.Text))
            {
                ShowBox.Invoke(AppendString, "Invalid Port!");
                return;
            }
            globalSocketManage.port = Int32.Parse(PortTextBox.Text);
            globalSocketManage.socket_ip = IpAddrTextBox.Text;

            //单开一个线程用来处理按键信息
            if(!is_thread_start)
            {
                Thread process_thread = new Thread(ConnetStartThread);
                process_thread.IsBackground = true;
                process_thread.Start();
            }
            else
            {
                if(globalSocketManage.TcpSocket != null)
                {
                    globalSocketManage.TcpSocket.Close();
                }
                ConnectBtn.Text = "打开连接";
            }
        }

        private void ConnetStartThread()
        {
            try
            {
                //创建新的Socket连接，绑定在本地端口，并监听
                //AddressFamily 指定网络类型，其中AddressFamily.InteRingNetwork表示IVP4地址
                //SocketType.Stream指定Socket的类型(udp: Dgram, tcp：Stream, ICMP/Raw: Raw
                //ProtocolType: 指定可用的协议类型
                IPEndPoint IpGlobal = new IPEndPoint(IPAddress.Parse(globalSocketManage.socket_ip), globalSocketManage.port);
                globalSocketManage.TcpSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                globalSocketManage.TcpSocket.ExclusiveAddressUse = false;
                globalSocketManage.TcpSocket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
                globalSocketManage.TcpSocket.Connect(IpGlobal);

                ShowBox.Invoke(AppendString, String.Format("IPAddr:{0}:{1} Connect Success!", globalSocketManage.socket_ip, globalSocketManage.port));
                ConnectBtn.Text = "关闭连接";
                is_thread_start = true;

                string recvStr;
                byte[] recvBytes = new byte[1024];
                int length;

                while (true)
                {
                    //获得当前Socket连接传输的数据，并转换为utf-8格式
                    length = globalSocketManage.TcpSocket.Receive(recvBytes, recvBytes.Length, 0);
                    recvStr = Encoding.UTF8.GetString(recvBytes, 0, length);
                    ShowBox.Invoke(AppendString, recvStr);
                }
            }
            catch
            {
                ShowBox.Invoke(AppendString, String.Format("TCP Connect Close Now!"));
            }

            ConnectBtn.Text = "打开连接";
            is_thread_start = false;
        }

        private void ShowBox_TextChanged(object sender, EventArgs e)
        {
            //滚动到最新光标位置
            ShowBox.SelectionStart = ShowBox.Text.Length;
            ShowBox.ScrollToCaret();
        }
    }
}