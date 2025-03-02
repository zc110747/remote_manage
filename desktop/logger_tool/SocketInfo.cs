using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Net.Sockets;

namespace Tools
{
    public partial class SocketManage
    {
        /*socket ipaddress*/
        public string socket_ip = "127.0.0.1";

        /*config file*/
        public string ConfigFile = "config.ini";

        /*logger file*/
        public string LoggerFile = "logger.txt";

        /*端口号*/
        public int port;

        /*工作模式，客户端、服务器*/
        public int work_mode;

        /*Tcp Socket*/
        public Socket? TcpSocket;

        /*Tcp Client Socket*/
        public HashSet<Socket> TcpClientSocket = new HashSet<Socket>();

        /*socket tx count*/
        public UInt64 tx_count;
    
        /*socket tx count*/
        public UInt64 rx_count;

        /*支持协议组合*/
        public string[] protocolList =
        {
            "TCP Client",
            "TCP Server"
        };

        /*Out for Check ipaddress and port*/
        public delegate bool IsMethod(string str);
        public IsMethod IsIPv4Address = new IsMethod(ip =>
        {
            if (string.IsNullOrEmpty(ip) || ip.Length < 7 || ip.Length > 15)
            {
                return false;
            }
            string regformat = @"^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$";
            Regex regex = new Regex(regformat, RegexOptions.IgnoreCase);
            return regex.IsMatch(ip);
        });
        public IsMethod IsPort = new IsMethod(port =>
            {
            int Port;
            bool isport = Int32.TryParse(port, out Port);
            if (isport && Port >= 0 && Port <= 65536)
                return true;
            return false;
        });

        public void initilize()
        {
            socket_ip = "127.0.0.1";
            port = 15535;
            tx_count = 0;
            rx_count = 0;

            TcpClientSocket.Clear();

            //获得远端dns服务器的ip地址
            //IPAddress[] Address = Dns.GetHostAddresses("www.baidu.com");
            //WebSocketOut(Address[0].ToString());
            //查询本地ip类，检索出符合ipv4格式的地址
            IPAddress[] AddressList = Dns.GetHostAddresses(Dns.GetHostName());
            foreach (IPAddress ip in AddressList)
            {
                if (IsIPv4Address(ip.ToString()))
                {
                    socket_ip = ip.ToString();
                    break;
                }
            }

            if (File.Exists(ConfigFile))
            {
                string[] lines = File.ReadAllLines(ConfigFile);
                HashSet<string[]> Hashlist = new HashSet<string[]>();

                foreach (var line in lines)
                {
                    if (line.Split('=').Length == 2)
                    {
                        Hashlist.Add(line.Split('='));
                    }
                }

                foreach (var strarry in Hashlist)
                {
                    if (strarry[0].ToLower().Equals("mode"))
                    {
                        int data;
                        if (int.TryParse(strarry[1], out data))
                        {
                            work_mode = data;
                        }
                    }

                    if (strarry[0].ToLower().Equals("ipaddress"))
                    {
                        socket_ip = strarry[1];
                    }

                    if (strarry[0].ToLower().Equals("port"))
                    {
                        int data;
                        if (int.TryParse(strarry[1], out data))
                        {
                            port = data;
                        }
                    }
                }
            }
        }
    }
}
