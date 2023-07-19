/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef IOT_CONFIG_H
#define IOT_CONFIG_H

// /<CONFIG THE LOG
/* if you need the iot log for the development , please enable it, else please comment it */
#define CONFIG_LINKLOG_ENABLE 1

// /<CONFIG THE WIFI
/* Please modify the ssid and pwd for the own */
#define CONFIG_AP_SSID  "wzc" // WIFI SSID
#define CONFIG_AP_PWD  "30442500" // WIFI PWD

#define PHONE "8613281268772F"          //电话号码前加86，后加F
// #define SMSCENTERNUM "8613800200500F"   //广东移动短信中心号码
#define SMSCENTERNUM "8613010200500F"   //广东联通短信中心号码

// /<Configure the iot platform
/* Please modify the device id and pwd for your own */
// 设备ID名称，请参考华为物联网云文档获取该设备的ID。例如:60790e01ba4b2702c053ff03_helloMQTT
// #define CONFIG_DEVICE_ID  "643d4a30168bb870b3564e35_202023320"
// // 设备密码，请参考华为物联网云文档设置该设备密码。例如：hispark2021
// #define CONFIG_DEVICE_PWD "2d0fca1383f9c2153414495c15688746f8c28c0dd6b8e509f6a1e9d1ceab07b6"
// #define CONFIG_CLIENTID "643d4a30168bb870b3564e35_202023320_0_0_2023041713"

// #define CONFIG_DEVICE_ID  "612261"
// #define CONFIG_DEVICE_PWD "qRPj1xUohDDL3UBV2uQutoXSQyI="
// #define CONFIG_CLIENTID "1105106676"

// #define CONFIG_DEVICE_ID  "6WbahF4uRp"
// #define CONFIG_DEVICE_PWD "version=2018-10-31&res=products%2F6WbahF4uRp%2Fdevices%2Ftest&et=1719938977&method=md5&sign=jO%2FPT4V5erVwmlhOiqkE9A%3D%3D"
// #define CONFIG_CLIENTID "test"
// #define CONFIG_DEVICE_ID  "f2YMWcDt9B"
// #define CONFIG_DEVICE_PWD "version=2018-10-31&res=products%2Ff2YMWcDt9B%2Fdevices%2Ftest1&et=1719938977&method=md5&sign=uqmmMbrgXqBH%2Fu2zAVLBvA%3D%3D"
// #define CONFIG_CLIENTID "test1"
#define CONFIG_DEVICE_ID  "f2YMWcDt9B"
#define CONFIG_DEVICE_PWD "version=2018-10-31&res=products%2Ff2YMWcDt9B%2Fdevices%2Fnew_test&et=1719938977&method=md5&sign=Bhodl5Lp2CSJ4z2V2x%2FRtg%3D%3D"
#define CONFIG_CLIENTID "new_test"
#define CONFIG_SERVER "mqtts.heclouds.com"

#define TUMBLE_MSG "80014EBA645450124E86FF018BF753BB5C0F7A0B5E8F67E5770B51774F534FE1606F3002"
#define OUT_MSG "8BF76CE8610F80014EBA53EF80FD79BB5F0075355B5056F4680FFF0C8BF7524D5F805C0F7A0B5E8F67E5770B51774F536D88606FFF01"
#define HELP_MSG "80014EBA63094E0B6C4265516309952EFF0C53EF80FD9700898183B753D65E2E52A9FF01"
// #define IN_MSG "80014EBA645450124E86FF018BF753BB5C0F7A0B5E8F67E5770B51774F534FE1606F3002"

#define CONFIG_CMD_REQUEST_TOPIC "$sys/f2YMWcDt9B/new_test/cmd/request/+"
#endif