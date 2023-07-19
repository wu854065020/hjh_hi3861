#ifndef PACKGE_H
#define PACKGE_H
#include <stdint.h>

typedef struct
{
    float data[2];
    uint8_t tail[4];
}JustFloat;

// uint8_t GetCheck(uint8_t *src, uint16_t len);

void SendJustFloat(float input1, float input2);

#endif
