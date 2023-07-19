#ifndef COMMUNITY_TASK_H_
#define COMMUNITY_TASK_H_

#include <stdint.h>

#define PACKAGE_HEADER 0xA5

typedef __attribute__((packed)) struct
{
    uint8_t header;
    int8_t led;
    uint8_t speakCmd;
    uint16_t crc;
} Board1SendPackage;

typedef __attribute__((packed)) struct
{
    uint8_t header;
    uint8_t lcdLight;
    uint8_t speakCmd;
    uint8_t batV;
    uint16_t crc;
} Board2SendPackage;

void communityUartInit();

void communityUartWrite(uint8_t *buf, int len);

int communityUartRead(unsigned char *data, unsigned int dataLen);

void batReport(uint8_t);

void sendPackage();

int recevicePackage(Board1SendPackage *pack, uint8_t *buf, uint8_t buflen);

void community_task();


#endif
