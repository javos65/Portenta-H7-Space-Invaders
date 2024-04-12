
#include <Arduino.h>
#include <stdint.h>   // uint definitions

#include "screen.h"   // for h7image definition and COLOR MODE DEFINITIONS
#include "lvgl_a4.h"    // for font4bpp definitions
#include "stm32h7xx_hal_dma2d.h" // for Video definitions
#include "DEBUGF.h"

//
// PIXEL CONVERSIONS
//
//
//

 // ARGB888 to ARGB1555 conversion
uint16_t Convert8888to1555(uint32_t argb8888Pixel,bool swaprb,bool transparant, uint32_t alpha0)
{
          uint32_t blue   = (uint32_t) ((argb8888Pixel   ) & 0x000000FF);
          uint32_t green = (uint32_t) ((argb8888Pixel>>8) & 0x000000FF);
          uint32_t red  = (uint32_t) ((argb8888Pixel>>16)& 0x000000FF);
          uint32_t alpha = (uint32_t) ((argb8888Pixel>>24)& 0x000000FF);  
          if (swaprb==true) {uint8_t unu = blue; blue=red; red=unu;}   
          if ( alpha<=ALPHANEARZERO && alpha0>=ZALPHAREPLACE ) {red= (alpha0>>16)&0x000000FF; green= (alpha0>>8)&0x000000FF;  blue= (alpha0)&0x000000FF; }      // if not transparant and aplha color defined: replace near zero transparancy colors
          if ( !transparant) alpha = 0xff; // allp opaque colors
          uint16_t b = ((uint16_t) blue >> 3)  & 0b0000000000011111;
          uint16_t g = ((uint16_t) green <<2 ) & 0b0000001111100000;
          uint16_t r = ((uint16_t) red <<7)    & 0b0111110000000000;
          uint16_t a = 0x8000; if (alpha <0x40) a=0x0000;
          return (uint16_t) (a | r | g | b);
}

// Convert ARGB8888 to ARGB4444
uint16_t Convert8888to4444(uint32_t argb8888Pixel,bool swaprb,bool transparant, uint32_t alpha0)
{
          uint32_t blue  = (uint32_t) ((argb8888Pixel   ) & 0x000000FF);
          uint32_t green = (uint32_t) ((argb8888Pixel>>8) & 0x000000FF);
          uint32_t red   = (uint32_t) ((argb8888Pixel>>16)& 0x000000FF);
          uint32_t alpha = (uint32_t) ((argb8888Pixel>>24)& 0x000000FF);    
          if (swaprb==true) {uint32_t unu = blue; blue=red; red=unu;}    
          if ( alpha<=ALPHANEARZERO && alpha0>=ZALPHAREPLACE ) {red= (alpha0>>16)&0x000000FF; green= (alpha0>>8)&0x000000FF;  blue= (alpha0)&0x000000FF; }      // if not transparant and aplha color defined: replace near zero transparancy colors
          if ( !transparant) alpha = 0xff; // allp opaque colors
          uint16_t b = ((uint16_t) blue >> 4) & 0b0000000000001111;
          uint16_t g = ((uint16_t) green    ) & 0b0000000011110000;
          uint16_t r = ((uint16_t) red  <<4 ) & 0b0000111100000000;
          uint16_t a = ((uint16_t) alpha <<8 )& 0b1111000000000000;
          return (uint16_t) (a | r | g | b); 
}


// Convert ARGB8888 to RGB565 mix alpha channel to background color alpha0
uint16_t Convert8888to565(uint32_t argb8888Pixel,bool swaprb, bool transparant, uint32_t alpha0)
{
          uint32_t blue   = ((argb8888Pixel   ) & 0x000000FF);
          uint32_t green =  ((argb8888Pixel>>8) & 0x000000FF);
          uint32_t red  =   ((argb8888Pixel>>16)& 0x000000FF);
          uint32_t alpha =  ((argb8888Pixel>>24)& 0x000000FF);
          uint32_t blue0   = ((alpha0   ) & 0x000000FF);
          uint32_t green0 =  ((alpha0>>8) & 0x000000FF);
          uint32_t red0  =   ((alpha0>>16)& 0x000000FF);
          if (! transparant) alpha = 0xff;
          if (swaprb==true) {uint32_t unu = blue; blue=red; red=unu;}    
          red=(red*alpha)/0xFF + (red0*(0xFF-alpha))/0xFF;   blue=(blue*alpha)/0xFF + (blue0*(0xFF-alpha))/0xFF;   green=(green*alpha)/0xFF + (green0*(0xFF-alpha))/0xFF;  // mix alpha channel to background color
          uint16_t b = ((uint16_t) blue >> 3 ) & 0b0000000000011111;
          uint16_t g = ((uint16_t) green <<3 ) & 0b0000011111100000;
          uint16_t r = ((uint16_t) red  <<8  ) & 0b1111100000000000;
          return (uint16_t) ( r | g | b);  
} 


// Convert ARGB8888 to RGB565
uint32_t Convert8888to8888(uint32_t argb8888Pixel,bool swaprb, bool transparant, uint32_t alpha0)
{
          uint32_t blue   = ((argb8888Pixel   ) & 0x000000FF);
          uint32_t green =  ((argb8888Pixel>>8) & 0x000000FF);
          uint32_t red  =   ((argb8888Pixel>>16)& 0x000000FF);
          uint32_t alpha =  ((argb8888Pixel>>24)& 0x000000FF);
          if (swaprb==true) {uint32_t unu = blue; blue=red; red=unu;}    
          if ( alpha<=ALPHANEARZERO && alpha0>=ZALPHAREPLACE ) {red= (alpha0>>16)&0x000000FF; green= (alpha0>>8)&0x000000FF;  blue= (alpha0)&0x000000FF; }      // if not transparant and aplha color defined: replace near zero transparancy colors
          if ( !transparant) alpha = 0xff; // allp opaque colors                
          uint32_t b = ((uint32_t) blue      ) & 0x000000FF;
          uint32_t g = ((uint32_t) green<<8  ) & 0x0000FF00;
          uint32_t r = ((uint32_t) red <<16  ) & 0x00FF0000;
          uint32_t a = ((uint32_t) alpha<<24 ) & 0xFF000000;
          return (uint32_t) ( a| r | g | b);  
} 

// Convert RGB565 to XRGB1555 /ARGB1555
uint16_t Convert565to1555(uint16_t rgb565Pixel,bool swaprb)
{
//DEBUGF("565pixel[%0.4x] ",rgb565Pixel);
          uint16_t blue   = ((rgb565Pixel<<3 ) & 0b0000000011111000) + ((rgb565Pixel>>2 ) & 0b0000000000000111); // convert to full 8 bit
          uint16_t green =  ((rgb565Pixel>>3 ) & 0b0000000011111100) + ((rgb565Pixel>>9 ) & 0b0000000000000011); // convert to full 8 bit
          uint16_t red  =   ((rgb565Pixel>>8 ) & 0b0000000011111000) + ((rgb565Pixel>>13) & 0b0000000000000111); // convert to full 8 bit
//DEBUGF("888pixel[FF%0.2x%02x%02x] ",red,green,blue);          
          if (swaprb==true) {uint16_t unu = blue; blue=red; red=unu;}      
          uint16_t b = ((uint16_t) blue  >>3) & 0b0000000000011111;
          uint16_t g = ((uint16_t) green <<2) & 0b0000001111100000;
          uint16_t r = ((uint16_t) red   <<7) & 0b0111110000000000;
          uint16_t a = 0x8000; ; // fixed alpha
//DEBUGF("1555pixel[%0.4x]\n",(a | r | g | b));            
          return (uint16_t) (a | r | g | b); 
} 

 // ARGB1555 to ARGB1555 conversion
uint16_t Convert1555to1555(uint16_t argb1555Pixel,bool swaprb,bool transparant, uint32_t alpha0)
{
//DEBUGF("1555pixel[%0.4x] ",rgb565Pixel);
          uint16_t blue   = ((argb1555Pixel<<3 ) & 0b0000000011111000) + ((argb1555Pixel>>2 ) & 0b0000000000000111); // convert to full 8 bit
          uint16_t green =  ((argb1555Pixel>>2 ) & 0b0000000011111000) + ((argb1555Pixel>>7 ) & 0b0000000000000111); // convert to full 8 bit
          uint16_t red  =   ((argb1555Pixel>>7 ) & 0b0000000011111000) + ((argb1555Pixel>>12) & 0b0000000000000111); // convert to full 8 bit  
          uint16_t alpha =0x00; if (argb1555Pixel&&0x80) alpha =0xff;
//DEBUGF("8888pixel[%0.2x%0.2x%02x%02x] ",alpha,red,green,blue);             
          if (swaprb==true) {uint16_t unu = blue; blue=red; red=unu;}   
          if ( alpha<=ALPHANEARZERO && alpha0>=ZALPHAREPLACE ) {red= (alpha0>>16)&0x000000FF; green= (alpha0>>8)&0x000000FF;  blue= (alpha0)&0x000000FF; }      // if not transparant and aplha color defined: replace near zero transparancy colors
          if ( !transparant) alpha = 0xff; // allp opaque colors
          uint16_t b = ((uint16_t) blue >> 3)  & 0b0000000000011111;
          uint16_t g = ((uint16_t) green <<2 ) & 0b0000001111100000;
          uint16_t r = ((uint16_t) red <<7)    & 0b0111110000000000;
          uint16_t a = 0x8000; if (alpha <0x40) a=0x0000;
//DEBUGF("1555pixel[%0.4x]\n",(a | r | g | b));             
          return (uint16_t) (a | r | g | b);
}

 // ARGB4444 to ARGB1555 conversion
uint16_t Convert4444to1555(uint16_t argb4444Pixel,bool swaprb,bool transparant, uint32_t alpha0)
{
//DEBUGF("4444pixel[%0.4x] ",rgb565Pixel);
          uint16_t blue   = ((argb4444Pixel<<4 ) & 0b0000000011110000) + ((argb4444Pixel    ) & 0b0000000000001111); // convert to full 8 bit
          uint16_t green =  ((argb4444Pixel    ) & 0b0000000011110000) + ((argb4444Pixel>>4 ) & 0b0000000000001111); // convert to full 8 bit
          uint16_t red  =   ((argb4444Pixel>>4 ) & 0b0000000011110000) + ((argb4444Pixel>>8 ) & 0b0000000000001111); // convert to full 8 bit  
          uint16_t alpha =  ((argb4444Pixel>>8 ) & 0b0000000011110000) + ((argb4444Pixel>>12) & 0b0000000000001111); // convert to full 8 bit  
//DEBUGF("8888pixel[%0.2x%0.2x%02x%02x] ",alpha,red,green,blue);             
          if (swaprb==true) {uint16_t unu = blue; blue=red; red=unu;}   
          if ( alpha<=ALPHANEARZERO && alpha0>=ZALPHAREPLACE ) {red= (alpha0>>16)&0x000000FF; green= (alpha0>>8)&0x000000FF;  blue= (alpha0)&0x000000FF; }      // if not transparant and aplha color defined: replace near zero transparancy colors
          if ( !transparant) alpha = 0xff; // allp opaque colors
          uint16_t b = ((uint16_t) blue >> 3)  & 0b0000000000011111;
          uint16_t g = ((uint16_t) green <<2 ) & 0b0000001111100000;
          uint16_t r = ((uint16_t) red <<7)    & 0b0111110000000000;
          uint16_t a = 0x8000; if (alpha <0x40) a=0x0000;
//DEBUGF("1555pixel[%0.4x]\n",(a | r | g | b));             
          return (uint16_t) (a | r | g | b);
}


// Convert RGB565 to XRGB4444 /ARGB4444
uint16_t Convert565to4444(uint16_t rgb565Pixel,bool swaprb)
{
//DEBUGF("565pixel[%0.4x] ",rgb565Pixel);
          uint16_t blue   = ((rgb565Pixel<<3 ) & 0b0000000011111000) + ((rgb565Pixel>>2 ) & 0b0000000000000111); // convert to full 8 bit
          uint16_t green =  ((rgb565Pixel>>3 ) & 0b0000000011111100) + ((rgb565Pixel>>9 ) & 0b0000000000000011); // convert to full 8 bit
          uint16_t red  =   ((rgb565Pixel>>8 ) & 0b0000000011111000) + ((rgb565Pixel>>13) & 0b0000000000000111); // convert to full 8 bit
//DEBUGF("888pixel[FF%0.2x%02x%02x] ",red,green,blue);          
          if (swaprb==true) {uint16_t unu = blue; blue=red; red=unu;}      
          uint16_t b = ((uint16_t) blue >> 4) & 0b0000000000001111;
          uint16_t g = ((uint16_t) green    ) & 0b0000000011110000;
          uint16_t r = ((uint16_t) red  <<4 ) & 0b0000111100000000;
          uint16_t a = 0xF000; ; // fixed alpha with 565 input
//DEBUGF("4444pixel[%0.4x]\n",(a | r | g | b));            
          return (uint16_t) (a | r | g | b); 
} 

 // ARGB1555 to ARGB4444 conversion
uint16_t Convert1555to4444(uint16_t argb1555Pixel,bool swaprb,bool transparant, uint32_t alpha0)
{
//DEBUGF("1555pixel[%0.4x] ",rgb565Pixel);
          uint16_t blue   = ((argb1555Pixel<<3 ) & 0b0000000011111000) + ((argb1555Pixel>>2 ) & 0b0000000000000111); // convert to full 8 bit
          uint16_t green =  ((argb1555Pixel>>2 ) & 0b0000000011111000) + ((argb1555Pixel>>7 ) & 0b0000000000000111); // convert to full 8 bit
          uint16_t red  =   ((argb1555Pixel>>7 ) & 0b0000000011111000) + ((argb1555Pixel>>12) & 0b0000000000000111); // convert to full 8 bit  
          uint16_t alpha =0x00; if (argb1555Pixel&&0x80) alpha =0xff;
//DEBUGF("8888pixel[%0.2x%0.2x%02x%02x] ",alpha,red,green,blue);             
          if (swaprb==true) {uint16_t unu = blue; blue=red; red=unu;}   
          if ( alpha<=ALPHANEARZERO && alpha0>=ZALPHAREPLACE ) {red= (alpha0>>16)&0x000000FF; green= (alpha0>>8)&0x000000FF;  blue= (alpha0)&0x000000FF; }      // if not transparant and aplha color defined: replace near zero transparancy colors
          if ( !transparant) alpha = 0xff; // allp opaque colors
          uint16_t b = ((uint16_t) blue >> 4) & 0b0000000000001111;
          uint16_t g = ((uint16_t) green    ) & 0b0000000011110000;
          uint16_t r = ((uint16_t) red  <<4 ) & 0b0000111100000000;
          uint16_t a = ((uint16_t) alpha <<8 )& 0b1111000000000000;
//DEBUGF("4444pixel[%0.4x]\n",(a | r | g | b));             
          return (uint16_t) (a | r | g | b);
}
 // ARGB4444 to ARGB4444 conversion
uint16_t Convert4444to4444(uint16_t argb4444Pixel,bool swaprb,bool transparant, uint32_t alpha0)
{
//DEBUGF("4444pixel[%0.4x] ",rgb565Pixel);
          uint16_t blue   = ((argb4444Pixel<<4 ) & 0b0000000011110000) + ((argb4444Pixel    ) & 0b0000000000001111); // convert to full 8 bit
          uint16_t green =  ((argb4444Pixel    ) & 0b0000000011110000) + ((argb4444Pixel>>4 ) & 0b0000000000001111); // convert to full 8 bit
          uint16_t red  =   ((argb4444Pixel>>4 ) & 0b0000000011110000) + ((argb4444Pixel>>8 ) & 0b0000000000001111); // convert to full 8 bit  
          uint16_t alpha =  ((argb4444Pixel>>8 ) & 0b0000000011110000) + ((argb4444Pixel>>12) & 0b0000000000001111); // convert to full 8 bit  
//DEBUGF("8888pixel[%0.2x%0.2x%02x%02x] ",alpha,red,green,blue);             
          if (swaprb==true) {uint16_t unu = blue; blue=red; red=unu;}   
          if ( alpha<=ALPHANEARZERO && alpha0>=ZALPHAREPLACE ) {red= (alpha0>>16)&0x000000FF; green= (alpha0>>8)&0x000000FF;  blue= (alpha0)&0x000000FF; }      // if not transparant and aplha color defined: replace near zero transparancy colors
          if ( !transparant) alpha = 0xff; // allp opaque colors
          uint16_t b = ((uint16_t) blue >> 4) & 0b0000000000001111;
          uint16_t g = ((uint16_t) green    ) & 0b0000000011110000;
          uint16_t r = ((uint16_t) red  <<4 ) & 0b0000111100000000;
          uint16_t a = ((uint16_t) alpha <<8 )& 0b1111000000000000;
//DEBUGF("4444pixel[%0.4x]\n",(a | r | g | b));             
          return (uint16_t) (a | r | g | b);
}


// Convert RGB565 to XRGB8888 /ARGB8888
uint32_t Convert565to8888(uint16_t rgb565Pixel,bool swaprb)
{
//DEBUGF("565pixel[%0.4x] ",rgb565Pixel);
          uint16_t blue   = ((rgb565Pixel<<3 ) & 0b0000000011111000) + ((rgb565Pixel>>2 ) & 0b0000000000000111); // convert to full 8 bit
          uint16_t green =  ((rgb565Pixel>>3 ) & 0b0000000011111100) + ((rgb565Pixel>>9 ) & 0b0000000000000011); // convert to full 8 bit
          uint16_t red  =   ((rgb565Pixel>>8 ) & 0b0000000011111000) + ((rgb565Pixel>>13) & 0b0000000000000111); // convert to full 8 bit
//DEBUGF("888pixel[FF%0.2x%02x%02x] ",red,green,blue);          
          if (swaprb==true) {uint16_t unu = blue; blue=red; red=unu;}      
          uint32_t b = ((uint32_t) blue      ) & 0x000000FF;
          uint32_t g = ((uint32_t) green<<8  ) & 0x0000FF00;
          uint32_t r = ((uint32_t) red <<16  ) & 0x00FF0000;
          uint32_t a = 0xFF000000;
//DEBUGF("8888pixel[%0.8x]\n",(a | r | g | b));            
          return (uint32_t) (a | r | g | b); 
} 

 // ARGB1555 to ARGB8888 conversion
uint32_t Convert1555to8888(uint16_t argb1555Pixel,bool swaprb,bool transparant, uint32_t alpha0)
{
//DEBUGF("1555pixel[%0.4x] ",argb1555Pixel);
          uint16_t blue   = ((argb1555Pixel<<3 ) & 0b0000000011111000) + ((argb1555Pixel>>2 ) & 0b0000000000000111); // convert to full 8 bit
          uint16_t green =  ((argb1555Pixel>>2 ) & 0b0000000011111000) + ((argb1555Pixel>>7 ) & 0b0000000000000111); // convert to full 8 bit
          uint16_t red  =   ((argb1555Pixel>>7 ) & 0b0000000011111000) + ((argb1555Pixel>>12) & 0b0000000000000111); // convert to full 8 bit  
          uint16_t alpha =0x00; if (argb1555Pixel&&0x80) alpha =0xff;
//DEBUGF("8888pixel[%0.2x%0.2x%02x%02x] ",alpha,red,green,blue);             
          if (swaprb==true) {uint16_t unu = blue; blue=red; red=unu;}   
          if ( alpha<=ALPHANEARZERO && alpha0>=ZALPHAREPLACE ) {red= (alpha0>>16)&0x000000FF; green= (alpha0>>8)&0x000000FF;  blue= (alpha0)&0x000000FF; }      // if not transparant and aplha color defined: replace near zero transparancy colors
          if ( !transparant) alpha = 0xff; // allp opaque colors
          uint32_t b = ((uint32_t) blue      ) & 0x000000FF;
          uint32_t g = ((uint32_t) green<<8  ) & 0x0000FF00;
          uint32_t r = ((uint32_t) red <<16  ) & 0x00FF0000;
          uint32_t a = ((uint32_t) alpha<<24 ) & 0xFF000000;
//DEBUGF("8888pixel[%0.8x]\n",(a | r | g | b));             
          return (uint32_t) (a | r | g | b);
}

 // ARGB4444 to ARGB8888 conversion
uint32_t Convert4444to8888(uint16_t argb4444Pixel,bool swaprb,bool transparant, uint32_t alpha0)
{
//DEBUGF("4444pixel[%0.4x] ",argb4444Pixel);
          uint16_t blue   = ((argb4444Pixel<<4 ) & 0b0000000011110000) + ((argb4444Pixel    ) & 0b0000000000001111); // convert to full 8 bit
          uint16_t green =  ((argb4444Pixel    ) & 0b0000000011110000) + ((argb4444Pixel>>4 ) & 0b0000000000001111); // convert to full 8 bit
          uint16_t red  =   ((argb4444Pixel>>4 ) & 0b0000000011110000) + ((argb4444Pixel>>8 ) & 0b0000000000001111); // convert to full 8 bit  
          uint16_t alpha =  ((argb4444Pixel>>8 ) & 0b0000000011110000) + ((argb4444Pixel>>12) & 0b0000000000001111); // convert to full 8 bit  
//DEBUGF("8888pixel[%0.2x%0.2x%02x%02x] ",alpha,red,green,blue);             
          if (swaprb==true) {uint16_t unu = blue; blue=red; red=unu;}   
          if ( alpha<=ALPHANEARZERO && alpha0>=ZALPHAREPLACE ) {red= (alpha0>>16)&0x000000FF; green= (alpha0>>8)&0x000000FF;  blue= (alpha0)&0x000000FF; }      // if not transparant and aplha color defined: replace near zero transparancy colors
          if ( !transparant) alpha = 0xff; // allp opaque colors
          uint32_t b = ((uint32_t) blue      ) & 0x000000FF;
          uint32_t g = ((uint32_t) green<<8  ) & 0x0000FF00;
          uint32_t r = ((uint32_t) red <<16  ) & 0x00FF0000;
          uint32_t a = ((uint32_t) alpha<<24 ) & 0xFF000000;
//DEBUGF("8888pixel[%0.8x]\n",(a | r | g | b));             
          return (uint32_t) (a | r | g | b);
}

// Convert ARGB4444 to RGB565 mix alpha channel to background color alpha0
uint16_t Convert4444to565(uint16_t argb4444Pixel,bool swaprb,bool transparant, uint32_t alpha0)
{
 // DEBUGF("4444pixel[%0.4x] ",argb4444Pixel);
          uint32_t blue   = ((argb4444Pixel<<4 ) & 0b0000000011110000) + ((argb4444Pixel    ) & 0b0000000000001111); // convert to full 8 bit
          uint32_t green =  ((argb4444Pixel    ) & 0b0000000011110000) + ((argb4444Pixel>>4 ) & 0b0000000000001111); // convert to full 8 bit
          uint32_t red  =   ((argb4444Pixel>>4 ) & 0b0000000011110000) + ((argb4444Pixel>>8 ) & 0b0000000000001111); // convert to full 8 bit  
          uint32_t alpha =  ((argb4444Pixel>>8 ) & 0b0000000011110000) + ((argb4444Pixel>>12) & 0b0000000000001111); // convert to full 8 bit  
          if (! transparant) alpha = 0xff;
 // DEBUGF("8888pixel[%0.2x%0.2x%02x%02x] ",alpha,red,green,blue);              
          uint32_t blue0   = ((alpha0   ) & 0x000000FF);
          uint32_t green0 =  ((alpha0>>8) & 0x000000FF);
          uint32_t red0  =   ((alpha0>>16)& 0x000000FF);
          if (swaprb==true) {uint32_t unu = blue; blue=red; red=unu;}    
          red=(red*alpha)/0xFF + (red0*(0xFF-alpha))/0xFF;   blue=(blue*alpha)/0xFF + (blue0*(0xFF-alpha))/0xFF;   green=(green*alpha)/0xFF + (green0*(0xFF-alpha))/0xFF;  // mix alpha channel to background color
  //DEBUGF("8888pixel2[%0.2x%0.2x%02x%02x] ",alpha,red,green,blue);               
          uint16_t b = ((uint16_t) blue >> 3 ) & 0b0000000000011111;
          uint16_t g = ((uint16_t) green <<3 ) & 0b0000011111100000;
          uint16_t r = ((uint16_t) red  <<8  ) & 0b1111100000000000;
 // DEBUGF("565pixel[%0.4x]\n",(r | g | b));          
          return (uint16_t) ( r | g | b);  
} 

// Convert ARGB1555 to RGB565, mix alpha channel to background color alpha0
uint16_t Convert1555to565(uint16_t argb1555Pixel,bool swaprb,bool transparant, uint32_t alpha0)
{
          uint32_t blue   = ((argb1555Pixel<<3 ) & 0b0000000011111000) + ((argb1555Pixel>>2 ) & 0b0000000000000111); // convert to full 8 bit
          uint32_t green =  ((argb1555Pixel>>2 ) & 0b0000000011111000) + ((argb1555Pixel>>7 ) & 0b0000000000000111); // convert to full 8 bit
          uint32_t red  =   ((argb1555Pixel>>7 ) & 0b0000000011111000) + ((argb1555Pixel>>12) & 0b0000000000000111); // convert to full 8 bit  
          uint32_t alpha =0x00; if (argb1555Pixel&&0x80) alpha =0xff;
          if (! transparant) alpha = 0xff;
          uint32_t blue0   = ((alpha0   ) & 0x000000FF);
          uint32_t green0 =  ((alpha0>>8) & 0x000000FF);
          uint32_t red0  =   ((alpha0>>16)& 0x000000FF);  
          if (swaprb==true) {uint32_t unu = blue; blue=red; red=unu;}    
          red=(red*alpha)/0xFF + (red0*(0xFF-alpha))/0xFF;   blue=(blue*alpha)/0xFF + (blue0*(0xFF-alpha))/0xFF;   green=(green*alpha)/0xFF + (green0*(0xFF-alpha))/0xFF;  // mix alpha channel to background color
          uint16_t b = ((uint16_t) blue >> 3 ) & 0b0000000000011111;
          uint16_t g = ((uint16_t) green <<3 ) & 0b0000011111100000;
          uint16_t r = ((uint16_t) red  <<8  ) & 0b1111100000000000;
          return (uint16_t) ( r | g | b);  
} 

// Convert RGB565 to RGB565 - RB swap option only
uint16_t Convert565to565(uint16_t rgb565Pixel,bool swaprb)
{
          uint16_t blue   = ((rgb565Pixel<<3 ) & 0b0000000011111000) + ((rgb565Pixel>>2 ) & 0b0000000000000111); // convert to full 8 bit
          uint16_t green =  ((rgb565Pixel>>3 ) & 0b0000000011111100) + ((rgb565Pixel>>9 ) & 0b0000000000000011); // convert to full 8 bit
          uint16_t red  =   ((rgb565Pixel>>8 ) & 0b0000000011111000) + ((rgb565Pixel>>13) & 0b0000000000000111); // convert to full 8 bit
          if (swaprb==true) {uint32_t unu = blue; blue=red; red=unu;}    
          uint16_t b = ((uint16_t) blue >> 3 ) & 0b0000000000011111;
          uint16_t g = ((uint16_t) green <<3 ) & 0b0000011111100000;
          uint16_t r = ((uint16_t) red  <<8  ) & 0b1111100000000000;
          return (uint16_t) ( r | g | b);  
} 
