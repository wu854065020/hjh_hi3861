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

#include <string.h>
#include <hi_mem.h>
#include <cJSON.h>
#include "hi_stdlib.h"
#include "stdio.h"
#include "hi_mem.h"
#include "iot_main.h"
#include "iot_profile.h"
#include "air780eg/air780eg.h"
#include "iot_config.h"

#ifdef cJSON

// format the report data to json string mode
// static cJSON *FormatProfileValue(IoTProfileKV *kv)
// {
//     cJSON  *ret = NULL;
//     switch (kv->type) {
//         case EN_IOT_DATATYPE_INT:
//             ret = cJSON_CreateNumber(kv->iValue);
//             break;
//         case EN_IOT_DATATYPE_LONG:
//             ret = cJSON_CreateNumber((double)(*(long *)kv->value));
//             break;
//         case EN_IOT_DATATYPE_STRING:
//             ret = cJSON_CreateString((const char *)kv->value);
//             break;
//         default:
//             break;
//     }
//     return ret;
// }

// static cJSON *MakeKvs(IoTProfileKV *kvlst)
// {
//     cJSON *root;
//     cJSON *kv;
//     IoTProfileKV *kvInfo;

//     // build a root node
//     root = cJSON_CreateObject();
//     if (root == NULL) {
//         return root;
//     }

//     // add all the property to the properties
//     kvInfo = kvlst;
//     while (kvInfo != NULL) {
//         kv = FormatProfileValue(kvInfo);
//         if (kv == NULL) {
//             if (root != NULL) {
//                 cJSON_Delete(root);
//                 root = NULL;
//             }
//             return root;
//         }

//         cJSON_AddItemToObject(root, kvInfo->key, kv);
//         kvInfo = kvInfo->nxt;
//     }
//     // OK, now we return it
//     return root;
// }

#define CN_PROFILE_SERVICE_KEY_SERVICEID "service_id"
#define CN_PROFILE_SERVICE_KEY_PROPERTIIES "properties"
#define CN_PROFILE_SERVICE_KEY_EVENTTIME "event_time"
#define CN_PROFILE_KEY_SERVICES "services"
// static cJSON *MakeService(IoTProfileService *serviceInfo)
// {
//     cJSON *root;
//     cJSON *serviceID;
//     cJSON *properties;
//     cJSON *eventTime;

//     // build a root node
//     root = cJSON_CreateObject();
//     if (root == NULL) {
//         return root;
//     }

//     // add the serviceID node to the root node
//     serviceID = cJSON_CreateString(serviceInfo->serviceID);
//     if (serviceID == NULL) {
//         if (root != NULL) {
//             cJSON_Delete(root);
//             root = NULL;
//         }
//         return root;
//     }
//     cJSON_AddItemToObjectCS(root, CN_PROFILE_SERVICE_KEY_SERVICEID, serviceID);

//     // add the properties node to the root
//     properties = MakeKvs(serviceInfo->serviceProperty);
//     if (properties == NULL) {
//         if (root != NULL) {
//             cJSON_Delete(root);
//             cJSON_Delete(properties);
//             root = NULL;
//         }
//         return root;
//     }
//     cJSON_AddItemToObjectCS(root, CN_PROFILE_SERVICE_KEY_PROPERTIIES, properties);
//     // add the event time (optional) to the root
//     if (serviceInfo->eventTime != NULL) {
//         eventTime = cJSON_CreateString(serviceInfo->eventTime);
//         if (eventTime == NULL) {
//             if (root != NULL) {
//                 cJSON_Delete(root);
//                 root = NULL;
//             }
//             return root;
//         }
//         cJSON_AddItemToObjectCS(root, CN_PROFILE_SERVICE_KEY_EVENTTIME, eventTime);
//     }
//     // OK, now we return it
//     return root;
// }
// // {"id":"1688453741812","params":{"led":{"value":true}}}

// static cJSON *MakeServices(IoTProfileService *serviceInfo)
// {
//     cJSON *services = NULL;
//     cJSON *service;
//     IoTProfileService  *serviceTmp;

//     // create the services array node
//     services = cJSON_CreateArray();
//     if (services == NULL) {
//         return services;
//     }

//     serviceTmp = serviceInfo;
//     while (serviceTmp != NULL) {
//         service = MakeService(serviceTmp);
//         if (service == NULL) {
//             return services;
//         }
//         cJSON_AddItemToArray(services, service);
//         serviceTmp = serviceTmp->nxt;
//     }

//     // now we return the services
//     return services;
// }

// use this function to make a topic to publish
// if request_id  is needed depends on the fmt
static char *MakeTopic(const char *fmt, const char *deviceId, const char *requestID)
{
    int len;
    char *ret = NULL;

    len = strlen(fmt) + strlen(deviceId);
    if (requestID != NULL) {
        len += strlen(requestID);
    }

    ret = hi_malloc(0, len);
    if (ret != NULL) {
        if (requestID != NULL) {
            if (snprintf_s(ret, len + 1, len, fmt, deviceId, requestID) < 0) {
                printf("string is null\r\n");
            }
        } else {
            if (snprintf_s(ret, len + 1, len, fmt, deviceId) < 0) {
                printf("string is null\r\n");
            }
        }
    }
    return ret;
}

// #define CN_PROFILE_CMDRESP_KEY_RETCODE    "result_code"
// #define CN_PROFILE_CMDRESP_KEY_RESPNAME    "response_name"
// #define CN_PROFILE_CMDRESP_KEY_PARAS    "paras"
// static char *MakeProfileCmdResp(IoTCmdResp *payload)
// {
//     char *ret = NULL;
//     cJSON *root;
//     cJSON *retCode;
//     cJSON *respName;
//     cJSON *paras;

//     // create the root node
//     root = cJSON_CreateObject();
//     if (root == NULL) {
//         return ret;
//     }

//     // create retcode and retdesc and add it to the root
//     retCode = cJSON_CreateNumber(payload->retCode);
//     if (retCode == NULL) {
//         if (root != NULL) {
//             cJSON_Delete(root);
//         }
//         return ret;
//     }
//     cJSON_AddItemToObjectCS(root, CN_PROFILE_CMDRESP_KEY_RETCODE, retCode);

//     if (payload->respName != NULL) {
//         respName = cJSON_CreateString(payload->respName);
//         if (respName == NULL) {
//             if (root != NULL) {
//                 cJSON_Delete(root);
//             }
//             return ret;
//         }
//         cJSON_AddItemToObjectCS(root, CN_PROFILE_CMDRESP_KEY_RESPNAME, respName);
//     }

//     if (payload->paras != NULL) {
//         paras = MakeKvs(payload->paras);
//         if (paras == NULL) {
//             if (root != NULL) {
//                 cJSON_Delete(root);
//             }
//             return ret;
//         }
//         cJSON_AddItemToObjectCS(root, CN_PROFILE_CMDRESP_KEY_PARAS, paras);
//     }

//     // OK, now we make it to a buffer
//     ret = cJSON_PrintUnformatted(root);
//     cJSON_Delete(root);
//     return ret;
// }

// #define CN_PROFILE_TOPICFMT_CMDRESP    "$oc/devices/%s/sys/commands/response/request_id=%s"
// int IoTProfileCmdResp(char *deviceID, IoTCmdResp *payload)
// {
//     int ret = -1;
//     char *topic;
//     char *msg;

//     if ((deviceID == NULL) || (payload == NULL) || (payload->requestID == NULL)) {
//         return ret;
//     }

//     topic = MakeTopic(CN_PROFILE_TOPICFMT_CMDRESP, deviceID, payload->requestID);
//     if (topic == NULL) {
//         return ret;
//     }
//     msg = MakeProfileCmdResp(payload);
//     if ((topic != NULL) && (msg != NULL)) {
//         ret = IotSendMsg(0, topic, msg);
//     }

//     hi_free(0, topic);
//     cJSON_free(msg);
//     return ret;
// }

// // static char *MakeProfilePropertyReport(IoTProfileService *payload)
// char *MakeProfilePropertyReport(IoTProfileService *payload)
// {
//     char *ret = NULL;
//     cJSON *root;
//     cJSON *services;

//     // create the root node
//     root = cJSON_CreateObject();
//     if (root == NULL) {
//         return ret;
//     }

//     // create the services array node to the root
//     services = MakeServices(payload);
//     if (services == NULL) {
//         if (root != NULL) {
//             cJSON_Delete(root);
//         }
//         return ret;
//     }
//     cJSON_AddItemToObjectCS(root, CN_PROFILE_KEY_SERVICES, services);

//     // OK, now we make it to a buffer
//     ret = cJSON_PrintUnformatted(root);
//     cJSON_Delete(root);
//     return ret;
// }

static cJSON *FormatProfileValue(IoTProfileKV *kv)
{
    cJSON *ret = NULL;
    cJSON *value = NULL;
    ret = cJSON_CreateObject();
    switch (kv->type) {
        case EN_IOT_DATATYPE_INT:
            value = cJSON_CreateNumber(kv->iValue);
            break;
        case EN_IOT_DATATYPE_LONG:
            value = cJSON_CreateNumber((double)(*(long *)kv->value));
            break;
        case EN_IOT_DATATYPE_STRING:
            value = cJSON_CreateString((const char *)kv->value);
            break;
        default:
            cJSON_Delete(ret);
            break;
    }
    cJSON_AddItemToObjectCS(ret, "value", value);
    return ret;
}

static cJSON *MakeKvs(IoTProfileKV *kvlst)
{
    cJSON *root;
    cJSON *kv;
    IoTProfileKV *kvInfo;

    // build a root node
    root = cJSON_CreateObject();
    if (root == NULL) {
        return root;
    }

    // add all the property to the properties
    kvInfo = kvlst;
    while (kvInfo != NULL) {
        kv = FormatProfileValue(kvInfo);
        if (kv == NULL) {
            if (root != NULL) {
                cJSON_Delete(root);
                root = NULL;
            }
            return root;
        }

        cJSON_AddItemToObject(root, kvInfo->key, kv);
        kvInfo = kvInfo->nxt;
    }
    // OK, now we return it
    return root;
}

char *MakeProfilePropertyReport(IoTProfileService *payload)
{
    char *ret = NULL;
    cJSON *root;
    cJSON *params;
    cJSON *Id;

    // create the root node
    root = cJSON_CreateObject();
    if (root == NULL) {
        return ret;
    }

    Id = cJSON_CreateString(payload->serviceID);
    cJSON_AddItemToObjectCS(root, "id", Id);
    // printf("make id ok\n");

    // create the services array node to the root
    params = MakeKvs(payload->serviceProperty);
    // printf("make kvs ok\n");
    if (params == NULL) {
        if (root != NULL) {
            cJSON_Delete(root);
        }
        return ret;
    }
    cJSON_AddItemToObjectCS(root, "params", params);
    // printf("make root ok");

    // OK, now we make it to a buffer
    ret = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    // printf("delete cJSON ok\n");
    return ret;
}

int strstrNum(char *str1, char *str2)
{
    int count = 0;
    int num2 = strlen(str2);
    if (!num2) {
        return 0;
    }
    int num1 = strlen(str1);
    while (num1 >= num2) {
        num1--;
        if (!memcmp(str1,str2, num2)) {
            count++;
        }
        str1++;
    }
    return count;
}

char *strReplace(char *str, char *old, char *new)
{
    int strnum = strstrNum(str, old);
    int strl = strlen(str);
    int oldl = strlen(old);
    int newl = strlen(new);
    int delta = newl - oldl;
    int j = 0;
    int bufsize = strlen(str) + delta * strnum;
    char *buf = hi_malloc(0, bufsize);
    if (buf != NULL) {
        while (strl >= oldl) {
            strl--;
            if (memcmp(str, old, oldl)) {
                buf[j] = *str;
                j++;
            } else {
                memcpy(&(buf[j]), new, newl);
                j += delta + 1;
            }
            str++;
        }
        buf[j] = '\0';
        return buf;
    }
    return NULL;
}

#define CN_PROFILE_TOPICFMT_PROPERTYREPORT    "$sys/%s/%s/thing/property/post"
int IoTProfilePropertyReport(char *deviceID, IoTProfileService *payload)
{
    int ret = -1;
    char *topic;
    char *msg;

    if ((deviceID == NULL) || (payload == NULL) || (payload->serviceID == NULL) || (payload->serviceProperty == NULL)) {
        return ret;
    }
    topic = MakeTopic(CN_PROFILE_TOPICFMT_PROPERTYREPORT, CONFIG_DEVICE_ID, CONFIG_CLIENTID);
    if (topic == NULL) {
        return ret;
    }
    // printf("make topic ok\n");
    msg = MakeProfilePropertyReport(payload);
    // printf("make payload ok\n");
    printf(msg);
    printf("\n");
    // 合宙AT指令，字符串内的“号需要替换成\22
    char *pay = strReplace(msg, "\"", "\\22");
    printf(pay);
    printf("\n");
    if ((topic != NULL) && (msg != NULL)) {
        ret = airMqttPub(0, topic, pay);
    }
    hi_free(0,pay);


    hi_free(0, topic);
    cJSON_free(msg);
    return ret;
}

// static IoTProfileKV *GetPropertys(cJSON *propertys)
// {
//     cJSON *t_cjpropertytmp;
//     IoTProfileKV *t_property;
//     t_cjpropertytmp = propertys->child;
//     while (t_cjpropertytmp != NULL)
//     {
//         printf("222\n");
//         IoTProfileKV *t_propertytmp = hi_malloc(0, sizeof(IoTProfileKV));
//         t_propertytmp->key = t_cjpropertytmp->string;
//         switch (t_cjpropertytmp->type)
//         {
//             case cJSON_Number:
//                 t_propertytmp->type = EN_IOT_DATATYPE_INT;
//                 t_propertytmp->iValue = t_cjpropertytmp->valueint;
//                 break;
//             case cJSON_String:
//                 t_propertytmp->type = EN_IOT_DATATYPE_STRING;
//                 t_propertytmp->value = t_cjpropertytmp->valuestring;
//         }
//         t_propertytmp->nxt = NULL;
//         if (t_property != NULL) {
//             t_propertytmp->nxt = t_property;
//             t_property = t_propertytmp;
//         } else{
//             t_property = t_propertytmp;
//         }
//         printf("cj1:%d\n", (int)t_cjpropertytmp);
//         t_cjpropertytmp = t_cjpropertytmp->next;    
//         printf("cj2:%d\n", (int)t_cjpropertytmp);
//     }
//     return t_property;
// }

// static IoTProfileService *GetServers(cJSON *cjService)
// {
//     IoTProfileService *t_service = NULL;
//     IoTProfileKV *t_property;
//     cJSON *t_cjServiceid, *t_cjservicetmp, *t_cjpropertys;
//     t_cjservicetmp = cjService;
//     while (t_cjservicetmp != NULL)
//     {
//         t_cjServiceid = t_cjservicetmp->child;
//         t_cjpropertys = t_cjServiceid->next;
//         IoTProfileService *t_servicetmp = hi_malloc(0, sizeof(IoTProfileService));
//         printf("111\n");
//         t_servicetmp->serviceID = t_cjServiceid->valuestring;
//         t_servicetmp->serviceProperty = GetPropertys(t_cjpropertys);
//         t_servicetmp->nxt = NULL;
//         if (t_service != NULL) {
//             t_servicetmp->nxt = t_service;
//             t_service = t_servicetmp;
//         } else{
//             t_service = t_servicetmp;
//         }
//         t_cjservicetmp = t_cjservicetmp->next;
//         printf("cj3:%d", (int)t_cjservicetmp);
//     }
//     return t_service;
// }

cJSON *IotProfileParse(char *payload)
{
    cJSON *t_root, *t_services;
    t_root = cJSON_Parse(payload);
    // t_services = t_root->child->child->child;
    return t_root;
}

cJSON *IotGetService(cJSON *root)
{
    return cJSON_GetObjectItem(root, "service_id");
}

// 匹配返回1，不匹配返回0
uint8_t IotIsService(cJSON *root, char *name)
{
    return !strcmp(IotGetService(root)->valuestring, name);
}

// 匹配返回1，不匹配返回0
uint8_t IotIsCommand(cJSON *root, char *name)
{
    return !strcmp(cJSON_GetObjectItem(root, "command_name")->valuestring, name);
}

cJSON *IotGetPropertys(cJSON *root)
{
    return cJSON_GetObjectItem(root, "paras");
}

int IotGetIntProperty(cJSON *root, char *name)
{
    cJSON *tmp = cJSON_GetObjectItem(IotGetPropertys(root), name);
    return tmp->valueint;
}

char *IotGetStringProperty(cJSON *root, char *name)
{
    cJSON *tmp = cJSON_GetObjectItem(IotGetPropertys(root), name);
    return tmp->valuestring;
}

float IotGetFloatProperty(cJSON *root, char *name)
{
    cJSON *tmp = cJSON_GetObjectItem(IotGetPropertys(root), name);
    return tmp->valuedouble;
}

void IotProfileDelete(cJSON *root)
{
    cJSON_Delete(root);
}

#else

static char *MakeTopic(const char *fmt, const char *deviceId, const char *clientId, const char *cmdId)
{
    int len;
    char *ret = NULL;

    len = strlen(fmt) + strlen(deviceId) + strlen(clientId);
    if (cmdId != NULL) {
        len += strlen(cmdId);
    }

    ret = hi_malloc(0, len);
    if (ret != NULL) {
        if (cmdId != NULL) {
            if (snprintf_s(ret, len + 1, len, fmt, deviceId, clientId, cmdId) < 0) {
                printf("string is null\r\n");
            }
        } else {
            if (snprintf_s(ret, len + 1, len, fmt, deviceId, clientId) < 0) {
                printf("string is null\r\n");
            }
        }
    }
    return ret;
}

static cJSON *FormatProfileValue(IoTProfileKV *kv)
{
    cJSON *ret = NULL;
    cJSON *value = NULL;
    cJSON *obj = NULL;
    ret = cJSON_CreateArray();
    obj = cJSON_CreateObject();
    switch (kv->type) {
        case EN_IOT_DATATYPE_INT:
            value = cJSON_CreateNumber(kv->iValue);
            break;
        case EN_IOT_DATATYPE_LONG:
            value = cJSON_CreateNumber((double)(*(long *)kv->value));
            break;
        case EN_IOT_DATATYPE_STRING:
            value = cJSON_CreateString((const char *)kv->value);
            break;
        case EN_IOT_DATATYPE_FLOAT:
        case EN_IOT_DATATYPE_DOUBLE:
            value = cJSON_CreateNumber(kv->LedValue);
            break;
        default:
            cJSON_Delete(ret);
            break;
    }
    cJSON_AddItemToObjectCS(obj, "v", value);
    cJSON_AddItemToArray(ret, obj);
    return ret;
}

static cJSON *MakeKvs(IoTProfileKV *kvlst)
{
    cJSON *root;
    cJSON *kv;
    IoTProfileKV *kvInfo;

    // build a root node
    root = cJSON_CreateObject();
    if (root == NULL) {
        return root;
    }

    // add all the property to the properties
    kvInfo = kvlst;
    while (kvInfo != NULL) {
        kv = FormatProfileValue(kvInfo);
        if (kv == NULL) {
            if (root != NULL) {
                cJSON_Delete(root);
                root = NULL;
            }
            return root;
        }

        cJSON_AddItemToObject(root, kvInfo->key, kv);
        // printf("make value %s\n", kvInfo->key);
        kvInfo = kvInfo->nxt;
    }
    // OK, now we return it
    return root;
}

char *MakeProfilePropertyReport(int id, IoTProfileKV *payload)
{
    char *ret = NULL;
    cJSON *root;
    cJSON *params;
    cJSON *Id;

    // create the root node
    root = cJSON_CreateObject();
    if (root == NULL) {
        return ret;
    }

    Id = cJSON_CreateNumber(id);
    cJSON_AddItemToObjectCS(root, "id", Id);
    // printf("make id ok\n");

    // create the services array node to the root
    params = MakeKvs(payload);
    // printf("make kvs ok\n");
    if (params == NULL) {
        if (root != NULL) {
            cJSON_Delete(root);
        }
        return ret;
    }
    cJSON_AddItemToObjectCS(root, "dp", params);
    // printf("make root ok");

    // OK, now we make it to a buffer
    ret = cJSON_PrintUnformatted(root);
    // printf(ret);
    // printf("\n");
    cJSON_Delete(root);
    // printf("delete cJSON ok\n");
    return ret;
}

int strstrNum(char *str1, char *str2)
{
    int count = 0;
    int num2 = strlen(str2);
    if (!num2) {
        return 0;
    }
    int num1 = strlen(str1);
    while (num1 >= num2) {
        num1--;
        if (!memcmp(str1,str2, num2)) {
            count++;
        }
        str1++;
    }
    return count;
}

char *strReplace(char *str, char *old, char *new)
{
    int strnum = strstrNum(str, old);
    int strl = strlen(str);
    int oldl = strlen(old);
    int newl = strlen(new);
    int delta = newl - oldl;
    int j = 0;
    int bufsize = strlen(str) + delta * strnum + 1;
    char *buf = hi_malloc(0, bufsize);
    memset_s(buf, bufsize + 1, 0 , bufsize);
    if (buf != NULL) {
        while (strl >= oldl) {
            strl--;
            if (memcmp(str, old, oldl)) {
                buf[j] = *str;
                j++;
            } else {
                memcpy(&(buf[j]), new, newl);
                j += delta + 1;
            }
            str++;
        }
        buf[j] = '\0';
        return buf;
    }
    return NULL;
}

#define CN_PROFILE_TOPICFMT_PROPERTYREPORT    "$sys/%s/%s/dp/post/json"
int IoTProfilePropertyReport(int id, IoTProfileKV *payload)
{
    int ret = -1;
    char *topic;
    char *msg;

    if (payload == NULL) {
        return ret;
    }
    topic = MakeTopic(CN_PROFILE_TOPICFMT_PROPERTYREPORT, CONFIG_DEVICE_ID, CONFIG_CLIENTID, NULL);
    if (topic == NULL) {
        return ret;
    }
    // printf("make topic ok\n");
    msg = MakeProfilePropertyReport(id, payload);
    // printf("make payload ok\n");
    // printf(msg);
    // printf("\n");
    // 合宙AT指令，字符串内的“号需要替换成\22
    char *pay = strReplace(msg, "\"", "\\22");
    // printf(pay);
    // printf("\n");
    if ((topic != NULL) && (msg != NULL)) {
        ret = airMqttPub(0, topic, pay);
    }
    hi_free(0,pay);


    hi_free(0, topic);
    cJSON_free(msg);
    return ret;
}

// char *MakeProfileCmdResp()
// {
//     cJSON
// }

#define CN_PROFILE_TOPICFMT_CMDRESP    "$sys/%s/%s/cmd/response/%s"
int IoTProfileCmdResp(char *cmdid)
{
    int ret = -1;
    char *topic;
    char *msg;

    // if ((deviceID == NULL) || (payload == NULL) || (payload->requestID == NULL)) {
    //     return ret;
    // }

    topic = MakeTopic(CN_PROFILE_TOPICFMT_CMDRESP, CONFIG_DEVICE_ID, CONFIG_CLIENTID, cmdid);
    if (topic == NULL) {
        return ret;
    }
    // msg = MakeProfileCmdResp();
    msg = "ok!";
    if ((topic != NULL) && (msg != NULL)) {
        ret = airMqttPub(0, topic, msg);
    }

    hi_free(0, topic);
    // cJSON_free(msg);
    return ret;
}

#endif