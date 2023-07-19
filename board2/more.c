#include <stdio.h>
#include "more.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_pwm.h"
#include "adc/iot_adc.h"
#include "iot_errno.h"
#include "hi_pwm.h"
#include "hi_time.h"

#define BAT_PWM_FREQ 2500
#define LIGHT_PWM_FREQ 2500

#define BAT_PWM_PORT HI_PWM_PORT_PWM0
#define LIGHT_PWM_PORT HI_PWM_PORT_PWM1

uint8_t g_lightAuto = 0;
uint8_t g_light = 3;
uint8_t g_isLedOpen = 0;

void batInit()
{
    // ADC Init
    IoTGpioInit(BAT_ADC_GPIO);
    IoSetFunc(BAT_ADC_GPIO, IOT_IO_FUNC_GPIO_13_GPIO);
    IoTGpioSetDir(BAT_ADC_GPIO, IOT_GPIO_DIR_IN);
    // PWM Init
    IoTGpioInit(BAT_PWM_GPIO);
    IoSetFunc(BAT_PWM_GPIO, IOT_IO_FUNC_GPIO_9_PWM0_OUT);
    IoTGpioSetDir(BAT_PWM_GPIO, IOT_GPIO_DIR_OUT);
    IoTPwmInit(BAT_PWM_PORT);
    IoTPwmStart(BAT_PWM_PORT, 50, BAT_PWM_FREQ);
}

float batGet()
{
    unsigned int data = 0;
    int ret = 0;
    float total = 0;
    uint8_t i = 4;
    while (i--) {
        ret = AdcRead(IOT_ADC_CHANNEL_6, &data, IOT_ADC_EQU_MODEL_4, IOT_ADC_CUR_BAIS_DEFAULT, 0xff);
        total += data;
    }
    // printf("bat adc :%d\n", data);
    total /= 4.0f;
    total = total * 3.28f / 1864.0f;
    return total;   
}

#define BAT_MAX_V 4.1f / 2
#define BAT_MIN_V 3.5f / 2
#define CLAMP(x,min,max) x>max?max:x<min?min:x
uint8_t batValueGet()
{
    float volt = 0;
    uint8_t batV = 0;
    float temp;
    volt = batGet();
    temp = (volt - BAT_MIN_V) / (BAT_MAX_V - BAT_MIN_V) * 100;
    batV = (uint8_t)CLAMP(temp, 1.0f, 100.0f);
    return batV;
}

void lightInit()
{
    // ADC Init
    IoTGpioInit(LIGHT_ADC_GPIO);
    IoSetFunc(LIGHT_ADC_GPIO, IOT_IO_FUNC_GPIO_7_GPIO);
    IoTGpioSetDir(LIGHT_ADC_GPIO, IOT_GPIO_DIR_IN);
    // PWM Init
    IoTGpioInit(LIGHT_PWM_GPIO);
    IoSetFunc(LIGHT_PWM_GPIO, IOT_IO_FUNC_GPIO_10_PWM1_OUT);
    IoTGpioSetDir(LIGHT_PWM_GPIO, IOT_GPIO_DIR_OUT);
    IoTPwmInit(LIGHT_PWM_PORT);
    IoTPwmStop(LIGHT_PWM_PORT);
}

int16_t lightOpen(uint8_t open)
{
    uint16_t tmp = 0;
    if (open) {
        g_isLedOpen = 1;
        tmp = (uint16_t)((float)(g_light - 1) / 4.0f * (49 - 9) + 9);
        IoTPwmStart(LIGHT_PWM_PORT, tmp, LIGHT_PWM_FREQ);
    } else {
        IoTPwmStop(LIGHT_PWM_PORT);
        g_isLedOpen = 0;
    }
}

// intensity 1-5, -1 close, -2 auto
int16_t lightSet(int8_t intensity)
{
    if (intensity == -1) {
        lightAutoMode(0);
        lightOpen(0);
        return 0;
    } else if (intensity == -2) {
        lightAutoMode(1);
    } else if (intensity == -3) {
        lightAutoMode(0);
        lightOpen(1);
    } else if (intensity >= 1 && intensity <= 5) {
        g_light = intensity;
        if (g_isLedOpen) {
            lightOpen(1);
        }
        return 0;
    }
    return -1;
}

void lightAutoMode(uint8_t mode)
{
    if (mode) {
        g_lightAuto = 1;
    } else {
        g_lightAuto = 0;
    }
}

float lightIntensityGet()
{
    uint16_t data = 0;
    float total = 0;
    int ret = 0;
    uint8_t i = 5;
    while (i--) {
        ret = AdcRead(IOT_ADC_CHANNEL_3, &data, IOT_ADC_EQU_MODEL_4, IOT_ADC_CUR_BAIS_3P3V, 0xff);
        total += data;
        hi_udelay(2000);
    }
    total /= 5.0f;
    return data;
}

#define LIGHT_ON_THRE 150
#define LIGHT_OFF_THRE 160


void lightAutoSet()
{
    float data;
    if (g_lightAuto) {
        data = lightIntensityGet();
        printf("light adc:%f\n", data);
        if (data < LIGHT_ON_THRE && !g_isLedOpen) {
            lightOpen(1);
        } else if (data > LIGHT_OFF_THRE && g_isLedOpen) {
            lightOpen(0);
        }
    }
}
