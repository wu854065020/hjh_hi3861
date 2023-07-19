#include "tumble_detect_task.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "mpu6050/mpu6050.h"
#include "mpu6050/inv_mpu.h"
#include "iot_uart.h"
#include "packge.h"
#include "ringbuffer/ringbuffer.h"
#include "air780eg/air780eg.h"
#include "air_task.h"
#include "iot_config.h"

#define GRAVITY 9.788f
#define BUFFER_LENGTH 400

uint8_t g_tumbleFlag = 0;

RingBuffer_Handle g_ringBuffer = {0};
float buffer[BUFFER_LENGTH] = {0};

void findMax(uint16_t *max_index, float *max, RingBuffer_Handle *buf, uint16_t bufIndex, uint16_t len){
    float t_max = RingBufIndex(buf, bufIndex);
    float t_maxIndex = bufIndex;
    float temp;
    if (len == 1) {
        *max_index = t_maxIndex;
        *max = t_max;
        return;
    }
    for (uint16_t i = bufIndex+1;i < bufIndex + len - 1; i++) {
        temp = RingBufIndex(buf, i);
        if (temp > t_max) {
            t_max = temp;
            t_maxIndex = i;
        }
    }
    *max_index = t_maxIndex;
    *max = t_max;
}

uint8_t g_flag = 0;
int8_t detectTumble()
{
    uint16_t index = 0;
    uint8_t flag = 0;
    uint16_t timeToClear = 0;
    uint8_t countFlag = 0;
    uint16_t count = 0;
    uint16_t timeToClearCount = 0;
    uint16_t maxIndex;
    float max;
    while (index < BUFFER_LENGTH - 2) {
        if (flag < 4 && timeToClear >= 80) {
            timeToClear = 0;
            flag = 0;
        }
        if (flag == 0) {
            if (RingBufIndex(&g_ringBuffer, index+1) - RingBufIndex(&g_ringBuffer, index) > 15) {
                flag = 1;
            }
        } else if (flag == 1) {
            findMax(&maxIndex, &max, &g_ringBuffer, index, 4);
            if (maxIndex + 1 < BUFFER_LENGTH) {
                // if (max > 65 && RingBufIndex(&g_ringBuffer, maxIndex - 1) > 20 && RingBufIndex(&g_ringBuffer, maxIndex + 1) > 10) {
                if (max > 50 && RingBufIndex(&g_ringBuffer, maxIndex - 1) > 9 && RingBufIndex(&g_ringBuffer, maxIndex + 1) > 9) {
                    flag = 2;
                }
            }
        } else if (flag == 2) {
            if (RingBufIndex(&g_ringBuffer, index) - RingBufIndex(&g_ringBuffer, index+1) > 15) {
                flag = 3;
            }
        } else if (flag == 3) {
            if ((RingBufIndex(&g_ringBuffer, index) > 8.0f && RingBufIndex(&g_ringBuffer, index) < 11.0f) &&
                (RingBufIndex(&g_ringBuffer, index+1) > 8.0f && RingBufIndex(&g_ringBuffer, index+2) < 11.0f) &&
                (RingBufIndex(&g_ringBuffer, index+1) > 8.0f && RingBufIndex(&g_ringBuffer, index+2) < 11.0f)) {
                flag = 4;
            }
        }
        // if (timeToClearCount >= 3) {
        //     count = 0;
        //     timeToClearCount = 0;
        // }
        // if (RingBufIndex(&g_ringBuffer, index) > 15) {
        //     count++;
        //     if (count >= 2) {
        //         countFlag = 1;
        //     }
        // }
        // timeToClearCount++;

        timeToClear++;
        index++;
    }
    printf("\nflag=%d\n", flag);
    g_flag = flag;
    if (flag == 4) {
        printf("tumble detected\n");
        return 1;
    }
    return 0;
}


void tumbleDetectTask()
{
    float pitch, roll, yaw;
    uint32_t start;
    short ax, ay, az;
    Accele_t acc = {0};
    uint8_t state;
    int16_t i = 1;
    float totalAccele = 0;
    static uint16_t timeToRetry = 0;
    static uint8_t isTumble = 0;

    RingBufInit(&g_ringBuffer, buffer, BUFFER_LENGTH);

    while(1){
        state = mpu_dmp_init();
        if(state == 0) break;
        printf("state:%d!\n", state);
        osDelay(10);
    }

    while (1) {
        start = osKernelGetTickCount();
        // mpu_dmp_get_data(&pitch, &roll, &yaw);
        // printf("pitch:%f,roll:%f,yaw:%f\n", pitch, roll, yaw);
        if (!MPU_Get_Accelerometer(&ax, &ay, &az)) {
            acc.ax = (float)(ax) / 4096 * GRAVITY;
            acc.ay = (float)(ay) / 4096 * GRAVITY;
            acc.az = (float)(az) / 4096 * GRAVITY;
            totalAccele = sqrt(pow(acc.ax, 2) + pow(acc.ay, 2));
            // SendJustFloat(totalAccele, g_flag);
            RingBufFullPush(&g_ringBuffer, totalAccele);
        }
        if (ringq_is_full((&g_ringBuffer)) && i++ >= 50) {
            if (!isTumble && g_tumbleFlag == 0) {
                if (detectTumble()) {
                    g_tumbleFlag = 1;
                    isTumble = 1;
                }
            } else {
                timeToRetry++;
                if (timeToRetry > 8) {
                    timeToRetry = 0;
                    isTumble = 0;
                }
            }
            i = 1;
        }
        // printf("pitch:%f,roll:%f,yaw:%f\n", mpuData.acc.ax, mpuData.acc.ay, mpuData.acc.az);
        osDelayUntil(start + 1);
    }
}


