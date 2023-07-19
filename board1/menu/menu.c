/*
    自己设计的菜单部分，每个页面都有自己的菜单结构体实例，
    结构体中有父页面和子页面，并有功能函数指针，方便自定义每个页面的具体实现
*/

#include "menu.h"
#include "key/key.h"
#include <string.h>
#include "st7789/st7789.h"
#include "task/time_task.h"
#include "air780eg/air780eg.h"
#include "community/community.h"
#include "board1.h"
#include "task/community_task.h"
#include "task/air_task.h"

static int8_t lcdLight = 7;

uint8_t g_helpFlag = 0;

Page_t g_page[10] = 
{
    {MAIN_PAGE, 0, 0, LIGHT_MENU, 0, MAIN_KIND, mainMenuFunc},
    {LIGHT_MENU, VOLUME_MENU, LCD_LIGHT_MENU, LIGHT_PAGE, MAIN_PAGE, MENU_KIND, lightMenuFunc},
    {LCD_LIGHT_MENU, LIGHT_MENU, VOLUME_MENU, VOLUME_PAGE, MAIN_PAGE, MENU_KIND, lcdLightMenuFunc},
    {VOLUME_MENU, LCD_LIGHT_MENU, LIGHT_MENU, LCD_LIGHT_PAGE, MAIN_PAGE, MENU_KIND, volumeMenuFunc},
    {LIGHT_PAGE, 0, 0, 0, LIGHT_MENU, FUNC_KIND, lightPageFunc},
    {VOLUME_PAGE, 0, 0, 0, VOLUME_MENU, FUNC_KIND, volumePageFunc},
    {LCD_LIGHT_PAGE, 0, 0, 0, LCD_LIGHT_MENU, FUNC_KIND, lcdLightPageFunc},
};

// 1000 * 10 = 10s
#define TIME_TO_CLOSE 1000
void menuPeriod()
{
    uint8_t key;
    uint8_t isChange = 0;
    uint8_t status = CURRENT;
    static uint8_t isHelp = 0;
    static uint16_t timeToHelp = 0;
    static uint16_t timeToClose = 0;
    static uint8_t isClose = 0;
    static uint8_t lastIndex = 0;
    static uint8_t lastKind = MAIN_KIND;
    static uint8_t pageIndex = MAIN_PAGE;
    static uint8_t isInit = 0;
    if (isInit == 0) {
        // ST7789_Fill_Color(WHITE);
        g_page[pageIndex].function(ENTER_KEY, ENTER);
        isInit = 1;
    }
    if (isClose == 0) {
        if (timeToClose >= TIME_TO_CLOSE) {
            st7789LightSet(-1);
            isClose = 1;
            timeToClose = 0;
        } else {
            timeToClose++;
        }
    }
    key = keyRead();
    if (key > 0) {
        printf("key: %d", key);
        if (key == HELP_KEY && !g_helpFlag) {
            g_helpFlag = 1;
        }
        if (isClose == 1) {
            if (key == ENTER_KEY) {
                st7789LightSet(lcdLight);
                isClose = 0;
            }
        } else {
            timeToClose = 0;
            if (key == UP_KEY) {
                if (g_page[pageIndex].kind == MENU_KIND) {
                    lastIndex = pageIndex;
                    pageIndex = g_page[pageIndex].previous;
                    isChange = 1;
                    status = ENTER;
                    lastKind = g_page[lastIndex].kind;
                }
            } else if (key == DOWN_KEY) {
                if (g_page[pageIndex].kind == MENU_KIND) {
                    lastIndex = pageIndex;
                    pageIndex = g_page[pageIndex].next;
                    isChange = 1;
                    status = ENTER;
                    lastKind = g_page[lastIndex].kind;
                }
            } else if (key == ENTER_KEY) {
                if (g_page[pageIndex].kind == MAIN_KIND || g_page[pageIndex].kind == MENU_KIND) {
                    lastIndex = pageIndex;
                    pageIndex = g_page[pageIndex].enter;
                    isChange = 1;
                    status = ENTER;
                    lastKind = g_page[lastIndex].kind;
                }
            } else if (key == BACK_KEY) {
                if (g_page[pageIndex].kind == FUNC_KIND || g_page[pageIndex].kind == MENU_KIND) {
                    lastIndex = pageIndex;
                    pageIndex = g_page[pageIndex].back;
                    isChange = 1;
                    status = ENTER;
                    lastKind = g_page[lastIndex].kind;
                }
            }
            if (lastKind != MENU_KIND && g_page[pageIndex].kind == MENU_KIND) {
                ST7789_DrawImageU8(120, 200, 21, 23, leftArrow);
                ST7789_DrawImageU8(180, 200, 21, 23, rightArrow);
            } else if (lastKind == MENU_KIND && g_page[pageIndex].kind != MENU_KIND) {
                ST7789_Fill(120, 200, 120+21-1, 200+23-1, WHITE);
                ST7789_Fill(180, 200, 180+21-1, 200+23-1, WHITE);
            }
            if (isChange) {
                g_page[lastIndex].function(key, QUIT);
            }
        }
    }
    g_page[pageIndex].function(key, status);
}

void mainMenuFunc(uint8_t key, uint8_t status)
{
    TimeStruct tTime;
    static TimeStruct time;
    static char str[20] = {0};
    uint8_t t_batV = 0;
    static uint8_t batV = 0;
    uint8_t batLine = 0;
    uint16_t color;
    if (status == ENTER) {
        if (key > 0) {
            time = *getTime();
            snprintf_s(str, 20, 20, "%02d:%02d", time.hour, time.minute);
            // printf();
            ST7789_WriteString(120, 100, str, Font_16x26, BLACK, WHITE);
            ST7789_DrawRectangle(260, 20-1, 300, 40+1, BLACK);
            batV = getBatV();
            if (batV > 50) {
                color = GREEN;
            } else if (batV > 25) {
                color = YELLOW;
            } else {
                color = RED;
            }
            batLine = batV / 100.0f * (300 - 1 - 260 - 1);
            ST7789_Fill(260+1, 20, 260+1+batLine, 40, color);
            snprintf_s(str, 20, 20, "%3d", batV);
            ST7789_WriteString(220, 22, str, Font_11x18, BLACK, WHITE);
        }
    } else if (status == QUIT) {
        ST7789_Fill(120, 100, 120+Font_16x26.width*5-1, 120+Font_16x26.height-1, WHITE);
        ST7789_Fill(260-1, 20-1, 300, 40+1, WHITE);
        ST7789_Fill(220, 22, 220+Font_11x18.width*3-1, 22+Font_11x18.height-1, WHITE);
    } else if (status == CURRENT) {
        tTime = *getTime();
        if (time.minute != tTime.minute) {
            time = tTime;
            snprintf_s(str, 20, 20, "%02d:%02d", time.hour, time.minute);
            // printf(str);
            ST7789_WriteString(120, 100, str, Font_16x26, BLACK, WHITE);
        }
        t_batV = getBatV();
        if (t_batV != batV) {
            batV = t_batV;
            if (batV > 50) {
                color = GREEN;
            } else if (batV > 25) {
                color = YELLOW;
            } else {
                color = RED;
            }
            batLine = batV / 100.0f * (300 - 1 - 260 - 1);
            ST7789_Fill(260-1, 20-1, 300, 40+1, WHITE);
            ST7789_Fill(220, 22, 220+Font_11x18.width*3-1, 22+Font_11x18.height-1, WHITE);
            ST7789_Fill(260+1, 20, 260+1+batLine, 40, color);
            snprintf_s(str, 20, 20, "%3d", batV);
            ST7789_WriteString(220, 22, str, Font_11x18, BLACK, WHITE);
        }
    }
}

void lightMenuFunc(uint8_t key, uint8_t status)
{
    if (status == ENTER) {
        if (key > 0) {
            ST7789_DrawImageU8(120, 60, 80, 80, diantong);
            ST7789_WriteHanziStr(124, 160, 3, 3, Hanzi_24x24, BLACK, WHITE);
        }
    } else if (status == QUIT) {
        ST7789_Fill(120, 60, 120+80-1, 60+80-1, WHITE);
        ST7789_Fill(124, 160, 124+Hanzi_24x24.width*3-1, 160+Hanzi_24x24.height-1, WHITE);
    }
}

void lcdLightMenuFunc(uint8_t key, uint8_t status)
{
    if (status == ENTER) {
        if (key > 0) {
            ST7789_DrawImageU8(120, 60, 80, 80, liangdu);
            ST7789_WriteHanziStr(136, 160, 6, 2, Hanzi_24x24, BLACK, WHITE);
        }
    } else if (status == QUIT) {
        ST7789_Fill(120, 60, 120+80-1, 60+80-1, WHITE);
        ST7789_Fill(136, 160, 136+Hanzi_24x24.width*2-1, 160+Hanzi_24x24.height-1, WHITE);
    }
}

void volumeMenuFunc(uint8_t key, uint8_t status)
{
    if (status == ENTER) {
        if (key > 0) {
            ST7789_DrawImageU8(120, 60, 80, 80, yinliang);
            ST7789_WriteHanziStr(136, 160, 1, 2, Hanzi_24x24, BLACK, WHITE);
        }
    } else if (status == QUIT) {
        ST7789_Fill(120, 60, 120+80-1, 60+80-1, WHITE);
        ST7789_Fill(136, 160, 136+Hanzi_24x24.width*3-1, 160+Hanzi_24x24.height-1, WHITE);
    }
}

void lightPageFunc(uint8_t key, uint8_t status)
{
    static uint8_t choice = 2;
    static uint8_t arrow = 0;
    static int8_t led = 3;
    static uint8_t setLight = 0;
    char str[20] = {0};
    if (status == ENTER) {
        if (key > 0) {
            ST7789_DrawImageU8(30,32, 21, 23, rightArrow);
            ST7789_WriteHanziStr(60, 30, 10, 4, Hanzi_24x24, BLACK, WHITE);
            ST7789_WriteHanziStr(60, 60, 14, 1, Hanzi_24x24, BLACK, WHITE);
            ST7789_WriteHanziStr(60, 90, 15, 1, Hanzi_24x24, BLACK, WHITE);
            ST7789_WriteHanziStr(60, 120, 6, 2, Hanzi_24x24, BLACK, WHITE);
            snprintf_s(str, 20 + 1, 20, "%2d", led);
            ST7789_WriteString(120, 123, str, Font_11x18, BLACK, WHITE);
            ST7789_DrawCircle(180, 42+30*choice, 8, RED);
        }
    } else if (status == QUIT) {
        arrow = 0;
        setLight = 0;
        ST7789_Fill(30, 32, 30+21-1, 32+120-1, WHITE);
        ST7789_Fill(60, 30, 60+24*4-1, 30+24-1, WHITE);
        ST7789_Fill(60, 60, 60+24-1, 60+24-1, WHITE);
        ST7789_Fill(60, 90, 60+24-1, 90+24-1, WHITE);
        ST7789_Fill(60, 120, 120+11*3-1, 120+24-1, WHITE);
        ST7789_Fill(180-8, 42-8, 180-8+30-1, 42-8+30*3-1, WHITE);
    } else if (status == CURRENT) {
        if (key == UP_KEY || key == DOWN_KEY) {
            if (!setLight) {
                if (key == UP_KEY) {
                    arrow = (arrow==0)?3:(arrow-1);
                } else if (key == DOWN_KEY) {
                    arrow = (arrow+1)%4;
                }
                ST7789_Fill(30, 32, 30+21-1, 32+120-1, WHITE);
                ST7789_DrawImageU8(30,32+arrow*30, 21, 23, rightArrow);
            } else {
                if (key == UP_KEY) {
                    led = led % 5 + 1;
                } else if (key == DOWN_KEY) {
                    led = led<=1?5:led-1;
                }
                ST7789_Fill(120, 123, 120+11*3-1, 123+18-1, WHITE);
                snprintf_s(str, 20 + 1, 20, "%2d", led);
                ST7789_WriteString(120, 123, str, Font_11x18, BLUE, WHITE);
                ledPower(led);
            }
        } else if (key == ENTER_KEY) {
            if (arrow < 3) {
                choice = arrow;
                ST7789_Fill(180-15, 42-15, 180-8+30-1, 42-8+30*3-1, WHITE);
                ST7789_DrawCircle(180, 42+30*choice, 8, RED);
                switch (choice) {
                    case 0: ledPower(-2);break;         // 自动调节
                    case 1: ledPower(-3);break;        // 开
                    case 2: ledPower(-1);break;         // 关灯
                }
            } else {
                setLight = !setLight;
                if (setLight) {
                    ST7789_Fill(60, 120, 120+11*3-1, 120+24-1, WHITE);
                    ST7789_WriteHanziStr(60, 120, 6, 2, Hanzi_24x24, BLUE, WHITE);
                    snprintf_s(str, 20 + 1, 20, "%2d", led);
                    ST7789_WriteString(120, 123, str, Font_11x18, BLUE, WHITE);
                } else {
                    ST7789_Fill(60, 120, 120+11*3-1, 120+24-1, WHITE);
                    ST7789_WriteHanziStr(60, 120, 6, 2, Hanzi_24x24, BLACK, WHITE);
                    snprintf_s(str, 20 + 1, 20, "%2d", led);
                    ST7789_WriteString(120, 123, str, Font_11x18, BLACK, WHITE);
                }
            }
        }
    }
}

void lcdLightPageFunc(uint8_t key, uint8_t status)
{
    char str[20] = {0};
    static uint8_t set = 0;
    if (status == ENTER) {
        if (key > 0) {
            ST7789_WriteHanziStr(60, 30, 6, 2, Hanzi_24x24, BLACK, WHITE);
            snprintf_s(str, 20 + 1, 20, "%2d", lcdLight);
            ST7789_WriteString(120, 33, str, Font_11x18, BLACK, WHITE);
        }
    } else if (status == QUIT) {
        set = 0;
        ST7789_Fill(60, 30, 120+24*2-1, 120+24-1, WHITE);
        ST7789_Fill(120, 33, 120+11*3-1, 33+18-1, WHITE);
    } else if (status == CURRENT) {
        if (key == UP_KEY || key == DOWN_KEY) {
            if (set) {
                if (key == UP_KEY) {
                    lcdLight = lcdLight % 10 + 1;
                } else if (key == DOWN_KEY) {
                    lcdLight = lcdLight<=1?10:lcdLight-1;
                }
                snprintf_s(str, 20 + 1, 20, "%2d", lcdLight);
                ST7789_WriteString(120, 33, str, Font_11x18, BLUE, WHITE);
                st7789LightSet(lcdLight);
            }
        } else if (key == ENTER_KEY) {
            set = !set;
            if (set) {
                ST7789_Fill(60, 30, 120+24*2-1, 120+24-1, WHITE);
                ST7789_Fill(120, 33, 120+11*3-1, 33+18-1, WHITE);
                ST7789_WriteHanziStr(60, 30, 6, 2, Hanzi_24x24, BLUE, WHITE);
                snprintf_s(str, 20 + 1, 20, "%2d", lcdLight);
                ST7789_WriteString(120, 33, str, Font_11x18, BLUE, WHITE);
            } else {
                ST7789_Fill(60, 30, 120+24*2-1, 120+24-1, WHITE);
                ST7789_Fill(120, 33, 120+11*3-1, 33+18-1, WHITE);
                ST7789_WriteHanziStr(60, 30, 6, 2, Hanzi_24x24, BLACK, WHITE);
                snprintf_s(str, 20 + 1, 20, "%2d", lcdLight);
                ST7789_WriteString(120, 33, str, Font_11x18, BLACK, WHITE);
            }
        }
    }
}

void volumePageFunc(uint8_t key, uint8_t status)
{
    char str[20] = {0};
    static int8_t volume = 2;
    static uint8_t set = 0;
    if (status == ENTER) {
        if (key > 0) {
            ST7789_WriteHanziStr(60, 30, 1, 2, Hanzi_24x24, BLACK, WHITE);
            snprintf_s(str, 20 + 1, 20, "%3d", volume);
            ST7789_WriteString(120, 33, str, Font_11x18, BLACK, WHITE);
        }
    } else if (status == QUIT) {
        set = 0;
        ST7789_Fill(60, 30, 120+24*2-1, 120+24-1, WHITE);
        ST7789_Fill(120, 33, 120+11*3-1, 33+18-1, WHITE);
    } else if (status == CURRENT) {
        if (key == UP_KEY || key == DOWN_KEY) {
            if (set) {
                if (key == UP_KEY) {
                    volume = volume % 4 + 1;
                } else if (key == DOWN_KEY) {
                    volume = volume<=1?4:volume-1;
                }
                snprintf_s(str, 20 + 1, 20, "%3d", volume);
                ST7789_WriteString(120, 33, str, Font_11x18, BLUE, WHITE);
                volumeSet(volume);
            }
        } else if (key == ENTER_KEY) {
            set = !set;
            if (set) {
                ST7789_Fill(60, 30, 120+24*2-1, 120+24-1, WHITE);
                ST7789_Fill(120, 33, 120+11*3-1, 33+18-1, WHITE);
                ST7789_WriteHanziStr(60, 30, 1, 2, Hanzi_24x24, BLUE, WHITE);
                snprintf_s(str, 20 + 1, 20, "%3d", volume);
                ST7789_WriteString(120, 33, str, Font_11x18, BLUE, WHITE);
            } else {
                ST7789_Fill(60, 30, 120+24*2-1, 120+24-1, WHITE);
                ST7789_Fill(120, 33, 120+11*3-1, 33+18-1, WHITE);
                ST7789_WriteHanziStr(60, 30, 1, 2, Hanzi_24x24, BLACK, WHITE);
                snprintf_s(str, 20 + 1, 20, "%3d", volume);
                ST7789_WriteString(120, 33, str, Font_11x18, BLACK, WHITE);
            }
        }
    }
}
