//位置页面

Page({
  data:{
      latitude_temp: '23.052',
      longitude_temp: '113.380',
      scale: 16, //5-18
      markersStatus: true,
      longitude: {},
      latitude: {},
      markers: {},
      time: {},
      polyline:{},
      data_length: '',
      circles: [
        {
          latitude: 23.052,
          longitude: 113.380,
          color: '#FF0000DD',
          fillColor: '#d1edff88',
          radius: 100,
          strokeWidth: 1
        },
      ]
  },

  adddetial(){
    var that = this;

    wx.getStorage({
      key: "mobile",
      success(res){
        that.setData({
          mobile: res.data,
        })
        that.setData({
          markers: {}
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
      url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=longitude&start="+new Date(Date.parse(new Date())- 60*60*1000*16).toISOString().substring(0,19)+"&limit=100", 
      //将请求行中的数字换成自己的设备ID
      header: {
        "authorization": that.data.passwd, //自己的api-key
      },
      method: "GET",
      success: function (e) {
        console.log("获取成功",e)
        //that.data.longitude.splice(0)
        if(e.data.data.datastreams.length){
          for(let i=0; i<e.data.data.datastreams[0].datapoints.length; i++){ //e.data.data.datastreams[0].datapoints.length
            that.setData({
              ['longitude['+i+']']: e.data.data.datastreams[0].datapoints[i].value,
              ['time['+i+']']: e.data.data.datastreams[0].datapoints[i].at.substring(0,16),
              data_length: e.data.data.datastreams[0].datapoints.length
            })
          }
        }else{
          wx.showToast({
            title: '未能获取到位置，可能是未绑定或当天无更新数据',
            icon:"none",
            duration:2000,
          })
        }
      }
    })
    }, 800)

    setTimeout(()=>{
      wx.request({
        url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=latitude&start="+new Date(Date.parse(new Date())- 60*60*1000*16).toISOString().substring(0,19)+"&limit=100", 
        //将请求行中的数字换成自己的设备ID
        header: {
          "authorization": that.data.passwd, //自己的api-key
        },
        method: "GET",
        success: function (e) {
          console.log("获取成功",e)
          //that.data.latitude.splice(0)
          if(e.data.data.datastreams.length){
            for(let i=0; i<e.data.data.datastreams[0].datapoints.length; i++){ //e.data.data.datastreams[0].datapoints.length
              that.setData({
                ['latitude['+i+']']: e.data.data.datastreams[0].datapoints[i].value,
                data_length: e.data.data.datastreams[0].datapoints.length
              })
            }
            for(let i=0; i<e.data.data.datastreams[0].datapoints.length; i++){ //that.data.data_length
              that.setData({
                ['markers['+i+'].id']: i+1,
                ['markers['+i+'].latitude']: that.data.latitude[i],
                ['markers['+i+'].longitude']: that.data.longitude[i],
                ['markers['+i+'].callout.content']: that.data.time[i]+"\n/到此处",
                ['markers['+i+'].callout.fontSize']: 15,
                ['polyline[0].points['+(e.data.data.datastreams[0].datapoints.length-1-i)+'].latitude']: that.data.latitude[i],
                ['polyline[0].points['+(e.data.data.datastreams[0].datapoints.length-1-i)+'].longitude']: that.data.longitude[i],
                'polyline[0].arrowLine': true,
                'polyline[0].color': 'blue',
                'polyline[0].width': 8,
              })
              if(i == 0){
                that.setData({
                  ['markers['+i+'].callout.display']: 'ALWAYS',
                  longitude_temp: that.data.longitude[i],
                  latitude_temp: that.data.latitude[i]
                }) 
              }else{
                that.setData({
                  ['markers['+i+'].callout.display']: 'BYCLICK'
                })
              }
            }
          }
        }
      })

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
          switch(e.data.data.datastreams[0].datapoints[0].value){
            case 0: that.setData({'circles[0].radius': 100}); break;
            case 1: that.setData({'circles[0].radius': 250}); break;
            case 2: that.setData({'circles[0].radius': 500}); break;
            case 3: that.setData({'circles[0].radius': 1000}); break;
          }
        }
      })
    }, 1000)

    if(JSON.stringify(that.data.latitude) == '{}'){
      that.setData({
        latitude_temp: that.data.circles[0].latitude,
        longitude_temp: that.data.circles[0].longitude
      })
    }else{
      that.setData({
        latitude_temp: that.data.latitude[0],
        longitude_temp: that.data.longitude[0]
      })
    }

    wx.setStorageSync('lat',that.data.latitude_temp)
    wx.setStorageSync('lon', that.data.longitude_temp)
  },

  markertap: function(e){
    var that = this
    var index = that.data.markers.findIndex(item => item.id === e.detail.markerId)
    console.log(index)
    for(let i=0; i<that.data.markers.length; i++){
      if(i!=index){
        that.setData({
          ['markers['+i+'].callout.display']: ''
        })
      }
    }
  },

  callouttap: function(e){
    var that = this
    var index = that.data.markers.findIndex(item => item.id === e.detail.markerId)
    console.log(index)
    wx.openLocation({
      latitude:that.data.markers[index].latitude,	//维度
      longitude: that.data.markers[index].longitude, //经度
      name: "目的地",	//目的地定位名称
      scale: 15,	//缩放比例
      address: "目的地"	//导航详细地址
    })
    console.log("id:", e)
  },

  onLoad(){
    var that = this;
    that.mapCtx = wx.createMapContext('mapId');
    wx.cloud.init();

    wx.getStorage({
      key: "mobile",
      success(res){
        that.setData({
          mobile: res.data,
        })
        that.setData({
          markers: {}
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
      })}, 200)

      setTimeout(()=>{
        wx.request({
          url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=longitude&start="+new Date(Date.parse(new Date())- 60*60*1000*16).toISOString().substring(0,19)+"&limit=100", 
          //将请求行中的数字换成自己的设备ID
          header: {
            "authorization": that.data.passwd, //自己的api-key
          },
          method: "GET",
          success: function (e) {
            console.log("获取成功",e)
            //that.data.longitude.splice(0)
            if(e.data.data.datastreams.length){
              for(let i=0; i<e.data.data.datastreams[0].datapoints.length; i++){ //e.data.data.datastreams[0].datapoints.length
                that.setData({
                  ['longitude['+i+']']: e.data.data.datastreams[0].datapoints[i].value,
                  ['time['+i+']']: e.data.data.datastreams[0].datapoints[i].at.substring(0,16),
                  data_length: e.data.data.datastreams[0].datapoints.length
                })
              }
            }else{
              wx.showToast({
                title: '未能获取到位置，可能是未绑定或当天无更新数据',
                icon:"none",
                duration:2000,
              })
            }
          }
        })
        }, 800)
  
      setTimeout(()=>{
        wx.request({ 
          url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=latitude&start="+new Date(Date.parse(new Date())- 60*60*1000*16).toISOString().substring(0,19)+"&limit=100", 
          //将请求行中的数字换成自己的设备ID
          header: {
            "authorization": that.data.passwd, //自己的api-key
          },
          method: "GET",
          success: function (e) {
            console.log("获取成功",e)
            //that.data.latitude.splice(0)
            if(e.data.data.datastreams.length){
              for(let i=0; i<e.data.data.datastreams[0].datapoints.length; i++){ //e.data.data.datastreams[0].datapoints.length
                that.setData({
                  ['latitude['+i+']']: e.data.data.datastreams[0].datapoints[i].value,
                  data_length: e.data.data.datastreams[0].datapoints.length
                })
              }
              for(let i=0; i<e.data.data.datastreams[0].datapoints.length; i++){ //that.data.data_length
                that.setData({
                  ['markers['+i+'].id']: i+1,
                  ['markers['+i+'].latitude']: that.data.latitude[i],
                  ['markers['+i+'].longitude']: that.data.longitude[i],
                  ['markers['+i+'].callout.content']: that.data.time[i]+"\n/到此处",
                  ['markers['+i+'].callout.fontSize']: 15,
                  ['polyline[0].points['+(e.data.data.datastreams[0].datapoints.length-1-i)+'].latitude']: that.data.latitude[i],
                  ['polyline[0].points['+(e.data.data.datastreams[0].datapoints.length-1-i)+'].longitude']: that.data.longitude[i],
                  'polyline[0].arrowLine': true,
                  'polyline[0].color': 'blue',
                  'polyline[0].width': 8,
                })
                if(i == 0){
                  that.setData({
                    ['markers['+i+'].callout.display']: 'ALWAYS',
                    longitude_temp: that.data.longitude[i],
                    latitude_temp: that.data.latitude[i]
                  }) 
                }else{
                  that.setData({
                    ['markers['+i+'].callout.display']: 'BYCLICK'
                  })
                }
              }
            }
          }
        })
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
            switch(e.data.data.datastreams[0].datapoints[0].value){
              case 0: that.setData({'circles[0].radius': 100}); break;
              case 1: that.setData({'circles[0].radius': 250}); break;
              case 2: that.setData({'circles[0].radius': 500}); break;
              case 3: that.setData({'circles[0].radius': 1000}); break;
            }
          }
        })
      }, 1000)
      
      if(JSON.stringify(that.data.latitude) == '{}'){
        that.setData({
          latitude_temp: that.data.circles[0].latitude,
          longitude_temp: that.data.circles[0].longitude
        })
      }else{
        that.setData({
          latitude_temp: that.data.latitude[0],
          longitude_temp: that.data.longitude[0]
        })
      }
  
  
      wx.setStorageSync('lat',that.data.latitude_temp)
      wx.setStorageSync('lon', that.data.longitude_temp)

    //这个在真机可以用，调试的时候不行
    // that.mapCtx.addMarkers({markers:[{
    //     id: '0',
    //     latitude: '23.052',
    //     longitude: '113.380',
    //     iconPath: '../../image/position_icon.png',
    //     width: 20,
    //     height: 20,
    //     alpha: '1'
    //   }]
    // });
    
  },

  onShow(){
    var that = this;

    setTimeout(()=>{wx.getStorage({
      key: "mobile",
      success(res){
        that.setData({
          mobile: res.data,
        })
        that.setData({
          markers: {}
        })
      }
    })}, 500)

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
      })}, 500)

    setTimeout(()=>{
      wx.request({
        url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=longitude&start="+new Date(Date.parse(new Date())- 60*60*1000*16).toISOString().substring(0,19)+"&limit=100", 
        //将请求行中的数字换成自己的设备ID
        header: {
          "authorization": that.data.passwd, //自己的api-key
        },
        method: "GET",
        success: function (e) {
          console.log("获取成功",e)
          //that.data.longitude.splice(0)
          if(e.data.data.datastreams.length){
            for(let i=0; i<e.data.data.datastreams[0].datapoints.length; i++){ //e.data.data.datastreams[0].datapoints.length
              that.setData({
                ['longitude['+i+']']: e.data.data.datastreams[0].datapoints[i].value,
                ['time['+i+']']: e.data.data.datastreams[0].datapoints[i].at.substring(0,16),
                data_length: e.data.data.datastreams[0].datapoints.length
              })
            }
          }else{
            wx.showToast({
              title: '未能获取到位置，可能是未绑定或当天无更新数据',
              icon:"none",
              duration:2000,
            })
          }
        }
      })
      }, 800)

    setTimeout(()=>{
      wx.request({
        url: "https://iot-api.heclouds.com/datapoint/history-datapoints?product_id="+that.data.domain+"&device_name="+that.data.name+"&datastream_id=latitude&start="+new Date(Date.parse(new Date())- 60*60*1000*16).toISOString().substring(0,19)+"&limit=100", 
        //将请求行中的数字换成自己的设备ID
        header: {
          "authorization": that.data.passwd, //自己的api-key
        },
        method: "GET",
        success: function (e) {
          console.log("获取成功",e)
          //that.data.latitude.splice(0)
          if(e.data.data.datastreams.length){
            for(let i=0; i<e.data.data.datastreams[0].datapoints.length; i++){ //e.data.data.datastreams[0].datapoints.length
              that.setData({
                ['latitude['+i+']']: e.data.data.datastreams[0].datapoints[i].value,
                data_length: e.data.data.datastreams[0].datapoints.length
              })
            }
            for(let i=0; i<e.data.data.datastreams[0].datapoints.length; i++){ //that.data.data_length
              that.setData({
                ['markers['+i+'].id']: i+1,
                ['markers['+i+'].latitude']: that.data.latitude[i],
                ['markers['+i+'].longitude']: that.data.longitude[i],
                ['markers['+i+'].callout.content']: that.data.time[i]+"\n/到此处",
                ['markers['+i+'].callout.fontSize']: 15,
                ['polyline[0].points['+(e.data.data.datastreams[0].datapoints.length-1-i)+'].latitude']: that.data.latitude[i],
                ['polyline[0].points['+(e.data.data.datastreams[0].datapoints.length-1-i)+'].longitude']: that.data.longitude[i],
                'polyline[0].arrowLine': true,
                'polyline[0].color': 'blue',
                'polyline[0].width': 8,
              })
              if(i == 0){
                that.setData({
                  ['markers['+i+'].callout.display']: 'ALWAYS',
                  longitude_temp: that.data.longitude[i],
                  latitude_temp: that.data.latitude[i]
                }) 
              }else{
                that.setData({
                  ['markers['+i+'].callout.display']: 'BYCLICK'
                })
              }
            }
          }
        }
      })
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
          switch(e.data.data.datastreams[0].datapoints[0].value){
            case 0: that.setData({'circles[0].radius': 100}); break;
            case 1: that.setData({'circles[0].radius': 250}); break;
            case 2: that.setData({'circles[0].radius': 500}); break;
            case 3: that.setData({'circles[0].radius': 1000}); break;
          }
        }
      })
    }, 1000)
    
    if(JSON.stringify(that.data.latitude) == '{}'){
      that.setData({
        latitude_temp: that.data.circles[0].latitude,
        longitude_temp: that.data.circles[0].longitude
      })
    }else{
      that.setData({
        latitude_temp: that.data.latitude[0],
        longitude_temp: that.data.longitude[0]
      })
    }


    wx.setStorageSync('lat',that.data.latitude_temp)
    wx.setStorageSync('lon', that.data.longitude_temp)


    //这个在真机可以用，调试的时候不行
    // that.mapCtx.addMarkers({markers:[{
    //     id: '0',
    //     latitude: '23.052',
    //     longitude: '113.380',
    //     iconPath: '../../image/position_icon.png',
    //     width: 20,
    //     height: 20,
    //     alpha: '1'
    //   }]
    // });
  },
})