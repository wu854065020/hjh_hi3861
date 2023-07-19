#ifndef TIME_TASK_H_
#define TIME_TASK_H_

#include <stdint.h>
#include "air780eg/air780eg.h"

void timeTask();
void timeUpdate(TimeStruct *time);
void timeInc();
TimeStruct *getTime();

#endif
