#include <string.h>

#include "asrpro.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"
#include "iot_uart.h"
#include "hi_uart.h"
#include "iot_watchdog.h"
#include "iot_errno.h"

#define ASR_UART HI_UART_IDX_2

void asrproUartWrite(const unsigned char *data, unsigned int datalen)
{
    IoTUartWrite(ASR_UART, data, datalen);
}

int asrproUartRead(unsigned char *data, unsigned int dataLen)
{
    return IoTUartRead(ASR_UART, data, dataLen);
}

void asrproUartInit()
{
    IoTGpioInit(IOT_IO_NAME_GPIO_11);
    // 设置GPIO0的管脚复用关系为UART1_TX Set the pin reuse relationship of GPIO0 to UART1_ TX
    IoSetFunc(IOT_IO_NAME_GPIO_11, IOT_IO_FUNC_GPIO_11_UART2_TXD);
    IoTGpioInit(IOT_IO_NAME_GPIO_12);
    // 设置GPIO1的管脚复用关系为UART1_RX Set the pin reuse relationship of GPIO1 to UART1_ RX
    IoSetFunc(IOT_IO_NAME_GPIO_12, IOT_IO_FUNC_GPIO_12_UART2_RXD);
    uint32_t ret;
    /* 初始化UART配置，波特率 9600，数据bit为8,停止位1，奇偶校验为NONE */
    /* Initialize UART configuration, baud rate is 9600, data bit is 8, stop bit is 1, parity is NONE */
    IotUartAttribute uart_attr = {
        .baudRate = 115200,
        .dataBits = 8,
        .stopBits = 1,
        .parity = 0,
    };
    ret = IoTUartInit(ASR_UART, &uart_attr);
    if (ret != IOT_SUCCESS) {
        printf("Init Uart2 Falied Error No : %d\n", ret);
        return;
    }
}

int asrSpeakCmd(uint8_t cmd)
{
    uint8_t tmp = cmd + '0';
    asrproUartWrite(&tmp, 1);
    return 0;
}

int asrVolumeCmd(uint8_t volume)
{
    uint8_t tmp[2] = {0xAA};
    tmp[1] = volume - 10;
    asrproUartWrite(tmp, 2);
}