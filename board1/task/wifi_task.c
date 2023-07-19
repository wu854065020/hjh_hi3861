#include <stdio.h>
#include "wifi_task.h"
#include "wifi_connecter.h"

extern int g_connected;

void wifiTask()
{
    while (1) {
        if (!g_connected) {
            ConnectToHotspot();
        }
        printf("g_connected:%d\n", g_connected);
        osDelay(50);
    }
}
