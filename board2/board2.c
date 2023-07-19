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

#include <stdio.h>
#include <string.h>
// #include "iot_config.h"
// #include "iot_main.h"
// #include "iot_profile.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
// #include "wifi_connecter.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"
#include "iot_watchdog.h"
#include "hi_stdlib.h"
#include "board2.h"
#include "asrpro/asrpro.h"
#include "adc/iot_adc.h"
#include "more.h"
#include "task/community_task.h"
#include "task/more_task.h"

// /< this is the demo main task entry,here we will set the wifi/cjson/mqtt ready ,and
// /< wait if any work to do in the while

static void asrTask()
{
    unsigned char rx[200] = {0};
    int len = 0;
    while (1) {
        len = asrproUartRead(rx, sizeof(rx));
        if (len > 0) {
            
        }
        TaskMsleep(50);
    }

}

static void DemoEntry(void)
{
    while (1) {
        TaskMsleep(1000);
    }
}

// This is the demo entry, we create a task here, and all the works has been done in the demo_entry
#define CN_IOT_TASK_STACKSIZE  0x1000
#define CN_IOT_TASK_PRIOR 28
#define CN_IOT_TASK_NAME "IOTDEMO"
static void AppDemoIot(void)
{
    osThreadAttr_t attr;
    IoTWatchDogDisable();
    // init
    asrproUartInit();
    batInit();
    communityUartInit();
    lightInit();
    // lightSet(3);

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

    attr.name = "COMMUNITY_TASK";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CN_IOT_TASK_STACKSIZE;
    attr.priority = CN_IOT_TASK_PRIOR;

    if (osThreadNew((osThreadFunc_t)community_task, NULL, &attr) == NULL) {
        printf("[TrafficLight] Failed to create COMMUNITY_TASK!\n");
    }

    attr.name = "MORE_TASK";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CN_IOT_TASK_STACKSIZE;
    attr.priority = CN_IOT_TASK_PRIOR;

    if (osThreadNew((osThreadFunc_t)moreTask, NULL, &attr) == NULL) {
        printf("[TrafficLight] Failed to create MORE_TASK!\n");
    }
}

SYS_RUN(AppDemoIot);