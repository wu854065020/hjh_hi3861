#ifndef MORE_H_
#define MORE_H_

#include <stdint.h>
#include "iot_gpio_ex.h"
#include "iot_gpio.h"

#define BAT_ADC_GPIO IOT_IO_NAME_GPIO_13
#define BAT_PWM_GPIO IOT_IO_NAME_GPIO_9
#define LIGHT_PWM_GPIO IOT_IO_NAME_GPIO_10
#define LIGHT_ADC_GPIO IOT_IO_NAME_GPIO_7

void batInit();

float batGet();

uint8_t batValueGet();

void lightInit();

int16_t lightSet(int8_t intensity);

float lightIntensityGet();

void lightAutoSet();

void lightAutoMode(uint8_t mode);

#endif