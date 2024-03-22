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
******************************************************************************/

#include "Arduino.h"
#include "mbed.h"
#include "Arduino_H7_Video.h"
#include "dsi.h"
#include <SDRAM.h>

#define sleep _sleep

#include "SDebug.h"         // JV for Serial debugging
#include "USBC_Canvas.h"

#include <stdio.h>
#include <unistd.h>

Arduino_H7_Video display(GENERIC_RESX, GENERIC_RESY, USBCVideo);
uint32_t* DG_ScreenBuffer = 0;
uint32_t active,current,end,size;
uint16_t* RGB565Canvas;
char TBUF[60];

struct color {
    uint32_t b:8;
    uint32_t g:8;
    uint32_t r:8;
    uint32_t a:8;
};
struct color colors[256];

// mix on black
uint16_t rgb565fade(uint16_t rgb565Pixel,uint8_t mix)
{
    uint32_t r =  ( (((uint32_t) rgb565Pixel ) & 0x0000F800) + 0x00000700 ) ;
    uint32_t g =  ( (((uint32_t) rgb565Pixel << 5) & 0x0000FC00) + 0x00000300 ) ;
    uint32_t b =  ( (((uint32_t) rgb565Pixel <<11 ) & 0x0000F800) + 0x00000700 ) ;  
    r=  (((r*( (uint32_t) mix ) )/256)     )& 0X0000F800;
    g=  (((g*( (uint32_t) mix ) )/256)>>5  )& 0X000007E0;
    b=  (((b*( (uint32_t) mix ) )/256)>>11 )& 0X0000001F;
    return  (uint16_t) (r | g | b)   ;
}

uint32_t rgb565to8888(uint16_t rgb565Pixel)
{
    uint32_t r =  ( (((uint32_t) rgb565Pixel << 8) & 0x00F80000) + 0x00070000 ) ;
    uint32_t g =  ( (((uint32_t) rgb565Pixel << 5) & 0x0000FC00) + 0x00000300 ) ;
    uint32_t b =  ( (((uint32_t) rgb565Pixel <<3 ) & 0x000000F8) + 0x00000007 ) ;  
    uint32_t a =  0xFF000000;
    return   (uint32_t) (a | r | g | b);
}

uint16_t argb8888to565(uint32_t argb8888Pixel)
{
    uint8_t red   = (uint8_t) ((argb8888Pixel   ) & 0x000000FF);
    uint8_t green = (uint8_t) ((argb8888Pixel>>8) & 0x000000FF);
    uint8_t blue  = (uint8_t) ((argb8888Pixel>>16)& 0x000000FF);
    uint16_t b = (blue >> 3) & 0x1f;
    uint16_t g = ((green >> 2) & 0x3f) << 5;
    uint16_t r = ((red >> 3) & 0x1f) << 11;
    return (uint16_t) (r | g | b);
}

static void InitCLUT(uint32_t * clut)
{
  uint32_t  red = 0, green = 0, blue = 0;
  uint32_t  i = 0x00;

  /* Color map generation */
  for (i = 0; i < 256; i++)
  {
    /* Generate red, green and blue values */
    red = (i * 8) % 256;
    green = (i * 6) % 256;
    blue = (i * 4) % 256;

    red = red << 16;
    green = green << 8;

    /* Store the 32-bit value */
    clut[i] = 0xFF000000 | (red + green + blue);
  }
}


void Canvas_Init()
{
  pinMode(LEDR, OUTPUT);pinMode(LEDB, OUTPUT);pinMode(LEDG, OUTPUT);
  digitalWrite(LEDR, HIGH); digitalWrite(LEDB, HIGH);  digitalWrite(LEDG, HIGH);    
  digitalWrite(LEDG, LOW);                                   // JV set green on
  display.begin();   
  current=(uint32_t) dsi_getCurrentFrameBuffer();active=(uint32_t) dsi_getActiveFrameBuffer(); end =(uint32_t) dsi_getFramebufferEnd();  
  if(active > current ) size = end-current;    
  else size = end-active;                  
  InitCLUT( (uint32_t *) colors);
  dsi_configueCLUT( (uint32_t *) colors);                    // set CLU (needed ????
  dsi_lcdFillArea((void *)(dsi_getCurrentFrameBuffer()), GENERIC_RESX, GENERIC_RESY, 0x00555555);                   // setup clear screen via dsi-functions
  dsi_lcdFillArea((void *)(dsi_getCurrentFrameBuffer() + GENERIC_RESX*GENERIC_RESY/2), GENERIC_RESX,2, 0x80AAAAAA); // Make Cross
  dsi_lcdFillArea((void *)(dsi_getCurrentFrameBuffer() + GENERIC_RESX/2), 2, GENERIC_RESY, 0x80AAAAAA);
  dsi_drawCurrentFrameBuffer();
  RGB565Canvas= (uint16_t*)ea_malloc(GENERIC_RESX*GENERIC_RESY*2);  // Allocate Canvas using RGB565 - 2 bytes
  DG_ScreenBuffer = (uint32_t*) RGB565Canvas;                       // make pointer to Canvas for Draw_frame function
  digitalWrite(LEDG, HIGH); Debugln(" * Display initialized");          // JV init display, green off
    Debug(" * Display set at "); Debug(dsi_getDisplayXSize());Debug(" x ");Debugln(dsi_getDisplayYSize()); 
    Debug(" * Dsi FrameBuffers - Active:0x"); Debug(active,HEX);Debug(", Current:0x");Debugln(current,HEX); 
    Debug(" * Dsi Buffers ending at 0x"); Debug(end,HEX);;Debug(", Size ");Debug(size/1024); Debugln(" Kbytes");
    Debug(" * Canvas RGB565 color initialized:"); Debug((GENERIC_RESX*GENERIC_RESY*2)/1024);Debug(" Kbytes starting at 0x");Debugln((uint32_t) RGB565Canvas,HEX); 
  delay(1000);
}

void Canvas_DrawFrame()
{
  dsi_lcdDrawImage((void*) DG_ScreenBuffer, (void *)(dsi_getCurrentFrameBuffer()) ,GENERIC_RESX, GENERIC_RESY, DMA2D_INPUT_RGB565);
  dsi_drawCurrentFrameBuffer();
  //check_BLE();                                     // JV check BLE key reception -----------------------------------
}


void Canvas_FillScreen(uint16_t rgb565color){
  dsi_lcdFillArea( (void *)(RGB565Canvas),(uint32_t)GENERIC_RESX,(uint32_t)GENERIC_RESY, (uint32_t) rgb565to8888(rgb565color) );
  Canvas_DrawFrame();
}

void Canvas_DrawColors(uint16_t x, uint16_t y, uint16_t xw, uint16_t  yw, uint16_t *image)
{
  if ( ((x+xw) < GENERIC_RESX) || ((y + yw) < GENERIC_RESY) ) {
  dsi_lcdDrawImage((void*)image, (void *)(RGB565Canvas + x + y*GENERIC_RESX) ,(uint32_t)xw, (uint32_t)yw, DMA2D_INPUT_RGB565);
  }
}



void Canvas_FillRect(uint16_t x, uint16_t y,uint16_t xw, uint16_t yw,uint16_t rgb565color)
{
  if ( ((x+xw) < GENERIC_RESX) || ((y + yw) < GENERIC_RESY) ) {
    dsi_lcdFillArea( (void *)(RGB565Canvas + x + y*GENERIC_RESX),(uint32_t)xw,(uint32_t)yw, (uint32_t)rgb565to8888(rgb565color) );
    }
}


// Wrapper Large Text
void Canvas_Text(const char * txt, uint16_t x, uint16_t y,uint16_t color,uint8_t size){
  Canvas_Drawtext( (uint16_t *) RGB565Canvas, txt, ArialBold, x, y, color,size);
}



// 8bbp font, capitals only 
void Canvas_Drawtext( uint16_t *Dest, const char *text, font8bpp myfont, uint16_t x, int16_t y,uint16_t color,uint8_t scale)
{
char c=0;
uint16_t width,ip,cc,width_read,height = myfont.height;
uint8_t  *fmap=myfont.fontmap;
uint16_t *findex = myfont.fontindex;
uint8_t sizex,sizey,pixelbyte, parray[18], bpp = myfont.bpp;
uint16_t charcursor=0; // pixelcursor=0;
switch (scale)
    {
    case 1 : sizex = 1; sizey=1;break;
    case 2 : sizex = 2; sizey=1; break;
    case 3 : sizex = 1; sizey=2;break;
    case 4 : sizex = 2; sizey=2; break;
    case 5 : sizex = 3; sizey=2; break;
    default :sizex = 1; sizey=1;break;
    };

cc=0;
while( (c=text[cc]) !=0 ){
 if(c>myfont.last) c=c-32; // change small to large ascii
 if ( (c>=myfont.first) && (c<=myfont.last)){
      ip =  *( (uint16_t*) findex + (c-myfont.first)*2 + 1);         // pointer to first pixel
      width= *( (uint16_t*) findex + (c-myfont.first)*2 + 0);        // number of pixels wide
      for(int i=0;i<height;++i){
          for(int t=0;t<width;++t) {
               pixelbyte=*( (uint8_t*) fmap + ip + i*width + t);
               for(int u=0;u<sizex;++u) parray[t*sizex+u] = pixelbyte;  // load line pixels into expanded array
              }
          if (sizex >1) for (int v=1; v<width*sizex;v++) { parray[v]= (parray[v]>>1)+(parray[v+1]>>1) ; }// average pixel line to expa
          for (int w=0; w<sizey;w++) 
              for(int v=0;v<width*sizex;v++)
                *((uint16_t*) Dest + charcursor +  (i*sizey+w+y)*GENERIC_RESX + x + v ) = (uint16_t) rgb565fade(color,parray[v]); // write pixles
      } // i height
      charcursor+=width*sizex+1;
  }
 cc++; if(cc>36) break;
 }

}






