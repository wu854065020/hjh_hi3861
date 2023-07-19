//语音提醒页面

Page({
  data:{

  },

  onShow(){

  },

  bind1(){
    var that = this
    wx.request({
      url: "https://iot-api.heclouds.com/datapoint/synccmds?product_id="+that.data.domain+"&device_name="+that.data.name+"&timeout=5",
      header: {
        "authorization": that.data.passwd, //自己的api-key
      },
      data: {
        "speak_cmd": 1,
      },
      method: "POST",
      success:function(e){
        console.log(e);
        wx.showToast({
          title: '已发送',
          icon:"success",
          duration:2000,
        })
      }
    })
  },

  bind2(){
    var that = this
    wx.request({
      url: "https://iot-api.heclouds.com/datapoint/synccmds?product_id="+that.data.domain+"&device_name="+that.data.name+"&timeout=5",
      header: {
        "authorization": that.data.passwd, //自己的api-key
      },
      data: {
        "speak_cmd": 2,
      },
      method: "POST",
      success:function(e){
        console.log(e);
        wx.showToast({
          title: '已发送',
          icon:"success",
          duration:2000,
        })
      }
    })
  },
  
  bind3(){
    var that = this
    wx.request({
      url: "https://iot-api.heclouds.com/datapoint/synccmds?product_id="+that.data.domain+"&device_name="+that.data.name+"&timeout=5",
      header: {
        "authorization": that.data.passwd, //自己的api-key
      },
      data: {
        "speak_cmd": 3,
      },
      method: "POST",
      success:function(e){
        console.log(e);
        wx.showToast({
          title: '已发送',
          icon:"success",
          duration:2000,
        })
      }
    })
  },
  
  bind4(){
    var that = this
    wx.request({
      url: "https://iot-api.heclouds.com/datapoint/synccmds?product_id="+that.data.domain+"&device_name="+that.data.name+"&timeout=5",
      header: {
        "authorization": that.data.passwd, //自己的api-key
      },
      data: {
        "speak_cmd": 4,
      },
      method: "POST",
      success:function(e){
        console.log(e);
        wx.showToast({
          title: '已发送',
          icon:"success",
          duration:2000,
        })
      }
    })
  },

  onLoad(){
    var that = this

    that.setData({
      domain: this.options.domain,
      name: this.options.name,
      passwd: wx.getStorageSync('onenetpasswd')
    })

  },
})