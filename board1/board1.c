/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "board1.h"
#include <unistd.h>
#include <math.h>
#include "hi_nvm.h"
#include "hi_nv.h"

#define ST7789_BLE_PORT IOT_IO_NAME_GPIO_8

void st7789BoardInit()
{
    // LED3的GPIO初始化 GPIO initialization of LED3
    IoTGpioInit(IOT_IO_NAME_GPIO_2);
    IoTGpioInit(IOT_IO_NAME_GPIO_14);
    // 设置GPIO9的管脚复用关系为GPIO Set the pin reuse relationship of GPIO9 to GPIO
    IoSetFunc(IOT_IO_NAME_GPIO_2, IOT_IO_FUNC_GPIO_2_GPIO);
    IoSetFunc(IOT_IO_NAME_GPIO_14, IOT_IO_FUNC_GPIO_14_GPIO);
    // GPIO方向设置为输出 GPIO direction set to output
    IoTGpioSetDir(IOT_IO_NAME_GPIO_2, IOT_GPIO_DIR_OUT);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_14, IOT_GPIO_DIR_OUT);

    uint32_t ret;
    uint8_t test[1] = {1};
    hi_spi_cfg_init_param t_initParam = {
        .is_slave = 0,
        .pad = 0
    };
    hi_spi_cfg_basic_info t_spiInfo = {
        .cpol = HI_SPI_CFG_CLOCK_CPOL_1,
        .cpha = HI_SPI_CFG_CLOCK_CPHA_0,
        .fram_mode = HI_SPI_CFG_FRAM_MODE_MOTOROLA,
        .data_width = HI_SPI_CFG_DATA_WIDTH_E_8BIT,
        .endian = HI_SPI_CFG_ENDIAN_LITTLE,
        .freq = 20000000
    };
    ret = hi_spi_init(HI_SPI_ID_0, t_initParam, &t_spiInfo);
    printf("spi_init ret:%d\n", ret);
    IoSetFunc(IOT_IO_NAME_GPIO_10, IOT_IO_FUNC_GPIO_10_SPI0_CK);
    // IoSetFunc(IOT_IO_NAME_GPIO_1, IOT_IO_FUNC_GPIO_1_SPI1_RXD);
    IoSetFunc(IOT_IO_NAME_GPIO_9, IOT_IO_FUNC_GPIO_9_SPI0_TXD);

    IoTGpioInit(ST7789_BLE_PORT);
    IoSetFunc(ST7789_BLE_PORT, IOT_IO_FUNC_GPIO_8_PWM1_OUT);
    IoTGpioSetDir(ST7789_BLE_PORT, IOT_GPIO_DIR_OUT);
    IoTPwmInit(HI_PWM_PORT_PWM1);
    osDelay(5);
}

void st7789LightSet(int8_t lcdLight)
{
    uint16_t pwm = 0;
    if (lcdLight > 0 && lcdLight <= 10) {
        pwm = (uint16_t)((lcdLight - 1) / (10.0f - 1.0f) * (99.0f - 9.0f) + 9.0f);
        IoTPwmStart(HI_PWM_PORT_PWM1, pwm, 2500);
    } else if (lcdLight == -1) {
        IoTPwmStop(HI_PWM_PORT_PWM1);
    }
}

float laititude[] = {23.0565085, 23.0560263, 23.0557354, 23.0557187, 23.0554527, 23.0552864, 23.0553446};
float longitude[] = {113.3944218, 113.3939547, 113.3930474, 113.3917897, 113.3907836, 113.3899482, 113.3887265};


// void IotPublishSample(void)
// {
//     IoTProfileKV property1;
//     IoTProfileKV property2;
//     static uint8_t i = 0;
//     // GPSInfo gps = {0};
//     // gps = airGPSRead();
//     memset_s(&property1, sizeof(property1), 0, sizeof(property1));
//     memset_s(&property2, sizeof(property2), 0, sizeof(property2));
//     if (i < 7) {
//         property1.type = EN_IOT_DATATYPE_DOUBLE;
//         property1.key = "latitude";                 // 纬度
//         property1.LedValue = laititude[i];
//         property2.type = EN_IOT_DATATYPE_DOUBLE;
//         property2.key = "longitude";                // 经度
//         property2.LedValue = longitude[i];
//         i++;
//         property1.nxt = &property2;
//         IoTProfilePropertyReport(123, &property1);
//     }
// }

// void cmdReportData()
// {
//     char cmdid[100];
//     char msg[300];
//     // char *msg = "{\"latitude_safe\":23.051462766709125,\"longitude_safe\":113.38002759569724,\"position_alarm\":0,\"spo2_alarm\":0}";
//     uint8_t ret;
//     safe_value nv = {0};
//     cJSON *root = NULL, *tmp = NULL;
//     IoTProfileKV property1 = {0}, property2 = {0}, property3 = {0}, property4 = {0};
//     property1.type = EN_IOT_DATATYPE_DOUBLE;
//     property1.key = "latitude_safe";                 // 纬度
//     property1.nxt = &property2;

//     property2.type = EN_IOT_DATATYPE_DOUBLE;
//     property2.key = "longitude_safe";
//     property2.nxt = &property3;

//     property3.type = EN_IOT_DATATYPE_DOUBLE;
//     property3.key = "position_alarm";                 
//     property3.nxt = &property4;

//     property4.type = EN_IOT_DATATYPE_DOUBLE;
//     property4.key = "spo2_alarm";                 
//     ret = airMqttGetMsg(cmdid, msg);
//     if (!ret) {
//         root = cJSON_Parse(msg);
//         tmp = cJSON_GetObjectItem(root, "latitude_safe");
//         property1.LedValue = cJSON_GetNumberValue(tmp);
//         nv.latitude_safe = *((uint32_t *)(&property1.LedValue));

//         tmp = cJSON_GetObjectItem(root, "longitude_safe");
//         property2.LedValue = cJSON_GetNumberValue(tmp);
//         nv.longitude_safe = *((uint32_t *)(&property2.LedValue));

//         tmp = cJSON_GetObjectItem(root, "position_alarm");
//         property3.LedValue = cJSON_GetNumberValue(tmp);
//         nv.position_alarm = property3.LedValue;

//         tmp = cJSON_GetObjectItem(root, "spo2_alarm");
//         property4.LedValue = cJSON_GetNumberValue(tmp);
//         nv.spo2_alarm = property4.LedValue;

//         ret = hi_factory_nv_write(0x0B, &nv, sizeof(safe_value), 0);
//         printf("ret:%d\n",ret);
//         printf("%d %d %d %d\n",nv.latitude_safe, nv.longitude_safe, nv.position_alarm, nv.spo2_alarm);

//         printf(msg);
//         printf("\n");
//         printf("%f %f %f %f\n", property1.LedValue, property2.LedValue, property3.LedValue, property4.LedValue);
//         IoTProfilePropertyReport(1234, &property1);
//         IoTProfileCmdResp(cmdid);
//     }
// }

// /< this is the demo main task entry,here we will set the wifi/cjson/mqtt ready ,and
// /< wait if any work to do in the while
// static void DemoEntry(void)
// {
//     // // 连接热点
//     // ConnectToHotspot();
//     // RedLedInit();
//     // // Cjson初始化直接用cjson_init.c提供的函数即�?
//     CJsonInit();
//     // // 创建一个新进程，负责初始化mqtt客服端，并断线重连，以及循环发布消息，触发消息接收回�?
//     // IoTMain();
//     // // 设置消息接收回调
//     // IoTSetMsgCallback(DemoMsgRcvCallBack);
//     // TaskMsleep(30000); // 30000 = 3s连接华为�?
// // /* 主动上报 */
// //     while (1) {
// //         // here you could add your own works here--we report the data to the IoTplatform
// //         TaskMsleep(TASK_SLEEP_1000MS);
// //         // know we report the data to the iot platform
// //         IotPublishSample();
// // //     }
// //     keyInit();
// //     uint8_t key;
//     // unsigned int data = 1864;
//     // float keyV;
//     // keyV = (float)data*3.28f/1864.0f;
//     // printf("ADC Value is %f \r\n", keyV);
//     // while (1) {
//     //     TaskMsleep(1000);
//     //     key = keyRead();
//     //     printf("%d\n", key);
//     // }

//     // printf("fast reset\n");
//     // osDelay(300);
//     // st7789SPIInit();
//     airUartInit();
//     airGPSOpen();
//     airMqttCfg();
//     airMsgInit();
//     airMqttSub(0, CONFIG_CMD_REQUEST_TOPIC);
//     // airSendMsg("80014EBA645450124E86FF018BF753BB5C0F7A0B5E8F67E5770B51774F534FE1606F3002");

//     // char *str = "sjdaijiaso:1,0,,,50.60,0.000";
//     // GPSInfo gps = parseGPSInfo(str, strlen(str));

//     // IotPublishSample();
//     while (1) {
//         TaskMsleep(1000);
//         cmdReportData();
//         // gps = airGPSRead();
//         // printf("%d %d %d %f %f", gps.runStatus, gps.fixStatus, gps.time.year, gps.laitude, gps.longitude);
//         // IotPublishSample();
//     }
// }
static void boardInit()
{
    communityUartInit();
    airUartInit();
    st7789BoardInit();
    MPU_IIC_Init();
}

static void DemoEntry(void)
{
    // // 连接热点
    // ConnectToHotspot();
    // GPSInfo gps = {0};
    // float latitude = 0;

    // float lat, longi;
    // int ret = 0;
    // char strtest[20];
    // char strnew[20] = "saidojidossad";
    // snprintf_s(strtest, 20+1, 20, "%f", laititude[0]);
    // safe_value nv = {0};
    // safe_value nvnew = {0};
    // memcpy_s(&nv.latitude_safe[0], 20, strtest, 9);
    // nv.latitude_safe = *((uint32_t *)(&laititude[0]));
    // nv.longitude_safe = *((uint32_t *)(&longitude[0]));
    // nv.latitude_safe = 80;
    // nv.longitude_safe = 160;
    // nv.position_alarm = 100;
    // nv.spo2_alarm = 80;
    // ret = hi_factory_nv_write(0x0B, &nv, sizeof(safe_value), 0);
    // printf("ret:%d\n",ret);
    // ret = hi_factory_nv_read(0x0B, &nvnew, sizeof(safe_value), 0);
    // lat = *((float *)(&nvnew.latitude_safe));
    // printf("ret:%d\n",ret);
    // printf("latitude:%f\n", lat);
    // printf("position:%d\n", nvnew.position_alarm);

    // char strtest[20];
    // char strnew[20] = "saidojidossad";
    // snprintf_s(strtest, 20+1, 20, "%f", laititude[0]);
    // printf(strtest);
    // printf("\n");
    // AddKVCache("latitude", strtest, 1);
    // int ret = GetValueByCache("latitude", strnew, 10);
    // printf("ret:%d\n", ret);
    // printf(strnew);

    // printf("\n");
    // printf("latitude :%f\n", latitude);
    // float latitude_safe = ;
    // float longitude_safe = ;
    // float r = 100;

    // GPSInfo a,b;
    // a.laitude = laititude[0];
    // a.longitude = longitude[0];
    // b.laitude = laititude[1];
    // b.longitude = longitude[1];
    // float dis = getDistance(a,b);
    // printf("distance:%f\n", dis);
    // gps = airGPSRead();
    // timeUpdate(&gps.time);
    
    // airMqttCfg();
    // airMsgInit();
    // airMqttSub(0, CONFIG_CMD_REQUEST_TOPIC);
    // airSendMsg("80014EBA645450124E86FF018BF753BB5C0F7A0B5E8F67E5770B51774F534FE1606F3002");

    // char *str = "sjdaijiaso:1,0,,,50.60,0.000";
    // GPSInfo gps = parseGPSInfo(str, strlen(str));

    // IotPublishSample();
    while (1) {
        // TaskMsleep(100);
        osDelay(100);
        // cmdReportData();
        // gps = airGPSRead();
        // printf("%d %d %d %f %f", gps.runStatus, gps.fixStatus, gps.time.year, gps.laitude, gps.longitude);
        // IotPublishSample();
    }
}

// This is the demo entry, we create a task here, and all the works has been done in the demo_entry
#define CN_IOT_TASK_STACKSIZE  0x1000
#define CN_IOT_TASK_PRIOR 28
#define CN_IOT_TASK_NAME "IOTDEMO"
static void AppDemoIot(void)
{
    osThreadAttr_t attr;
    GPSInfo gps = {0};
    IoTWatchDogDisable();
    
    boardInit();
    CJsonInit();
    ST7789_Init();
    st7789LightSet(7);
    printf("fast reset\n");
    osDelay(200);
    airMqttCfg();
    airMqttSub(0, CONFIG_CMD_REQUEST_TOPIC);
    airGPSOpen();
    airMsgInit();
    airTaskInit();

    attr.name = "IOTDEMO";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CN_IOT_TASK_STACKSIZE;
    attr.priority = CN_IOT_TASK_PRIOR;

    if (osThreadNew((osThreadFunc_t)DemoEntry, NULL, &attr) == NULL) {
        printf("[TrafficLight] Failed to create IOTDEMO!\n");
    }
    
    attr.name = "TIME_TASK";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CN_IOT_TASK_STACKSIZE;
    attr.priority = CN_IOT_TASK_PRIOR + 1;
    
    if (osThreadNew((osThreadFunc_t)timeTask, NULL, &attr) == NULL) {
        printf("[keyTask] Failed to create TIME_TASK!\n");
    }
    
    attr.name = "UI_TASK";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CN_IOT_TASK_STACKSIZE;
    attr.priority = CN_IOT_TASK_PRIOR;
    
    if (osThreadNew((osThreadFunc_t)uiTask, NULL, &attr) == NULL) {
        printf("[keyTask] Failed to create UI_TASK!\n");
    }
    
    attr.name = "COMMUNITY_TASK";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CN_IOT_TASK_STACKSIZE;
    attr.priority = CN_IOT_TASK_PRIOR;
    
    if (osThreadNew((osThreadFunc_t)communityTask, NULL, &attr) == NULL) {
        printf("[keyTask] Failed to create COMMUNITY_TASK!\n");
    }
    
    attr.name = "AIR_TASK";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CN_IOT_TASK_STACKSIZE;
    attr.priority = CN_IOT_TASK_PRIOR;
    
    if (osThreadNew((osThreadFunc_t)airTask, NULL, &attr) == NULL) {
        printf("[keyTask] Failed to create AIR_TASK!\n");
    }
    
    attr.name = "WIFI_TASK";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CN_IOT_TASK_STACKSIZE;
    attr.priority = CN_IOT_TASK_PRIOR;
    
    if (osThreadNew((osThreadFunc_t)wifiTask, NULL, &attr) == NULL) {
        printf("[keyTask] Failed to create WIFI_TASK!\n");
    }
    
    
    attr.name = "TUMBLE_DETECT_TASK";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CN_IOT_TASK_STACKSIZE;
    attr.priority = CN_IOT_TASK_PRIOR;
    
    if (osThreadNew((osThreadFunc_t)tumbleDetectTask, NULL, &attr) == NULL) {
        printf("[keyTask] Failed to create TUMBLE_DETECT_TASK!\n");
    }
}

SYS_RUN(AppDemoIot);