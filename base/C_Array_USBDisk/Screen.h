/*****************************************************************************
* | File      	: Image data file
* | Function    :	Demo for Portenta H7 USB-drive conversion program
* | Info        : JV 2024
* | Github      : 
*----------------
* |	This version:   V2.0
* | Date        :   2024-03-16
* | https://github.com/javos65/H7-CArray-Converter-for-USBDisk
*
* Graphic screen example Data
*
******************************************************************************/

#ifndef SCREEN_H
#define SCREEN_H
#include <Arduino.h>
#include "bmppng.h" // for image definitions



// OUTPUT structure example, without the un-needed text names
typedef struct {
void *imagearray;  // array pointer
uint16_t width;    // width image
uint16_t height;   // heigt image
uint32_t colormode;// colormode DMA2D
uint8_t bpp;                  /// bits per pixel => /8 bytes per pixel
uint16_t xpos;     // x position
uint16_t ypos;     // y position
char name[24];     // name in text
} G_image;         // end structure

#define INVAD_Y 30
#define INVAD_X 36

extern const uint32_t Invaders1_1_array[INVAD_Y][INVAD_X]; 
// OUTPUT array structure example



// example array definitions
#define ENEMYSX  36
#define ENEMYSY   30

#define PLAYERSX    36
#define PLAYERSY    30

#define ICONHEIGHT  55
#define ICONWIDTH   55

#define MINIX   12
#define MINIY   10

#define BOMBY 12
#define BOMBX 1

#define RAYY 8
#define RAYX 1


// define invader images in structure for test of C-array conversion
extern h7image invader11;
extern h7image invader12;
//extern h7image invader21;
//extern h7image invader22;
//extern h7image invader31;
//extern h7image invader32;
//extern h7image invader41;
//extern h7image invader42;


extern const uint16_t shipcount[MINIY][MINIX]; // RGB565
extern const uint16_t Ray[RAYY][RAYX];         // RGB565
extern const uint16_t Bomb[BOMBY][BOMBX];      // RGB565

extern  const uint32_t laser[PLAYERSY][PLAYERSX] ;          // ARGB8888
extern  const uint32_t invader4_2[ENEMYSY][ENEMYSX];          // ARGB8888
extern  const uint32_t invader4_1[ENEMYSY][ENEMYSX];          // ARGB8888
extern  const uint32_t invader3_2[ENEMYSY][ENEMYSX];          // ARGB8888
extern  const uint32_t invader3_1[ENEMYSY][ENEMYSX];          // ARGB8888
extern  const uint32_t invader2_2[ENEMYSY][ENEMYSX];          // ARGB8888
extern  const uint32_t invader2_1[ENEMYSY][ENEMYSX];          // ARGB8888
extern  const uint32_t invader1_2[ENEMYSY][ENEMYSX];          // ARGB8888
extern  const uint32_t invader1_1[ENEMYSY][ENEMYSX];          // ARGB8888

extern const uint16_t BLE_on[ICONHEIGHT][ICONWIDTH];       // RGB565
extern const uint16_t BLE_off[ICONHEIGHT][ICONWIDTH];       // RGB565

#endif  SCREEN_H