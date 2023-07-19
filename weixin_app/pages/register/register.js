// 注册页面

//自定义变量，存放用户名
let userName = ''
//自定义变量，存放账号
let Account = ''
//自定义变量，存放密码
let Password = ''

Page({

  //获取用户输入的用户名/账号/密码
  getUserName(e){
    console.log("输入的用户名",e.detail.value);
    userName = e.detail.value
  },
  getAccount(e){
    console.log("输入的账号",e.detail.value);
    Account = e.detail.value
  },
  getPassword(e){
    console.log("输入的密码",e.detail.value);
    Password = e.detail.value
  },

  //注册
  sign(){
    //校验用户名
    var verify = /^[1]{1}[3,4,5,7,8,9]{1}[0-9]{2,10}$/
    if(!(verify.test(userName)&&(userName.length == 11))){
      wx.showToast({
        title: '手机号码格式不符',
        icon:"none"
      })
      return  //如果不满足，代码不再往下执行
    }
    //校验账号
    if(Account.length<4){
      wx.showToast({
        title: '用户名太短',
        icon:"none"
      })
      return
    }
    //校验密码
    if(Password.length<4){
      wx.showToast({
        title: '密码至少4位',
        icon:"none"
      })
      return
    }
    //注册功能的实现
    wx.cloud.database().collection("users").where({
      Mobile: userName
    }).get({
      success(res){
        if(res.data.length != 0){
          console.log('找到记录', res);
          wx.showToast({
            title: '手机号重复！',
            icon:"none",
            duration:2000,
          })
        }
        else{
          console.log('没找到记录', res);
          wx.cloud.database().collection("users")
          .add({
            data:{
              Mobile:userName,
              Username:Account,
              Password:Password
            }
          }).then(res=>{
            console.log("注册数据添加到数据库成功",res);
            //注册成功提示
            wx.showToast({
              title: '注册成功',
              icon:"success",
              duration:2000,
              success:function(){
              //提示框停留2秒后跳转到用户登录页
              setTimeout(function(){
                wx.navigateTo({
                  url: 'pages/position/position',
                })
              },2000)}
            })
          }).catch(err=>{
            console.log("注册数据添加失败",err);
          })
        }
      },
      fail(err){
        console.log('没找到记录', err);
      }
    })
  //   wx.cloud.database().collection("users")
  //   .add({
  //     data:{
  //       Mobile:userName,
  //       Username:Account,
  //       Password:Password
  //     }
  //   }).then(res=>{
  //     console.log("注册数据添加到数据库成功",res);
  //     //注册成功提示
  //     wx.showToast({
  //       title: '注册成功',
  //       icon:"success",
  //       duration:2000,
  //       success:function(){
  //       //提示框停留2秒后跳转到用户登录页
  //       setTimeout(function(){
  //         wx.navigateTo({
  //           url: 'pages/position/position',
  //         })
  //       },2000)}
  //     })
  //   }).catch(err=>{
  //     console.log("注册数据添加失败",err);
  //   })
  },

  onLoad() {
    wx.cloud.init();
  },

  onUnload: function () {
    wx.navigateTo({
      url: 'pages/position/position',
    })
  },

})
