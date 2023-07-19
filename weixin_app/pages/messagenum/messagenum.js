//绑定收信手机号页面

let mobile_temp = ''

Page({
  data: {
    message_mobile: "",
  },

  getMobile(e){
    console.log("输入的手机号",e.detail.value);
    mobile_temp = e.detail.value
  },

  sign(){
    var verify = /^[1]{1}[3,4,5,7,8,9]{1}[0-9]{2,10}$/
    var that = this
    if((verify.test(mobile_temp))&&(mobile_temp.length == 11)){
      wx.request({
        url: "https://iot-api.heclouds.com/datapoint/synccmds?product_id="+that.data.domain+"&device_name="+that.data.name+"&timeout=5",
        header: {
          "authorization": that.data.passwd, //自己的api-key
        },
        data: {
          "phone": mobile_temp
        },
        method: "POST",
        success:function(e){
          console.log(e);
          wx.showToast({
            title: '设置成功',
            icon:"success"
          })
        }
      })
    }else{
      wx.showToast({
        title: '手机号码格式不符！',
        icon:"none"
      })
    }
  },

  onLoad(){
    var that = this
    that.setData({
      domain: this.options.domain,
      name: this.options.name,
      passwd: wx.getStorageSync('onenetpasswd')
    })

    wx.request({
      url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=phone&start="+new Date(Date.parse(new Date())- 60*60*1000*28*24).toISOString().substring(0,10)+"T00:00:00&limit=100",
      header: {
        "authorization": that.data.passwd, //自己的api-key
      },
      method: "GET",
      success(e){
        console.log(e)
        that.setData({
          message_mobile: e.data.data.datastreams[0].datapoints[0].value
        })
      }
    })
    setTimeout(()=>{
      mobile_temp = that.data.message_mobile}, 500)
  },

  onShow(){
    
  },
})