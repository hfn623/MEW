using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace ModbusCenter
{
    public class Command
    {
        public List<Point> cindex;
        public List<byte[]> request;
        public List<int> result;
        public int now;
        public bool finish
        {
            get
            {
                if (request.Count == now )
                {
                    return true;
                }
                return false;
            }
        }
        public int p100
        {
            get
            {
                return now * 100 / (request.Count );
            }
        }
        public Command()
        {
            request = new List<byte[]>();
            result = new List<int>();
            cindex = new List<Point>();
        }
        public void Reset()
        {
            now = 0;
            request.Clear();
            result.Clear();
            cindex.Clear();
        }
        public void Insert(byte[] data,int x,int y)
        {
            Point p = new Point(x, y);
            cindex.Add(p);
            request.Add(data);
            result.Add(-1);
        }
    }
    public class Node
    {
        public int addr = 0;
        public string name = "未命名";
        public string[] names;
        public bool[] value;
        public int[] week;
        public bool[] enable;
        public bool[] single_cmd;
        public bool[] keep_cmd;

        public bool getValue(int i)
        {
            return value[i];
        }

        public void setValue(int i, bool v)
        {
            inited[i] = true;
            value[i] = v;
        }
        public DateTime[] openTime;
        public DateTime[] closeTime;
        public bool[] enableTime;
        public bool[] inited;


        public Node()
        {
            keep_cmd = new bool[12];
            single_cmd = new bool[12];
            week = new int[12];
            names = new string[12];
            enable = new bool[12];
            for (int i = 0; i < 12; i++)
            {
                enable[i] = true;
                names[i] = "未命名" + (i + 1);
            }
            inited = new bool[12];
            value = new bool[12];
            enableTime = new bool[12];
            openTime = new DateTime[12];
            closeTime = new DateTime[12];
        }
    }
}
