#include <stdio.h>
#include "air_task.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "air780eg/air780eg.h"
#include "hi_nv.h"
#include "hi_nvm.h"
#include "board1.h"
#include "community/community.h"

void *g_pubQueue;

extern int g_connected;
int g_last_connected = 0;

safe_value g_nvSafe = {0};

static float alarm_distance[] = {200, 500, 1000, 2000};

GPSInfo g_safeGPS = {0};
uint32_t g_position_alarm = 0;

extern uint8_t g_tumbleFlag;
extern uint8_t g_helpFlag;

void airTaskInit()
{
    g_pubQueue = osMessageQueueNew(20, sizeof(void *), NULL);
}

void pubAlarm(uint8_t alarm)
{
    IoTProfileKV property1;
    memset_s(&property1, sizeof(property1), 0, sizeof(property1));
    property1.type = EN_IOT_DATATYPE_INT;
    property1.key = "alarm";                 // 纬度
    property1.iValue = alarm;
    IoTProfilePropertyReport(123, &property1);
}

void pubGPS(GPSInfo gps)
{
    IoTProfileKV property1;
    IoTProfileKV property2;
    // GPSInfo gps = {0};
    // gps = airGPSRead();
    memset_s(&property1, sizeof(property1), 0, sizeof(property1));
    memset_s(&property2, sizeof(property2), 0, sizeof(property2));
    property1.type = EN_IOT_DATATYPE_DOUBLE;
    property1.key = "latitude";                 // 纬度
    property1.LedValue = gps.laitude;
    property2.type = EN_IOT_DATATYPE_DOUBLE;
    property2.key = "longitude";                // 经度
    property2.LedValue = gps.longitude;
    property1.nxt = &property2;
    IoTProfilePropertyReport(123, &property1);
}

void cmdReportData()
{
    char cmdid[100];
    char msg[300];
    // char *msg = "{\"latitude_safe\":23.051462766709125,\"longitude_safe\":113.38002759569724,\"position_alarm\":0,\"spo2_alarm\":0}";
    uint8_t ret;
    uint8_t alarm_index = 0;
    cJSON *root = NULL, *tmp = NULL;
    IoTProfileKV property1 = {0}, property2 = {0}, property3 = {0}, property4 = {0};       
    ret = airMqttGetMsg(cmdid, msg);
    if (!ret) {
        root = cJSON_Parse(msg);
        tmp = cJSON_GetObjectItem(root, "latitude_safe");
        if (tmp != NULL) {
            property1.type = EN_IOT_DATATYPE_DOUBLE;
            property1.key = "latitude_safe";                 // 纬度
            property1.nxt = &property2;

            property2.type = EN_IOT_DATATYPE_DOUBLE;
            property2.key = "longitude_safe";
            property2.nxt = &property3;

            property3.type = EN_IOT_DATATYPE_DOUBLE;
            property3.key = "position_alarm";                 
            property3.nxt = &property4;

            property4.type = EN_IOT_DATATYPE_DOUBLE;
            property4.key = "spo2_alarm";          
            property1.LedValue = cJSON_GetNumberValue(tmp);
            g_nvSafe.latitude_safe = *((uint32_t *)(&property1.LedValue));
            g_safeGPS.laitude = property1.LedValue;

            tmp = cJSON_GetObjectItem(root, "longitude_safe");
            property2.LedValue = cJSON_GetNumberValue(tmp);
            g_nvSafe.longitude_safe = *((uint32_t *)(&property2.LedValue));
            g_safeGPS.longitude = property2.LedValue;

            tmp = cJSON_GetObjectItem(root, "position_alarm");
            property3.LedValue = cJSON_GetNumberValue(tmp);
            g_nvSafe.position_alarm = property3.LedValue;
            alarm_index = property3.LedValue;
            g_position_alarm = alarm_distance[alarm_index];

            tmp = cJSON_GetObjectItem(root, "spo2_alarm");
            property4.LedValue = cJSON_GetNumberValue(tmp);
            g_nvSafe.spo2_alarm = property4.LedValue;

            ret = hi_factory_nv_write(0x0B, &g_nvSafe, sizeof(safe_value), 0);
            // printf("ret:%d\n",ret);
            printf("nv write:%d %d %d %d\n",g_nvSafe.latitude_safe, g_nvSafe.longitude_safe, g_nvSafe.position_alarm, g_nvSafe.spo2_alarm);

            // printf(msg);
            // printf("\n");
            // printf("%f %f %f %f\n", property1.LedValue, property2.LedValue, property3.LedValue, property4.LedValue);
            printf("latitude:%f longitude:%f\n", g_safeGPS.laitude, g_safeGPS.longitude);
            IoTProfilePropertyReport(1234, &property1);
            cJSON_free(root);
        }
        tmp = cJSON_GetObjectItem(root, "speak_cmd");
        if (tmp != NULL) {
            uint8_t speak = cJSON_GetNumberValue(tmp);
            printf("speak: %d\n", speak);
            speakCmd(speak);
        }
        tmp = cJSON_GetObjectItem(root, "phone");
        if (tmp != NULL) {
            char *phone = cJSON_GetStringValue(tmp);
            uint16_t len = strlen(phone);
            printf("phone: %s, len: %d\n", phone, len);
            memcpy_s(g_nvSafe.phone_num, 16, phone, 12);
            ret = hi_factory_nv_write(0x0B, &g_nvSafe, sizeof(safe_value), 0);
            airSetPhoneNum(phone);
            property1.key = "phone";
            property1.type = EN_IOT_DATATYPE_STRING;
            property1.value = phone;
            IoTProfilePropertyReport(12345, &property1);
            cJSON_free(phone);
            cJSON_free(root);
        }
        IoTProfileCmdResp(cmdid);
    }
}

#define CN_QUEUE_WAITTIMEOUT 10
static int ProcessQueueMsg()
{
    hi_u32     ret;
    uint8_t     msgSize;
    char *msg;
    hi_u32     timeout;

    timeout = CN_QUEUE_WAITTIMEOUT;
    msg = NULL;
    msgSize = sizeof(hi_pvoid);
    // 从队列中获取数据，当发送消息时则会往队列中塞入数据
    ret = osMessageQueueGet(g_pubQueue, &msg, &msgSize, timeout);
    if (ret != 0) {
        return -1;
    }
    if (msg != NULL) {
        airMqttPubProcess(msg);
        hi_free(0, msg);
    }
    timeout = 0;  // continuos to deal the message without wait here
    return 0;
}

void airTask()
{
    int ret = 0;
    uint32_t start = 0;
    uint8_t outFlag = 0;
    uint8_t isGpsGet = 0;
    GPSInfo last_gps = {0};
    GPSInfo gps = {0};
    float distance = 0;
    int16_t i = 1;
    ret = hi_factory_nv_read(0x0B, &g_nvSafe, sizeof(safe_value), 0);
    g_safeGPS.laitude = *((float *)(&g_nvSafe.latitude_safe));
    g_safeGPS.longitude = *((float *)(&g_nvSafe.longitude_safe));
    g_position_alarm = alarm_distance[g_nvSafe.position_alarm];
    airSetPhoneNum(g_nvSafe.phone_num);
    printf("nv read:latitude:%f longitude:%f position_alarm:%d\n", g_safeGPS.laitude, g_safeGPS.longitude, g_position_alarm);
    printf("phone: %s\n",g_nvSafe.phone_num);
    while (1) {
        ret = airGPSRead(&gps);
        if (!(gps.time.year <= 10)) {
            timeUpdate(&gps.time);
            break;
        }
        osDelay(20);
    }
    while (1) {
        start = osKernelGetTickCount();
        if (i % 10 == 0) {
            cmdReportData();
        }
        // 50 * 10 * 10 = 5s
        if (g_tumbleFlag) {
            if (g_tumbleFlag == 1) {
                airSendMsg(TUMBLE_MSG);
                osDelay(10);
                pubAlarm(TUMBLE);
                osDelay(5);
                g_tumbleFlag = 2;
            } else if (g_tumbleFlag == 2) {
                ret = airGPSRead(&gps);
                if (!ret) {
                    pubGPS(gps);
                    g_tumbleFlag = 0;
                }
            }
        }
        if (g_helpFlag) {
            airSendMsg(HELP_MSG);
            osDelay(10);
            pubAlarm(OLD_MAN_HELP);
            g_helpFlag = 0;
        }
        if (i >= 100) {
            if (!g_connected) {
                ret = airGPSRead(&gps);
                if (!ret) {
                    if (!isGpsGet) {
                        pubGPS(gps);
                        last_gps = gps;
                        isGpsGet = 1;
                    } else {
                        distance = getDistance(gps, last_gps);
                        printf("last distance:%f\n", distance);
                        if (distance > 15.0f) {
                            distance = getDistance(gps, g_safeGPS);
                            printf("safe distance:%f\n", distance);
                            if (!outFlag) {
                                if (distance > g_position_alarm / 2 + 20) {
                                    outFlag = 1;
                                    printf("old man get out! position_alarm:%d\n", g_position_alarm);
                                    pubAlarm(OUT_POSITION);
                                    airSendMsg(OUT_MSG);
                                    osDelay(5);
                                }
                            } else if (outFlag) {
                                if (distance < g_position_alarm / 2 - 10) {
                                    outFlag = 0;
                                    printf("old man return! position_alarm:%d\n", g_position_alarm);
                                    pubAlarm(IN_POSITION);
                                }
                            }
                            pubGPS(gps);
                            last_gps = gps;
                        }
                    }
                    
                }
            } else {
                if (!g_last_connected) {
                    pubGPS(g_safeGPS);
                    pubAlarm(OLD_MAN_IN_HOME);
                }
            }
            g_last_connected = g_connected;
            printf("ret:%d\n", ret);
            i = 0;
        }
        // if (g_batV != 0) {
        //     pubBatV(g_batV);
        //     osDelay(5);
        // }
        ProcessQueueMsg();
        i++;
        osDelayUntil(start + 10);
    }
}
