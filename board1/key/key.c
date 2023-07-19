/*
    ADC按键，由于按下按键后，电压存在变化过程，故需要防抖措施，
    延时10ms后再次读取，相同了才算按下。
*/

#include "key.h"
#include "cmsis_os2.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"
#include "adc/iot_adc.h"
#include "iot_errno.h"

#define KEY_DELAY 1

void keyInit()
{
    IoTGpioInit(IOT_IO_NAME_GPIO_7);
    IoSetFunc(IOT_IO_NAME_GPIO_7, IOT_IO_FUNC_GPIO_7_GPIO);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_7, IOT_GPIO_DIR_IN);
}

uint8_t keyRead()
{
    static uint8_t lastKey = 0;
    static uint8_t isHelp = 0;
    static uint16_t timeToHelp = 0;
    unsigned short data = 0;
    float keyV;
    uint8_t key = 0;
    int ret = 0;
    /* ADC_Channal_6  自动识别模式  CNcomment:4次平均算法模式 CNend 0xff */
    /* ADC_ Channal_ 6 Automatic recognition mode CNcomment: 4 times average algorithm mode CNend 0xff */
    ret = AdcRead(IOT_ADC_CHANNEL_3, &data, IOT_ADC_EQU_MODEL_4, IOT_ADC_CUR_BAIS_DEFAULT, 0xff);
    if (ret != IOT_SUCCESS) {
        printf("hi_adc_read failed\n");
        return 0;
    }
    // printf("ADC value is %d \r\n", data);
    // keyV = (float)(data - 128)*3.28f/(1864.0f - 128.0f);
    keyV = (float)(data)*3.28f/(1864.0f);
    if (keyV <= 3.3f*0.08) {
        key = KEY1_BIT;
    } else if (keyV <= 3.3f/2+0.02*3.3f && keyV >= 3.3f/2-0.02*3.3f) {
        key = KEY2_BIT;
    } else if (keyV <= 3.3f*2/3+0.02*3.3f && keyV >= 3.3f*2/3-0.02*3.3f) {
        key = KEY3_BIT;
    } else if (keyV <= 3.3f*3/4+0.02*3.3f && keyV >= 3.3f*3/4-0.02*3.3f) {
        key = KEY4_BIT;
    }
    if (key & lastKey == BACK_KEY) {
        if (!isHelp) {
            timeToHelp++;
            printf("help num:%d\n", timeToHelp);
            if (timeToHelp >= 200) {
                printf("old man help!\n");
                isHelp = 1;
                return HELP_KEY;
            }
        }
        return 0;
    } else if (key & lastKey) {
        timeToHelp = 0;
        isHelp = 0;
        return 0;
    } else {
        timeToHelp = 0;
        isHelp = 0;
        lastKey = 0;
    }
    if (key) {
        osDelay(1);
        ret = AdcRead(IOT_ADC_CHANNEL_3, &data, IOT_ADC_EQU_MODEL_4, IOT_ADC_CUR_BAIS_DEFAULT, 0xff);
        if (ret != IOT_SUCCESS) {
            printf("hi_adc_read failed\n");
            return 0;
        }
        keyV = (float)(data)*3.28f/(1864.0f);
        if (keyV <= 3.3f*0.08) {
            key &= KEY1_BIT;
        } else if (keyV <= 3.3f/2+0.02*3.3f && keyV >= 3.3f/2-0.02*3.3f) {
            key &= KEY2_BIT;
        } else if (keyV <= 3.3f*2/3+0.02*3.3f && keyV >= 3.3f*2/3-0.02*3.3f) {
            key &= KEY3_BIT;
        } else if (keyV <= 3.3f*3/4+0.02*3.3f && keyV >= 3.3f*3/4-0.02*3.3f) {
            key &= KEY4_BIT;
        }
        lastKey = key;
        // printf("key:%d\n", key);
        return key;
    }
    return 0;
}