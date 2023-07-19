#include <stdio.h>
#include "community_task.h"
#include "community/community.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_profile.h"

uint8_t g_batV = 0;

uint8_t getBatV()
{
    return g_batV;
}

void pubBatV(uint8_t bat)
{
    IoTProfileKV property1;
    memset_s(&property1, sizeof(property1), 0, sizeof(property1));
    property1.type = EN_IOT_DATATYPE_INT;
    property1.key = "bat";                 // 纬度
    property1.iValue = bat;
    IoTProfilePropertyReport(123, &property1);
}

void communityTask()
{
    Board2SendPackage package = {0};
    uint8_t str[100] = {0};
    int16_t len = 0;
    uint8_t i = 1;
    int ret = 0;
    while (1) {
        len = communityUartRead(str, 100);
        if (len > 0) {
            ret = recevicePackage(&package, str, len);
            if (ret == 1) {
                pubBatV(package.batV);
                g_batV = package.batV;
            }
            // printf("len:%d\n", len);
            // printf("batV:%d\n", package.batV);
        }
        sendPackage();
        osDelay(10);
    }
}


