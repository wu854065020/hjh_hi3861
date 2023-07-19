#include "time_task.h"
#include "ohos_init.h"
#include "cmsis_os2.h"

static TimeStruct g_time = {2023, 7, 13, 12, 0, 0};
const uint8_t g_mouthDay[] = {
31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

void timeUpdate(TimeStruct *time)
{
    g_time = *time;
    // g_time.hour += 8;
    // if (g_time.hour >= 24) {
    //     g_time.hour = 0;
    //     g_time.day++;
    // }
    // if (g_time.day > g_mouthDay[g_time.mouth - 1]) {
    //     g_time.day = 1;
    //     g_time.mouth++;
    // }
    // if (g_time.mouth > 12) {
    //     g_time.mouth = 1;
    //     g_time.year++;
    // }
}

void timeInc()
{
    g_time.second++;
    if (g_time.second >= 60) {
        g_time.second = 0;
        g_time.minute++;
    }
    if (g_time.minute >= 60) {
        g_time.minute = 0;
        g_time.hour++;
    }
    if (g_time.hour >= 24) {
        g_time.hour = 0;
        g_time.day++;
    }
    if (g_time.day > g_mouthDay[g_time.mouth - 1]) {
        g_time.day = 1;
        g_time.mouth++;
    }
    if (g_time.mouth > 12) {
        g_time.mouth = 1;
        g_time.year++;
    }
}

TimeStruct *getTime()
{
    return &g_time;
}

void timeTask(void)
{
    uint32_t start = 0;
    while (1) {
        start = osKernelGetTickCount();         //获取计数值
        timeInc();
        // printf("%d %d %d %d %d %d", g_time.year, g_time.mouth, g_time.day, g_time.hour, g_time.minute, g_time.second);
        osDelayUntil(start + 100);      //保证每次周期任务延时1s
    }
}

