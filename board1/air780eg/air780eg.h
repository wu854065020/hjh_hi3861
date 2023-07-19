#ifndef AIR780EG_H_
#define AIR780EG_H_

#include <stdint.h>

#define PI 3.1415926535f
typedef struct timeStruct
{
    int year;
    uint8_t mouth;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
}TimeStruct;

typedef struct GPSInfo
{   
    uint8_t runStatus;
    uint8_t fixStatus;  
    TimeStruct time;    //UTC时间
    float laitude;      //纬度
    float longitude;    //经度
    // float altitude;     //海拔高度
    // float speed;        //对地速度
    // float course;       //对地航向
    // float fixMode;      //定位模式
}GPSInfo;

void airUartInit();

//从字符串中解析浮点数
float parseFloat(char *buf, int len);

//从GPS反馈字符串中解析出经纬度和时间
GPSInfo parseGPSInfo(unsigned char *buf, unsigned int buflen);

//开启GPS
void airGPSOpen();

//读取GPS信息
int airGPSRead(GPSInfo *);

//获取两个经纬度之间的距离
float getDistance(GPSInfo a, GPSInfo b);

void airMqttCfg();

int airMqttPub(uint8_t qos, char *topic, char *payload);

int airMqttPubProcess(char *msg);

int airMqttSub(uint8_t qos, char *topic);

int airMqttGetMsg(char *cmdid, char *msg);

// void airMqttRevice();

void airMsgInit();

void airSetPhoneNum(char *str);

uint8_t airSendMsg(char *msg);



#endif
