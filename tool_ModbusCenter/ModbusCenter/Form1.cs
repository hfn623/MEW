using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ModbusCenter
{
    public partial class Form1 : Form
    {
        int cow = 12;
        bool busy;
        Image img_on;
        Image img_off;
        Image img_error;
        static int node_max_count = 252;
        List<Node> nodes = new List<Node>();

        DateTime start_time;
        string port_name = "COM3";
        int baudrate = 9600;
        Command command;

        List<Point> selected_k = new List<Point>();

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            
            try
            {
                toolStripTextBox1.Text = port_name = System.IO.File.ReadAllText("config.ini");
            }
            catch
            {
                toolStripTextBox1.Text = port_name = "COM1";
            }
            Bitmap timg = new Bitmap(Image.FromFile("light_on.png"), 32, 32);
            img_on = timg;

            timg = new Bitmap(Image.FromFile("light_off.png"), 32, 32);
            img_off = timg;

            timg = new Bitmap(Image.FromFile("error.png"), 32, 32);
            img_error = timg;

            start_time = DateTime.Now;

            toolStripStatusLabel1.Text = "就绪";
            dataGridView1.Font = new Font("微软雅黑", 16);
            dataGridView1.RowTemplate.Height = 40;

            command = new Command();

            //byte[] td1 = new byte[3];
            //byte[] td2 = new byte[3];

            //td1[0] = 1;
            //td1[1] = 2;
            //td1[2] = 3;

            //td2[0] = 1;
            //td2[1] = 2;
            //td2[2] = 3;


            //if (td1.Equals(td2))
            //{
            timer1.Enabled = true;
            //}
            //else
            //{
            //    timer1.Enabled = false;
            //}

            try
            {
                toolStripTextBox2.Text = System.IO.File.ReadAllText("intval.ini");
                timer2.Interval = int.Parse(toolStripTextBox2.Text)*1000;
            }
            catch
            {
                toolStripTextBox2.Text = "30";
                timer2.Interval = int.Parse(toolStripTextBox2.Text)*1000;
            }

            if (System.IO.File.Exists("oi.dat"))
            {
                this.Text = (string)Deserialize(System.IO.File.ReadAllBytes("oi.dat"));
            }
            else
            {
                //Close();
                return;
            }
        }
        public static object Deserialize(byte[] data)
        {
            if (data == null)
            {
                return null;
            }
            System.Runtime.Serialization.Formatters.Binary.BinaryFormatter deserializer = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();
            System.IO.MemoryStream memStream = new System.IO.MemoryStream(data);
            object newobj = deserializer.Deserialize(memStream);
            memStream.Close();
            return newobj;
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

        byte[] modbus03(int addr, int reg, int cnt)
        {
            byte[] td = new byte[8];
            td[0] = (byte)addr;
            td[1] = 3;
            td[2] = (byte)(reg >> 8);
            td[3] = (byte)reg;
            td[4] = (byte)(cnt >> 8);
            td[5] = (byte)cnt;
            int crc = CRC16(td, 6);
            td[6] = (byte)(crc >> 8);
            td[7] = (byte)crc;

            return td;
        }
        byte[] modbus06(int addr, int reg, int val)
        {
            byte[] td = new byte[8];
            td[0] = (byte)addr;
            td[1] = 6;
            td[2] = (byte)(reg >> 8);
            td[3] = (byte)reg;
            td[4] = (byte)(val >> 8);
            td[5] = (byte)val;
            int crc = CRC16(td, 6);
            td[6] = (byte)(crc >> 8);
            td[7] = (byte)crc;

            return td;
        }

        void update_gui()
        {
            //if (sort)
            //{
            //    for (int i = 0; i < nodes.Count; i++)
            //    {
            //        int j = nodes.Count - 1 - i;
            //        dataGridView1.Rows[i].Cells[0].Value = j + 1;
            //        dataGridView1.Rows[i].Cells[1].Value = nodes[j].name;
            //        dataGridView1.Rows[i].Cells[2].Value = nodes[j].addr;

            //        for (int k = 0; k < 8; k++)
            //        {
            //            if (nodes[j].value[k])
            //            {
            //                dataGridView1.Rows[i].Cells[3 + k ].Value = img_on;
            //            }
            //            else
            //            {
            //                dataGridView1.Rows[i].Cells[3 + k ].Value = img_off;
            //            }
            //        }
            //    }
            //}
            //else
            {
                for (int i = 0; i < nodes.Count; i++)
                {
                    if (i % 2 == 0)
                    {
                        dataGridView1.Rows[i].DefaultCellStyle.BackColor = Color.LightBlue;
                    }
                    else
                    {
                        dataGridView1.Rows[i].DefaultCellStyle.BackColor = Color.LightCyan;
                    }
                    dataGridView1.Rows[i].Cells[0].Value = i + 1;
                    dataGridView1.Rows[i].Cells[1].Value = nodes[i].name;
                    dataGridView1.Rows[i].Cells[2].Value = nodes[i].addr;


                    for (int k = 0; k < cow; k++)
                    {
                        //dataGridView1.Rows[i].Cells[3 + k].
                        string week = "";
                        if ((nodes[i].week[k] & 0x40) == 0x40)
                        {
                            week += "星期一 ";
                        }
                        if ((nodes[i].week[k] & 0x20) == 0x20)
                        {
                            week += "星期二 ";
                        }
                        if ((nodes[i].week[k] & 0x10) == 0x10)
                        {
                            week += "星期三 ";
                        }
                        if ((nodes[i].week[k] & 8) == 8)
                        {
                            week += "星期四 ";
                        }
                        if ((nodes[i].week[k] & 4) == 4)
                        {
                            week += "星期五 ";
                        }
                        if ((nodes[i].week[k] & 2) == 2)
                        {
                            week += "星期六 ";
                        }
                        if ((nodes[i].week[k] & 1) == 1)
                        {
                            week += "星期日 ";
                        }
                        dataGridView1.Rows[i].Cells[3 + k].ToolTipText = /*"备注：" + nodes[i].name + */"触点名：" + nodes[i].names[k] + "\r\n地址：" + nodes[i].addr + "." + (k + 1) + "\r\n\r\n" + week + "\r\n\r\n开启时间：" + nodes[i].openTime[k].ToString("HH:mm:ss") + "\r\n" + "关断时间：" + nodes[i].closeTime[k].ToString("HH:mm:ss") + "\r\n\r\n激活时间控制：" + nodes[i].enableTime[k];
                        if (nodes[i].inited[k] && nodes[i].enable[k])
                        {
                            if (nodes[i].getValue(k))
                            {
                                dataGridView1.Rows[i].Cells[3 + k].Value = img_on;
                            }
                            else
                            {
                                dataGridView1.Rows[i].Cells[3 + k].Value = img_off;
                            }
                        }
                        else
                        {
                            dataGridView1.Rows[i].Cells[3 + k].Value = img_error;
                        }
                    }
                }
            }
            toolStripStatusLabel2.Text = "" + DateTime.Now.ToString() + "    ";
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            update_gui();
        }

        private void 添加ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (nodes.Count < node_max_count)
            {
                Node node = new Node();
                nodes.Add(node);
                dataGridView1.Rows.Add();
            }
        }

        private void 删除ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int j = 0;
            int n = dataGridView1.SelectedRows.Count;
            for (int i = 0; i < n; i++)
            {
                nodes.RemoveAt(dataGridView1.SelectedRows[i].Index);

            }
            for (int i = 0; i < n; i++)
            {
                dataGridView1.Rows.RemoveAt(0);
            }
            dataGridView1.Refresh();

            //select_row_index = (int)dataGridView1.SelectedCells[0].RowIndex;

            toolStripStatusLabel1.Text = "已删除";
        }

        private void dataGridView1_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            try
            {
                if (e.ColumnIndex == 1)
                {
                    nodes[e.RowIndex].name = (string)dataGridView1.Rows[e.RowIndex].Cells[1].Value;
                }
                if (e.ColumnIndex == 2)
                {
                    string tmpstr = (string)dataGridView1.Rows[e.RowIndex].Cells[2].Value;
                    nodes[e.RowIndex].addr = int.Parse(tmpstr);
                    //string[] tmpstrs = tmpstr.Split(new string[] { "." }, StringSplitOptions.RemoveEmptyEntries);
                    //if (tmpstrs.Length == 2)
                    //{
                    //    int a = int.Parse(tmpstrs[0]);
                    //    int c = int.Parse(tmpstrs[1]);
                    //    if (a < 256 && c < 8)
                    //    {
                    //        nodes[selected_rows_index[0]].addr = a;
                    //        nodes[selected_rows_index[0]].channel = c;
                    //    }
                    //}
                }
            }
            catch { }
        }

        void selected_copy()
        {
            selected_k.Clear();
            for (int i = 0; i < dataGridView1.SelectedCells.Count; i++)
            {
                if (dataGridView1.SelectedCells[i].ColumnIndex >= 3)
                {
                    Point p = new Point();
                    p.X = dataGridView1.SelectedCells[i].ColumnIndex - 3;
                    p.Y = dataGridView1.SelectedCells[i].RowIndex;
                    selected_k.Add(p);
                }
            }


            //if (sort)
            //{
            //    for (int i = 0; i < dataGridView1.SelectedRows.Count; i++)
            //    {
            //        if (dataGridView1.SelectedRows[i].Cells[0].Value != null)
            //        {
            //            selected_rows_index.Insert(0,dataGridView1.Rows.Count - 1 - dataGridView1.SelectedRows[i].Index);
            //        }
            //    }
            //}
            //else
            //{
            //    for (int i = 0; i < dataGridView1.SelectedRows.Count; i++)
            //    {
            //        if (dataGridView1.SelectedRows[i].Cells[0].Value != null)
            //        {
            //            selected_rows_index.Insert(0,dataGridView1.SelectedRows[i].Index);
            //        }
            //    }
            //}
        }

        private void dataGridView1_SelectionChanged(object sender, EventArgs e)
        {
            selected_copy();
            //for (int i = 0; i < selected_rows_index.Count; i++)
            //{
            //    if (i == 0)
            //    {
            //        tmpstr = "" + (selected_rows_index[i] + 1) + "" + tmpstr;
            //    }
            //    else
            //    {
            //        tmpstr = "" + (selected_rows_index[i] + 1) + "," + tmpstr;
            //    }
            //}
            toolStripStatusLabel1.Text = "已选中" + selected_k.Count + "个触点";
        }

        private void 初始化ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            nodes.Clear();
            dataGridView1.Rows.Clear();
            for (int i = 0; i < node_max_count; i++)
            {
                Node node = new Node();
                nodes.Add(node);
                dataGridView1.Rows.Add();
                //nodes[i].name = "name" + (i + 1);
            }
        }

        private void 保存ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.Filter = "*.scn|*.scn";
            if (sfd.ShowDialog() == DialogResult.OK)
            {
                string text = "";
                for (int i = 0; i < nodes.Count; i++)
                {
                    string line = "";
                    line += nodes[i].name + ",";
                    line += nodes[i].addr + ",";

                    for (int j = 0; j < cow; j++)
                    {
                        line += nodes[i].names[j] + ",";
                        line += nodes[i].getValue(j) + ",";
                        line += nodes[i].enableTime[j] + ",";
                        line += nodes[i].week[j] + ",";
                        line += nodes[i].openTime[j] + ",";
                        line += nodes[i].closeTime[j] + ",";
                        line += nodes[i].enable[j] + ",";
                        line += nodes[i].single_cmd[j] + ",";
                        line += nodes[i].keep_cmd[j] + ",";
                    }

                    text += line + "\r\n";
                }
                System.IO.File.WriteAllText(sfd.FileName, text);
            }
        }

        private void 载入ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "*.scn|*.scn";
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                nodes.Clear();
                dataGridView1.Rows.Clear();
                string text = System.IO.File.ReadAllText(ofd.FileName);
                string[] lines = text.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
                for (int i = 0; i < lines.Length; i++)
                {
                    if (i >= node_max_count)
                    {
                        break;
                    }
                    dataGridView1.Rows.Add();
                    Node node = new Node();
                    string[] words = lines[i].Split(new string[] { "," }, StringSplitOptions.RemoveEmptyEntries);

                    node.name = words[0];
                    node.addr = int.Parse(words[1]);
                    int valnum = 9;
                    for (int j = 0; j < cow; j++)
                    {
                        node.names[j] = words[2 + j * valnum];
                        node.value[j] = bool.Parse(words[2 + j * valnum + 1]);
                        node.enableTime[j] = bool.Parse(words[2 + j * valnum + 2]);
                        node.week[j] = int.Parse(words[2 + j * valnum + 3]);
                        node.openTime[j] = DateTime.Parse(words[2 + j * valnum + 4]);
                        node.closeTime[j] = DateTime.Parse(words[2 + j * valnum + 5]);
                        node.enable[j] = bool.Parse(words[2 + j * valnum + 6]);
                        node.single_cmd[j] = bool.Parse(words[2 + j * valnum + 7]);
                        node.keep_cmd[j] = bool.Parse(words[2 + j * valnum + 8]);
                    }
                    nodes.Add(node);
                }
            }
        }

        private void dataGridView1_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {

            }
            else if (selected_k.Count == 1)
            {

            }
        }

        private void dataGridView1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            //try
            //{
            command.Reset();
            for (int i = 0; i < selected_k.Count; i++)
            {
                if (nodes[selected_k[i].Y].getValue(selected_k[i].X))
                {
                    byte[] td;                    
                    //td = modbus03(nodes[selected_k[i].Y].addr, selected_k[i].X + 3, 8);
                    //command.Insert(td, selected_k[i].Y, selected_k[i].X);
                    td = modbus06(nodes[selected_k[i].Y].addr, selected_k[i].X + 3, 0);
                    command.Insert(td, selected_k[i].Y, selected_k[i].X);

                }
                else
                {
                    byte[] td;
                    //td = modbus03(nodes[selected_k[i].Y].addr, selected_k[i].X + 3, 8);
                    //command.Insert(td, selected_k[i].Y, selected_k[i].X);
                    td = modbus06(nodes[selected_k[i].Y].addr, selected_k[i].X + 3, 1);
                    command.Insert(td, selected_k[i].Y, selected_k[i].X);
                }
            }
            Form3 form3 = new Form3(port_name, command);
            if (form3.ShowDialog() == DialogResult.OK)
            {
                int i = 0;
                if (command.result[i] == 1)
                {
                    nodes[command.cindex[i].X].setValue(command.cindex[i].Y, true);
                }
                else if (command.result[i] == -1)
                {
                    nodes[command.cindex[i].X].inited[command.cindex[i].Y] = false;
                }
                else
                {
                    nodes[command.cindex[i].X].setValue(command.cindex[i].Y, false);
                }
            }
            last_refresh_time = DateTime.Now; 
            command.Reset();
            //}
            //catch { }
            timer2.Stop();
            timer2.Start();
        }

        private void toolStripButton9_Click(object sender, EventArgs e)
        {
            //if (selected_k.Count == 1)
            //{
            //    toolStripStatusLabel1.Text = "定时参数设置";
            //    DateTime ot = nodes[selected_k[0].Y].openTime[selected_k[0].X];
            //    DateTime ct = nodes[selected_k[0].Y].closeTime[selected_k[0].X];
            //    Form2 form2 = new Form2(nodes[selected_k[0].Y], selected_k[0].X);
            //    form2.ShowDialog();
            //}

            toolStripStatusLabel1.Text = "定时参数设置";
            DateTime ot = nodes[selected_k[0].Y].openTime[selected_k[0].X];
            DateTime ct = nodes[selected_k[0].Y].closeTime[selected_k[0].X];
            Form2 form2 = new Form2(nodes[selected_k[0].Y], selected_k[0].X);
            form2.ShowDialog();

            for (int i = 0; i < selected_k.Count; i++)
            {
                //if (nodes[selected_k[i].Y].enable[selected_k[i].X] == false)
                //{
                //    continue;
                //}
                nodes[selected_k[i].Y].keep_cmd = nodes[selected_k[0].Y].keep_cmd;
                nodes[selected_k[i].Y].single_cmd = nodes[selected_k[0].Y].single_cmd;
                nodes[selected_k[i].Y].week = nodes[selected_k[0].Y].week;
                nodes[selected_k[i].Y].openTime = nodes[selected_k[0].Y].openTime;
                nodes[selected_k[i].Y].closeTime = nodes[selected_k[0].Y].closeTime;
                nodes[selected_k[i].Y].enableTime = nodes[selected_k[0].Y].enableTime;
                nodes[selected_k[i].Y].enable = nodes[selected_k[0].Y].enable;
                //byte[] td;
                //td = modbus03(nodes[selected_k[i].Y].addr, selected_k[i].X + 3, 8);
                //command.Insert(td, selected_k[i].Y, selected_k[i].X);
                //td = modbus06(nodes[selected_k[i].Y].addr, selected_k[i].X + 3, 1);
                //command.Insert(td, selected_k[i].Y, selected_k[i].X);
            }
        }

        private void 刷新状态ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            command.Reset();
            for (int i = 0; i < nodes.Count; i++)
            {
                for (int j = 0; j < nodes[i].value.Length; j++)
                {
                    if (nodes[i].enable[j] == false)
                    {
                        continue;
                    }
                    byte[] td = modbus03(nodes[i].addr, 3, cow);
                    command.Insert(td, i, j);
                }
            } if (command.result.Count > 0)
            {
                Form3 form3 = new Form3(port_name, command);
                if (form3.ShowDialog() == DialogResult.OK)
                {
                    for (int i = 0; i < command.result.Count; i++)
                    {
                        if (command.result[i] == 1)
                        {
                            nodes[command.cindex[i].X].setValue(command.cindex[i].Y, true);
                        }
                        else if (command.result[i] == -1)
                        {
                            nodes[command.cindex[i].X].inited[command.cindex[i].Y] = false;
                        }
                        else
                        {
                            nodes[command.cindex[i].X].setValue(command.cindex[i].Y, false);
                        }
                    }
                }
                last_refresh_time = DateTime.Now; 
            }
            command.Reset();
            timer2.Stop();
            timer2.Start();
        }
     
        private void 刷新选中节点状态ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            command.Reset();
            for (int i = 0; i < selected_k.Count; i++)
            {
                if (nodes[selected_k[i].Y].enable[selected_k[i].X] == false)
                {
                    continue;
                }
                byte[] td = modbus03(nodes[selected_k[i].Y].addr, 3, cow);
                command.Insert(td, selected_k[i].Y, selected_k[i].X);
            }
            if (command.result.Count > 0)
            {
                Form3 form3 = new Form3(port_name, command);
                if (form3.ShowDialog() == DialogResult.OK)
                {
                    for (int i = 0; i < command.result.Count; i++)
                    {
                        if (command.result[i] == 1)
                        {
                            nodes[command.cindex[i].X].setValue(command.cindex[i].Y, true);
                        }
                        else if (command.result[i] == -1)
                        {
                            nodes[command.cindex[i].X].inited[command.cindex[i].Y] = false;
                        }
                        else
                        {
                            nodes[command.cindex[i].X].setValue(command.cindex[i].Y, false);
                        }
                    }
                }
                last_refresh_time = DateTime.Now; 
            }
            command.Reset();
            timer2.Stop();
            timer2.Start();
        }

        private void 全开ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            command.Reset();
            for (int i = 0; i < nodes.Count; i++)
            {
                for (int j = 0; j < nodes[i].value.Length; j++)
                {
                    if (nodes[i].enable[j] == false)
                    {
                        continue;
                    }
                    byte[] td;
                    //td = modbus03(nodes[i].addr, 3 + j, 8);
                    //command.Insert(td, i, j);
                    td = modbus06(nodes[i].addr, 3 + j, 1);
                    command.Insert(td, i, j);
                }
            }
            if (command.result.Count > 0)
            {
                Form3 form3 = new Form3(port_name, command);
                if (form3.ShowDialog() == DialogResult.OK)
                {
                    for (int i = 0; i < command.result.Count; i++)
                    {
                        if (command.result[i] == 1)
                        {
                            nodes[command.cindex[i].X].setValue(command.cindex[i].Y, true);
                        }
                        else if (command.result[i] == -1)
                        {
                            nodes[command.cindex[i].X].inited[command.cindex[i].Y] = false;
                        }
                        else
                        {
                            nodes[command.cindex[i].X].setValue(command.cindex[i].Y, false);
                        }
                    }
                }
                last_refresh_time = DateTime.Now; 
            }
            command.Reset();
            timer2.Stop();
            timer2.Start();
        }

        private void 全关ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            command.Reset();
            for (int i = 0; i < nodes.Count; i++)
            {
                for (int j = 0; j < nodes[i].value.Length; j++)
                {
                    if (nodes[i].enable[j] == false)
                    {
                        continue;
                    }
                    byte[] td;
                    //td = modbus03(nodes[i].addr, 3 + j, 8);
                    //command.Insert(td, i, j);
                    td = modbus06(nodes[i].addr, 3 + j, 0);
                    command.Insert(td, i, j);
                }
            }
            if (command.result.Count > 0)
            {
                Form3 form3 = new Form3(port_name, command);
                if (form3.ShowDialog() == DialogResult.OK)
                {
                    for (int i = 0; i < command.result.Count; i++)
                    {
                        if (command.result[i] == 1)
                        {
                            nodes[command.cindex[i].X].setValue(command.cindex[i].Y, true);
                        }
                        else if (command.result[i] == -1)
                        {
                            nodes[command.cindex[i].X].inited[command.cindex[i].Y] = false;
                        }
                        else
                        {
                            nodes[command.cindex[i].X].setValue(command.cindex[i].Y, false);
                        }
                    }
                }
                last_refresh_time = DateTime.Now; 
            }
            command.Reset();
            timer2.Stop();
            timer2.Start();
        }
   
        private void 选中开ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            command.Reset();
            for (int i = 0; i < selected_k.Count; i++)
            {
                if (nodes[selected_k[i].Y].enable[selected_k[i].X] == false)
                {
                    continue;
                }
                byte[] td;
                //td = modbus03(nodes[selected_k[i].Y].addr, selected_k[i].X + 3, 8);
                //command.Insert(td, selected_k[i].Y, selected_k[i].X);
                td = modbus06(nodes[selected_k[i].Y].addr, selected_k[i].X + 3, 1);
                command.Insert(td, selected_k[i].Y, selected_k[i].X);
            }
            if (command.result.Count > 0)
            {
                Form3 form3 = new Form3(port_name, command);
                if (form3.ShowDialog() == DialogResult.OK)
                {
                    for (int i = 0; i < command.result.Count; i++)
                    {
                        if (command.result[i] == 1)
                        {
                            nodes[command.cindex[i].X].setValue(command.cindex[i].Y, true);
                        }
                        else if (command.result[i] == -1)
                        {
                            nodes[command.cindex[i].X].inited[command.cindex[i].Y] = false;
                        }
                        else
                        {
                            nodes[command.cindex[i].X].setValue(command.cindex[i].Y, false);
                        }
                    }
                }
                last_refresh_time = DateTime.Now; 
            }
            command.Reset();
            timer2.Stop();
            timer2.Start();
            //dataGridView1.Update();
        }

        private void 选中关ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            command.Reset();
            for (int i = 0; i < selected_k.Count; i++)
            {
                if (nodes[selected_k[i].Y].enable[selected_k[i].X] == false)
                {
                    continue;
                }
                byte[] td;
                //td = modbus03(nodes[selected_k[i].Y].addr, selected_k[i].X + 3, 8);
                //command.Insert(td, selected_k[i].Y, selected_k[i].X);
                td = modbus06(nodes[selected_k[i].Y].addr, selected_k[i].X + 3, 0);
                command.Insert(td, selected_k[i].Y, selected_k[i].X);
            }
            if (command.result.Count > 0)
            {
                Form3 form3 = new Form3(port_name, command);
                if (form3.ShowDialog() == DialogResult.OK)
                {
                    for (int i = 0; i < command.result.Count; i++)
                    {
                        if (command.result[i] == 1)
                        {
                            nodes[command.cindex[i].X].setValue(command.cindex[i].Y, true);
                        }
                        else if (command.result[i] == -1)
                        {
                            nodes[command.cindex[i].X].inited[command.cindex[i].Y] = false;
                        }
                        else
                        {
                            nodes[command.cindex[i].X].setValue(command.cindex[i].Y, false);
                        }
                    }
                }
                last_refresh_time = DateTime.Now; 
            }
            command.Reset();
            timer2.Stop();
            timer2.Start();
            //dataGridView1.Update();
        }

        private void 串口ToolStripMenuItem_DropDownClosed(object sender, EventArgs e)
        {
            try
            {
                port_name = toolStripTextBox1.Text;
                System.IO.File.WriteAllText("config.ini", port_name);
            }
            catch { }
        }

        private void 关于ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //new Form4().ShowDialog();
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            timer2.Stop();
            //Form f = Application.OpenForms["Form3"];
            //if (f != null)
            //{
            //    return;
            //}
            //command.Reset();
            
            
            timer2.Start();
        }

        private void timer3_Tick(object sender, EventArgs e)
        {
            DateTime nt = DateTime.Now;
            if (nt.Year != 2015 || nt.Month != 12)
            {
                //return;
            }
            //command.Reset();
            for (int i = 0; i < nodes.Count; i++)
            {
                for (int j = 0; j < cow; j++)
                {
                    if (nodes[i].enable[j] == false)
                    {
                        continue;
                    }
                    if (nodes[i].enableTime[j])
                    {
                        if (nodes[i].single_cmd[j])
                        {
                            if ((nt.Year == nodes[i].openTime[j].Year) && (nt.Month == nodes[i].openTime[j].Month) && (nt.Day == nodes[i].openTime[j].Day) && (nt.Hour == nodes[i].openTime[j].Hour) && (nt.Minute == nodes[i].openTime[j].Minute) && (nt.Second == nodes[i].openTime[j].Second))
                            {
                                byte[] td;

                                td = modbus06(nodes[i].addr, j + 3, 1);
                                command.Insert(td, i, j);
                            }
                            if ((nt.Year == nodes[i].closeTime[j].Year) && (nt.Month == nodes[i].closeTime[j].Month) && (nt.Day == nodes[i].closeTime[j].Day) && (nt.Hour == nodes[i].closeTime[j].Hour) && (nt.Minute == nodes[i].closeTime[j].Minute) && (nt.Second == nodes[i].closeTime[j].Second))
                            {
                                byte[] td;
                                td = modbus06(nodes[i].addr, j + 3, 0);
                                command.Insert(td, i, j);
                            }
                        }
                        else
                        {
                            if ((nodes[i].week[j] & 0x40) == 0x40)
                            {
                                if (nt.DayOfWeek == DayOfWeek.Monday)
                                {
                                    if ((nt.Hour == nodes[i].openTime[j].Hour) && (nt.Minute == nodes[i].openTime[j].Minute) && (nt.Second == nodes[i].openTime[j].Second))
                                    {
                                        byte[] td;

                                        td = modbus06(nodes[i].addr, j + 3, 1);
                                        command.Insert(td, i, j);
                                    }
                                    if ((nt.Hour == nodes[i].closeTime[j].Hour) && (nt.Minute == nodes[i].closeTime[j].Minute) && (nt.Second == nodes[i].closeTime[j].Second))
                                    {
                                        byte[] td;
                                        td = modbus06(nodes[i].addr, j + 3, 0);
                                        command.Insert(td, i, j);
                                    }
                                }
                            }
                            if ((nodes[i].week[j] & 0x20) == 0x20)
                            {
                                if (nt.DayOfWeek == DayOfWeek.Tuesday)
                                {
                                    if ((nt.Hour == nodes[i].openTime[j].Hour) && (nt.Minute == nodes[i].openTime[j].Minute) && (nt.Second == nodes[i].openTime[j].Second))
                                    {
                                        byte[] td;
                                        td = modbus06(nodes[i].addr, j + 3, 1);
                                        command.Insert(td, i, j);
                                    }
                                    if ((nt.Hour == nodes[i].closeTime[j].Hour) && (nt.Minute == nodes[i].closeTime[j].Minute) && (nt.Second == nodes[i].closeTime[j].Second))
                                    {
                                        byte[] td;
                                        td = modbus06(nodes[i].addr, j + 3, 0);
                                        command.Insert(td, i, j);
                                    }
                                }
                            }
                            if ((nodes[i].week[j] & 0x10) == 0x10)
                            {
                                if (nt.DayOfWeek == DayOfWeek.Wednesday)
                                {
                                    if ((nt.Hour == nodes[i].openTime[j].Hour) && (nt.Minute == nodes[i].openTime[j].Minute) && (nt.Second == nodes[i].openTime[j].Second))
                                    {
                                        byte[] td;
                                        td = modbus06(nodes[i].addr, j + 3, 1);
                                        command.Insert(td, i, j);
                                    }
                                    if ((nt.Hour == nodes[i].closeTime[j].Hour) && (nt.Minute == nodes[i].closeTime[j].Minute) && (nt.Second == nodes[i].closeTime[j].Second))
                                    {
                                        byte[] td;
                                        td = modbus06(nodes[i].addr, j + 3, 0);
                                        command.Insert(td, i, j);
                                    }
                                }
                            }
                            if ((nodes[i].week[j] & 8) == 8)
                            {
                                if (nt.DayOfWeek == DayOfWeek.Thursday)
                                {
                                    if ((nt.Hour == nodes[i].openTime[j].Hour) && (nt.Minute == nodes[i].openTime[j].Minute) && (nt.Second == nodes[i].openTime[j].Second))
                                    {
                                        byte[] td;
                                        td = modbus06(nodes[i].addr, j + 3, 1);
                                        command.Insert(td, i, j);
                                    }
                                    if ((nt.Hour == nodes[i].closeTime[j].Hour) && (nt.Minute == nodes[i].closeTime[j].Minute) && (nt.Second == nodes[i].closeTime[j].Second))
                                    {
                                        byte[] td;
                                        td = modbus06(nodes[i].addr, j + 3, 0);
                                        command.Insert(td, i, j);
                                    }
                                }
                            }
                            if ((nodes[i].week[j] & 4) == 4)
                            {
                                if (nt.DayOfWeek == DayOfWeek.Friday)
                                {
                                    if ((nt.Hour == nodes[i].openTime[j].Hour) && (nt.Minute == nodes[i].openTime[j].Minute) && (nt.Second == nodes[i].openTime[j].Second))
                                    {
                                        byte[] td;
                                        td = modbus06(nodes[i].addr, j + 3, 1);
                                        command.Insert(td, i, j);
                                    }
                                    if ((nt.Hour == nodes[i].closeTime[j].Hour) && (nt.Minute == nodes[i].closeTime[j].Minute) && (nt.Second == nodes[i].closeTime[j].Second))
                                    {
                                        byte[] td;
                                        td = modbus06(nodes[i].addr, j + 3, 0);
                                        command.Insert(td, i, j);
                                    }
                                }
                            }
                            if ((nodes[i].week[j] & 2) == 2)
                            {
                                if (nt.DayOfWeek == DayOfWeek.Saturday)
                                {
                                    if ((nt.Hour == nodes[i].openTime[j].Hour) && (nt.Minute == nodes[i].openTime[j].Minute) && (nt.Second == nodes[i].openTime[j].Second))
                                    {
                                        byte[] td;
                                        td = modbus06(nodes[i].addr, j + 3, 1);
                                        command.Insert(td, i, j);
                                    }
                                    if ((nt.Hour == nodes[i].closeTime[j].Hour) && (nt.Minute == nodes[i].closeTime[j].Minute) && (nt.Second == nodes[i].closeTime[j].Second))
                                    {
                                        byte[] td;
                                        td = modbus06(nodes[i].addr, j + 3, 0);
                                        command.Insert(td, i, j);
                                    }
                                }
                            }
                            if ((nodes[i].week[j] & 1) == 1)
                            {
                                if (nt.DayOfWeek == DayOfWeek.Sunday)
                                {
                                    if ((nt.Hour == nodes[i].openTime[j].Hour) && (nt.Minute == nodes[i].openTime[j].Minute) && (nt.Second == nodes[i].openTime[j].Second))
                                    {
                                        byte[] td;
                                        td = modbus06(nodes[i].addr, j + 3, 1);
                                        command.Insert(td, i, j);
                                    }
                                    if ((nt.Hour == nodes[i].closeTime[j].Hour) && (nt.Minute == nodes[i].closeTime[j].Minute) && (nt.Second == nodes[i].closeTime[j].Second))
                                    {
                                        byte[] td;
                                        td = modbus06(nodes[i].addr, j + 3, 0);
                                        command.Insert(td, i, j);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            Form f = Application.OpenForms["Form3"];
            if (f != null)
            {
                return;
            }
            if (command.request.Count > 0)
            {
                Form3 form3 = new Form3(port_name, command);
                if (form3.ShowDialog() == DialogResult.OK)
                {
                    for (int i = 0; i < command.result.Count; i++)
                    {
                        if (command.result[i] == 1)
                        {
                            nodes[command.cindex[i].X].setValue(command.cindex[i].Y, true);
                        }
                        else
                        {
                            nodes[command.cindex[i].X].setValue(command.cindex[i].Y, false);
                        }
                    }
                }
                command.Reset();
            }
            else if ((DateTime.Now - last_refresh_time).TotalMilliseconds >= timer2.Interval)
            {
                for (int i = 0; i < nodes.Count; i++)
                {
                    for (int j = 0; j < nodes[i].value.Length; j++)
                    {
                        byte[] td = modbus03(nodes[i].addr, 3, cow);
                        command.Insert(td, i, j);
                    }
                }
                if (command.request.Count > 0)
                {
                    Form3 form3 = new Form3(port_name, command);
                    if (form3.ShowDialog() == DialogResult.OK)
                    {
                        for (int i = 0; i < command.result.Count; i++)
                        {
                            if (command.result[i] == 1)
                            {
                                nodes[command.cindex[i].X].setValue(command.cindex[i].Y, true);
                            }
                            else
                            {
                                nodes[command.cindex[i].X].setValue(command.cindex[i].Y, false);
                            }
                        }
                    }
                    last_refresh_time = DateTime.Now; 
                    command.Reset();
                }
            }
        }
        DateTime last_refresh_time = DateTime.Now;

        private void toolStripMenuItem1_DropDownClosed(object sender, EventArgs e)
        {
            try
            {
                timer2.Interval = int.Parse(toolStripTextBox2.Text) * 1000;
                System.IO.File.WriteAllText("intval.ini", toolStripTextBox2.Text);
            }
            catch { }
        }
        
    }
}
