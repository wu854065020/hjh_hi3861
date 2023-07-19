//绑定ONENET云平台设备信息的页面

Page({
  data: { //要渲染的数据
    result: '',
    domain: '',
    name: '',
    passwd: '',
    mobile: '',
  },
  touchtoken(){
    console.log("获取中");
    this.gettoken();
    // wx.cloud.collection('user').where({
    // }).update({
    //   iot_user: this.data.domain,
    //   iot_passwd: this.data.passwd,
    // })
  },

  getDomain(e){
    this.setData({
      domain: e.detail.value
    });
  },
  getName(e){
    this.setData({
      name: e.detail.value
    })
  },
  getPassword(e){
    this.setData({
      passwd: e.detail.value
    })
  },
  
  settoken(_token){
    wx.setStorageSync('token', _token);
    this.setData({result: "token认证成功"});
    this.getinfo();
  },

  //弃用的华为云
  // getinfo(){ //查询设备影子数据
  //   var that = this; //this像java一样，是指当前对象
  //   var token = wx.getStorageSync('token');
  //   wx.request({ //向指定域名发送http请求
  //     url: 'https://14e5be63be.st1.iotda-app.cn-north-4.myhuaweicloud.com/v5/iot/49855b88f2d84095bd0b3db98732a058/devices/6457b5eb4f1d680324508d51_test/properties?service_id=6457b5eb4f1d680324508d51',
  //     data: '',
  //     method: 'GET',
  //      header: {'X-Auth-Token': token},
  //     // header: {'Instance-Id': '71c604f0-23f1-4061-ad4c-22d025753ad6'},
  //     success: function(res){
  //       console.log(res);
  //       wx.navigateTo("/pages/position/position")
  //       wx.showToast({
  //         title: '绑定成功！',
  //         icon:"success",
  //         duration:2000,
  //       })
  //     },
  //     fail: function(){

  //     },
  //     complete: function(){

  //     }
  //   });
  // },

  getinfo2(){
    var that = this;
    wx.request({
    url: "https://iot-api.heclouds.com/device/detail?product_id="+that.data.domain+"&device_name="+that.data.name,  
    //将请求行中的数字换成自己的设备ID
    header: {
      "authorization": that.data.passwd, //自己的api-key
    },
    method: "GET",
    success: function (e) {
      console.log("获取成功",e)
      if(e.data.code == 0){
        wx.showToast({
          title: '绑定成功！',
          icon:"success",
          duration:2000,
        })
        wx.cloud.database().collection("users").where({
          Mobile: that.data.mobile,
        }).update({
          data:{
            iot_domain: that.data.domain,
            iot_name:that.data.name,
            iot_passwd: that.data.passwd,
          }
        })
      }else{
        wx.showToast({
          title: '请求内容加载出错，不进行任何操作！',
          icon:"none",
          duration:2000,
        })
      }
      // that.setData({
      //   temp:e.data.data.datastreams[1].datapoints[0].value
      // })
      // console.log("temp==",that.data.temp)
    }
   });
  },

  cancel(){
    var that = this;
    wx.cloud.database().collection("users").where({
      Mobile: that.data.mobile,
    }).update({
      data:{
        iot_domain: wx.cloud.database().command.remove(),
        iot_name: wx.cloud.database().command.remove(),
        iot_passwd: wx.cloud.database().command.remove(),
      }
    })
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
  },

  //初始操作，加载的时候自动运行
  onLoad() {
    var that = this;
    console.log("测试");
    wx.getStorage({
      key: "mobile",
      success(res){
        that.setData({
          mobile: res.data,
        })
      }
    })
  },
  
})
