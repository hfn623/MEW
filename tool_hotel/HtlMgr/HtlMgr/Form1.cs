using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace HtlMgr
{
    public partial class Form1 : Form
    {
        List<room_info> rooms = new List<room_info>();
        void set_room_txt(Button btn, string name, string type, string st)
        {
            room_info ri = new room_info();
            btn.Text = "";
            btn.Text += name;
            btn.Text += "\n\n";
            ri.name = name;

            btn.Text += type;
            btn.Text += "\n\n";
            ri.type = type;

            btn.Text += st;

            ri.stat = st;
            //btn.DoubleClick += new EventHandler(btn_DoubleClick);
            //btn.MouseDoubleClick += new MouseEventHandler(btn_MouseDoubleClick);

            btn.Click += new EventHandler(btn_Click);

            //rooms.Add(ri);

            //btn.Text += "\n";
            if (type == "办公室")
            {
                btn.BackColor = Color.LightSkyBlue;
            }
            else if (type == "卫生间")
            {
                btn.BackColor = Color.LightSalmon;
            }
            else
            {
                if (st == "未安装")
                {
                    btn.BackColor = Color.LightCoral;
                }
                else if (st == "未入住")
                {
                    btn.BackColor = Color.LightGreen;
                }
                else if (st == "-")
                {
                    btn.BackColor = Color.LightCoral;
                }
                else
                {
                    btn.BackColor = Color.Yellow;
                }
            }
        }

        void set_room_txt_2(Button btn, string name, string type, string st)
        {
            room_info ri = new room_info();
            btn.Text = "";
            btn.Text += name;
            btn.Text += "\n\n";
            ri.name = name;

            btn.Text += type;
            btn.Text += "\n\n";
            ri.type = type;

            btn.Text += st;

            ri.stat = st;
            //btn.DoubleClick += new EventHandler(btn_DoubleClick);
            //btn.MouseDoubleClick += new MouseEventHandler(btn_MouseDoubleClick);

            //btn.Click += new EventHandler(btn_Click);

            //rooms.Add(ri);

            //btn.Text += "\n";
            if (type == "办公室")
            {
                btn.BackColor = Color.LightSkyBlue;
            }
            else if (type == "卫生间")
            {
                btn.BackColor = Color.LightSalmon;
            }
            else
            {
                if (st == "未安装")
                {
                    btn.BackColor = Color.LightCoral;
                }
                else if (st == "未入住")
                {
                    btn.BackColor = Color.LightGreen;
                }
                else if (st == "-")
                {
                    btn.BackColor = Color.LightCoral;
                }
                else
                {
                    btn.BackColor = Color.Yellow;
                }
            }
        }

        void btn_Click(object sender, EventArgs e)
        {
            string title = ((Button)sender).Text;
            string[] words = title.Split(new string[] { "\n" }, StringSplitOptions.RemoveEmptyEntries);
            if (words[2] == "已入住")
            {
                if (MessageBox.Show(title + "\n\n确定退房?") == DialogResult.OK)
                {
                    int i = findIndexByRoomName(words[0]);
                    if (i >= 0)
                    {
                        rooms[i].card_count = 0;
                        saveRooms();
                        room_ref();
                    }
                }
            }
        }

        //void btn_MouseDoubleClick(object sender, MouseEventArgs e)
        //{
           
        //}

        //void btn_DoubleClick(object sender, EventArgs e)
        //{
           
        //    //throw new NotImplementedException();
        //}
        void saveRooms()
        {
            System.IO.File.Delete("rooms.dat");
            string str = "";
            for (int i = 0; i < rooms.Count; i++)
            {
                str += rooms[i].name + "," + rooms[i].type + "," + rooms[i].stat + "," + rooms[i].card_count + "\r\n";
            }
            System.IO.File.WriteAllText("rooms.dat", str);
        }
        void readRooms()
        {
            rooms.Clear();
            string[] lines = System.IO.File.ReadAllLines("rooms.dat");
            for (int l = 0; l < lines.Length; l++)
            {
                string[] words = lines[l].Split(new string[] { "," }, StringSplitOptions.RemoveEmptyEntries);
                if (words.Length == 4)
                {
                    room_info ri = new room_info();
                    ri.name = words[0];
                    ri.type = words[1];
                    ri.stat = words[2];
                    ri.card_count = int.Parse(words[3]);
                    rooms.Add(ri);
                }
            }
        }
        int findIndexByRoomName(string name)
        {
            for (int i = 0; i < rooms.Count; i++)
            {
                if (rooms[i].name == name)
                {
                    return i;
                }
            }
            return -1;
        }

        Button findBtnByName(string name)
        {
            
            for(int i=0;i<tabControl2.Controls.Count;i++)
            {
               // if (tabControl2.Controls[i] is TabControl)
                {
                    for (int j = 0; j < tabControl2.Controls[i].Controls.Count; j++)
                    {

                        //for (int k = 0; k < tabControl2.Controls[i].Controls[j].Controls.Count; k++)
                        {
                            if (((Button)tabControl2.Controls[i].Controls[j]).Text.StartsWith(name))
                            {
                                return (Button)tabControl2.Controls[i].Controls[j];
                            }
                        }
                    }
                }
            }
            return null;
        }
        void room_init()
        {
            //floor 2
            set_room_txt(button5, "报告厅（二）", "72人", "未安装");
            set_room_txt(button6, "会议室VIP1", "30人", "未安装");
            set_room_txt(button7, "1211", "办公室", "未安装");
            set_room_txt(button8, "1210", "办公室", "未安装");
            set_room_txt(button9, "1209", "办公室", "未安装");
            set_room_txt(button10, "男", "卫生间", "未安装");
            set_room_txt(button11, "女", "卫生间", "未安装");
            set_room_txt(button12, "208", "双标", "未开卡");
            set_room_txt(button13, "207", "双标", "未开卡");
            set_room_txt(button14, "206", "双标", "未开卡");
            set_room_txt(button15, "会议室VIP1", "30人", "未安装");
            set_room_txt(button16, "-", "-", "-");
            set_room_txt(button17, "205", "办公室", "未开卡");
            set_room_txt(button18, "203", "双标", "未开卡");
            set_room_txt(button19, "202", "双标", "未开卡");
            set_room_txt(button20, "201", "双标", "未开卡");
            set_room_txt(button21, "报告厅（一）", "144人", "未安装");

            //floor 3
            set_room_txt(button22, "324", "办公室", "未开卡");
            set_room_txt(button23, "322", "双标", "未开卡");
            set_room_txt(button24, "320", "双标", "未开卡");
            set_room_txt(button25, "318", "双标", "未开卡");
            set_room_txt(button26, "1301", "办公室", "未开卡");
            set_room_txt(button27, "316", "双标", "未开卡");
            set_room_txt(button28, "312", "双标", "未开卡");
            set_room_txt(button29, "男女", "卫生间", "未安装");
            set_room_txt(button30, "310", "双标", "未开卡");
            set_room_txt(button31, "308", "双标", "未开卡");
            set_room_txt(button32, "306", "双标", "未开卡");
            set_room_txt(button33, "302", "双标", "未开卡");

            set_room_txt(button34, "简报采编中心", "", "未安装");
            set_room_txt(button35, "315", "双标", "未开卡");
            set_room_txt(button36, "311", "双标", "未开卡");
            set_room_txt(button37, "309", "双标", "未开卡");
            set_room_txt(button38, "307", "大床", "未开卡");
            set_room_txt(button39, "305", "双标", "未开卡");
            set_room_txt(button40, "303", "两间套", "未开卡");
            set_room_txt(button41, "301", "三间套", "未开卡");
            set_room_txt(button42, "活动室", "-", "未安装");

            //floor 4
            set_room_txt(button43, "424", "双标", "未开卡");
            set_room_txt(button44, "422", "双标", "未开卡");
            set_room_txt(button45, "420", "双标", "未开卡");
            set_room_txt(button46, "418", "双标", "未开卡");
            set_room_txt(button47, "1401", "办公室", "未开卡");
            set_room_txt(button48, "416", "双标", "未开卡");
            set_room_txt(button49, "412", "双标", "未开卡");
            set_room_txt(button50, "男女", "卫生间", "未安装");
            set_room_txt(button51, "410", "双标", "未开卡");
            set_room_txt(button52, "408", "双标", "未开卡");
            set_room_txt(button53, "406", "双标", "未开卡");
            set_room_txt(button54, "402", "双标", "未开卡");

            set_room_txt(button55, "429", "双标", "未开卡");
            set_room_txt(button56, "427", "双标", "未开卡");
            set_room_txt(button57, "425", "双标", "未开卡");
            set_room_txt(button58, "423", "双标", "未开卡");
            set_room_txt(button59, "421", "双标", "未开卡");
            set_room_txt(button60, "419", "双标", "未开卡");
            set_room_txt(button61, "417", "双标", "未开卡");
            set_room_txt(button62, "415", "双标", "未开卡");
            set_room_txt(button63, "411", "双标", "未开卡");
            set_room_txt(button64, "409", "双标", "未开卡");
            set_room_txt(button65, "407", "双标", "未开卡");
            set_room_txt(button66, "405", "双标", "未开卡");
            set_room_txt(button67, "403", "双标", "未开卡");
            set_room_txt(button68, "401", "双标", "未开卡");

            //floor 5
            set_room_txt(button69, "524", "三标", "未开卡");
            set_room_txt(button70, "522", "三标", "未开卡");
            set_room_txt(button71, "520", "三标", "未开卡");
            set_room_txt(button72, "518", "三标", "未开卡");
            set_room_txt(button73, "1501", "办公室", "未开卡");
            set_room_txt(button74, "516", "三标", "未开卡");
            set_room_txt(button75, "512", "三标", "未开卡");
            set_room_txt(button76, "男女", "卫生间", "未安装");
            set_room_txt(button77, "510", "三标", "未开卡");
            set_room_txt(button78, "508", "三标", "未开卡");
            set_room_txt(button79, "506", "三标", "未开卡");
            set_room_txt(button80, "502", "三标", "未开卡");

            set_room_txt(button81, "529", "三标", "未开卡");
            set_room_txt(button82, "527", "三标", "未开卡");
            set_room_txt(button83, "525", "三标", "未开卡");
            set_room_txt(button84, "523", "三标", "未开卡");
            set_room_txt(button85, "521", "三标", "未开卡");
            set_room_txt(button86, "519", "三标", "未开卡");
            set_room_txt(button87, "517", "三标", "未开卡");
            set_room_txt(button88, "515", "三标", "未开卡");
            set_room_txt(button89, "511", "双标", "未开卡");
            set_room_txt(button90, "509", "双标", "未开卡");
            set_room_txt(button91, "507", "双标", "未开卡");
            set_room_txt(button92, "505", "双标", "未开卡");
            set_room_txt(button93, "503", "双标", "未开卡");
            set_room_txt(button94, "501", "双标", "未开卡");

            //for (int i = 0; i < rooms.Count; i++)
            //{
            //    System.IO.File.AppendAllText("rooms.dat", rooms[i].name + "," + rooms[i].type + "," + rooms[i].stat + "," + rooms[i].card_count + "\r\n");
            //}

            
        }
        void room_ref()
        {
            readRooms();
            for (int i = 0; i < rooms.Count; i++)
            {

                Button b = findBtnByName(rooms[i].name);
                if (b != null)
                {
                    string st = rooms[i].stat;
                    if (st == "未安装")
                    {
                    }
                    else
                    {
                        if (rooms[i].card_count > 0)
                        {
                            st = "已入住";
                        }
                        else
                        {
                            st = "未入住";
                        }
                    }

                    set_room_txt_2(b, rooms[i].name, rooms[i].type, st);
                }
            }
        }

        int g_hReader;
        int g_retCode;
        byte[] tagType = new byte[100];
        byte tagLen;
        byte[] tagSN = new byte[100];

        byte[] rblock = new byte[16];

        bool card_conn;

        byte CARD_TYPE;
        ushort CARD_ROOM;
        ushort CARD_YER;
        byte CARD_MON;
        byte CARD_DAY;
        byte CARD_HOU;
        byte CARD_MIN;
        byte CARD_SEC;

        void make_room_card(ushort r,DateTime dt)
        { //制房间卡按钮
            byte[] tb = new byte[16];
            tb[0] = 4;
            tb[1] = (byte)(r >> 8);
            tb[2] = (byte)r;

            tb[3] = (byte)(dt.Year >> 8);
            tb[4] = (byte)dt.Year;
            tb[5] = (byte)dt.Month;
            tb[6] = (byte)dt.Day;
            tb[7] = (byte)dt.Hour;
            tb[8] = (byte)dt.Minute;
            tb[9] = (byte)dt.Second;

            g_retCode = ACR110U.ACR110_Write(g_hReader, 4, ref tb[0]);
            if (g_retCode == 0)
            {
                //textBox1.AppendText("[DEBUG] MAKE TIME CARD = " + g_retCode + "\n");

                System.IO.File.WriteAllText(nowsn + ".dat", dt + "");
                int i = findIndexByRoomName(r + "");
                if (i >= 0)
                {
                    rooms[i].card_count = 1;
                    saveRooms();
                    room_ref();
                }
                System.IO.File.AppendAllText("oplog\\" + dt.ToString("yyyy_MM_dd") + ".dat", dt + ": make room card sn=" + nowsn + ", room=" + r + "\r\n");
                textBox1.AppendText("[DEBUG] MAKE ROOM CARD DONE\n");
            }
        }

        void make_time_card()
        {//制时间卡按钮
            byte[] tb = new byte[16];
            tb[0] = 3;
            tb[3] = (byte)(ushort.Parse(textBox2.Text) >> 8);
            tb[4] = (byte)ushort.Parse(textBox2.Text);
            tb[5] = byte.Parse(textBox3.Text);
            tb[6] = byte.Parse(textBox4.Text);
            tb[7] = byte.Parse(textBox5.Text);
            tb[8] = byte.Parse(textBox6.Text);
            tb[9] = byte.Parse(textBox7.Text);

            g_retCode = ACR110U.ACR110_Write(g_hReader, 4, ref tb[0]);
            if (g_retCode == 0)
            {
                //textBox1.AppendText("[DEBUG] MAKE TIME CARD = " + g_retCode + "\n");
                textBox1.AppendText("[DEBUG] MAKE TIME CARD DONE\n");
            }
        }

        void make_admin_card()
        {//制管理卡按钮
            byte[] tb = new byte[16];
            tb[0] = 1;
            //tb[3] = (byte)(ushort.Parse(textBox2.Text) >> 8);
            //tb[4] = (byte)ushort.Parse(textBox2.Text);
            //tb[5] = byte.Parse(textBox3.Text);
            //tb[6] = byte.Parse(textBox4.Text);
            //tb[7] = byte.Parse(textBox5.Text);
            //tb[8] = byte.Parse(textBox6.Text);
            //tb[9] = byte.Parse(textBox7.Text);

            g_retCode = ACR110U.ACR110_Write(g_hReader, 4, ref tb[0]);
            if (g_retCode == 0)
            {
                //textBox1.AppendText("[DEBUG] MAKE TIME CARD = " + g_retCode + "\n");
                textBox1.AppendText("[DEBUG] MAKE ADMIN CARD DONE\n");
            }
        }

        void make_useless_card()
        {//制管理卡按钮
            byte[] tb = new byte[16];
            tb[0] = 0;
            //tb[3] = (byte)(ushort.Parse(textBox2.Text) >> 8);
            //tb[4] = (byte)ushort.Parse(textBox2.Text);
            //tb[5] = byte.Parse(textBox3.Text);
            //tb[6] = byte.Parse(textBox4.Text);
            //tb[7] = byte.Parse(textBox5.Text);
            //tb[8] = byte.Parse(textBox6.Text);
            //tb[9] = byte.Parse(textBox7.Text);

            g_retCode = ACR110U.ACR110_Write(g_hReader, 4, ref tb[0]);
            if (g_retCode == 0)
            {
                //textBox1.AppendText("[DEBUG] MAKE TIME CARD = " + g_retCode + "\n");
                textBox1.AppendText("[DEBUG] MAKE USELESS CARD DONE\n");
            }
        }

        void make_floor_card(byte floor)
        {
            //制楼层卡按钮
            byte[] tb = new byte[16];
            tb[0] = 2;
            tb[2] = floor;
            //tb[3] = (byte)(ushort.Parse(textBox2.Text) >> 8);
            //tb[4] = (byte)ushort.Parse(textBox2.Text);
            //tb[5] = byte.Parse(textBox3.Text);
            //tb[6] = byte.Parse(textBox4.Text);
            //tb[7] = byte.Parse(textBox5.Text);
            //tb[8] = byte.Parse(textBox6.Text);
            //tb[9] = byte.Parse(textBox7.Text);

            g_retCode = ACR110U.ACR110_Write(g_hReader, 4, ref tb[0]);
            if (g_retCode == 0)
            {
                //textBox1.AppendText("[DEBUG] MAKE TIME CARD = " + g_retCode + "\n");
                textBox1.AppendText("[DEBUG] MAKE FLOOR CARD DONE\n");
            }
        }

        string nowsn = "";
        bool select_login()
        {
            nowsn = "";
            g_retCode = ACR110U.ACR110_Select(g_hReader, ref tagType[0], ref tagLen, ref tagSN[0]);

            textBox1.AppendText("[DEBUG] SELECT CARD = " + g_retCode + "\n");
            textBox1.AppendText("[DEBUG] CARD TYPE = " + ACR110U.GetTagType1(tagType[0]) + "\n");
            string str = "";
            for (int i = 0; i < tagLen; i++)
            {
                str += tagSN[i].ToString("X2") + " ";
            }
            nowsn = str;
            textBox1.AppendText("[DEBUG] CARD SN = " + str + "\n");
            if (g_retCode == 0)
            {
                byte[] key = new byte[6] { 0xab, 0xab, 0xab, 0xab, 0xab, 0xab };
                g_retCode = ACR110U.ACR110_Login(g_hReader, 1, ACR110U.ACR110_LOGIN_KEYTYPE_A, 0, ref key[0]);

                textBox1.AppendText("[DEBUG] LOGIN = " + g_retCode + "\n");
                if (g_retCode == 0)
                {
                    return true;
                }

            }
            return false;
        }

        void read_card()
        {
            textBox8.Clear();


            g_retCode = ACR110U.ACR110_Read(g_hReader, (byte)4, ref rblock[0]);

            if (g_retCode == 0)
            {

                textBox8.Clear();

                CARD_TYPE = rblock[0];
                textBox8.AppendText("卡类型：");

                if (CARD_TYPE == 1)
                {
                    textBox8.AppendText("管理卡\n");
                    textBox8.AppendText("权  限：全部");
                }
                else if (CARD_TYPE == 2)
                {
                    textBox8.AppendText("楼层卡\n");
                    CARD_ROOM = rblock[1];
                    CARD_ROOM <<= 8;
                    CARD_ROOM |= rblock[2];
                    textBox8.AppendText("权  限：楼层");
                    textBox8.AppendText("" + CARD_ROOM);
                }
                else if (CARD_TYPE == 3)
                {
                    textBox8.AppendText("时间卡\n");

                    CARD_YER = rblock[3];
                    CARD_YER <<= 8;
                    CARD_YER |= rblock[4];
                    CARD_MON = rblock[5];
                    CARD_DAY = rblock[6];

                    CARD_HOU = rblock[7];
                    CARD_MIN = rblock[8];
                    CARD_SEC = rblock[9];

                    textBox8.AppendText("时  间：");
                    textBox8.AppendText(CARD_YER + "-" + CARD_MON + "-" + CARD_DAY + " " + CARD_HOU + ":" + CARD_MIN + ":" + CARD_SEC + "");
                }
                else if (CARD_TYPE == 4)
                {
                    textBox8.AppendText("普通卡\n");
                    textBox8.AppendText("权  限：房间");

                    CARD_ROOM = rblock[1];
                    CARD_ROOM <<= 8;
                    CARD_ROOM |= rblock[2];
                    textBox8.AppendText("" + CARD_ROOM + "\n");
                    textBox8.AppendText("有效期：");

                    CARD_YER = rblock[3];
                    CARD_YER <<= 8;
                    CARD_YER |= rblock[4];
                    CARD_MON = rblock[5];
                    CARD_DAY = rblock[6];

                    CARD_HOU = rblock[7];
                    CARD_MIN = rblock[8];
                    CARD_SEC = rblock[9];

                    textBox8.AppendText(CARD_YER + "-" + CARD_MON + "-" + CARD_DAY + " " + CARD_HOU + ":" + CARD_MIN + ":" + CARD_SEC + "");
                }
                else
                {
                    textBox8.AppendText("无效卡\n");
                }

            }
        }

        public Form1()
        {
            InitializeComponent();
            g_hReader = -1;

            g_hReader = ACR110U.ACR110_Open(ACR110U.ACR110_USB1);

            textBox1.AppendText("[DEBUG] OPEN USB1 = " + g_hReader + "\n");

            if (g_hReader == 0)
            {
                toolStripStatusLabel1.Text = "读卡器已连接";
            }
            else
            {
                toolStripStatusLabel1.Text = "读卡器未连接";
            }

            tabControl1.SelectTab(0);

            room_init();
            room_ref();

            
        }

        private void 连接ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            g_hReader = ACR110U.ACR110_Open(ACR110U.ACR110_USB1);

            textBox1.AppendText("[DEBUG] OPEN USB1 = " + g_hReader + "\n");
        }

        private void 复位ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            card_conn = false;
            g_retCode = ACR110U.ACR110_Reset(g_hReader);

            textBox1.AppendText("[DEBUG] RESET " + g_hReader + " = " + g_retCode + "\n");
        }


        private void 读卡ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //if (!card_conn)
            {
                 select_login();
            }
            //if (card_conn)
            {
                read_card();
            }

            tabControl1.SelectTab(1);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            DateTime dt = DateTime.Now;

            textBox2.Text = "" + dt.Year;
            textBox3.Text = "" + dt.Month;
            textBox4.Text = "" + dt.Day;
            textBox5.Text = "" + dt.Hour;
            textBox6.Text = "" + dt.Minute;
            textBox7.Text = "" + dt.Second;

            if (g_hReader == -1)
            {
                textBox9.Text = "读卡器未连接";
            }
            else
            {
                textBox9.Text = "读卡器已连接";
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            timer1.Enabled = !checkBox1.Checked;
        }



        private void button1_Click(object sender, EventArgs e)
        {
            //if (!card_conn)
            {
                 select_login();
            }
            //if (card_conn)
            {
                make_time_card();
            }
        }

        private void tabControl1_SelectedIndexChanged(object sender, EventArgs e)
        {
            //翻页到制时间卡
            if (tabControl1.SelectedIndex == 1)
            {
                //if (!card_conn)
                {
                     select_login();
                }
                //if (card_conn)
                {
                    read_card();
                }
            }
            if (tabControl1.SelectedIndex == 2)
            {
                DateTime dt = DateTime.Now;
                DateTime dt2 = new DateTime(dt.Year, dt.Month, dt.Day, 14, 0, 0);
                dt2 = dt2.AddDays(1);
                dateTimePicker1.Value = dt2;
            }
            if (tabControl1.SelectedIndex == 3)
            {
                timer1.Enabled = true;
                checkBox1.Checked = false;
            }
        }

        private void 退出ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        string get_last_line(string[] lines)
        {
            string str = "";
            for (int i = 0; i < lines.Length; i++)
            {
                str = lines[lines.Length - 1 - i];
                if (str.Length > 0)
                {
                    return str;
                }
            }
            return null;
        }

        private void textBox1_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.KeyValue == 13)
            {
                string line = get_last_line(textBox1.Lines);
                if (line == "CARD_INIT")
                {

                    {
                        g_retCode = ACR110U.ACR110_Select(g_hReader, ref tagType[0], ref tagLen, ref tagSN[0]);

                        textBox1.AppendText("[DEBUG] SELECT CARD = " + g_retCode + "\n");
                        textBox1.AppendText("[DEBUG] CARD TYPE = " + ACR110U.GetTagType1(tagType[0]) + "\n");
                        string str = "";
                        for (int i = 0; i < tagLen; i++)
                        {
                            str += tagSN[i].ToString("X2") + " ";
                        }
                        textBox1.AppendText("[DEBUG] CARD SN = " + str + "\n");
                        byte[] key = new byte[6] { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
                        g_retCode = ACR110U.ACR110_Login(g_hReader, 1, ACR110U.ACR110_LOGIN_KEYTYPE_A, 0, ref key[0]);
                        if (g_retCode == 0)
                        {
                            byte[] wbuf = new byte[16] { 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xff, 0x07, 0x80, 0x69, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
                            g_retCode = ACR110U.ACR110_Write(g_hReader, 7, ref wbuf[0]);
                            if (g_retCode == 0)
                            {
                                textBox1.AppendText("CARD_INIT DONE\n");
                            }
                            else
                            {
                                textBox1.AppendText("CARD_INIT ERR 2\n");
                            }
                        }
                        else
                        {
                            textBox1.AppendText("CARD_INIT ERR 1\n");
                        }
                    }
                }
                else if (line == "CARD_ADMIN")
                {
                    //if (!card_conn)
                    //{
                    //    card_conn = 
                    //}
                    //if (card_conn)
                    {
                        select_login();
                        make_admin_card();
                    }
                }
                else if (line.StartsWith("CARD_FLOOR "))
                {
                    string[] words = line.Split(new string[] { " " }, StringSplitOptions.RemoveEmptyEntries);
                    if (words.Length == 2)
                    {
                        byte f = byte.Parse(words[1]);
                        //if (!card_conn)
                        //{
                        //    card_conn = select_login();
                        //}
                        //if (card_conn)
                        {
                            select_login();
                            make_floor_card(f);
                        }
                    }
                }
                else if (line == "CLS")
                {
                    textBox1.Clear();
                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (g_hReader == -1)
            {
                textBox9.Text = "读卡器未连接";
            }
            else
            {
                textBox9.Text = "读卡器已连接";
            }
        }

        private void button3_Click_1(object sender, EventArgs e)
        {
            //if (!card_conn)
            {
                 select_login();
            }
            //if (card_conn)
            {
                read_card();
            }

        }

        private void button4_Click_1(object sender, EventArgs e)
        {
            //制普通房卡
            //if (!card_conn)
            {
                select_login();
            }
            //if (card_conn)
            {
                
                make_room_card(ushort.Parse(comboBox1.Text),dateTimePicker1.Value);
            }
        }

        private void 销卡ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //销卡
            //if (!card_conn)
            {
                select_login();
            }
           // if (card_conn)
            {
                make_useless_card();
            }
        }
    }
}
