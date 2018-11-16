#ifndef PAGES_H
#define PAGES_H

#define AJAX_HTML \
"<!DOCTYPE html>"\
"<html>"\
"<head>"\
"<title>IoT-IO AJAX</title>"\
"<meta http-equiv='Content-Type' content='text/html; charset=GB2312'/>"\
\
"<script type=\"text/javascript\">"\
"function $(id) { return document.getElementById(id); };"\
"function test(){"\
"	if (window.XMLHttpRequest){"\
"		xmlhttp=new XMLHttpRequest();"\
"	}"\
"	else{"\
"		xmlhttp=new ActiveXObject(\"Microsoft.XMLHTTP\");"\
"	}"\
"	xmlhttp.onreadystatechange=function(){"\
"		if(xmlhttp.readyState==4 && xmlhttp.status==200){"\
"			test2(xmlhttp.responseText);"\
"		}"\
"	};"\
"	xmlhttp.open(\"get\", \"ajax.js\", true); "\
"	xmlhttp.send(); "\
"};"\
"function test2(data){"\
"		$('txtSec').value = data;"\
"};"\
"setInterval(test, 1000);"\
"</script>"\
\
"</head>"\
"<body>"\
"<input type='text' id='txtSec' name='sub' size='16' /></p>"\
"</tbody>"\
"</table>"\
"</body>"\
"</html>"


#define CONFIG_HTML  "<!DOCTYPE html>"\
"<html>"\
"<head>"\
"<title>IoT-IO模块参数配置</title>"\
"<meta http-equiv='Content-Type' content='text/html; charset=GB2312'/>"\
"<style type='text/css'>"\
"body {text-align:center; background-color:#c0deed;font-family:Verdana;}"\
"#main {margin-right:auto;margin-left:auto;margin-top:30px;}"\
"label{display:inline-block;width:150px;}"\
"#main h3{color:#66b3ff; text-decoration:underline;}"\
"</style>"\
"<script>"\
"function $(id) { return document.getElementById(id); };"\
"function settingsCallback(o) {"\
"if ($('txtVer')) $('txtVer').value = o.ver;"\
"if ($('txtMac')) $('txtMac').value = o.mac;"\
"if ($('txtIp')) $('txtIp').value = o.ip;"\
"if ($('txtSub')) $('txtSub').value = o.sub;"\
"if ($('txtGw')) $('txtGw').value = o.gw;"\
"};"\
"</script>"\
"</head>"\
"<body>"\
"<div id='main'>"\
"<div style='background:snow; display:block;padding:10px 20px;'>"\
"<h3>配置网络参数</h3>"\
"<form id='frmSetting' method='POST' action='config.cgi'>"\
"<p><label for='txt'>固件版本:</label><input type='text' id='txtVer' name='ver' size='16' disabled='disabled' /></p>"\
"<p><label for='txt'>MAC地址:</label><input type='text' id='txtMac' name='mac' size='16' disabled='disabled' /></p>"\
"<p><label for='txt'>IP地址:</label><input type='text' id='txtIp' name='ip' size='16' /></p>"\
"<p><label for='txt'>子网掩码:</label><input type='text' id='txtSub' name='sub' size='16' /></p>"\
"<p><label for='txt'>默认网关:</label><input type='text' id='txtGw' name='gw' size='16' /></p>"\
"<p><input type='submit' value='保存并重启' /></p>"\
"</form>"\
"</div>"\
"</div>"\
"<div style='margin:5px 5px;'>"\
"&copy; 2018 BORYWORKS"\
"</div>"\
"<script type='text/javascript' src='w5500.js'></script>"\
"</body>"\
"</html>"
/*
"<script>"\
"function $() { test();"\
"setInterval(test, 3000);"\
"function test(){"\
\
"		$.ajax({"\
"            type: 'GET',"\
"            url: 'ajax.js',"\
"						 data: {'txtSec':'59'},"\
"            dataType: 'json',"\
"            success: function(data){"\
"								$('txtSec').value = data.sec;"\
"										},"\
"						 error: function(data){$('txtSec').value = 'err';}"\
"		});"\
"	};"\
"};"\
"function $(id) { return document.getElementById(id); };"\
"</script>"\*/

#endif
