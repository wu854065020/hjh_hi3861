//我的页面

Page({
  data:{
    username: "",
    bind_state: "",
    mobile: "",
    domain: "",
    name: "",
    passwd: "",
  },
  outsign(){
    wx.setStorageSync("key", 1);
    wx.redirectTo({
      url: '/pages/login/login',
    })
  },

  bind(){
    wx.navigateTo({
      url: '/pages/index/index',
    })
  },

  messagenum(){
    var that = this
    wx.navigateTo({
      url: '/pages/messagenum/messagenum?domain='+that.data.domain+'&name='+that.data.name
    })
    wx.setStorageSync('onenetpasswd', that.data.passwd)
  },

  onShow(){
    var that = this

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

    wx.getStorage({
      key: "user",
      success(res){
        that.setData({
          username: res.data
        })
      }
    })
    
    setTimeout(()=>{wx.cloud.database().collection("users").where({
        Mobile: that.data.mobile
      }).get({
        success(res){
          if('iot_domain' in res.data[0]){
            that.setData({
              bind_state: "已绑定",
            })
          }else{
            that.setData({
              bind_state: "未绑定",
            })
          }
        },
        fail(e){
          console.log("无法获取");
        }
      })},100)

  }
})
