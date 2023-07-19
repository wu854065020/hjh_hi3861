/*
    Air780EG模块，AT指令相关
    自己编写了发送各种AT指令如获取GPS、连接MQTT、发送消息等
    同时两个经纬度之间的距离获取函数和GPS反馈字符串、MQTT订阅信息字符串的解析也在此处定义
*/

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include "air780eg.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"
#include "iot_uart.h"
#include "hi_uart.h"
#include "iot_watchdog.h"
#include "iot_errno.h"
#include "iot_config.h"

#define RECEVICE_DELAY 6

#define AIR_UART HI_UART_IDX_2

char g_phoneNum[20] = "8613281268772F";

extern void *g_pubQueue;

static inline void airUartWrite(const unsigned char *data, unsigned int datalen)
{
    IoTUartWrite(AIR_UART, data, datalen);
}

static inline int airUartRead(unsigned char *data, unsigned int dataLen)
{
    return IoTUartRead(AIR_UART, data, dataLen);
}

void airUartInit()
{
    IoTGpioInit(IOT_IO_NAME_GPIO_11);
    // 设置GPIO0的管脚复用关系为UART1_TX Set the pin reuse relationship of GPIO0 to UART1_ TX
    IoSetFunc(IOT_IO_NAME_GPIO_11, IOT_IO_FUNC_GPIO_11_UART2_TXD);
    IoTGpioInit(IOT_IO_NAME_GPIO_12);
    // 设置GPIO1的管脚复用关系为UART1_RX Set the pin reuse relationship of GPIO1 to UART1_ RX
    IoSetFunc(IOT_IO_NAME_GPIO_12, IOT_IO_FUNC_GPIO_12_UART2_RXD);
    uint32_t ret;
    /* 初始化UART配置，波特率 9600，数据bit为8,停止位1，奇偶校验为NONE */
    /* Initialize UART configuration, baud rate is 9600, data bit is 8, stop bit is 1, parity is NONE */
    IotUartAttribute uart_attr = {
        .baudRate = 115200,
        .dataBits = 8,
        .stopBits = 1,
        .parity = 0,
    };
    ret = IoTUartInit(AIR_UART, &uart_attr);
    if (ret != IOT_SUCCESS) {
        printf("Init Uart2 Falied Error No : %d\n", ret);
        return;
    }
}

void airGPSOpen()
{
    const unsigned char *str;
    int len;
    unsigned char rx[300] = {0};
    str = "AT+CGNSPWR=1\r\n";
    airUartWrite(str, strlen(str));
    osDelay(RECEVICE_DELAY);
    if (len = airUartRead(rx, sizeof(rx))) {
        rx[len] = '\0';
        printf("length:%d\n", len);
        printf(rx);
    }
    str = "AT+CGNSAID=31,1,1,1\r\n";
    airUartWrite(str, strlen(str));
    osDelay(RECEVICE_DELAY);
    if (len = airUartRead(rx, sizeof(rx))) {
        rx[len] = '\0';
        printf("length:%d\n", len);
        printf(rx);
    }
}

int parseInt(char *buf, int len)
{
    int num = 0;
    uint8_t i = 0;
    int8_t fuhao = 1;
    uint8_t strflag = 0;
    while (i < len) {
        if (strflag == 0) {
            if (buf[i] == '-') {
                fuhao = -1;
                i++;
            }
            strflag = 1;
        } else if ((buf[i] >= '0' && buf[i] <= '9') && strflag == 1) {
            num = num * 10 + (buf[i] - '0');
            i++;
        } else {
            break;
        }
    }
    return fuhao*num;
}

float parseFloat(char *buf, int len)
{
    uint8_t i = 0, j = 0;
    float num = 0.0f;
    float nump = 0.0f;
    int8_t fuhao = 1;
    uint8_t flag = 0;
    uint8_t num_point = 0;
    while (i < len) {
        if (flag == 0) {
            if (buf[i] == '-') {
                fuhao = -1;
                i++;
            }
            flag = 1;
        } else if (flag == 1) {
            if (buf[i] <= '9' && buf[i] >= '0') {
                num = num * 10 + (buf[i] - '0');
            } else if (buf[i] == '.') {
                flag = 2;
            }
            i++;
        } else if (flag == 2) {
            if (buf[i] <= '9' && buf[i] >= '0') {
                nump = (float)(buf[i] - '0');
                for (j = 0;j < num_point + 1;j++) {
                    nump = nump/10.0f;
                }
                num += nump;
                num_point++;
            }
            i++;
        }
    }
    return num;
}

GPSInfo parseGPSInfo(unsigned char *buf, unsigned int buflen)
{
    int i = 1;
    GPSInfo gps = {0};
    char *utctime = NULL;
    int num = 0;
    uint8_t flag = 0;
    uint8_t start = 0, end = 0; uint8_t strflag = 0;
    while (i < buflen) {
        if (flag == 0 && buf[i-1] == ':' && buf[i] == ' ') {
            flag = 1;
        } else if (flag == 1) {
            if (buf[i] <= '9' && buf[i] >= '0') {
                gps.runStatus = buf[i] - '0';
            } else {
                flag = 2;
            }
        } else if (flag == 2) {
            if (buf[i] <= '9' && buf[i] >= '0') {
                gps.fixStatus = buf[i] - '0';
            } else {
                flag = 3;
            }
        } else if (flag == 3) {
            if (buf[i] <= '9' && buf[i] >= '0') {
                gps.time.year = parseInt(&(buf[i]), 4);
                i += 4;
                gps.time.mouth = parseInt(&(buf[i]), 2);
                i += 2;
                gps.time.day = parseInt(&(buf[i]), 2);
                i += 2;
                gps.time.hour = parseInt(&(buf[i]), 2);
                i += 2;
                gps.time.minute = parseInt(&(buf[i]), 2);
                i += 2;
                gps.time.second = parseInt(&(buf[i]), 2);
                i++;
            } else {
                flag = 4;
            }
        } else if (flag == 4) {
            if (((buf[i] <= '9' && buf[i] >= '0') || buf[i] == '-') && strflag == 0) {
                start = i;
                strflag = 1;
            } else if (strflag == 1) {
                if (!((buf[i] <= '9' && buf[i] >= '0') || buf[i] == '.')) {
                    end = i;
                    gps.laitude = parseFloat(&(buf[start]), end - start);
                    start = 0;
                    end = 0;
                    strflag = 0;
                    flag = 5;
                }
            } else {
                flag = 5;
                strflag = 0;
            }
        } else if (flag == 5) {
            if (((buf[i] <= '9' && buf[i] >= '0') || buf[i] == '-') && strflag == 0) {
                start = i;
                strflag = 1;
            } else if (strflag == 1) {
                if (!((buf[i] <= '9' && buf[i] >= '0') || buf[i] == '.')) {
                    end = i;
                    gps.longitude = parseFloat(&(buf[start]), end - start);
                    start = 0;
                    end = 0;
                    strflag = 0;
                    flag = 6;
                }
            } else {
                flag = 6;
                strflag = 0;
            }
        }
        i++;
        // printf("flag:%d", flag);
        // osDelay(10);
    }
    return gps;
}

int airGPSRead(GPSInfo *gps)
{
    const unsigned char *str;
    int len = 0;
    unsigned char rx[300] = {0};
    str = "AT+CGNSINF\r\n";
    airUartRead(rx, sizeof(rx));
    airUartWrite(str, strlen(str));
    osDelay(15);
    if (len = airUartRead(rx, sizeof(rx))) {
        rx[len] = '\0';
        printf("length:%d\n", len);
        printf(rx);
        *gps = parseGPSInfo(rx, len);
        (*gps).laitude -= 0.00253f;
        (*gps).longitude += 0.005567f;
        if (((*gps).laitude < 0.05 && (*gps).laitude > -0.05) || ((*gps).longitude < 0.05 && (*gps).longitude > -0.05)) {
            return -1;
        }
        return 0;
    }
    return -1;
}

#define RAD(x) x*PI/180.0f
#define EARTH_RADIUS 6378137;
float getDistance(GPSInfo a, GPSInfo b)
{
    float latitude_cha = RAD(a.laitude) - RAD(b.laitude);
    float longitude_cha = RAD(a.longitude) - RAD(b.longitude);
    float s;
    s = 2 * asin(sqrt(pow(sin(latitude_cha/2), 2) + cos(RAD(a.laitude))*cos(RAD(b.laitude))*pow(sin(longitude_cha/2),2)));
    s = s * EARTH_RADIUS;
    return s;
}

void airMqttCfg()
{
    int len = 0;
    const unsigned char *str;
    unsigned char rx[300] = {0};

    str = "AT+CPIN?\r\n";
    airUartWrite(str, strlen(str));
    osDelay(20);
    if (len = airUartRead(rx, sizeof(rx))) {
        rx[len] = '\0';
        printf("length:%d\n", len);
        printf(rx);
    }
    str = "AT+MCONFIG="CONFIG_CLIENTID",";
    airUartWrite(str, strlen(str));
    str = CONFIG_DEVICE_ID",";
    airUartWrite(str, strlen(str));
    str = CONFIG_DEVICE_PWD"\r\n";
    airUartWrite(str, strlen(str));
    printf("ok1\n");
    osDelay(50);
    if (len = airUartRead(rx, sizeof(rx))) {
        rx[len] = '\0';
        printf("length:%d\n", len);
        printf(rx);
    }

    str = "AT+MIPSTART="CONFIG_SERVER", 1883\r\n";
    airUartWrite(str, strlen(str));
    printf("ok2\n");
    osDelay(50);
    if (len = airUartRead(rx, sizeof(rx))) {
        rx[len] = '\0';
        printf("length:%d\n", len);
        printf(rx);
    }

    str = "AT+MCONNECT=1,300\r\n";
    airUartWrite(str, strlen(str));
    printf("ok3\n");
    osDelay(100);
    if (len = airUartRead(rx, sizeof(rx))) {
        rx[len] = '\0';
        printf("length:%d\n", len);
        printf(rx);
    }

    str = "AT+MQTTMSGSET=1\r\n";
    airUartWrite(str, strlen(str));
    printf("ok4\n");
    osDelay(100);
    if (len = airUartRead(rx, sizeof(rx))) {
        rx[len] = '\0';
        printf("length:%d\n", len);
        printf(rx);
    }
    // osDelay(600);
    // if (len = airUartRead(rx, sizeof(rx))) {
    //     printf("length:%d\n", len);
    //     printf(rx);
    // }
}

// int airMqttPub(uint8_t qos, char *topic, char *payload)
// {
//     int rc = -1;
//     unsigned char rx[300] = {0};
//     char *buf;
//     hi_u32 len;
//     // 将一块连续的数据空间分配给buf，可以避免消息存储空间过于离散
//     len = strlen(AIR_PUB_FMT) + strlen(topic) + strlen(payload);
//     // printf("len:%d\n",len);
//     buf = hi_malloc(0, len);
//     memset_s(buf, len + 1, 0, len);
//     printf(payload);
//     printf("\n");
//     if (buf != NULL) {
//         if (snprintf_s(buf, len + 1, len, AIR_PUB_FMT, topic, 0, payload) < 0) {
//             printf("string is null\r\n");
//         }
//         airUartWrite(buf, strlen(buf));
//         // printf(buf);
//         // printf("\n");
//         osDelay(RECEVICE_DELAY);
//         if (len = airUartRead(rx, sizeof(rx))) {
//             rx[len] = '\0';
//             printf("length:%d\n", len);
//             printf(rx);
//         }
//         rc = 0;
//         hi_free(0, buf);
//     }
//     return rc;
// }

#define AIR_PUB_FMT "AT+MPUB=\"%s\",%d,0,\"%s\"\r\n"
int airMqttPub(uint8_t qos, char *topic, char *payload)
{
    int rc = 0;
    char *buf;
    hi_u32 len;
    // 将一块连续的数据空间分配给buf，可以避免消息存储空间过于离散
    len = strlen(AIR_PUB_FMT) + strlen(topic) + strlen(payload);
    // printf("len:%d\n",len);
    buf = hi_malloc(0, len);
    memset_s(buf, len + 1, 0, len);
    printf(payload);
    printf("\n");
    if (buf != NULL) {
        if (snprintf_s(buf, len + 1, len, AIR_PUB_FMT, topic, 0, payload) < 0) {
            printf("string is null\r\n");
            return -1;
        }
        if (IOT_SUCCESS != osMessageQueuePut(g_pubQueue, &buf, 0, 10)) {
            printf("Write queue failed\r\n");
            hi_free(0, buf);
            return -2;
        }
    }
    return rc;
}

int airMqttPubProcess(char *msg)
{
    int rc = -1;
    hi_u32 len;
    unsigned char rx[300] = {0};
    if (msg != NULL) {
        airUartWrite(msg, strlen(msg));
        // printf(buf);
        // printf("\n");
        osDelay(RECEVICE_DELAY);
        if (len = airUartRead(rx, sizeof(rx))) {
            rx[len] = '\0';
            printf("pub length:%d\n", len);
            printf(rx);
        }
        rc = 0;
    }
    return rc;
}

#define AIR_SUB_FMT "AT+MSUB=\"%s\",%d\r\n"
int airMqttSub(uint8_t qos, char *topic)
{
    int rc = -1;
    unsigned char rx[300] = {0};
    char *buf;
    hi_u32 len;
    // 将一块连续的数据空间分配给buf，可以避免消息存储空间过于离散
    len = strlen(AIR_SUB_FMT) + strlen(topic);
    buf = hi_malloc(0, len);
    if (buf != NULL) {
        if (snprintf_s(buf, len + 1, len, AIR_SUB_FMT, topic, qos) < 0) {
            printf("string is null\r\n");
        }
        airUartWrite(buf, strlen(buf));
        printf(buf);
        printf("\n");
        osDelay(50);
        if (len = airUartRead(rx, sizeof(rx))) {
            rx[len] = '\0';
            printf("length:%d\n", len);
            printf(rx);
        }
        rc = 0;
        hi_free(0, buf);
    }
    return rc;
}

int airMqttGetMsg(char *cmdid, char *msg)
{
    int len = 0;
    int paylen = 0;
    unsigned char *str;
    unsigned char rx[500] = {0};
    // unsigned char *rx = "\n+MSUB: \"$sys/f2YMWcDt9B/test1/cmd/request/8e62edc2-c7b0-4293-ae5c-b0904fbfcf97\",10 byte,howareyou?\r\n";
    unsigned char *index = NULL;

    str = "AT+MQTTMSGGET\r\n";
    airUartWrite(str, strlen(str));
    osDelay(20);
    if (len = airUartRead(rx, sizeof(rx))) {
        rx[len] = '\0';
        printf("length:%d\n", len);
        printf(rx);
    }
    index = strstr(rx, "request/");
    if (index != NULL) {
        index += strlen("request/");
        // printf("bias = %d", (int)(index - rx));
        memcpy_s(cmdid, 36, index, 36);
        cmdid[36] = 0;
        printf("cmdid ok\n");

        index += 36 + 2;
        paylen = parseInt(index, 3);
        printf("parseInt ok\n");
        
        index = strstr(index, ",") + 1;
        memcpy_s(msg, paylen, index, paylen);
        msg[paylen] = 0;

        printf(cmdid);
        printf("\n");
        // printf(msg);
        // printf("\n");
    } else {
        printf("no msg\n");
        return -1;
    }
    return 0;
}

void airMsgInit()
{
    unsigned char rx[300] = {0};
    unsigned char *str;
    hi_u32 len;
    str = "AT+CMGF=0\r\n";
    airUartWrite(str, strlen(str));
    osDelay(RECEVICE_DELAY);
    if (len = airUartRead(rx, sizeof(rx))) {
        rx[len] = '\0';
        printf("length:%d\n", len);
        printf(rx);
    }
}

void airSetPhoneNum(char *str)
{
    snprintf(g_phoneNum, 20, "86%sF", str);
    uint16_t len = strlen(g_phoneNum);
    printf("g_phone:%s, len: %d\n", g_phoneNum, len);
}

void exchangeEO(char *str, int len)
{
    int i = 0;
    char tmp;
    for (i=0;i<len/2;i++) {
        tmp = str[2*i];
        str[2*i] = str[2*i+1];
        str[2*i+1] = tmp;
    }
}

uint8_t airSendMsg(char *msg)
{
    int rc = -1;
    unsigned char rx[300] = {0};
    unsigned char str[13] = "AT+CMGS=";
    char *buf;
    int bufsize;
    hi_u32 len;
    int payloadLen = 0;
    char smscenter[19] = "0891";
    memcpy_s(smscenter+4, 14, SMSCENTERNUM, 14);
    smscenter[18] = 0;
    exchangeEO(smscenter+4, 14);
    char phone[29] = "11000D91";
    memcpy_s(phone+8, 14, g_phoneNum, 14);
    exchangeEO(phone+8, 14);
    memcpy_s(phone+22, 7, "000800", 7);
    phone[28] = 0;
    len = strlen(msg);
    bufsize = 18 + 28 + 2 + len + 1 + 1;
    buf = hi_malloc(0, bufsize);
    payloadLen = (28 + 2 + len) / 2;
    memcpy_s(buf, 18, smscenter, 18);
    memcpy_s(buf+18, 28, phone, 28);
    memcpy_s(buf+48, len, msg, len);
    buf[46] = (len/2) / 16 + '0';
    buf[47] = (len/2) % 16 + '0';
    buf[bufsize - 2] = 0x1A;
    buf[bufsize - 1] = 0;
    str[8] = (payloadLen / 10) + '0';
    str[9] = (payloadLen % 10) + '0';
    str[10] = '\r';
    str[11] = '\n';
    str[12] = '\0';
    airUartWrite(str, sizeof(str) - 1);
    osDelay(RECEVICE_DELAY);
    // printf("send msg success!\n");
    if (len = airUartRead(rx, sizeof(rx))) {
        rx[len] = '\0';
        printf("test length:%d\n", len);
        printf(rx);
    }
    airUartWrite(buf, bufsize - 1);
    printf("%s%s\n", str, buf);
    osDelay(100);
    if (len = airUartRead(rx, sizeof(rx))) {
        rx[len] = '\0';
        printf("send length:%d\n", len);
        printf(rx);
    }
    hi_free(0, buf);
}
