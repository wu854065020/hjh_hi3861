#include "packge.h"
#include "iot_uart.h"

// uint8_t GetCheck(uint8_t *src, uint16_t len)
// {
//     uint8_t check = 0;
//     while(len--)
//     {
//         check += *src;
//         src++;
//     }
//     return check;
// }

void SendJustFloat(float input1, float input2)
{
    JustFloat justpack;
    justpack.data[0] = input1;
    justpack.data[1] = input2;
    justpack.tail[0] = 0x00;
    justpack.tail[1] = 0x00;
    justpack.tail[2] = 0x80;
    justpack.tail[3] = 0x7F;
    IoTUartWrite(0, (uint8_t*)&justpack, sizeof(justpack));
}


