Page({
  data: {
    latitude: 45.724156,
    longitude: 126.60649,
    circles: [{
      latitude: 45.724156,
      longitude: 126.60649,
      radius: 15,
      color: "#00aabbaa",
      fillColor: "#bb00aaaa",
      strokeWidth: 5,
    }],
    b1en: false,
    b2en: true,
  },

  onLoad: function(e) {
    var that = this;

    wx.onSocketClose(function(res) {
      that.setData({
        b1en: false,
        b2en: true
      });
    });

    wx.onSocketOpen(function(res) {
      that.setData({
        b1en: true,
        b2en: false,
      });
      that.timerEvent();
    });

    wx.onSocketMessage(function(res) {
      var str = res.data;
      var info = JSON.parse(str);
      if (info["type"] == "auth") {
        wx.sendSocketMessage({
          data: "{\"type\":\"mc20_demo\"}",
        })
      } else if (info["type"] == "DATA") {
        var loc = that.WGS84_TO_GCJ02(info["latitude"], info["longitude"]);
        that.setData({
          circles: [{
            latitude: loc[0],
            longitude: loc[1],
            radius: 15,
            color: "#00aabbaa",
            fillColor: "#bb00aaaa",
            strokeWidth: 5,
          }],
          latitude: loc[0],
          longitude: loc[1],
        });
      }

    });


  },

  bws_conn: function(e) {
    wx.connectSocket({
      url: 'ws://www.boryworks.com:9001',
    })
  },

  bws_disconn: function(e) {
    wx.closeSocket()
  },

  onReady: function(e) {
    this.mapCtx = wx.createMapContext('myMap')
  },

  WGS84_to_GCJ02_transformLat: function(x, y) {
    var ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * Math.sqrt(Math.abs(x));
    ret += (20.0 * Math.sin(6.0 * x * Math.PI) + 20.0 * Math.sin(2.0 * x * Math.PI)) * 2.0 / 3.0;
    ret += (20.0 * Math.sin(y * Math.PI) + 40.0 * Math.sin(y / 3.0 * Math.PI)) * 2.0 / 3.0;
    ret += (160.0 * Math.sin(y / 12.0 * Math.PI) + 320 * Math.sin(y * Math.PI / 30.0)) * 2.0 / 3.0;
    return ret;
  },

  WGS84_to_GCJ02_transformLon: function(x, y) {
    var ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * Math.sqrt(Math.abs(x));
    ret += (20.0 * Math.sin(6.0 * x * Math.PI) + 20.0 * Math.sin(2.0 * x * Math.PI)) * 2.0 / 3.0;
    ret += (20.0 * Math.sin(x * Math.PI) + 40.0 * Math.sin(x / 3.0 * Math.PI)) * 2.0 / 3.0;
    ret += (150.0 * Math.sin(x / 12.0 * Math.PI) + 300.0 * Math.sin(x / 30.0 * Math.PI)) * 2.0 / 3.0;
    return ret;
  },

  WGS84_TO_GCJ02: function(wgLat, wgLon) {
    var a = 6378245.0;
    var ee = 0.00669342162296594323;

    var dLat = this.WGS84_to_GCJ02_transformLat(wgLon - 105.0, wgLat - 35.0);
    var dLon = this.WGS84_to_GCJ02_transformLon(wgLon - 105.0, wgLat - 35.0);

    var radLat = wgLat / 180.0 * Math.PI;
    var magic = Math.sin(radLat);
    magic = 1 - ee * magic * magic;
    var sqrtMagic = Math.sqrt(magic);
    dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * Math.PI);
    dLon = (dLon * 180.0) / (a / sqrtMagic * Math.cos(radLat) * Math.PI);
    var mgLat = wgLat + dLat;
    var mgLon = wgLon + dLon;

    return [mgLat, mgLon];
  },

  timerEvent: function () {
    var that = this;
    var timer = setTimeout(function () {
      if (that.data.b1en) {        
        wx.sendSocketMessage({
          data: "{\"type\":\"req_data\"}",
        });
        that.timerEvent();
        //console.log("request data!");
      }
    }, 3000);
  },
})