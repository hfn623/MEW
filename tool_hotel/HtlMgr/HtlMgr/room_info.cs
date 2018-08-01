using System;
using System.Collections.Generic;
using System.Text;

namespace HtlMgr
{
    class room_info
    {
        public room_info()
        {
            name = "";
            //办公室 双标 三标卫生间
            type = "";
            //未安装 未开卡 开卡n张
            stat = "";
            card_count = 0;
        }
        public string name;
        public string type;
        public string stat;
        public int card_count;
    }
}
