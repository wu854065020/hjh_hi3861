#include "ui_task.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "menu/menu.h"


void uiTask()
{
    while (1) {
        menuPeriod();
        osDelay(1);
    }
}
