//报警阈值设置页面

Page({
  data:{
    array: ['200m', '500m', '1km', '2km'],
    index:0,
    array2: ['92%', '88%', '84%'],
    index2:0,
    mobile: '',
    domain: '',
    name: '',
    passwd: '',
    latitude: 23.053518,
    longitude: 113.372757,
    circles: [{
      latitude: 23.053518,
      longitude: 113.372757,
      color: '#FF0000DD',
      fillColor: '#d1edff88',
      radius: 100,//定位点半径
      strokeWidth: 1
    }]
  },

  bindPickerChange: function(e) {
    var that = this;
    console.log('picker发送选择改变，携带值为', e.detail.value)
    that.setData({
      index: parseInt(e.detail.value)
    })
    switch(e.detail.value){
      case '0': that.setData({'circles[0].radius': 100}); break;
      case '1': that.setData({'circles[0].radius': 250}); break;
      case '2': that.setData({'circles[0].radius': 500}); break;
      case '3': that.setData({'circles[0].radius': 1000}); break;
    }
  },

  bindPickerChange2: function(e) {
    var that = this;
    console.log('picker发送选择改变，携带值为', e.detail.value)
    that.setData({
      index2: parseInt(e.detail.value)
    })
  },

  regionchange: function(e){
    var that = this;
    // console.log(e)
    this.mapCtx.getCenterLocation({
      success: function(res) {
        console.log(res)
        that.setData({
          latitude: res.latitude,
          longitude: res.longitude,
          'circles[0].latitude': res.latitude,
          'circles[0].longitude': res.longitude,
          'circles[0].color': '#FF0000DD',
          'circles[0].fillColor': '#d1edff88',
          'circles[0].strokeWidth': 1
        })
      }
    })

  },

  sure(){
    var that = this
    wx.request({
      url: "https://iot-api.heclouds.com/datapoint/synccmds?product_id="+that.data.domain+"&device_name="+that.data.name+"&timeout=5",
      header: {
        "authorization": that.data.passwd, //自己的api-key
      },
      data: {
        "latitude_safe": that.data.latitude,
        "longitude_safe": that.data.longitude,
        "position_alarm": that.data.index,
        "spo2_alarm": that.data.index2
      },
      method: "POST",
      success:function(e){
        console.log(e);
        wx.navigateBack();
      }
    })

    wx.showToast({
      title: '设置成功！',
      icon:"success",
      duration:2000,
    })
  },


  onShow(){
    var that = this
    // this.mapCtx = wx.createMapContext("mapId2")
    // let lati = wx.getStorageSync('lat')
    // let longi = wx.getStorageSync('lon')

    // that.setData({
    //   latitude: lati,
    //   longitude: longi,
    //   'circles[0].latitude': lati,
    //   'circles[0].longitude': longi
    // })

    // wx.getStorage({
    //   key: "mobile",
    //   success(res){
    //     that.setData({
    //       mobile: res.data,
    //     })
    //   }
    // })

    // setTimeout(()=>{wx.cloud.database().collection("users").where({
    //   Mobile: that.data.mobile,
    // }).get({
    //   success(res){
    //     console.log(e);
    //     if('iot_domain' in res.data[0]){
    //       that.setData({
    //         domain: res.data[0].iot_domain,
    //         name: res.data[0].iot_name,
    //         passwd: res.data[0].iot_passwd,
    //       })
    //     }
    //   }
    // })}, 1000)

    setTimeout(()=>{
      wx.request({
        url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=latitude_safe&start="+new Date(Date.parse(new Date())- 60*60*1000*7*24).toISOString().substring(0,10)+"T00:00:00&limit=100", 
        //将请求行中的数字换成自己的设备ID
        header: {
          "authorization": that.data.passwd, //自己的api-key
        },
        method: "GET",
        success: function (e) {
          console.log("获取成功",e)
          that.setData({
            'circles[0].latitude': e.data.data.datastreams[0].datapoints[0].value
          })
        }
      })
      wx.request({
        url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=longitude_safe&start="+new Date(Date.parse(new Date())- 60*60*1000*7*24).toISOString().substring(0,10)+"T00:00:00&limit=100", 
        //将请求行中的数字换成自己的设备ID
        header: {
          "authorization": that.data.passwd, //自己的api-key
        },
        method: "GET",
        success: function (e) {
          console.log("获取成功",e)
          that.setData({
            'circles[0].longitude': e.data.data.datastreams[0].datapoints[0].value
          })
        }
      })
      wx.request({ 
        url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=position_alarm&start="+new Date(Date.parse(new Date())- 60*60*1000*7*24).toISOString().substring(0,10)+"T00:00:00&limit=100", 
        //将请求行中的数字换成自己的设备ID
        header: {
          "authorization": that.data.passwd, //自己的api-key
        },
        method: "GET",
        success: function (e) {
          console.log("获取成功",e)
          that.setData({
            index: e.data.data.datastreams[0].datapoints[0].value
          })
        }
      })
    }, 2000)

    setTimeout(()=>{
      that.setData({
        latitude: that.data.circles[0].latitude,
        longitude: that.data.circles[0].longitude
      })
    }, 800)

    setTimeout(()=>{
      switch(that.data.index){
        case 0: that.setData({'circles[0].radius': 100});console.log(0); break;
        case 1: that.setData({'circles[0].radius': 250}); break;
        case 2: that.setData({'circles[0].radius': 500});console.log(1); break;
        case 3: that.setData({'circles[0].radius': 1000}); break;
    }}, 2000)

  },

  onLoad(){
    var that = this
    this.mapCtx = wx.createMapContext("mapId2")
    let lati = wx.getStorageSync('lat')
    let longi = wx.getStorageSync('lon')

    that.setData({
      domain: this.options.domain,
      name: this.options.name,
      passwd: wx.getStorageSync('onenetpasswd')
    })


    // that.setData({
    //   latitude: lati,
    //   longitude: longi,
    //   'circles[0].latitude': lati,
    //   'circles[0].longitude': longi
    // })

    // wx.getStorage({
    //   key: "mobile",
    //   success(res){
    //     that.setData({
    //       mobile: res.data,
    //     })
    //   }
    // })

    // setTimeout(()=>{wx.cloud.database().collection("users").where({
    //   Mobile: that.data.mobile,
    // }).get({
    //   success(res){
    //     console.log(e);
    //     if('iot_domain' in res.data[0]){
    //       that.setData({
    //         domain: res.data[0].iot_domain,
    //         name: res.data[0].iot_name,
    //         passwd: res.data[0].iot_passwd,
    //       })
    //     }
    //   },
    //   fail(e){
    //     console(e)
    //   }
    // })}, 1000)

    setTimeout(()=>{
      wx.request({ 
        url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=latitude_safe&start="+new Date(Date.parse(new Date())- 60*60*1000*7*24).toISOString().substring(0,10)+"T00:00:00&limit=100", 
        //将请求行中的数字换成自己的设备ID
        header: {
          "authorization": that.data.passwd, //自己的api-key
        },
        method: "GET",
        success: function (e) {
          console.log("获取成功",e)
          that.setData({
            'circles[0].latitude': e.data.data.datastreams[0].datapoints[0].value
          })
        }
      })
      wx.request({ 
        url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=longitude_safe&start="+new Date(Date.parse(new Date())- 60*60*1000*7*24).toISOString().substring(0,10)+"T00:00:00&limit=100", 
        //将请求行中的数字换成自己的设备ID
        header: {
          "authorization": that.data.passwd, //自己的api-key
        },
        method: "GET",
        success: function (e) {
          console.log("获取成功",e)
          that.setData({
            'circles[0].longitude': e.data.data.datastreams[0].datapoints[0].value
          })
        }
      })
      wx.request({
        url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=position_alarm&start="+new Date(Date.parse(new Date())- 60*60*1000*7*24).toISOString().substring(0,10)+"T00:00:00&limit=100", 
        //将请求行中的数字换成自己的设备ID
        header: {
          "authorization": that.data.passwd, //自己的api-key
        },
        method: "GET",
        success: function (e) {
          console.log("获取成功",e)
          that.setData({
            index: e.data.data.datastreams[0].datapoints[0].value
          })
          // if(e.data.data.datastreams.length != 0){
          //   switch(e.data.data.datastreams[0].datapoints[0].value){
          //     case '0': that.setData({'circles[0].radius': 100}); break;
          //     case '1': that.setData({'circles[0].radius': 250}); break;
          //     case '2': that.setData({'circles[0].radius': 500}); break;
          //     case '3': that.setData({'circles[0].radius': 1000}); break;
          //   }
          // }
        }
      })
    }, 1000)

    setTimeout(()=>{
      switch(that.data.index){
        case 0: that.setData({'circles[0].radius': 100});console.log(0); break;
        case 1: that.setData({'circles[0].radius': 250}); break;
        case 2: that.setData({'circles[0].radius': 500});console.log(1); break;
        case 3: that.setData({'circles[0].radius': 1000}); break;
    }}, 2000)

    // setTimeout(()=>{
    //   that.setData({
    //     latitude: that.data.circles[0].latitude,
    //     longitude: that.data.circles[0].longitude
    //   })
    // }, 800)

  }

})