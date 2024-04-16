// C-array PNG Converter by JV
// Required for STM32 ChromArt DMA2D support 
// Red-Blue swop used : 1  

#include <stdint.h>


typedef struct {
void *imagearray;  // array pointer
uint16_t width;    // width image
uint16_t height;   // heigt image
uint32_t colormode;// colormode DMA2D
uint8_t bpp;       // bits per pixel
uint16_t xpos;     // x position
uint16_t ypos;     // y position
char name[24];     // name in text
} G_image;         // end structure

#define BUNK1_Y 12
#define BUNK1_X 18

G_image Bunk1 ={
(void*) Bunk1_array,BUNK1_X,BUNK1_Y,
0x00000004,16,0,0, // Alpha mode COLORMODE_ARGB4444 
"Bunk1\0",
};

const uint16_t Bunk1_array[BUNK1_Y][BUNK1_X] = {
0xf861,0xf861,0xf861,0xf861,0xf962,0xf973,0xf984,0xf973,0xf752,0xf972,0xf973,0xf984,0xf974,0xf973,0xf962,0xf862,0xf862,0xf861,
0xf861,0xf861,0xf861,0xf861,0xf862,0xf973,0xf973,0xf972,0xf751,0xf862,0xf972,0xf973,0xf973,0xf973,0xf973,0xf972,0xf862,0xf861,
0xf861,0xf861,0xf861,0xf861,0xf861,0xf973,0xf973,0xf962,0xf751,0xf861,0xf861,0xf861,0xf862,0xf972,0xf973,0xf973,0xf973,0xf862,
0xf862,0xf861,0xf861,0xf861,0xf861,0xf972,0xf973,0xf972,0xf751,0xf861,0xf861,0xf861,0xf861,0xf861,0xf972,0xf973,0xf973,0xf972,
0xf972,0xf973,0xf862,0xf861,0xf861,0xf962,0xf973,0xf972,0xf751,0xf862,0xf862,0xf862,0xf861,0xf861,0xf862,0xf973,0xf984,0xf984,
0xf973,0xf973,0xf973,0xf861,0xf861,0xf862,0xf972,0xf962,0xf751,0xf862,0xf972,0xf973,0xf972,0xf862,0xf862,0xf973,0xfa84,0xfa84,
0xf752,0xf762,0xf762,0xf751,0xf651,0xf651,0xf751,0xf651,0xf651,0xf651,0xf752,0xf762,0xf762,0xf752,0xf751,0xf762,0xf863,0xf863,
0xf861,0xf962,0xf962,0xf861,0xf861,0xf861,0xf861,0xf961,0xf750,0xf961,0xf862,0xf973,0xf972,0xf862,0xf861,0xf862,0xf973,0xf973,
0xf861,0xf861,0xf861,0xf861,0xf961,0xf861,0xf861,0xf861,0xf860,0xfa70,0xf861,0xf862,0xf862,0xf861,0xf861,0xf861,0xf862,0xf972,
0xf861,0xf861,0xf861,0xf861,0xf861,0xf861,0xf862,0xf861,0xf750,0xfa70,0xf961,0xf861,0xf861,0xf861,0xf961,0xf861,0xf862,0xf972,
0xf861,0xf862,0xf862,0xf861,0xf861,0xf861,0xf962,0xf862,0xf750,0xfa71,0xf971,0xf961,0xf971,0xf971,0xf961,0xf861,0xf862,0xf972,
0xf861,0xf862,0xf862,0xf861,0xf971,0xf861,0xf861,0xf861,0xf751,0xf971,0xf971,0xf971,0xfa71,0xfa71,0xf961,0xf861,0xf862,0xf862,
};// Converted from [00000010] to ARGB4444, 864 bytes

#define BUNK1_X_Y 12
#define BUNK1_X_X 18

G_image Bunk1_x ={
(void*) Bunk1_x_array,BUNK1_X_X,BUNK1_X_Y,
0x00000004,16,0,0, // Alpha mode COLORMODE_ARGB4444 
"Bunk1_x\0",
};

const uint16_t Bunk1_x_array[BUNK1_X_Y][BUNK1_X_X] = {
0x0000,0x0000,0x0000,0xf861,0x0000,0xd973,0xf973,0x0000,0xf862,0x0000,0xf973,0x2984,0x0000,0x0000,0xf862,0x0000,0xf861,0x2861,
0xf861,0x0000,0xf861,0x0000,0xe861,0xf973,0xf973,0x0000,0xe862,0xf861,0xf862,0x0000,0xe973,0xf973,0xf973,0x0000,0xf862,0x1861,
0x0000,0xe861,0xf861,0x0000,0xf861,0xf972,0x0000,0xe862,0xf861,0xf861,0xf861,0x0000,0x0000,0x0000,0x0000,0x0000,0xf972,0xf861,
0x0000,0x0000,0xf861,0xf861,0x0000,0xf962,0xf972,0x0000,0xf861,0x0000,0x0000,0x0000,0x0000,0xf861,0x0000,0x0000,0xf973,0xf972,
0x0000,0x0000,0xf962,0xf861,0x0000,0xf862,0x0000,0xf861,0xf861,0x0000,0xf861,0xf861,0x0000,0xf861,0xf861,0xf973,0xf973,0x0000,
0x0000,0x0000,0xf973,0x0000,0xf861,0xf862,0x0000,0x0000,0xf861,0x0000,0xf862,0x0000,0xf972,0x0000,0xf861,0xe972,0x0000,0xf984,
0x1973,0xf974,0xf973,0xf862,0xf861,0xf861,0x0000,0x0000,0xf861,0xf861,0xf972,0xf973,0xe973,0x0000,0xf861,0x0000,0xf973,0xe973,
0xf861,0x0000,0xf972,0x0000,0xf861,0x0000,0x2862,0xf861,0x0000,0xf861,0xd962,0x0000,0xf973,0xf862,0xf861,0xd861,0x0000,0x0000,
0x0000,0x0000,0xb862,0xf861,0xf861,0xf861,0x1862,0xb861,0xf971,0x1961,0xe861,0x4972,0x0000,0xe861,0x1861,0xe861,0x1861,0xe862,
0xc861,0xf862,0x0000,0xf861,0x1000,0xc862,0xf972,0xf862,0xf861,0xf961,0x1861,0x0000,0xe862,0xf861,0xf861,0xf861,0xf861,0x2862,
0xf862,0x0000,0xe972,0xf861,0x0000,0xf862,0x0000,0xf962,0x0000,0xe861,0x0000,0x0000,0xf861,0xf861,0x0000,0xe862,0x0000,0x0000,
0x0000,0xf973,0xf972,0xf861,0xf861,0x0000,0xf862,0xf862,0x0000,0xf861,0xf861,0x0000,0xf861,0x0000,0xf861,0xf962,0x0000,0x0000,
};// Converted from [00000010] to ARGB4444, 864 bytes

#define BUNK2_Y 12
#define BUNK2_X 18

G_image Bunk2 ={
(void*) Bunk2_array,BUNK2_X,BUNK2_Y,
0x00000004,16,0,0, // Alpha mode COLORMODE_ARGB4444 
"Bunk2\0",
};

const uint16_t Bunk2_array[BUNK2_Y][BUNK2_X] = {
0xf971,0xf971,0xfa71,0xfa71,0xf961,0xf862,0xf862,0xf861,0xf750,0xf861,0xf962,0xf973,0xf862,0xf961,0xf971,0xf971,0xf971,0xf961,
0xf971,0xf971,0xfa70,0xfa70,0xf971,0xf861,0xf861,0xf861,0xf750,0xf861,0xf862,0xf861,0xf861,0xf861,0xf861,0xf861,0xf861,0xf961,
0xf961,0xf971,0xfa71,0xfa70,0xf971,0xf861,0xf861,0xf861,0xf751,0xf862,0xf861,0xf861,0xf971,0xf961,0xf861,0xf861,0xf862,0xf861,
0xf861,0xf861,0xf971,0xfa71,0xf971,0xf861,0xf972,0xf973,0xf752,0xf972,0xf861,0xf961,0xfa71,0xf971,0xf861,0xf962,0xf973,0xf973,
0xf861,0xf861,0xf861,0xf971,0xf961,0xf862,0xf973,0xf973,0xf762,0xf861,0xf861,0xf961,0xfa71,0xf971,0xf861,0xf973,0xfa84,0xfa84,
0xf861,0xf862,0xf861,0xf961,0xf861,0xf962,0xf984,0xf973,0xf751,0xf861,0xf961,0xf861,0xf961,0xf961,0xf861,0xf973,0xfa85,0xfa85,
0xf651,0xf751,0xf751,0xf750,0xf750,0xf751,0xf762,0xf751,0xf750,0xf850,0xf750,0xf651,0xf651,0xf651,0xf651,0xf752,0xf863,0xf863,
0xf961,0xf861,0xf861,0xf861,0xf861,0xf861,0xf862,0xf861,0xf860,0xfa70,0xf861,0xf972,0xf972,0xf861,0xf861,0xf862,0xf973,0xf973,
0xfa71,0xf861,0xf861,0xf861,0xf861,0xf861,0xf861,0xf961,0xf860,0xfa71,0xf861,0xf973,0xf973,0xf862,0xf861,0xf861,0xf861,0xf862,
0xf971,0xf861,0xf861,0xf961,0xf961,0xf861,0xf861,0xf961,0xf860,0xf971,0xf861,0xf973,0xf973,0xf862,0xf861,0xf861,0xf861,0xf962,
0xf861,0xf862,0xf862,0xf961,0xf971,0xf961,0xf861,0xf861,0xf750,0xf961,0xf861,0xf972,0xf862,0xf861,0xf961,0xf861,0xf862,0xf973,
0xf861,0xf972,0xf962,0xf961,0xfa71,0xf971,0xf861,0xf861,0xf651,0xf861,0xf862,0xf862,0xf861,0xf971,0xf971,0xf861,0xf973,0xf973,
};// Converted from [00000010] to ARGB4444, 864 bytes

#define BUNK2_X_Y 12
#define BUNK2_X_X 18

G_image Bunk2_x ={
(void*) Bunk2_x_array,BUNK2_X_X,BUNK2_X_Y,
0x00000004,16,0,0, // Alpha mode COLORMODE_ARGB4444 
"Bunk2_x\0",
};

const uint16_t Bunk2_x_array[BUNK2_X_Y][BUNK2_X_X] = {
0x0000,0x0000,0x0000,0xf973,0xf862,0xf861,0x0000,0xf861,0xf862,0x0000,0xf862,0x0000,0x0000,0xf973,0x0000,0xf861,0x0000,0xf862,
0xf971,0xf861,0x0000,0xf973,0x0000,0xf861,0xf861,0xf861,0xf861,0xf861,0x0000,0x0000,0xf973,0xf973,0xf972,0xf861,0xf861,0x0000,
0xfa71,0x0000,0xf862,0xf973,0x0000,0xf972,0x0000,0xf862,0x0000,0xf861,0x0000,0x0000,0xf973,0xf973,0x0000,0xf861,0xf861,0xf861,
0x0000,0xf961,0x0000,0x0000,0x0000,0x0000,0xf973,0xf972,0x0000,0xf961,0xf961,0x0000,0xf973,0x0000,0xf972,0xf861,0x0000,0xf861,
0xf861,0xf861,0x0000,0x0000,0x0000,0xf973,0xf973,0x0000,0x0000,0xf961,0x0000,0xf861,0xf972,0x0000,0xf862,0xf861,0x0000,0x0000,
0xf861,0xf861,0xf861,0xf861,0x0000,0xf861,0x0000,0x0000,0x0000,0xf971,0xf971,0x0000,0xf972,0xf972,0xf862,0xf861,0xf861,0x0000,
0x0000,0xf861,0x0000,0xf861,0xf861,0xf861,0x0000,0x0000,0x0000,0xfa70,0x0000,0xf861,0xf972,0xf972,0x0000,0xf861,0x0000,0xf862,
0xf861,0xf862,0x0000,0xf862,0xf861,0x0000,0x0000,0xf861,0x0000,0xfa71,0x0000,0x0000,0x0000,0x0000,0xf861,0x0000,0x0000,0x0000,
0xf861,0xf972,0xf973,0x0000,0xf862,0x0000,0xf862,0x0000,0x0000,0x0000,0x0000,0xf861,0xf862,0xf861,0x0000,0xf861,0xf861,0x0000,
0x0000,0xf862,0xf972,0xf862,0xf861,0xf862,0x0000,0x0000,0xf861,0xf961,0x0000,0xf861,0x0000,0xf861,0xf861,0xf861,0xf861,0xf861,
0x0000,0xf861,0xf861,0x0000,0xf861,0xf861,0xf962,0x0000,0xf861,0x0000,0xf961,0xf861,0x0000,0xf861,0x0000,0xf861,0x0000,0xf861,
0x0000,0xf971,0x0000,0xf971,0xf971,0x0000,0xf861,0x0000,0x0000,0xfa70,0x0000,0x0000,0x0000,0x0000,0xf862,0xf861,0x0000,0xf961,
};// Converted from [00000010] to ARGB4444, 864 bytes

#define BUNK3_Y 12
#define BUNK3_X 18

G_image Bunk3 ={
(void*) Bunk3_array,BUNK3_X,BUNK3_Y,
0x00000004,16,0,0, // Alpha mode COLORMODE_ARGB4444 
"Bunk3\0",
};

const uint16_t Bunk3_array[BUNK3_Y][BUNK3_X] = {
0x7861,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0xf861,0xe861,0x6861,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0xf861,0xf861,0xf861,0xd861,0x5862,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0xf862,0xf962,0xf861,0xf861,0xf861,0xc973,0x3973,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0xf973,0xf973,0xf972,0xf861,0xf861,0xf973,0xf973,0xa973,0x2973,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0xf973,0xf974,0xf973,0xf862,0xf861,0xf972,0xf973,0xf973,0xf862,0x9973,0x1973,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0xf752,0xf763,0xf762,0xf751,0xf751,0xf752,0xf762,0xf762,0xf752,0xf752,0xf873,0x7984,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0xf861,0xf962,0xf962,0xf861,0xf861,0xf862,0xf972,0xf862,0xf651,0xf861,0xf973,0xf984,0xe984,0x5972,0x0000,0x0000,0x0000,0x0000,
0xf861,0xf861,0xf861,0xf861,0xf861,0xf861,0xf862,0xf861,0xf750,0xf861,0xf862,0xf973,0xf973,0xf972,0xc861,0x4861,0x0000,0x0000,
0xf861,0xf861,0xf861,0xf861,0xf861,0xf861,0xf962,0xf861,0xf750,0xf971,0xf861,0xf962,0xf972,0xf862,0xf861,0xf862,0xb962,0x0000,
0xf861,0xf862,0xf862,0xf861,0xf861,0xf861,0xf962,0xf862,0xf651,0xf971,0xf961,0xf861,0xf861,0xf861,0xf861,0xf862,0xf972,0xf972,
0xf861,0xf862,0xf862,0xf861,0xf971,0xf861,0xf861,0xf861,0xf750,0xf971,0xf971,0xf971,0xf971,0xf971,0xf861,0xf862,0xf962,0xf862,
};// Converted from [00000010] to ARGB4444, 864 bytes

#define BUNK4_Y 12
#define BUNK4_X 18

G_image Bunk4 ={
(void*) Bunk4_array,BUNK4_X,BUNK4_Y,
0x00000004,16,0,0, // Alpha mode COLORMODE_ARGB4444 
"Bunk4\0",
};

const uint16_t Bunk4_array[BUNK4_Y][BUNK4_X] = {
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x7862,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x6862,0xe862,0xf861,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x5862,0xd862,0xf862,0xf862,0xf861,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x3861,0xc972,0xf973,0xf973,0xf972,0xf862,0xf861,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x2971,0xa971,0xf861,0xf972,0xf973,0xf973,0xf972,0xf972,0xf972,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1972,0x9861,0xf961,0xf971,0xf861,0xf862,0xf972,0xf973,0xf973,0xf973,0xf973,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x7862,0xf862,0xf751,0xf750,0xf750,0xf750,0xf651,0xf751,0xf751,0xf752,0xf763,0xf863,
0x0000,0x0000,0x0000,0x0000,0x5861,0xe861,0xf861,0xf861,0xf651,0xf961,0xf961,0xf861,0xf861,0xf861,0xf861,0xf861,0xf973,0xf973,
0x0000,0x0000,0x4861,0xc861,0xf861,0xf961,0xfa71,0xf971,0xf750,0xf971,0xf861,0xf861,0xf861,0xf861,0xf971,0xf861,0xf862,0xf972,
0x0000,0xb861,0xf861,0xf861,0xf861,0xf961,0xfa70,0xfa70,0xf860,0xf961,0xf861,0xf862,0xf862,0xf862,0xf861,0xf861,0xf861,0xf962,
0xf861,0xf861,0xf861,0xf862,0xf861,0xf861,0xf961,0xf961,0xf750,0xf971,0xf861,0xf861,0xf972,0xf972,0xf862,0xf861,0xf861,0xf862,
0xf861,0xf861,0xf861,0xf861,0xf861,0xf861,0xf861,0xf861,0xf651,0xf961,0xf971,0xf861,0xf862,0xf972,0xf862,0xf861,0xf861,0xf861,
};// Converted from [00000010] to ARGB4444, 864 bytes

#define BUNK4_X_Y 12
#define BUNK4_X_X 18

G_image Bunk4_x ={
(void*) Bunk4_x_array,BUNK4_X_X,BUNK4_X_Y,
0x00000004,16,0,0, // Alpha mode COLORMODE_ARGB4444 
"Bunk4_x\0",
};

const uint16_t Bunk4_x_array[BUNK4_X_Y][BUNK4_X_X] = {
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe862,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1862,0x8861,0xe862,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1861,0x9862,0xf862,0x0000,0xf861,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x2971,0x0000,0x0000,0xf972,0xf972,0xf962,0xf862,0x0000,0xf861,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xa861,0xf971,0x0000,0xf861,0xf972,0x0000,0x0000,0x0000,0xf862,0xf861,
0x0000,0x0000,0x0000,0x0000,0x0000,0x4962,0xb972,0x0000,0x0000,0xf961,0x0000,0xf861,0x0000,0xf972,0x0000,0x0000,0xf973,0x0000,
0x0000,0x0000,0x0000,0x5962,0xc972,0xf962,0xf972,0x0000,0x0000,0xf961,0xf971,0xf861,0xf861,0xf861,0xa861,0xf862,0x0000,0xf973,
0x0000,0x6861,0x0000,0xf862,0x7861,0xf861,0x0000,0xe861,0xf861,0xf961,0xf961,0xf861,0x5861,0x7861,0x6961,0xf861,0xf972,0x0000,
0x9862,0x0000,0x3861,0xa861,0xf861,0x0000,0xd971,0x3971,0x0000,0x0000,0x0000,0xf861,0xc862,0xf861,0x9961,0xf861,0xf861,0xe972,
0xf861,0x0000,0xf861,0x5862,0xf861,0x9861,0x4961,0x0000,0xe971,0xf861,0x1861,0xf962,0x0000,0xf862,0x6861,0xf861,0x3861,0xf862,
0x0000,0x0000,0xf862,0xa962,0xf862,0xf861,0xb861,0x0000,0xf861,0x0000,0xf861,0xc862,0x0000,0x8973,0x9862,0xf861,0x0000,0xf861,
0x0000,0xf861,0xb861,0x5862,0xf862,0x6861,0xf861,0x0000,0x1000,0xf861,0x0000,0x0000,0x0000,0x0000,0x7862,0x4861,0x0000,0x1861,
};// Converted from [00000010] to ARGB4444, 864 bytes

#define BUNK3_X_Y 12
#define BUNK3_X_X 18

G_image Bunk3_x ={
(void*) Bunk3_x_array,BUNK3_X_X,BUNK3_X_Y,
0x00000004,16,0,0, // Alpha mode COLORMODE_ARGB4444 
"Bunk3_x\0",
};

const uint16_t Bunk3_x_array[BUNK3_X_Y][BUNK3_X_X] = {
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0xf861,0x0000,0xe861,0x0000,0x1862,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0xf861,0xf861,0xf861,0xf861,0x9973,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0xf861,0xf861,0x0000,0xf972,0xf973,0x0000,0x2861,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0xf973,0x0000,0xf972,0xf861,0x0000,0x0000,0x0000,0xf862,0xf861,0x0000,0x3862,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0xf973,0xf984,0x0000,0x0000,0x0000,0x0000,0xf962,0xf862,0xf861,0x0000,0xf973,0x0000,0x4972,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0xf984,0x0000,0x0000,0xf861,0xf861,0x0000,0x0000,0xf861,0xf861,0xf973,0x0000,0xf973,0x0000,0x5862,0x0000,0x0000,0x0000,
0xf861,0x0000,0xf973,0x0000,0xf861,0x0000,0x0000,0xf861,0x0000,0xf861,0xf972,0x0000,0xf973,0xf862,0xf861,0xd862,0x0000,0x0000,
0x2861,0x4862,0x0000,0x9861,0x9861,0x0000,0x0000,0xf861,0x0000,0xf971,0x0000,0x4972,0x7972,0x8861,0x5861,0xb861,0xd862,0x0000,
0xf861,0xf962,0x6862,0x8861,0xf861,0x0000,0xe972,0x1862,0x0000,0x0000,0x0000,0xf862,0xb862,0xf861,0xa861,0xf861,0xf861,0xf962,
0xd862,0xf972,0xf972,0x8861,0xf861,0xb862,0x2973,0x0000,0xf861,0xf861,0x3861,0xf861,0x0000,0xf861,0x4861,0xf861,0x2862,0xf962,
0x0000,0xf972,0xf972,0x7861,0xf861,0xf861,0xd862,0x0000,0xf861,0x0000,0xe861,0xa861,0x0000,0x7961,0xb861,0xf861,0x0000,0xf862,
};// Converted from [00000010] to ARGB4444, 864 bytes
