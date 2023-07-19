//历史记录展示页面

Page({
  data:{
    history_data:[],
  },

  onShow(){
    var that = this
    that.setData({
      domain: this.options.domain,
      name: this.options.name,
      passwd: wx.getStorageSync('onenetpasswd')
    })
  },

  onLoad(){
    var that = this

    that.setData({
      domain: this.options.domain,
      name: this.options.name,
      passwd: wx.getStorageSync('onenetpasswd')
    })
    wx.request({
      url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=alarm&start="+new Date(Date.parse(new Date())- 60*60*1000*28*24).toISOString().substring(0,10)+"T00:00:00&limit=100",
      header: {
        "authorization": that.data.passwd, //自己的api-key
      },
      method: "GET",
      success(e){
        console.log(e)
        for(let i=0; i<e.data.data.datastreams[0].datapoints.length; i++){
          var str
          switch(e.data.data.datastreams[0].datapoints[i].value){
            case 0: str="该时刻超出围栏范围"; break;
            case 1: str="该时刻可能跌倒"; break;
            case 2: str="该时刻重新回到围栏范围"; break;
            case 3: str="该时刻老人回到家中"; break;
            case 4: str="该时刻老人按下SOS，可能需要求救"; break;
          }
          var history_data = that.data.history_data
          history_data.push({
            time: e.data.data.datastreams[0].datapoints[i].at.substring(0,19),
            data: str,
          })
          that.setData({
            history_data: history_data
          })
        }
      },
    })
  },
})