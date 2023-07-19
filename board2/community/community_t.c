#include "community.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"
#include "iot_uart.h"
#include "hi_uart.h"
#include "iot_errno.h"

#define COMMUNITY_UART HI_UART_IDX_1

Board2SendPackage g_sendPackage = {0};

void communityUartWrite(uint8_t *buf, int len)
{
    IoTUartWrite(COMMUNITY_UART, buf, len);
}

int communityUartRead(unsigned char *data, unsigned int dataLen)
{
    return IoTUartRead(COMMUNITY_UART, data, dataLen);
}

void communityUartInit()
{
    IoTGpioInit(IOT_IO_NAME_GPIO_6);
    // 设置GPIO0的管脚复用关系为UART1_TX Set the pin reuse relationship of GPIO0 to UART1_ TX
    IoSetFunc(IOT_IO_NAME_GPIO_6, IOT_IO_FUNC_GPIO_6_UART1_TXD);
    IoTGpioInit(IOT_IO_NAME_GPIO_5);
    // 设置GPIO1的管脚复用关系为UART1_RX Set the pin reuse relationship of GPIO1 to UART1_ RX
    IoSetFunc(IOT_IO_NAME_GPIO_5, IOT_IO_FUNC_GPIO_5_UART1_RXD);
    uint32_t ret;
    /* 初始化UART配置，波特率 9600，数据bit为8,停止位1，奇偶校验为NONE */
    /* Initialize UART configuration, baud rate is 9600, data bit is 8, stop bit is 1, parity is NONE */
    IotUartAttribute uart_attr = {
        .baudRate = 115200,
        .dataBits = 8,
        .stopBits = 1,
        .parity = 0,
    };
    ret = IoTUartInit(COMMUNITY_UART, &uart_attr);
    if (ret != IOT_SUCCESS) {
        printf("Init Uart1 Falied Error No : %d\n", ret);
        return;
    }
}

void sendPackage()
{
    Append_CRC8_Check_Sum((uint8_t *)&g_sendPackage, sizeof(Board2SendPackage));
    communityUartWrite((uint8_t *)&g_sendPackage, sizeof(Board2SendPackage));
    memset_s(&g_sendPackage, sizeof(Board2SendPackage)+1, 0, sizeof(Board2SendPackage));
}

void recevicePackage(uint8_t *buf, uint8_t buflen)
{
    Board1SendPackage package;
    if (buflen == sizeof(Board1SendPackage)) {
        if (buf[0] == PACKAGE_HEADER) {
            if (Verify_CRC8_Check_Sum(buf, sizeof(Board1SendPackage))) {
                memcpy_s(&package, sizeof(Board1SendPackage), buf, sizeof(Board1SendPackage));
            }
        }
    }
}
