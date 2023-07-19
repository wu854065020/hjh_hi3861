#ifndef MENU_H_
#define MENU_H_

#include <stdint.h>

typedef enum
{
    MAIN_KIND = 0,
    MENU_KIND,
    FUNC_KIND,
} PAGE_KIND;

typedef struct
{
    uint16_t current;
    uint16_t previous;
    uint16_t next;
    uint16_t enter;
    uint16_t back;
    PAGE_KIND kind;
    void (*function)(uint8_t key, uint8_t status);
} Page_t;

enum
{
    MAIN_PAGE = 0,  // 主界面
    LIGHT_MENU,     // 手电筒菜单
    LCD_LIGHT_MENU, // 屏幕菜单
    VOLUME_MENU,    // 音量菜单
    LIGHT_PAGE,     // 手电筒设置
    LCD_LIGHT_PAGE, // 屏幕亮度设置
    VOLUME_PAGE,    // 音量设置

};

enum
{
    QUIT = 0,
    ENTER,
    CURRENT,
};

// void menuInit();
void menuPeriod();
void mainMenuFunc(uint8_t key, uint8_t status);
void lightMenuFunc(uint8_t key, uint8_t status);
void lcdLightMenuFunc(uint8_t key, uint8_t status);
void volumeMenuFunc(uint8_t key, uint8_t status);
void lightPageFunc(uint8_t key, uint8_t status);
void lcdLightPageFunc(uint8_t key, uint8_t status);
void volumePageFunc(uint8_t key, uint8_t status);

#endif
