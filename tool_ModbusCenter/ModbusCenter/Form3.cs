using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Timers;

namespace ModbusCenter
{
    public partial class Form3 : Form
    {
        Thread thd;
        System.IO.Ports.SerialPort com;
        Command cmd;
        int retry_times = 10;
        int timeout = 1000;//ms
        bool skip = false;
        AutoResetEvent autoResetEvent = new AutoResetEvent(false);
        System.Timers.Timer timer = new System.Timers.Timer();
        bool ok = false;
        int cmd_interval = 10;
        double ts;
        string l1text = "";
        List<byte> rbuf;

        int addr = 0;
        int func = 0;

        public Form3(string comn, Command c)
        {            
            InitializeComponent();
            com = new System.IO.Ports.SerialPort(comn);
            cmd = c;
            ok = false;
            com.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(com_DataReceived);
        }

        void com_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            int c = com.BytesToRead;
            byte[] td = new byte[c];
            com.Read(td, 0, c);
            rbuf.AddRange(td);
            ts = rt.TotalMilliseconds;
        }

        private void Form3_Load(object sender, EventArgs e)
        {
            if (!com.IsOpen)
            {

                try
                { com.Open(); }
                catch { }
            }
            thd = new Thread(thd_func);
            thd.IsBackground = true;
            thd.Start();
            rbuf = new List<byte>();
            timer.Elapsed += new ElapsedEventHandler(timer_Elapsed);
            timer.Interval = 1000;
        }

        void timer_Elapsed(object sender, ElapsedEventArgs e)
        {
            autoResetEvent.Set();
            timer.Stop();
        }

        int CRC16(byte[] data, int len)
        {
            int crc = 0xffff;
            byte i = 0, j = 0;
            for (j = 0; j < len; j++)
            {
                crc = crc ^ data[j];
                for (i = 0; i < 8; i++)
                {
                    if ((crc & 1) > 0)
                    {
                        crc = crc >> 1;
                        crc = crc ^ 0xa001;
                    }
                    else
                    {
                        crc = crc >> 1;
                    }
                }
            }
            i = (byte)crc;
            j = (byte)(crc >> 8);
            crc = 0;
            crc = i;
            crc <<= 8;
            crc |= j;
            return crc;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Abort;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (ok)
            {
                Close();
            }
            label1.Text = l1text;
            try
            {
                progressBar1.Value = cmd.p100;
            }
            catch
            { }
        }

        bool bytes_same(byte[] b1, byte[] b2)
        {
            if (b1 == null)
            {
                return false;
            }
            if (b2 == null)
            {
                return false;
            }
            if (b1.Length != b2.Length)
            {
                return false;
            }
            for (int i = 0; i < b1.Length; i++)
            {
                if (b1[i] != b2[i])
                {
                    return false;
                }
            }
            return true;
        }

        DateTime wt;
        TimeSpan et
        {
            get
            {
                return DateTime.Now - wt;
            }
        }
        TimeSpan rt
        {
            get
            {
                return DateTime.Now - wt;
            }
        }

        void thd_func()
        {
            //while (true)
            //{
            //    int c = com.BytesToRead;
            //    if (c > 0)
            //    {
            //        byte[] td = new byte[c];
            //        com.Read(td, 0, c);
            //        rbuf.AddRange(td);
            //    }
            //}

                //if (cmd.finish)
                //{
                //    DialogResult = DialogResult.OK;
                //    thd.Abort();
                //    break;
                //}
                //else
                //{
                //    int retry = 0;
                //    while (true)
                //    {
                //        bool responsed = false;
                //        byte[] td = cmd.request[cmd.now];
                //        //rbuf.Clear();
                //        com.Write(td, 0, td.Length);
                //        addr = td[0];
                //        //cmd.now++;
                //        wt = DateTime.Now;
                //        l1text = "报文已发出 等待响应";

                //        //rbuf.Clear();
                //        while (true)
                //        {
                //            //if (td[1] == 3)
                //            //{
                //            //    if (rbuf.Count >= 7)
                //            //    {
                //            //        break;
                //            //    }
                //            //}
                //            //else if (td[1] == 6)
                //            //{
                //            //    if (rbuf.Count >= 8)
                //            //    {
                //            //        break;
                //            //    }
                //            //}
                //            int j = -1;
                //            for (int i = 0; i < rbuf.Count; i++)
                //            {
                //                if (rbuf[i] == addr)
                //                {
                //                    j = i;
                //                    break;
                //                }
                //                j++;
                //            }
                //            if (j >= 0)
                //            {
                //                byte[] tb = rbuf.ToArray();
                //                int fc = tb[j + 1];
                //                if (fc == 3)
                //                {
                //                    byte[] tb1 = new byte[7];
                //                    for (int i = 0; i < tb1.Length; i++)
                //                    {
                //                        tb1[i] = tb[j + i];
                //                    }
                //                    int crc = CRC16(tb1, 2 + 1 + 2);
                //                    byte crch = (byte)(crc >> 8);
                //                    byte crcl = (byte)(crc & 255);
                //                    if (crch == tb1[5] && crcl == tb1[6])
                //                    {
                //                        l1text = "已得到正确响应03";
                //                        responsed = true;
                //                        int r = tb1[3];
                //                        r <<= 8;
                //                        r |= tb1[4];
                //                        cmd.result[cmd.now] = r;
                //                        rbuf.RemoveRange(j, 7);
                //                        break;
                //                    }
                //                }
                //                else if (fc == 6)
                //                {
                //                    byte[] tb1 = new byte[8];
                //                    for (int i = 0; i < tb1.Length; i++)
                //                    {
                //                        tb1[i] = tb[j + i];
                //                    }
                //                    int crc = CRC16(tb1, 2 + 2 + 2);
                //                    byte crch = (byte)(crc >> 8);
                //                    byte crcl = (byte)(crc & 255);
                //                    if (crch == tb1[6] && crcl == tb1[7])
                //                    {
                //                        l1text = "已得到正确响应06";
                //                        responsed = true; int r = tb1[4];
                //                        r <<= 8;
                //                        r |= tb1[5];
                //                        cmd.result[cmd.now] = r;
                //                        rbuf.RemoveRange(j, 8);
                //                        break;
                //                    }
                //                }
                //            }
                //            if (et.TotalMilliseconds > timeout)
                //            {
                //                //ts = et.TotalMilliseconds;
                //                //break;
                //            }

                //            //if (com.BytesToRead >= 0)
                //            //{
                //            //    byte[] rb = new byte[com.BytesToRead];
                //            //    com.Read(rb, 0, rb.Length);
                //            //    rbuf.AddRange(rb);

                //            //}   

                //        }

                //        //if (td[1] == 3)
                //        //{
                //        //    func = 3;
                //        //    while (true)
                //        //    {
                //        //        if (rbuf.Count >= 7)
                //        //        {
                //        //            break;
                //        //        }
                //        //        if ((DateTime.Now - wt).TotalMilliseconds > timeout)
                //        //        {
                //        //            //ts = et.TotalMilliseconds;
                //        //            break;
                //        //        }

                //        //        //if (com.BytesToRead >= 0)
                //        //        //{
                //        //        //    byte[] rb = new byte[com.BytesToRead];
                //        //        //    com.Read(rb, 0, rb.Length);
                //        //        //    rbuf.AddRange(rb);

                //        //        //}   

                //        //    }
                //        //    if (rbuf.Count >= 7)
                //        //    {
                //        //        //int crc = CRC16(rbuf.ToArray(), 1 + 1 + 1 + 2);
                //        //        //byte crch = (byte)(crc >> 8);
                //        //        //byte crcl = (byte)(crc & 255);
                //        //        //if (crch == rbuf[5] && crcl == rbuf[6])
                //        //        //{
                //        //        //    l1text = "已得到正确响应03";
                //        //        //    responsed = true;
                //        //        //    int r = rbuf[3];
                //        //        //    r <<= 8;
                //        //        //    r |= rbuf[4];
                //        //        //    cmd.result[cmd.now] = r;
                //        //        //    rbuf.RemoveRange(0, 7);
                //        //        //}

                //        //    }
                //        //}
                //        //else if (td[1] == 6)
                //        //{
                //        //    func = 6;
                //        //    while (true)
                //        //    {
                //        //        if (rbuf.Count >= 8)
                //        //            {
                //        //                break;
                //        //            }
                //        //        if ((DateTime.Now - wt).TotalMilliseconds > timeout)
                //        //        {
                //        //            //ts = et.TotalMilliseconds;
                //        //            break;
                //        //        }
                //        //        //if (com.BytesToRead >= 0)
                //        //        //{
                //        //        //    byte[] rb = new byte[com.BytesToRead];
                //        //        //    com.Read(rb, 0, rb.Length);
                //        //        //    rbuf.AddRange(rb);

                //        //        //}

                //        //        //if (com.BytesToRead >= 8)
                //        //        //{
                //        //        //    for (int i = 0; i < 8; i++)
                //        //        //    {
                //        //        //        rbuf.Add((byte)com.ReadByte());
                //        //        //    }
                //        //        //    break;
                //        //        //}                                
                //        //    }
                //        //    if (rbuf.Count >= 8)
                //        //    {
                //        //        int j = -1;
                //        //        for (int i = 0; i < rbuf.Count; i++)
                //        //        {
                //        //            if (rbuf[i] == addr)
                //        //            {
                //        //                j = i;
                //        //                break;
                //        //            }
                //        //            j++;
                //        //        }
                //        //        if (j >= 0)
                //        //        {
                //        //            byte[] tb = rbuf.ToArray();

                //        //        }
                //        //    }
                //        //}
                //        if (skip)
                //        {
                //            l1text = "手动跳过";
                //            skip = false;
                //            cmd.now++;
                //            timer.Start();
                //            autoResetEvent.WaitOne();
                //            Thread.Sleep(cmd_interval);
                //            if (cmd.finish)
                //            {
                //                break;
                //            }
                //            else
                //            {
                //                continue;
                //            }
                //        }
                //        if (responsed)
                //        {
                //            retry = 0;
                //            cmd.now++;
                //            Thread.Sleep(cmd_interval);
                //            if (cmd.finish)
                //            {
                //                break;
                //            }
                //            else
                //            {
                //                continue;
                //            }
                //        }
                //        else if (retry_times > 0)
                //        {
                //            cmd.now--;
                //            if (cmd.now < 0)
                //            {
                //                cmd.now = 0;
                //            }
                //            retry++;
                //            if (retry >= retry_times)
                //            {
                //                cmd.now++;
                //                l1text = "失败 请排除硬件故障";
                //                //if (cmd.finish)
                //                {
                //                    while (true) ;
                //                }
                //            }
                //            else
                //            {
                //                l1text = "超时" + timeout + "毫秒 已尝试" + retry + "/" + retry_times + "次";
                //            }
                //            timer.Start();
                //            autoResetEvent.WaitOne();
                //            Thread.Sleep(cmd_interval + 1000);
                //            continue;
                //        }
                //    }
                
            
        }

        private void button2_Click(object sender, EventArgs e)
        {
            skip = true;
        }

        private void Form3_FormClosing(object sender, FormClosingEventArgs e)
        {
            //thd.Abort();
            com.Close();
            skip = true;
            Dispose();
        }

        int step = 0;
        bool responsed = false;
        int retry = 0;

        private void timer2_Tick(object sender, EventArgs e)
        {
            if (com.IsOpen == false)
            { return; }
            timer2.Interval = 100;

            if (step == 0)
            {
                byte[] td = cmd.request[cmd.now];
                com.Write(td, 0, td.Length);
                addr = td[0];
                func = td[1];
                wt = DateTime.Now;
                l1text = "报文已发出 等待响应";
                step++;
            }
            else if (step == 1)
            {
                int j = -1;
                for (int i = 0; i < rbuf.Count; i++)
                {
                    if (rbuf[i] == addr)
                    {
                        j = i;
                        break;
                    }
                    j++;
                }
                    if (j >= 0)
                    {
                        byte[] tb = rbuf.ToArray();
                        int fc = tb[j + 1];
                        if (fc == 3)
                        {
                            int btr = tb[j + 2];
                            if (rbuf.Count >= j + 29)
                            {
                                byte[] tb1 = new byte[29];
                                for (int i = 0; i < tb1.Length; i++)
                                {
                                    tb1[i] = tb[j + i];
                                }
                                int crc = CRC16(tb1, 1 + 1 + 1 + 2 * 12);
                                byte crch = (byte)(crc >> 8);
                                byte crcl = (byte)(crc & 255);
                                if (crch == tb1[27] && crcl == tb1[28])
                                {
                                    int offset = cmd.cindex[cmd.now].Y;
                                    l1text = "已得到正确响应03";
                                    responsed = true;
                                    int r = tb1[3 + offset * 2];
                                    r <<= 8;
                                    r |= tb1[4 + offset * 2];
                                    cmd.result[cmd.now] = r;
                                    rbuf.RemoveRange(j, 29);
                                    step = 0;
                                }
                            }
                        }
                        else if (fc == 6)
                        {
                            if (rbuf.Count >= j + 8)
                            {
                                byte[] tb1 = new byte[8];
                                for (int i = 0; i < tb1.Length; i++)
                                {
                                    tb1[i] = tb[j + i];
                                }
                                int crc = CRC16(tb1, 2 + 2 + 2);
                                byte crch = (byte)(crc >> 8);
                                byte crcl = (byte)(crc & 255);
                                if (crch == tb1[6] && crcl == tb1[7])
                                {
                                    l1text = "已得到正确响应06";
                                    responsed = true;
                                    int r = tb1[4];
                                    r <<= 8;
                                    r |= tb1[5];
                                    cmd.result[cmd.now] = r;
                                    rbuf.RemoveRange(j, 8);
                                    step = 0;
                                }
                            }
                        }
                    }
                if (skip)
                {
                    retry = 0;
                    timer2.Interval = 1000;
                    responsed = false;
                    l1text = "手动跳过";
                    skip = false;
                    cmd.now++;
                    if (cmd.finish)
                    {
                        DialogResult = DialogResult.Abort;
                    }
                    step = 0;
                }
                if (responsed)
                {
                    timer2.Interval = cmd_interval;
                    responsed = false;
                    retry = 0;
                    cmd.now++;
                    if (cmd.finish)
                    {
                        DialogResult = DialogResult.OK;
                    }
                }
                else
                {                    
                    if (et.TotalMilliseconds > timeout)
                    {
                        timer2.Interval = 3000;
                        if (retry_times > 0)
                        {
                            //cmd.now--;
                            //if (cmd.now < 0)
                            //{
                            //    cmd.now = 0;
                            //}
                            retry++;
                            if (retry >= retry_times)
                            {
                                retry = 0;
                                cmd.now++;
                                l1text = "失败跳过 请排除硬件故障";
                                if (cmd.finish)
                                {
                                    DialogResult = DialogResult.Abort;
                                }
                            }
                            else
                            {
                                if (func == 6)
                                {
                                    cmd.now--;
                                    if (cmd.now < 0)
                                    {
                                        cmd.now = 0;
                                    }
                                }
                                l1text = "超时" + timeout + "毫秒 已尝试" + retry + "/" + retry_times + "次";
                                step = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}
