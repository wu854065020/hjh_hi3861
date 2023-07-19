#ifndef ASRPRO_H
#define ASRPRO_H

#include <stdint.h>

void asrproUartInit();

void asrproUartWrite(const unsigned char *data, unsigned int datalen);

int asrproUartRead(unsigned char *data, unsigned int dataLen);

int asrSpeakCmd(uint8_t cmd);

int asrVolumeCmd(uint8_t volume);


#endif
