/*****************************************************************************
* | File      	: Space Invaders Demo  / SpaceInvaders.ino
* | Function    :	Demo for Portenta H7  with HAT expansion board , USB-C Video and BLE remote control interface
* | Info        : JV 2024
* | Github      : https://github.com/javos65/Portenta-H7-Space-Invaders
*----------------
* |	This version:   V1.0
* | Date        :   2024-02-16
* | Port        :   Arduino Portenta H7 with usb-c and BLE support
* | Info        :   BLE version for remote control via Smartphone App
*
EDID_MODE_640x480_60Hz - too small
EDID_MODE_720x480_60Hz - average
EDID_MODE_800x600_59Hz - ok
EDID_MODE_1024x768_60Hz - flickering
******************************************************************************/

#ifndef USBC_CANVAS
#define USBC_CANVAS

#include <stdint.h>
#include "lvgl_8.h"

#define GENERIC_RESX 800
#define GENERIC_RESY 600

extern uint32_t* DG_ScreenBuffer;
extern uint16_t* RGB565Canvas;

extern char TBUF[60];
#define Printf_Canvas(x,y,c,s,__info,...) sprintf(TBUF,__info,##__VA_ARGS__);Canvas_Text(TBUF, x, y,c,s);

void Canvas_Init();
void Canvas_DrawFrame();
void Canvas_DrawColors(uint16_t x, uint16_t y, uint16_t xw, uint16_t  yw, uint16_t *image);
void Canvas_Text(const char * txt, uint16_t x, uint16_t y,uint16_t color,uint8_t size);
void Canvas_FillRect(uint16_t x, uint16_t y,uint16_t xw, uint16_t yw,uint16_t color);
void Canvas_FillScreen(uint16_t rgb565color);
void Canvas_Drawtext( uint16_t *Dest, const char *text, font8bpp myfont, uint16_t x, int16_t y,uint16_t color,uint8_t size);

#endif //USBC_CANVAS
