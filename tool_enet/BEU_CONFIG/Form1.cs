using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
//using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Runtime.InteropServices;
using System.Net.NetworkInformation;
using Microsoft.Win32;

namespace BEU_CONFIG
{
    public partial class Form1 : Form
    {
        int debug = 0;

        List<BEU_SESSION> session = new List<BEU_SESSION>();
        int session_index = -1;
        UdpClient udpClient;
        List<UdpClient> udp = new List<UdpClient>();

        Thread thd_udp_read;
        Thread thd_udp_send;

        List<byte> udp_rbuff = new List<byte>();
        List<byte> udp_sbuff = new List<byte>();

        APP_CONF app_conf = new APP_CONF();
        System.Windows.Forms.Timer timer1;

        string ip_send = "192.168.1.200";

        int port_send = 65531;
        int port_my = 65532;

        byte cmd;

        public Form1()
        {
            InitializeComponent();
            System.Windows.Forms.Control.CheckForIllegalCrossThreadCalls = false;
        }
        int size = 0;

        string get_ipaddr()
        {
            NetworkInterface[] adapters = NetworkInterface.GetAllNetworkInterfaces();
            foreach (NetworkInterface ni in adapters)
            {
                
                //string fRegistryKey = "SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\" + ni.Id + "\\Connection";
                //RegistryKey rk = Registry.LocalMachine.OpenSubKey(fRegistryKey, false);
                //if (rk != null)
                //{
                //    string fPnpInstanceID = rk.GetValue("PnpInstanceID", "").ToString();
                //    int fMediaSubType = Convert.ToInt32(rk.GetValue("MediaSubType", 0));  
                //    return "1";
                //}
            }
            return "";
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            //size = Marshal.SizeOf(app_conf);
            //get_ipaddr();
            IPAddress[] ipas=Dns.GetHostAddresses(Dns.GetHostName());
            //IPHostEntry iph= Dns.GetHostByName();
            for (int i = 0; i < ipas.Length; i++)
            {
                if (ipas[i].AddressFamily == AddressFamily.InterNetwork)
                {
                    comboBox5.Items.Add(ipas[i].ToString());
                    IPEndPoint ipe = new IPEndPoint(ipas[i], port_my);
                    udp.Add(new UdpClient(ipe));
                }
            }

            udpClient = new UdpClient();
            //udpClient = new UdpClient(1022);
            //udpClient.JoinMulticastGroup(IPAddress.Parse("192.168.1.88"), IPAddress.Parse("127.0.0.1"));

            thd_udp_read = new Thread(mtd_udp_read);
            //thd_udp_read.Priority = ThreadPriority.Lowest;
            thd_udp_read.IsBackground = true;
            thd_udp_read.Start();

            thd_udp_send = new Thread(mtd_udp_send);
            //thd_udp_send.Priority = ThreadPriority.Lowest;
            thd_udp_send.IsBackground = true;
            thd_udp_send.Start();

            timer1 = new System.Windows.Forms.Timer();
            timer1.Tick += new EventHandler(timer1_Tick);
            timer1.Interval = 1000;

            

        }

        void timer1_Tick(object sender, EventArgs e)
        {
            timer1.Enabled = false;
            if (cmd == 0x55)
            {
                mtd_get_sessions();
            }
            else if (cmd == 0x57)
            {
                mtd_get_conf(session_index);
            }
            else if (cmd == 0x59)
            {
                if (udp_rbuff.Count == 1)
                {
                    if (udp_rbuff[0] == 0x5a)
                    {
                        string str = "配置成功。\r\n";
                        textBox1.AppendText(str);
                        textBox1.SelectionStart = textBox1.Text.Length;
                        textBox1.SelectionLength = 0;
                        textBox1.ScrollToCaret();
                        //search();
                    }
                }
            }
            cmd = 0;
        }

        //void udp_endreceive(IAsyncResult ar)
        //{
        //    UdpClient uc = ar.AsyncState as UdpClient;
        //    IPEndPoint ipep = new IPEndPoint(uc.);
        //    byte[] rbuff=uc.EndReceive(ar,ref ipep);

        //}

        void mtd_udp_read()
        {
            while (true)
            {
                int count = udpClient.Available;
                if (count > 0)
                {
                    IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 0);
                    byte[] rbuff = udpClient.Receive(ref ipep);
                    if (rbuff.Length > 0)
                    {
                        udp_rbuff.AddRange(rbuff);
                    }

                    if (debug > 0)
                    {
                        string str = "";
                        for (int i = 0; i < rbuff.Length; i++)
                        {
                            str += rbuff[i].ToString("X02") + " ";
                        }
                        str += "\r\n";
                        str += "收到" + rbuff.Length + "字节。\r\n";
                        textBox1.AppendText(str);
                    }
                }
                Thread.Sleep(10);
            }
        }
        void mtd_udp_send()
        {
            while (true)
            {
                int count = udp_sbuff.Count;
                if (count > 0)
                {
                    udpClient.Send(udp_sbuff.ToArray(), count, ip_send, port_send);
                    udp_sbuff.RemoveRange(0, count);
                }
                Thread.Sleep(10);
            }
        }

        void search()
        {
            if (cmd != 0)
            {
                return;
            }
            string str = "搜索设备，请稍等。\r\n";
            textBox1.AppendText(str);
            session.Clear();
            listBox1.Items.Clear();
            udp_rbuff.Clear();
            ip_send = "255.255.255.255";
            cmd = 0x55;
            //port_send = 1021;
            udp_sbuff.Add(cmd);
            timer1.Enabled = true;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            search();
        }

        void mtd_get_sessions()
        {
            int count = udp_rbuff.Count % 26;
            if (count == 0)
            {
                count = udp_rbuff.Count / 26;
                for (int i = 0; i < count; i++)
                {
                    string n = Encoding.ASCII.GetString(udp_rbuff.ToArray(), i * 26 + 0, 16);
                    byte[] ipmac = new byte[10];
                    for (int j = 0; j < 10; j++)
                    {
                        ipmac[j] = udp_rbuff[i * 26 + 16 + j];
                    }
                    BEU_SESSION bs = new BEU_SESSION();
                    bs.name = n;
                    bs.ipmac = ipmac;
                    session.Add(bs);
                }
            }

            for (int i = 0; i < session.Count; i++)
            {
                listBox1.Items.Add(session[i].name + " | " + session[i].ipmacs);
            }

            string str = "找到" + count + "个模块。\r\n";
            textBox1.AppendText(str);
            textBox1.SelectionStart = textBox1.Text.Length;
            textBox1.SelectionLength = 0;
            textBox1.ScrollToCaret();
            //udpClient.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            textBox1.Clear();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (cmd != 0)
            {
                return;
            }
            if (session_index < 0)
            {
                return;
            }
            clear_conf();
            udp_rbuff.Clear();
            string str = "读取配置信息，请稍等。\r\n";
            textBox1.AppendText(str);
            ip_send = session[session_index].ip;
            //ip_send = "192.168.1.200";
            cmd = 0x57;
            //port_send = 1021;
            udp_sbuff.Add(cmd);
            timer1.Enabled = true;
        }

        void mtd_get_conf(int index)
        {
            int count = udp_rbuff.Count % 47;
            if (count == 0)
            {
                count = udp_rbuff.Count / 47;
                for (int i = 0; i < count; i++)
                {
                    byte[] buff = new byte[47];
                    for (int j = 0; j < buff.Length; j++)
                    {
                        buff[j] = udp_rbuff[i * 47 + j];
                    }
                    app_conf = (APP_CONF)BytesToStruct(buff, typeof(APP_CONF));
                    session[index].app_conf = app_conf;
                }
            }

            string str = "读取" + count + "个模块信息。\r\n";
            textBox1.AppendText(str);
            textBox1.SelectionStart = textBox1.Text.Length;
            textBox1.SelectionLength = 0;
            textBox1.ScrollToCaret();

            show_conf(index);
        }

        public static byte[] StructToBytes(Object obj)
        {
            int size = Marshal.SizeOf(obj);
            byte[] bytes = new byte[size];
            IntPtr arrPtr = Marshal.UnsafeAddrOfPinnedArrayElement(bytes, 0);
            Marshal.StructureToPtr(obj, arrPtr, true);
            return bytes;
        }

        public static Object BytesToStruct(byte[] bytes, Type StructStyle)
        {
            IntPtr arrPtr = Marshal.UnsafeAddrOfPinnedArrayElement(bytes, 0);
            return Marshal.PtrToStructure(arrPtr, StructStyle);
        }

        void clear_conf()
        {
            comboBox1.SelectedIndex = -1;

            textBox2.Text = "";
            textBox3.Text = "";
            textBox4.Text = "";
            textBox5.Text = "";
            textBox6.Text = "";
            textBox7.Text = "";
            textBox8.Text = "";
            textBox9.Text = "";
            comboBox2.SelectedIndex = -1;
            comboBox3.SelectedIndex = -1;
            comboBox4.SelectedIndex = -1;
        }

        void show_conf(int index)
        {
            if (session[index].app_conf != null)
            {
                //if (session[index].app_conf.MagicWord == 0x5050)
                {
                    comboBox1.SelectedIndex = session[index].app_conf.ConnectMode;

                    textBox2.Text = Encoding.ASCII.GetString(session[index].app_conf.Name);
                    textBox3.Text = session[index].app_conf.MyMACAddr[0].ToString("X02") + ":" + session[index].app_conf.MyMACAddr[1].ToString("X02") + ":" + session[index].app_conf.MyMACAddr[2].ToString("X02") + ":" + session[index].app_conf.MyMACAddr[3].ToString("X02") + ":" + session[index].app_conf.MyMACAddr[4].ToString("X02") + ":" + session[index].app_conf.MyMACAddr[5].ToString("X02");
                    textBox4.Text = session[index].app_conf.MyIPAddr[0].ToString() + "." + session[index].app_conf.MyIPAddr[1].ToString() + "." + session[index].app_conf.MyIPAddr[2].ToString() + "." + session[index].app_conf.MyIPAddr[3].ToString();
                    textBox5.Text = session[index].app_conf.MyMask[0].ToString() + "." + session[index].app_conf.MyMask[1].ToString() + "." + session[index].app_conf.MyMask[2].ToString() + "." + session[index].app_conf.MyMask[3].ToString();
                    textBox6.Text = session[index].app_conf.MyGateway[0].ToString() + "." + session[index].app_conf.MyGateway[1].ToString() + "." + session[index].app_conf.MyGateway[2].ToString() + "." + session[index].app_conf.MyGateway[3].ToString();
                    textBox7.Text = session[index].app_conf.MyPortAddr + "";
                    textBox8.Text = session[index].app_conf.RemoteIPAddr[0].ToString() + "." + session[index].app_conf.RemoteIPAddr[1].ToString() + "." + session[index].app_conf.RemoteIPAddr[2].ToString() + "." + session[index].app_conf.RemoteIPAddr[3].ToString();
                    textBox9.Text = session[index].app_conf.RemotePortAddr + "";
                    comboBox2.SelectedIndex = session[index].app_conf.UARTBaud;
                    comboBox3.SelectedIndex = 0;//time disconnect
                    comboBox4.SelectedIndex = 0;//dhcp
                    if ((session[index].app_conf.Flags & 4) == 4)
                    {
                        comboBox3.SelectedIndex = 1;
                    }
                    if ((session[index].app_conf.Flags & 1) == 1)
                    {
                        comboBox4.SelectedIndex = 1;
                    }
                }
            }
        }

        void save_conf(int index)
        {
            if (session[index].app_conf != null)
            {
                session[index].app_conf.ConnectMode = (byte)comboBox1.SelectedIndex;
                session[index].app_conf.Name = Encoding.ASCII.GetBytes(textBox2.Text);
                string[] strs = textBox3.Text.Split(new string[] { ":" }, StringSplitOptions.RemoveEmptyEntries);
                for (int i = 0; i < strs.Length; i++)
                {
                    session[index].app_conf.MyMACAddr[i] = Convert.ToByte(strs[i], 16);
                }
                strs = textBox4.Text.Split(new string[] { "." }, StringSplitOptions.RemoveEmptyEntries);
                for (int i = 0; i < strs.Length; i++)
                {
                    session[index].app_conf.MyIPAddr[i] = Convert.ToByte(strs[i], 10);
                }
                strs = textBox5.Text.Split(new string[] { "." }, StringSplitOptions.RemoveEmptyEntries);
                for (int i = 0; i < strs.Length; i++)
                {
                    session[index].app_conf.MyMask[i] = Convert.ToByte(strs[i], 10);
                }
                strs = textBox6.Text.Split(new string[] { "." }, StringSplitOptions.RemoveEmptyEntries);
                for (int i = 0; i < strs.Length; i++)
                {
                    session[index].app_conf.MyGateway[i] = Convert.ToByte(strs[i], 10);
                }
                session[index].app_conf.MyPortAddr = ushort.Parse(textBox7.Text);
                strs = textBox8.Text.Split(new string[] { "." }, StringSplitOptions.RemoveEmptyEntries);
                for (int i = 0; i < strs.Length; i++)
                {
                    session[index].app_conf.RemoteIPAddr[i] = Convert.ToByte(strs[i], 10);
                }
                session[index].app_conf.RemotePortAddr = ushort.Parse(textBox9.Text);
                session[index].app_conf.UARTBaud = (byte)comboBox2.SelectedIndex;
                session[index].app_conf.Flags = 0;
                if (comboBox3.SelectedIndex == 1)
                {
                    session[index].app_conf.Flags |= 4;
                }
                if (comboBox4.SelectedIndex == 1)
                {
                    session[index].app_conf.Flags |= 1;
                }
            }
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listBox1.SelectedIndex < 0)
            {
                return;
            }
            session_index = listBox1.SelectedIndex;
            //show_conf(index);
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            textBox7.Enabled = true;
            textBox8.Enabled = true;
            textBox9.Enabled = true;

            int index = comboBox1.SelectedIndex;
            if (index == 1)
            {
                //textBox9.Enabled = false;
            }
            if (index == 2)
            {
                textBox8.Enabled = false;
                textBox9.Enabled = false;
            }
            if (index == 3)
            {
                //textBox7.Enabled = false;
            }
        }

        

        private void button4_Click(object sender, EventArgs e)
        {
            if (cmd != 0)
            {
                return;
            }
            if (session_index < 0)
            {
                return;
            }
            save_conf(session_index);
            udp_rbuff.Clear();
            string str = "写入配置信息，请稍等。\r\n";
            textBox1.AppendText(str);
            ip_send = session[session_index].ip;
            cmd = 0x59;
            //port_send = 1021;
            byte[] t = StructToBytes(session[session_index].app_conf);
            byte[] t2 = new byte[48];
            t2[0] = cmd;
            t2[46] = 0x50;
            t2[47] = 0x50;
            for (int i = 0; i < 45; i++)
            {
                t2[i + 1] = t[i];
            }
            udp_sbuff.AddRange(t2);
            timer1.Enabled = true;            
        }

        private void button5_Click(object sender, EventArgs e)
        {
            if (session_index < 0)
            {
                return;
            }
            byte[] t = new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x04, 0x01, 0xff };

            ip_send = session[session_index].ip;
            port_send = session[session_index].app_conf.MyPortAddr;
            udp_sbuff.AddRange(t);
        } 

        private void button6_Click(object sender, EventArgs e)
        {
            if (session_index < 0)
            {
                return;
            }
            byte[] t = new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x04, 0x01, 0 };

            ip_send = session[session_index].ip;
            port_send = session[session_index].app_conf.MyPortAddr;
            udp_sbuff.AddRange(t);
        }

        private void comboBox5_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                udpClient = udp[comboBox5.SelectedIndex];
            }
            catch {
                MessageBox.Show("网卡荀泽有误，请检查！");
            }
        }

        int otimeH, otimeM, ctimeH, ctimeM;

        private void timer2_Tick(object sender, EventArgs e)
        {
            try
            {
                DateTime dt = DateTime.Now;
                timer2.Interval = 1000;
                if (dt.Hour == otimeH && dt.Minute == otimeM)
                {
                    byte[] t = new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x04, 0x01, 0xff };

                    //ip_send = session[session_index].ip;
                    //port_send = session[session_index].app_conf.MyPortAddr;
                    udp_sbuff.AddRange(t);
                    timer2.Interval = 70000;
                }
                if (dt.Hour == ctimeH && dt.Minute == ctimeM)
                {
                    byte[] t = new byte[] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00 };

                    //ip_send = session[session_index].ip;
                    //port_send = session[session_index].app_conf.MyPortAddr;
                    udp_sbuff.AddRange(t);
                    timer2.Interval = 70000;
                }
            }
            catch {
                MessageBox.Show("请保证输入信息格式正确");
            }
        }

        private void button7_Click(object sender, EventArgs e)
        {
            try
            {

                ip_send = textBox14.Text;
                port_send = int.Parse(textBox15.Text);
                otimeH = int.Parse(textBox10.Text);
                otimeM = int.Parse(textBox11.Text);
                ctimeH = int.Parse(textBox12.Text);
                ctimeM = int.Parse(textBox13.Text);
            }
            catch {
                MessageBox.Show("请保证输入信息格式正确");
            }
        }
    }
}
