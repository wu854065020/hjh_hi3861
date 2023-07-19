#ifndef AIR_TASK_H_
#define AIR_TASK_H_

#include <stdint.h>

enum
{
    OUT_POSITION = 0,
    TUMBLE,
    IN_POSITION,
    OLD_MAN_IN_HOME,
    OLD_MAN_HELP,
};

void airTask();
void airTaskInit();
void pubAlarm(uint8_t alarm);

#endif
