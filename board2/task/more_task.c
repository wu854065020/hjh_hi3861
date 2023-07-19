#include "more_task.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "more.h"
#include "community_task.h"

void moreTask()
{
    uint8_t batV = 0;
    uint16_t i = 3000;
    uint32_t start = 0;
    while (1) {
        start = osKernelGetTickCount();
        lightAutoSet();
        batV = batValueGet();
        // printf("batV:%d\n", batV);
        if (i >= 3000) {
            batReport(batV);
            i = 0;
        }
        i++;
        osDelayUntil(start + 10);
    }
}




