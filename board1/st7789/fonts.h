#ifndef __FONT_H
#define __FONT_H

#include "stdint.h"

//汉字索引

typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint8_t *data;
} HanziDef;

extern HanziDef Hanzi_24x24;

// Font define
typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint16_t *data;
} FontDef;

//Font lib.
extern FontDef Font_7x10;
extern FontDef Font_11x18;
extern FontDef Font_16x26;

//16-bit(RGB565) Image lib.
/*******************************************
 *             CAUTION:
 *   If the MCU onchip flash cannot
 *  store such huge image data,please
 *           do not use it.
 * These pics are for test purpose only.
 *******************************************/

/* 128x128 pixel RGB565 image */
// extern const uint16_t saber[][128];
extern const uint8_t leftArrow[];
extern const uint8_t rightArrow[];
extern const uint8_t diantong[];
extern const uint8_t yinliang[];
extern const uint8_t liangdu[];

/* 240x240 pixel RGB565 image 
extern const uint16_t knky[][240];
extern const uint16_t tek[][240];
extern const uint16_t adi1[][240];
*/
#endif
