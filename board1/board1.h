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


#ifndef APP_DEMO_IOT_H
#define APP_DEMO_IOT_H

#include <stdio.h>
#include <string.h>
#include "iot_config.h"
#include "iot_profile.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifi_connecter.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"
#include "iot_watchdog.h"
#include "cjson_init.h"
#include "hi_stdlib.h"
#include "key/key.h"
#include "air780eg/air780eg.h"
#include "adc/iot_adc.h"
#include "hi_spi.h"
#include "st7789/st7789.h"
#include "mpu6050/mpu6050.h"
#include "mpu6050/inv_mpu.h"
#include "iot_pwm.h"
#include "hi_pwm.h"
#include "menu/menu.h"
#include "task/time_task.h"
#include "task/ui_task.h"
#include "task/community_task.h"
#include "task/air_task.h"
#include "task/tumble_detect_task.h"
#include "task/wifi_task.h"

typedef struct {
    hi_u32 latitude_safe;
    hi_u32 longitude_safe;
    hi_u32 position_alarm;
    hi_u32 spo2_alarm;
    hi_u8 phone_num[16];
} safe_value;

void st7789LightSet(int8_t lcdLight);

#endif