
Page({
  data:{
    domain: '',
    name: '',
    passwd: '',
    mobile: '',
    online_state: '',
    sp02data: '',
    bat: '',
  },

  getsettings(){
    var that = this
    wx.navigateTo({
      url:'/pages/setting/setting?domain='+that.data.domain+'&name='+that.data.name
    })
    wx.setStorageSync('onenetpasswd', that.data.passwd)
  },

  gethistory(){
    var that = this
    wx.navigateTo({
      url:'/pages/history/history?domain='+that.data.domain+'&name='+that.data.name
    })
    wx.setStorageSync('onenetpasswd', that.data.passwd)
  },

  setwarn(){
    var that = this
    wx.navigateTo({
      url:'/pages/warn/warn?domain='+that.data.domain+'&name='+that.data.name
    })
    wx.setStorageSync('onenetpasswd', that.data.passwd)
  },

  startInter: function(){
    var that = this;
    setInterval(
    function () {
      wx.request({
            url: "https://iot-api.heclouds.com/device/detail?product_id="+that.data.domain+"&device_name="+that.data.name,  
            header: {
              "authorization": that.data.passwd, //自己的api-key
            },
            method: "GET",
            success: function (e) {
              console.log("获取成功",e)
              if(e.data.code == 0){
                if(e.data.data.status == 1){
                  that.setData({
                    online_state: "在线"
                  })
                }else{
                  that.setData({
                    online_state: "离线"
                  })
                }
              }else{
                that.setData({
                  online_state: "查询失败"
                })
              }
            },
        });
    }, 10000)    
  },

  endInter: function(){
    var that = this;
    clearInterval(that.data.timer)
  },

  onShow(){
    var that = this;
    wx.getStorage({
      key: "mobile",
      success(res){
        that.setData({
          mobile: res.data,
        })
      }
    })
    setTimeout(()=>{wx.cloud.database().collection("users").where({
        Mobile: that.data.mobile,
      }).get({
        success(res){
          if('iot_domain' in res.data[0]){
            that.setData({
              domain: res.data[0].iot_domain,
              name: res.data[0].iot_name,
              passwd: res.data[0].iot_passwd,
            })
          }
        }
      })}, 100)
      setTimeout(()=>{
        wx.request({
          url: "https://iot-api.heclouds.com/device/detail?product_id="+that.data.domain+"&device_name="+that.data.name,  
          header: {
            "authorization": that.data.passwd,
          },
          method: "GET",
          success: function (e) {
            console.log("获取成功",e)
            if(e.data.code == 0){
              if(e.data.data.status == 1){
                that.setData({
                  online_state: "在线"
                })
              }else{
                that.setData({
                  online_state: "离线"
                })
              }
            }else{
              that.setData({
                online_state: "查询失败"
              })
            }
          },
      })
      wx.request({ 
        url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=longitude&start="+new Date(Date.parse(new Date())- 60*60*1000*16).toISOString().substring(0,10)+"T00:00:00&limit=100", 
        //将请求行中的数字换成自己的设备ID
        header: {
          "authorization": that.data.passwd, //自己的api-key
        },
        method: "GET",
        success: function (e) {
          console.log("获取成功",e)
          if(e.data.code == 0 && e.data.data.datastreams.length != 0){
            that.setData({
              sp02data: 94,
            })
          }else{
            that.setData({
              sp02data: "（未能获取）",
            })
          }
        }
      })
      wx.request({ 
        url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=bat&start="+new Date(Date.parse(new Date())- 60*60*1000*16).toISOString().substring(0,10)+"T00:00:00&limit=100", 
        //将请求行中的数字换成自己的设备ID
        header: {
          "authorization": that.data.passwd, //自己的api-key
        },
        method: "GET",
        success: function (e) {
          console.log("获取成功",e)
          if(e.data.code == 0 && e.data.data.datastreams.length != 0){
            that.setData({
              bat: e.data.data.datastreams[0].datapoints[0].value,
            })
          }else{
            that.setData({
              bat: "（未能获取）",
            })
          }
        }
      })
      },500)
  },

  onLoad(){
    this.startInter();
  },

  onHide(){
    this.endInter();
  }

})