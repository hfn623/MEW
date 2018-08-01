using System;
using System.Collections.Generic;
//using System.Linq;
using System.Text;
using System.Runtime.InteropServices;


namespace BEU_CONFIG
{
    class BEU_SESSION
    {
        public APP_CONF app_conf;
        public string name = "";
        public byte[] ipmac = new byte[10];
        public string ip
        {
            get
            {
                string str = "";
                for (int i = 0; i < 4; i++)
                {
                    str += ipmac[i];
                    if (i < 3)
                    {
                        str += ".";
                    }
                }
                return str;
            }
        }
        public string mac
        {
            get
            {
                string str = "";
                for (int i = 4; i < 10; i++)
                {
                    str += ipmac[i].ToString("X02");
                    if (i < 9)
                    {
                        str += ":";
                    }
                }
                return str;
            }
        }
        public string ipmacs
        {
            get
            {
                string str = "IP=";
                for (int i = 0; i < 4; i++)
                {
                    str += ipmac[i];
                    if (i < 3)
                    {
                        str += ".";
                    }
                }
                str += "  MAC=";
                for (int i = 4; i < 10; i++)
                {
                    str += ipmac[i].ToString("X02");
                    if (i < 9)
                    {
                        str += ":";
                    }
                }
                //str += " )";
                return str;
            }
        }
    }
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)] 
    class APP_CONF
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)] 
        public byte[] Name = new byte[16];
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)] 
        public byte[] MyIPAddr = new byte[4];
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)] 
        public byte[] MyMask = new byte[4];
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)] 
        public byte[] MyGateway = new byte[4];
        public ushort MyPortAddr;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)] 
        public byte[] MyMACAddr = new byte[6];
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)] 
        public byte[] RemoteIPAddr = new byte[4];
        public ushort RemotePortAddr;
        public byte ConnectMode;
        public byte UARTBaud;
        public byte Flags;
        public ushort MagicWord;

        public APP_CONF()
        {
        }
    }
}
