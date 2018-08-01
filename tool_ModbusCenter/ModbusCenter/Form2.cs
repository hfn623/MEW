using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ModbusCenter
{
    public partial class Form2 : Form
    {
        Node tnode;
        int i = 0;

        public Form2(Node node,int index)
        {
            InitializeComponent();
            tnode = node;
            i = index;
        }

        private void Form2_Load(object sender, EventArgs e)
        {
            checkBox10.Checked = tnode.keep_cmd[i];
            checkBox11.Checked = tnode.single_cmd[i];
            if (tnode.single_cmd[i])
            {
                panel1.Enabled = true;
                panel2.Enabled = false;
            }
            else
            {
                panel1.Enabled = false;
                panel2.Enabled = true;
            }

            checkBox2.Checked = false;
            checkBox3.Checked = false;
            checkBox4.Checked = false;
            checkBox5.Checked = false;
            checkBox6.Checked = false;
            checkBox7.Checked = false;
            checkBox8.Checked = false;
            checkBox9.Checked = false;

            if ((tnode.week[i] & 0x40) == 0x40)
            {
                checkBox2.Checked = true;
            }
            if ((tnode.week[i] & 0x20) == 0x20)
            {
                checkBox3.Checked = true;
            }
            if ((tnode.week[i] & 0x10) == 0x10)
            {
                checkBox4.Checked = true;
            }
            if ((tnode.week[i] & 0x8) == 0x8)
            {
                checkBox5.Checked = true;
            }
            if ((tnode.week[i] & 0x4) == 0x4)
            {
                checkBox6.Checked = true;
            }
            if ((tnode.week[i] & 0x2) == 0x2)
            {
                checkBox7.Checked = true;
            }
            if ((tnode.week[i] & 0x1) == 0x1)
            {
                checkBox8.Checked = true;
            }

            textBox7.Text = "" + tnode.names[i];
            textBox8.Text = "" + tnode.openTime[i].Year;
            textBox9.Text = "" + tnode.openTime[i].Month;
            textBox10.Text = "" + tnode.openTime[i].Day;
            textBox11.Text = "" + tnode.closeTime[i].Year;
            textBox12.Text = "" + tnode.closeTime[i].Month;
            textBox13.Text = "" + tnode.closeTime[i].Day;
            textBox1.Text = "" + tnode.openTime[i].Hour;
            textBox2.Text = "" + tnode.openTime[i].Minute;
            textBox3.Text = "" + tnode.openTime[i].Second;
            textBox4.Text = "" + tnode.closeTime[i].Hour;
            textBox5.Text = "" + tnode.closeTime[i].Minute;
            textBox6.Text = "" + tnode.closeTime[i].Second;

            checkBox1.Checked = tnode.enableTime[i];
            checkBox9.Checked = tnode.enable[i];


        }

        private void button1_Click(object sender, EventArgs e)
        {
            tnode.keep_cmd[i] = checkBox10.Checked;
            tnode.single_cmd[i] = checkBox11.Checked;

            if (checkBox2.Checked)
            {
                tnode.week[i] |= 0x40;
            }
            if (checkBox3.Checked)
            {
                tnode.week[i] |= 0x20;
            }
            if (checkBox4.Checked)
            {
                tnode.week[i] |= 0x10;
            }
            if (checkBox5.Checked)
            {
                tnode.week[i] |= 0x8;
            }
            if (checkBox6.Checked)
            {
                tnode.week[i] |= 0x4;
            }
            if (checkBox7.Checked)
            {
                tnode.week[i] |= 0x2;
            }
            if (checkBox8.Checked)
            {
                tnode.week[i] |= 0x1;
            }
            tnode.names[i] = textBox7.Text;
            tnode.openTime[i] = DateTime.Parse(textBox8.Text + "-" + textBox9.Text + "-" + textBox10.Text + " " + textBox1.Text + ":" + textBox2.Text + ":" + textBox3.Text);
            tnode.closeTime[i] = DateTime.Parse(textBox11.Text + "-" + textBox12.Text + "-" + textBox13.Text + " " + textBox4.Text + ":" + textBox5.Text + ":" + textBox6.Text);
            tnode.enableTime[i] = checkBox1.Checked;
            tnode.enable[i] = checkBox9.Checked;
            Close();
        }

        private void checkBox11_CheckedChanged(object sender, EventArgs e)
        {
            tnode.single_cmd[i] = checkBox11.Checked;
            if (tnode.single_cmd[i])
            {
                panel1.Enabled = true;
                panel2.Enabled = false;
            }
            else
            {
                panel1.Enabled = false;
                panel2.Enabled = true;
            }
        }

        private void checkBox10_CheckedChanged(object sender, EventArgs e)
        {

        }
    }
}
