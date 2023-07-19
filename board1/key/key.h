#ifndef KEY_H
#define KEY_H

#include <stdint.h>

#define KEY1_BIT 0x01 << 0
#define KEY2_BIT 0x01 << 1
#define KEY3_BIT 0x01 << 2
#define KEY4_BIT 0x01 << 3

#define UP_KEY KEY4_BIT
#define DOWN_KEY KEY3_BIT
#define ENTER_KEY KEY2_BIT
#define BACK_KEY KEY1_BIT
#define HELP_KEY 0x01 << 4

void keyInit();
uint8_t keyRead();

#endif
